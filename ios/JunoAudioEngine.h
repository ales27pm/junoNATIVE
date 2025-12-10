// ============================================================

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface JunoAudioEngine : NSObject

+ (instancetype)sharedInstance;

- (void)start;
- (void)stop;

- (void)noteOn:(int)midiNote velocity:(float)velocity;
- (void)noteOff:(int)midiNote;
- (void)setPolyAftertouchForVoice:(int)voiceIndex pressure:(float)pressure;

- (void)setAnalogCharacterWithDCO:(float)dcoBeating
                      filterDrift:(float)filterDrift
                     envelopeClick:(float)envClick
                      cableLength:(float)cableLength
                      temperature:(float)temperature
                              age:(float)age;

@end

NS_ASSUME_NONNULL_END


// ============================================================
