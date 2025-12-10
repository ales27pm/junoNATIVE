#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
#import <React/RCTEventEmitter.h>
#import <React/RCTBridgeModule.h>
#import <cstring>

#import "RTNJunoEngine.h"
#import "JunoDSPEngine.hpp"
#import "Juno106PatchParser.hpp"  // For Juno106::PatchParser

static NSString * const EVENT_ENGINE_STARTED       = @"EngineStarted";
static NSString * const EVENT_ENGINE_STOPPED       = @"EngineStopped";
static NSString * const EVENT_PATCH_LOADED         = @"PatchLoaded";
static NSString * const EVENT_PARAMETER_CHANGED    = @"ParameterChanged";
static NSString * const EVENT_ERROR                = @"EngineError";

@interface RTNJunoEngine () <RCTBridgeModule>
@end

@implementation RTNJunoEngine {
  std::unique_ptr<JunoDSPEngine> _dspEngine;
  AVAudioEngine *_audioEngine;
  AVAudioSourceNode *_sourceNode;
  BOOL _isInitialized;
  int _bufferSize;
}

RCT_EXPORT_MODULE();

- (NSArray<NSString *> *)supportedEvents {
  return @[
    EVENT_ENGINE_STARTED,
    EVENT_ENGINE_STOPPED,
    EVENT_PATCH_LOADED,
    EVENT_PARAMETER_CHANGED,
    EVENT_ERROR
  ];
}

RCT_EXPORT_METHOD(initialize:(NSDictionary *)config)
{
  if (_isInitialized) return;

  int sr = config[@"sampleRate"] ? [config[@"sampleRate"] intValue] : 44100;
  int bs = config[@"bufferSize"] ? [config[@"bufferSize"] intValue] : 256;
  BOOL gpu = [config[@"useGPU"] boolValue];

  AVAudioSession *session = [AVAudioSession sharedInstance];
  NSError *sessionError = nil;
  [session setCategory:AVAudioSessionCategoryPlayAndRecord
           withOptions:AVAudioSessionCategoryOptionDefaultToSpeaker |
                       AVAudioSessionCategoryOptionMixWithOthers
                 error:&sessionError];
  if (sessionError) {
    NSLog(@"[RTNJunoEngine] AVAudioSession error: %@", sessionError);
  }
  [session setPreferredSampleRate:sr error:&sessionError];
  if (sessionError) {
    NSLog(@"[RTNJunoEngine] Failed to set preferred sample rate: %@", sessionError);
    [self sendEventWithName:EVENT_ERROR
                       body:@{ @"message": @"Failed to set preferred sample rate" }];
    sr = (int)session.sampleRate;
  } else {
    sr = (int)session.sampleRate;
  }

  _dspEngine = std::make_unique<JunoDSPEngine>();
  if (!_dspEngine->initialize(sr, bs, 8, gpu)) {
    [self sendEventWithName:EVENT_ERROR
                       body:@{ @"message": @"Failed to initialize DSP engine" }];
    _dspEngine.reset();
    return;
  }

  _audioEngine = [[AVAudioEngine alloc] init];
  _bufferSize = bs;

  AVAudioFormat *format =
    [[AVAudioFormat alloc] initStandardFormatWithSampleRate:sr channels:2];

  __weak typeof(self) weakSelf = self;
  _sourceNode = [[AVAudioSourceNode alloc]
    initWithRenderBlock:^OSStatus(BOOL *isSilence,
                                  const AudioTimeStamp *timestamp,
                                  AVAudioFrameCount frameCount,
                                  AudioBufferList *outputData) {
      __strong typeof(weakSelf) strongSelf = weakSelf;
      if (!strongSelf || !strongSelf->_dspEngine) {
        *isSilence = YES;
        for (UInt32 i = 0; i < outputData->mNumberBuffers; ++i) {
          std::memset(outputData->mBuffers[i].mData, 0,
                      outputData->mBuffers[i].mDataByteSize);
        }
        return noErr;
      }

      float *left  = (float *)outputData->mBuffers[0].mData;
      float *right = (outputData->mNumberBuffers > 1)
                     ? (float *)outputData->mBuffers[1].mData
                     : nullptr;

      if (!right) {
        // mono buffer: duplicate
        strongSelf->_dspEngine->renderAudio(left, left,
                                            static_cast<int>(frameCount));
      } else {
        strongSelf->_dspEngine->renderAudio(left, right,
                                            static_cast<int>(frameCount));
      }

      *isSilence = NO;
      return noErr;
    }];

  [_audioEngine attachNode:_sourceNode];
  [_audioEngine connect:_sourceNode to:_audioEngine.mainMixerNode format:format];

  NSError *startError = nil;
  if (![_audioEngine startAndReturnError:&startError]) {
    NSLog(@"[RTNJunoEngine] Failed to start AVAudioEngine: %@", startError);
    [self sendEventWithName:EVENT_ERROR
                       body:@{ @"message": @"Failed to start AVAudioEngine" }];
    _dspEngine.reset();
    return;
  }

  _isInitialized = YES;
  [self sendEventWithName:EVENT_ENGINE_STARTED body:@{}];
}

RCT_EXPORT_METHOD(setParameter:(NSString *)param value:(double)val)
{
  if (!_isInitialized || !_dspEngine) return;
  @try {
    _dspEngine->setParameter([param UTF8String], (float)val);
    [self sendEventWithName:EVENT_PARAMETER_CHANGED
                       body:@{ @"id": param, @"value": @(val) }];
  } @catch (NSException *ex) {
    [self sendEventWithName:EVENT_ERROR body:@{ @"message": ex.reason ?: @"setParameter exception" }];
  }
}

RCT_EXPORT_METHOD(setParametersBatch:(NSDictionary *)params)
{
  if (!_isInitialized || !_dspEngine) return;
  @try {
    for (NSString *k in params) {
      double v = [params[k] doubleValue];
      _dspEngine->setParameter([k UTF8String], (float)v);
    }
    [self sendEventWithName:EVENT_PARAMETER_CHANGED body:@{ @"batch": @YES }];
  } @catch (NSException *ex) {
    [self sendEventWithName:EVENT_ERROR body:@{ @"message": ex.reason ?: @"setParametersBatch exception" }];
  }
}

RCT_EXPORT_METHOD(noteOn:(int)note velocity:(double)vel)
{
  if (_isInitialized && _dspEngine) {
    _dspEngine->noteOn(note, (float)vel);
  }
}

RCT_EXPORT_METHOD(noteOff:(int)note)
{
  if (_isInitialized && _dspEngine) {
    _dspEngine->noteOff(note);
  }
}

RCT_EXPORT_METHOD(loadPatchFile:(NSString *)path
                  resolver:(RCTPromiseResolveBlock)resolve
                  rejecter:(RCTPromiseRejectBlock)reject)
{
  if (!_isInitialized || !_dspEngine) {
    reject(@"ENGINE_ERROR", @"Engine not initialized", nil);
    return;
  }
  @try {
    std::string cpath = [path UTF8String];
    auto patches = Juno106::PatchParser::parseFile(cpath);
    if (!patches.empty()) {
      _dspEngine->loadPatch(patches[0]);
      [self sendEventWithName:EVENT_PATCH_LOADED
                         body:@{ @"path": path, @"count": @(patches.size()) }];
      resolve(@YES);
    } else {
      reject(@"PATCH_ERROR", @"No patches in file", nil);
    }
  } @catch (const std::exception &e) {
    NSString *msg = [NSString stringWithUTF8String:e.what()];
    [self sendEventWithName:EVENT_ERROR body:@{ @"message": msg }];
    reject(@"PATCH_ERROR", msg, nil);
  }
}

- (void)invalidate {
  if (_isInitialized) {
    if (_audioEngine) {
      [_audioEngine pause];
      [_audioEngine stop];
      _audioEngine = nil;
    }
    if (_dspEngine) {
      _dspEngine->stop();
      _dspEngine.reset();
    }
    _isInitialized = NO;
    [self sendEventWithName:EVENT_ENGINE_STOPPED body:@{}];
  }
  [super invalidate];
}

@end
