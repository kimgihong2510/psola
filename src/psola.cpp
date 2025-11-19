#include "psola.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>

#include <juce_audio_basics/juce_audio_basics.h>
#include "pyin.h"

#include "sola.h"

namespace TD_pitch_shift {

static constexpr double kPyinUpdatePeriod = 0.05; // in seconds 

namespace {
static Sola GetSola(double sampleRate, int numChannels, int frameSize, float minFreq) {
    const int maxPeriod = static_cast<int>(std::ceil(sampleRate / minFreq));
    Sola sola(numChannels, frameSize, maxPeriod);
    return sola;
}

static pyin_pitch_detection::Pyin GetPyin(double sampleRate, int pyinFrameSize, float minFreq, float maxFreq) {
    using namespace pyin_pitch_detection;
    auto enhancedParams = Pyin::getDefaultEnhancedParams();
    Pyin pyin(sampleRate, pyinFrameSize, minFreq, maxFreq, enhancedParams);
    return pyin;
}
} // namespace

Psola::Psola(double sampleRate, int numChannels, float minFreq, float maxFreq) :
    mSampleRate(sampleRate),
    mNumChannels(numChannels),
    mFrameSize(getFrameSize(sampleRate, numChannels, minFreq, maxFreq)),
    mPyinFrameSize(pyin_pitch_detection::Pyin::getDefaultFrameSize(sampleRate, minFreq)),
    mNumFrameSplit(mPyinFrameSize / mFrameSize),
    mSola(GetSola(sampleRate, numChannels, mFrameSize, minFreq)),
    mPyin(GetPyin(sampleRate, mPyinFrameSize, minFreq, maxFreq)),
    mPyinBuffer(mPyinFrameSize, 0.0f)
{
    assert(numChannels > 0 && sampleRate > 0.0);
    assert(0 < minFreq && minFreq < maxFreq);
    assert(mPyinFrameSize % mFrameSize == 0);
    assert(mPyinFrameSize >= mFrameSize);
}

int Psola::getFrameSize(double sampleRate, int numChannels, float minFreq, float maxFreq) {
    using namespace pyin_pitch_detection;
    const int pyinFrameSize = Pyin::getDefaultFrameSize(sampleRate, minFreq);
    assert(pyinFrameSize > 0);
    assert((pyinFrameSize & (pyinFrameSize - 1)) == 0); // pyinFrameSize is power of 2

    const double pyinFrameDuration = static_cast<double>(pyinFrameSize) / sampleRate;
    int numFrameSplit = 1;
    for(; pyinFrameSize / numFrameSplit > kPyinUpdatePeriod * sampleRate; numFrameSplit*=2) {}

    return pyinFrameSize / numFrameSplit;
}

int Psola::getFrameSize() const noexcept {
    return mFrameSize;
}

void Psola::Process(juce::AudioBuffer<float>& frame, double semitones) {
    assert(frame.getNumChannels() == mNumChannels);
    assert(frame.getNumSamples() == mFrameSize);

    // Update mPyinBuffer
    std::copy_backward(mPyinBuffer.begin(), mPyinBuffer.end() - mFrameSize, mPyinBuffer.end());
    std::fill(mPyinBuffer.begin(), mPyinBuffer.begin() + mFrameSize, 0.0f);
    for(int c = 0; c < mNumChannels; ++c) {
        const float gain = 1.0f / static_cast<float>(mNumChannels);
        juce::FloatVectorOperations::addWithMultiply(mPyinBuffer.data(), frame.getReadPointer(c), gain, mFrameSize);
    }

    const auto pyinResult = mPyin.process(mPyinBuffer.data());
    const int period = static_cast<int>(std::round(mSampleRate / pyinResult.freq));

    if(pyinResult.isVoiced)
        mSola.Process(frame, period, semitones);
    else 
        mSola.Process(frame, period, 0.0);
}

} // namespace TD_pitch_shift