package com.pulsr.junonative;

import com.facebook.react.bridge.Promise;
import com.facebook.react.bridge.ReactApplicationContext;
import com.facebook.react.bridge.ReactContextBaseJavaModule;
import com.facebook.react.bridge.ReactMethod;
import com.facebook.react.module.annotations.ReactModule;

@ReactModule(name = JunoEngineModule.NAME)
public class JunoEngineModule extends ReactContextBaseJavaModule {
  public static final String NAME = "JunoEngineModule";

  static {
    System.loadLibrary("junobridge");
  }

  private native boolean nativeStart(int sr, int bs);
  private native void nativeStop();
  private native void nativeNoteOn(int note, float vel);
  private native void nativeNoteOff(int note);
  private native void nativeSetParam(String id, float value);

  public JunoEngineModule(ReactApplicationContext ctx) {
    super(ctx);
  }

  @Override
  public String getName() {
    return NAME;
  }

  @ReactMethod
  public void start(int sr, int bs, Promise promise) {
    boolean started = nativeStart(sr, bs);
    if (started) {
      promise.resolve(true);
    } else {
      promise.reject("JUNO_START_FAILED", "Failed to start Juno audio engine");
    }
  }

  @ReactMethod
  public void stop() {
    nativeStop();
  }

  @ReactMethod
  public void noteOn(int n, double v) {
    nativeNoteOn(n, (float) v);
  }

  @ReactMethod
  public void noteOff(int n) {
    nativeNoteOff(n);
  }

  @ReactMethod
  public void setParameter(String id, double val) {
    nativeSetParam(id, (float) val);
  }
}
