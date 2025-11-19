#include <iostream>
#include <vector>

#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include "pyin.h"

#include "psola.h"

static constexpr float kMinPitchFreq = 100.0f;
static constexpr float kMaxPitchFreq = 3000.0f;

int main(int argc, char* argv[]) {
    if(argc < 4) {
        std::cout << "Usage: psola_example <semitone> <input.wav> <output.wav>" << std::endl;
        return 1;
    }
    
    const int semitone = std::atoi(argv[1]);
    if(semitone < -12 || semitone > 12) {
        std::cerr << "Error: semitone value must be between -12 and 12." << std::endl;
        return 1;
    }
    juce::File inputFile(argv[2]);
    juce::File outputFile(argv[3]);

    if(!inputFile.existsAsFile()) {
        std::cerr << "Input file not found: " << inputFile.getFullPathName() << std::endl;
        return 1;
    }

    // 1. Load audio file
    juce::WavAudioFormat wavFormat{};
    auto* reader = wavFormat.createReaderFor(new juce::FileInputStream(inputFile), true);
    auto sampleRate = reader->sampleRate;
    juce::AudioBuffer<float> inputAudio(reader->numChannels, reader->lengthInSamples);

    auto readResult = reader->read(&inputAudio, 0, reader->lengthInSamples, 0, true, true);
    delete reader;
    if(!readResult) {
        std::cerr << "Error: Failed to read audio data from file.\n";
        return 1;
    }

    // 2. Instantiate pitch shifter
    TD_pitch_shift::Psola psola(
        sampleRate, 
        inputAudio.getNumChannels(), 
        kMinPitchFreq, 
        kMaxPitchFreq);

    // 3. Process audio frame by frame
    const int frameSize = psola.getFrameSize();
    juce::AudioBuffer<float> audioFrame(inputAudio.getNumChannels(), frameSize);
    juce::AudioBuffer<float> outputAudio(inputAudio.getNumChannels(), inputAudio.getNumSamples());
    for(int frameStartIdx = 0; frameStartIdx < inputAudio.getNumSamples(); frameStartIdx += frameSize) {
        // Set up the input buffer
        audioFrame.clear();
        int numSamplesToProcess = std::min(frameSize, inputAudio.getNumSamples() - frameStartIdx);
        for(int c = 0; c < audioFrame.getNumChannels(); ++c) 
            audioFrame.copyFrom(c, 0, inputAudio, c, frameStartIdx, numSamplesToProcess);

        // pitch shift
        psola.Process(audioFrame, static_cast<double>(semitone));

        // Write back to IO buffer
        for(int c = 0; c < audioFrame.getNumChannels(); ++c) 
            outputAudio.copyFrom(c, frameStartIdx, audioFrame, c, 0, numSamplesToProcess);
    }

    // 4. Save to output file
    auto* writer = wavFormat.createWriterFor (
        new juce::FileOutputStream(outputFile), 
        sampleRate,
        outputAudio.getNumChannels(), 
        24, 
        {}, 
        0);

    if(writer == nullptr) {
        delete writer;
        std::cerr << "Error: Failed to create WAV writer. Cannot write to file.\n";
        return 1;
    }
    bool writeOk = writer->writeFromAudioSampleBuffer(outputAudio, 0, outputAudio.getNumSamples());
    delete writer;

    if (!writeOk) {
        std::cerr << "Error: Failed to write audio data to output file.\n";
        return 1;
    }

    return 0;
}