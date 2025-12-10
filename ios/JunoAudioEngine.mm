// ============================================================

#import "JunoAudioEngine.h"
#import <AVFoundation/AVFoundation.h>

#ifdef __cplusplus
#import "JunoEngine.hpp"
#endif

@interface JunoAudioEngine () {
#ifdef __cplusplus
    JunoEngine *_engine;
#endif
}
@property (nonatomic, strong) AVAudioEngine *audioEngine;
@property (nonatomic, strong) AVAudioSourceNode *sourceNode;
@end

@implementation JunoAudioEngine

+ (instancetype)sharedInstance {
    static JunoAudioEngine *instance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        instance = [[JunoAudioEngine alloc] initPrivate];
    });
    return instance;
}

- (instancetype)initPrivate {
    self = [super init];
    if (self) {
        _audioEngine = [[AVAudioEngine alloc] init];
#ifdef __cplusplus
        _engine = new JunoEngine();
#endif
        [self setupAudio];
    }
    return self;
}

- (void)dealloc {
#ifdef __cplusplus
    delete _engine;
    _engine = nullptr;
#endif
}

- (void)setupAudio {
    AVAudioFormat *format = [[AVAudioFormat alloc] initStandardFormatWithSampleRate:44100 channels:2];
#ifdef __cplusplus
    _engine->init(format.sampleRate);
#endif

    __weak typeof(self) weakSelf = self;
    _sourceNode = [[AVAudioSourceNode alloc] initWithFormat:format
                                                renderBlock:^OSStatus(BOOL * _Nonnull isSilence,
                                                                      const AudioTimeStamp * _Nonnull timestamp,
                                                                      AVAudioFrameCount frameCount,
                                                                      AudioBufferList * _Nonnull outputData) {
        __strong typeof(self) strongSelf = weakSelf;
        if (!strongSelf) return noErr;

        float *left = (float *)outputData->mBuffers[0].mData;
        float *right = (float *)outputData->mBuffers[1].mData;
#ifdef __cplusplus
        _engine->render(left, right, (int)frameCount);
#else
        memset(left, 0, sizeof(float) * frameCount);
        memset(right, 0, sizeof(float) * frameCount);
#endif
        *isSilence = NO;
        return noErr;
    }];

    [_audioEngine attachNode:_sourceNode];
    [_audioEngine connect:_sourceNode to:_audioEngine.mainMixerNode format:format];
}

- (void)start {
    NSError *err = nil;
    if (![_audioEngine isRunning]) {
        [_audioEngine prepare];
        [_audioEngine startAndReturnError:&err];
        if (err) {
            NSLog(@"JunoAudioEngine start error: %@", err);
        }
    }
}

- (void)stop {
    if ([_audioEngine isRunning]) {
        [_audioEngine stop];
    }
}

- (void)noteOn:(int)midiNote velocity:(float)velocity {
#ifdef __cplusplus
    _engine->noteOn(midiNote, velocity);
#endif
}

- (void)noteOff:(int)midiNote {
#ifdef __cplusplus
    _engine->noteOff(midiNote);
#endif
}

- (void)setPolyAftertouchForVoice:(int)voiceIndex pressure:(float)pressure {
#ifdef __cplusplus
    _engine->setPolyAftertouch(voiceIndex, pressure);
#endif
}

- (void)setAnalogCharacterWithDCO:(float)dcoBeating
                      filterDrift:(float)filterDrift
                     envelopeClick:(float)envClick
                      cableLength:(float)cableLength
                      temperature:(float)temperature
                              age:(float)age {
#ifdef __cplusplus
    _engine->setAnalogCharacter(dcoBeating, filterDrift, envClick,
                                cableLength, temperature, age);
#endif
}

@end


// ============================================================
