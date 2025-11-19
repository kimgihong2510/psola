#pragma once

#include <vector>

#include <juce_audio_basics/juce_audio_basics.h>
#include "pyin.h"

#include "sola.h"

namespace TD_pitch_shift {

class Psola {
public:    
    Psola(double sampleRate, int numChannels, float minFreq, float maxFreq);
    Psola(const Psola& other) = delete;
    Psola& operator=(const Psola&) = delete;
    Psola(Psola&& other) = default;
    Psola& operator=(const Psola&&) = delete;
    
    static int getFrameSize(double sampleRate, int numChannels, float minFreq, float maxFreq);
    int getFrameSize() const noexcept;
    void Process(juce::AudioBuffer<float>& frame, double semitones);
private:
    const double mSampleRate;
    const int mNumChannels;
    const int mFrameSize;
    const int mPyinFrameSize;
    const int mNumFrameSplit;

    Sola mSola;
    pyin_pitch_detection::Pyin mPyin;
    std::vector<float> mPyinBuffer;
};

} // namespace TD_pitch_shift