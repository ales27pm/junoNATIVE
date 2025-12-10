// ============================================================

#pragma once

#include <vector>
#include <cstdint>
#include <string>
#include <fstream>
#include <stdexcept>

namespace Juno106 {

constexpr size_t SYSEX_MESSAGE_SIZE = 25;
constexpr uint8_t SYSEX_START  = 0xF0;
constexpr uint8_t SYSEX_END    = 0xF7;
constexpr uint8_t ROLAND_ID    = 0x41;

// The exact device/model IDs are not strictly required for parsing;
// we only sanity‑check start/end and payload length.

struct Switches {
    bool dcoFoot16 = false;
    bool dcoFoot8  = false;
    bool dcoFoot4  = false;
    bool pulseWaveOn = false;
    bool sawWaveOn   = false;
    bool chorusOn     = false;      // Level I or II engaged
    bool chorusLevelII = false;     // true = Level II, false = Level I
    bool pwmSourceLFO  = true;      // true=LFO, false=Manual
    bool vcfEnvPositive = true;     // true = positive polarity
    bool vcaModeEnv     = true;     // true = ENV, false = GATE
    uint8_t hpfSetting  = 0;        // 0–3
};

struct JunoPatch {
    // Raw sliders (0‑127)
    uint8_t lfoRate       = 0;
    uint8_t lfoDelay      = 0;
    uint8_t dcoLfoMod     = 0;
    uint8_t dcoPwmDepth   = 0;
    uint8_t dcoNoiseLevel = 0;
    uint8_t vcfCutoff     = 0;
    uint8_t vcfResonance  = 0;
    uint8_t vcfEnvMod     = 0;
    uint8_t vcfLfoMod     = 0;
    uint8_t vcfKeyFollow  = 0;
    uint8_t vcaLevel      = 0;
    uint8_t envAttack     = 0;
    uint8_t envDecay      = 0;
    uint8_t envSustain    = 0;
    uint8_t envRelease    = 0;
    uint8_t dcoSubLevel   = 0;

    // Front‑panel switches
    Switches switches;

    // Metadata (not critical for sound)
    uint8_t sourcePatchNumber = 0;
    uint8_t midiChannel       = 0;
    bool checksumValid        = false;
};

class PatchParser {
public:
    static std::vector<JunoPatch> parseFile(const std::string& filepath) {
        std::ifstream file(filepath, std::ios::binary | std::ios::ate);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file: " + filepath);
        }

        const std::streamsize fileSize = file.tellg();
        if (fileSize <= 0) {
            throw std::runtime_error("Empty file: " + filepath);
        }
        file.seekg(0, std::ios::beg);

        if (fileSize % static_cast<std::streamsize>(SYSEX_MESSAGE_SIZE) != 0) {
            throw std::runtime_error("Invalid Juno .106 file size");
        }

        const size_t numPatches = static_cast<size_t>(fileSize) / SYSEX_MESSAGE_SIZE;
        std::vector<uint8_t> buffer(static_cast<size_t>(fileSize));
        if (!file.read(reinterpret_cast<char*>(buffer.data()), fileSize)) {
            throw std::runtime_error("Failed to read Juno .106 file");
        }

        return parseBuffer(buffer, numPatches);
    }

    static std::vector<JunoPatch> parseBuffer(const std::vector<uint8_t>& buffer,
                                              size_t expectedPatches) {
        if (buffer.size() % SYSEX_MESSAGE_SIZE != 0) {
            throw std::runtime_error("Buffer size is not a multiple of SYSEX_MESSAGE_SIZE");
        }

        const size_t numPatches = buffer.size() / SYSEX_MESSAGE_SIZE;
        if (expectedPatches != 0 && numPatches != expectedPatches) {
            // We tolerate mismatch but warn via exception in strict use.
            // For engine use we usually pass expectedPatches=0.
        }

        std::vector<JunoPatch> patches;
        patches.reserve(numPatches);

        for (size_t i = 0; i < numPatches; ++i) {
            const size_t offset = i * SYSEX_MESSAGE_SIZE;
            std::vector<uint8_t> msg(
                buffer.begin() + static_cast<std::ptrdiff_t>(offset),
                buffer.begin() + static_cast<std::ptrdiff_t>(offset + SYSEX_MESSAGE_SIZE)
            );
            patches.push_back(parseSingleSysex(msg, i));
        }

        return patches;
    }

    static JunoPatch parseSingleSysex(const std::vector<uint8_t>& msg,
                                      size_t patchIndex = 0) {
        if (msg.size() != SYSEX_MESSAGE_SIZE) {
            throw std::runtime_error("Unexpected sysex length for Juno‑106 patch");
        }
        if (msg[0] != SYSEX_START || msg.back() != SYSEX_END) {
            throw std::runtime_error("Invalid sysex start/end bytes for Juno‑106");
        }
        if (msg[1] != ROLAND_ID) {
            throw std::runtime_error("Not a Roland sysex message");
        }

        const uint8_t deviceChannel = msg[2] & 0x0F;   // lower 4 bits
        const uint8_t patchNumber   = msg[3] & 0x7F;   // 0–127
        const bool checksumOk       = verifyChecksum(msg);

        JunoPatch patch;
        patch.midiChannel       = deviceChannel;
        patch.sourcePatchNumber = patchNumber;
        patch.checksumValid     = checksumOk;

        // Parameter bytes, based on Roland Juno‑106 layout:
        // LFO rate, LFO delay, DCO LFO, DCO PWM, noise, VCF cutoff, res,
        // env mod, LFO mod, key follow, VCA level, env A,D,S,R, sub level,
        // switches1, switches2, checksum, F7.
        const size_t offsets[] = {
            5,  6,  7,  8,
            9,  10, 11, 12,
            13, 14, 15, 16,
            17, 18, 19, 20,
            21
        };

        uint8_t* params[] = {
            &patch.lfoRate,
            &patch.lfoDelay,
            &patch.dcoLfoMod,
            &patch.dcoPwmDepth,
            &patch.dcoNoiseLevel,
            &patch.vcfCutoff,
            &patch.vcfResonance,
            &patch.vcfEnvMod,
            &patch.vcfLfoMod,
            &patch.vcfKeyFollow,
            &patch.vcaLevel,
            &patch.envAttack,
            &patch.envDecay,
            &patch.envSustain,
            &patch.envRelease,
            &patch.dcoSubLevel
        };

        // Copy the first 16 continuous parameters.
        for (size_t i = 0; i < 16; ++i) {
            *params[i] = msg[offsets[i]];
        }

        // Switches are stored in bytes 21 and 22
        decodeSwitches(msg[21], msg[22], patch.switches);

        return patch;
    }

private:
    static bool verifyChecksum(const std::vector<uint8_t>& msg) {
        // Roland checksum: 7‑bit sum of all data bytes, lower 7 bits inverted.
        // For the 106 patches bytes 5..22 are part of the checksum.
        uint32_t sum = 0;
        for (size_t i = 5; i <= 22; ++i) {
            sum += msg[i] & 0x7F;
        }
        uint8_t computed = static_cast<uint8_t>((128 - (sum & 0x7F)) & 0x7F);
        uint8_t stored   = msg[23] & 0x7F;
        return computed == stored;
    }

    static void decodeSwitches(uint8_t sw1, uint8_t sw2, Switches& out) {
        // sw1:
        // bit0: DCO Range 16'
        // bit1: DCO Range 8'
        // bit2: DCO Range 4'
        // bit3: Pulse on
        // bit4: Saw on
        // bit5: Chorus on (0 = on, 1 = off)
        // bit6: Chorus level II (0 = II, 1 = I)
        out.dcoFoot16 = (sw1 & 0x01) != 0;
        out.dcoFoot8  = (sw1 & 0x02) != 0;
        out.dcoFoot4  = (sw1 & 0x04) != 0;
        out.pulseWaveOn = (sw1 & 0x08) != 0;
        out.sawWaveOn   = (sw1 & 0x10) != 0;
        out.chorusOn     = (sw1 & 0x20) == 0;      // Inverted
        out.chorusLevelII = (sw1 & 0x40) == 0;     // Inverted

        // sw2:
        // bit0: PWM source (0 = LFO, 1 = Manual)
        // bit1: VCF ENV polarity (0 = positive, 1 = negative)
        // bit2: VCA mode (0 = ENV, 1 = GATE)
        // bit3‑4: HPF (0..3)
        out.pwmSourceLFO  = (sw2 & 0x01) == 0;
        out.vcfEnvPositive = (sw2 & 0x02) == 0;
        out.vcaModeEnv     = (sw2 & 0x04) == 0;
        out.hpfSetting     = static_cast<uint8_t>((sw2 >> 3) & 0x03);
    }
};


// ============================================================
