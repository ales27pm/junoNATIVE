// ============================================================

#import "NativeJunoModule.h"
#import "JunoAudioEngine.h"

@implementation NativeJunoModule

RCT_EXPORT_MODULE(JunoModule);

RCT_EXPORT_METHOD(startAudio) {
  [[JunoAudioEngine sharedInstance] start];
}

RCT_EXPORT_METHOD(stopAudio) {
  [[JunoAudioEngine sharedInstance] stop];
}

RCT_EXPORT_METHOD(noteOn:(NSInteger)midiNote velocity:(nonnull NSNumber *)velocity) {
  [[JunoAudioEngine sharedInstance] noteOn:(int)midiNote velocity:[velocity floatValue]];
}

RCT_EXPORT_METHOD(noteOff:(NSInteger)midiNote) {
  [[JunoAudioEngine sharedInstance] noteOff:(int)midiNote];
}

RCT_EXPORT_METHOD(setPolyAftertouch:(NSInteger)voiceIndex
                  pressure:(nonnull NSNumber *)pressure) {
  [[JunoAudioEngine sharedInstance] setPolyAftertouchForVoice:(int)voiceIndex
                                                    pressure:[pressure floatValue]];
}

RCT_EXPORT_METHOD(setAnalogCharacter:(NSDictionary *)settings) {
  float dcoBeating   = [settings[@"dcoBeating"] floatValue];
  float filterDrift  = [settings[@"filterDrift"] floatValue];
  float envClick     = [settings[@"envelopeClick"] floatValue];
  float cableLength  = [settings[@"cableLength"] floatValue];
  float temperature  = [settings[@"temperature"] floatValue];
  float age          = [settings[@"age"] floatValue];

  [[JunoAudioEngine sharedInstance] setAnalogCharacterWithDCO:dcoBeating
                                                  filterDrift:filterDrift
                                                 envelopeClick:envClick
                                                  cableLength:cableLength
                                                  temperature:temperature
                                                          age:age];
}

@end


// ============================================================
