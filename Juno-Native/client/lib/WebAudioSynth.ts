// ============================================================

import { Platform } from "react-native";
import { SynthParams, SynthSwitches } from "@/hooks/useSynthEngine";

interface Voice {
  oscillators: OscillatorNode[];
  gainNode: GainNode;
  filterNode: BiquadFilterNode;
  note: number;
  startTime: number;
}

class WebAudioSynth {
  private audioContext: AudioContext | null = null;
  private masterGain: GainNode | null = null;
  private voices: Map<number, Voice> = new Map();
  private maxVoices = 6;
  private params: SynthParams | null = null;
  private switches: SynthSwitches | null = null;
  private lfoNode: OscillatorNode | null = null;
  private lfoGain: GainNode | null = null;
  private chorusDelayL: DelayNode | null = null;
  private chorusDelayR: DelayNode | null = null;
  private chorusLfoL: OscillatorNode | null = null;
  private chorusLfoR: OscillatorNode | null = null;

  constructor() {
    if (Platform.OS === "web" && typeof window !== "undefined") {
      this.initializeAudio();
    }
  }

  private async initializeAudio() {
    try {
      this.audioContext = new (window.AudioContext ||
        (window as any).webkitAudioContext)();

      this.masterGain = this.audioContext.createGain();
      this.masterGain.gain.value = 0.5;
      this.masterGain.connect(this.audioContext.destination);

      this.setupLFO();
      this.setupChorus();
    } catch (error) {
      console.warn("Web Audio not available:", error);
    }
  }

  private setupLFO() {
    if (!this.audioContext || !this.masterGain) return;

    this.lfoNode = this.audioContext.createOscillator();
    this.lfoNode.type = "triangle";
    this.lfoNode.frequency.value = 1;

    this.lfoGain = this.audioContext.createGain();
    this.lfoGain.gain.value = 0;

    this.lfoNode.connect(this.lfoGain);
    this.lfoNode.start();
  }

  private setupChorus() {
    if (!this.audioContext || !this.masterGain) return;

    const baseDelay = 0.015;

    this.chorusDelayL = this.audioContext.createDelay(0.1);
    this.chorusDelayL.delayTime.value = baseDelay;

    this.chorusDelayR = this.audioContext.createDelay(0.1);
    this.chorusDelayR.delayTime.value = baseDelay;

    this.chorusLfoL = this.audioContext.createOscillator();
    this.chorusLfoL.type = "triangle";
    this.chorusLfoL.frequency.value = 0.5;
    this.chorusLfoL.start();

    this.chorusLfoR = this.audioContext.createOscillator();
    this.chorusLfoR.type = "triangle";
    this.chorusLfoR.frequency.value = 0.5;
    this.chorusLfoR.phase = Math.PI;
    this.chorusLfoR.start();

    const lfoGainL = this.audioContext.createGain();
    lfoGainL.gain.value = 0.002;
    this.chorusLfoL.connect(lfoGainL);
    lfoGainL.connect(this.chorusDelayL.delayTime);

    const lfoGainR = this.audioContext.createGain();
    lfoGainR.gain.value = 0.002;
    this.chorusLfoR.connect(lfoGainR);
    lfoGainR.connect(this.chorusDelayR.delayTime);
  }

  public updateParams(params: SynthParams, switches: SynthSwitches) {
    this.params = params;
    this.switches = switches;

    if (this.lfoNode && this.lfoGain) {
      this.lfoNode.frequency.value = (params.lfoRate / 127) * 10 + 0.1;
      this.lfoGain.gain.value = (params.dcoLfoMod / 127) * 50;
    }

    if (this.chorusLfoL && this.chorusLfoR) {
      const rates = [0, 0.513, 0.863, 9.75];
      const rate = rates[switches.chorusMode] || 0;
      this.chorusLfoL.frequency.value = rate;
      this.chorusLfoR.frequency.value = rate;
    }

    this.voices.forEach((voice) => {
      this.updateVoiceParams(voice);
    });
  }

  private updateVoiceParams(voice: Voice) {
    if (!this.params || !this.switches) return;

    const cutoffHz = 20 + (this.params.vcfCutoff / 127) * 19980;
    voice.filterNode.frequency.value = cutoffHz;
    voice.filterNode.Q.value = (this.params.vcfResonance / 127) * 20;
  }

  private midiNoteToFrequency(note: number): number {
    return 440 * Math.pow(2, (note - 69) / 12);
  }

  public async noteOn(note: number, velocity: number = 100) {
    if (
      !this.audioContext ||
      !this.masterGain ||
      !this.params ||
      !this.switches
    ) {
      await this.initializeAudio();
      if (!this.audioContext || !this.masterGain) return;
    }

    if (this.audioContext.state === "suspended") {
      await this.audioContext.resume();
    }

    if (this.voices.has(note)) {
      this.noteOff(note);
    }

    if (this.voices.size >= this.maxVoices) {
      const oldestNote = this.voices.keys().next().value;
      if (oldestNote !== undefined) {
        this.noteOff(oldestNote);
      }
    }

    const frequency = this.midiNoteToFrequency(note);
    const oscillators: OscillatorNode[] = [];

    const gainNode = this.audioContext.createGain();
    gainNode.gain.value = 0;

    const filterNode = this.audioContext.createBiquadFilter();
    filterNode.type = "lowpass";

    const cutoffHz = this.params
      ? 20 + (this.params.vcfCutoff / 127) * 19980
      : 5000;
    filterNode.frequency.value = cutoffHz;
    filterNode.Q.value = this.params
      ? (this.params.vcfResonance / 127) * 20
      : 1;

    if (this.switches?.sawWaveOn) {
      const sawOsc = this.audioContext.createOscillator();
      sawOsc.type = "sawtooth";
      sawOsc.frequency.value = frequency;

      if (this.switches.dcoFoot16) sawOsc.frequency.value = frequency * 0.5;
      else if (this.switches.dcoFoot4) sawOsc.frequency.value = frequency * 2;

      sawOsc.connect(filterNode);
      sawOsc.start();
      oscillators.push(sawOsc);

      if (this.lfoGain) {
        this.lfoGain.connect(sawOsc.frequency);
      }
    }

    if (this.switches?.pulseWaveOn) {
      const pulseOsc = this.audioContext.createOscillator();
      pulseOsc.type = "square";
      pulseOsc.frequency.value = frequency;

      if (this.switches.dcoFoot16) pulseOsc.frequency.value = frequency * 0.5;
      else if (this.switches.dcoFoot4) pulseOsc.frequency.value = frequency * 2;

      pulseOsc.connect(filterNode);
      pulseOsc.start();
      oscillators.push(pulseOsc);
    }

    if (this.params && this.params.dcoSubLevel > 10) {
      const subOsc = this.audioContext.createOscillator();
      subOsc.type = "square";
      subOsc.frequency.value = frequency * 0.5;

      const subGain = this.audioContext.createGain();
      subGain.gain.value = (this.params.dcoSubLevel / 127) * 0.5;

      subOsc.connect(subGain);
      subGain.connect(filterNode);
      subOsc.start();
      oscillators.push(subOsc);
    }

    if (oscillators.length === 0) {
      const defaultOsc = this.audioContext.createOscillator();
      defaultOsc.type = "sawtooth";
      defaultOsc.frequency.value = frequency;
      defaultOsc.connect(filterNode);
      defaultOsc.start();
      oscillators.push(defaultOsc);
    }

    filterNode.connect(gainNode);

    if (
      this.switches?.chorusMode &&
      this.switches.chorusMode > 0 &&
      this.chorusDelayL &&
      this.chorusDelayR
    ) {
      const dryGain = this.audioContext.createGain();
      dryGain.gain.value = 0.5;
      gainNode.connect(dryGain);
      dryGain.connect(this.masterGain);

      const wetGainL = this.audioContext.createGain();
      wetGainL.gain.value = 0.25;
      gainNode.connect(this.chorusDelayL);
      this.chorusDelayL.connect(wetGainL);
      wetGainL.connect(this.masterGain);

      const wetGainR = this.audioContext.createGain();
      wetGainR.gain.value = 0.25;
      gainNode.connect(this.chorusDelayR);
      this.chorusDelayR.connect(wetGainR);
      wetGainR.connect(this.masterGain);
    } else {
      gainNode.connect(this.masterGain);
    }

    const now = this.audioContext.currentTime;
    const velocityGain =
      (velocity / 127) * (this.params ? this.params.vcaLevel / 127 : 0.8);
    const attackTime = this.params
      ? (this.params.envAttack / 127) * 2 + 0.001
      : 0.01;
    const decayTime = this.params
      ? (this.params.envDecay / 127) * 2 + 0.01
      : 0.2;
    const sustainLevel = this.params ? this.params.envSustain / 127 : 0.7;

    gainNode.gain.setValueAtTime(0, now);
    gainNode.gain.linearRampToValueAtTime(velocityGain, now + attackTime);
    gainNode.gain.linearRampToValueAtTime(
      velocityGain * sustainLevel,
      now + attackTime + decayTime,
    );

    const voice: Voice = {
      oscillators,
      gainNode,
      filterNode,
      note,
      startTime: now,
    };

    this.voices.set(note, voice);
  }

  public noteOff(note: number) {
    const voice = this.voices.get(note);
    if (!voice || !this.audioContext) return;

    const now = this.audioContext.currentTime;
    const releaseTime = this.params
      ? (this.params.envRelease / 127) * 3 + 0.01
      : 0.3;

    voice.gainNode.gain.cancelScheduledValues(now);
    voice.gainNode.gain.setValueAtTime(voice.gainNode.gain.value, now);
    voice.gainNode.gain.linearRampToValueAtTime(0, now + releaseTime);

    setTimeout(
      () => {
        voice.oscillators.forEach((osc) => {
          try {
            osc.stop();
            osc.disconnect();
          } catch (error) {
            console.warn("Failed to stop oscillator", error);
          }
        });
        voice.gainNode.disconnect();
        voice.filterNode.disconnect();
        this.voices.delete(note);
      },
      releaseTime * 1000 + 50,
    );
  }

  public allNotesOff() {
    this.voices.forEach((_, note) => {
      this.noteOff(note);
    });
  }

  public dispose() {
    this.allNotesOff();

    if (this.lfoNode) {
      this.lfoNode.stop();
      this.lfoNode.disconnect();
    }
    if (this.chorusLfoL) {
      this.chorusLfoL.stop();
      this.chorusLfoL.disconnect();
    }
    if (this.chorusLfoR) {
      this.chorusLfoR.stop();
      this.chorusLfoR.disconnect();
    }
    if (this.audioContext) {
      this.audioContext.close();
    }
  }
}

export const webAudioSynth = new WebAudioSynth();

// ============================================================
