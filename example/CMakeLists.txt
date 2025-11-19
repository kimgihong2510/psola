add_executable(psola_example psola_example.cpp)

target_link_libraries(psola_example
    PRIVATE
        psola_lib
        pyin_lib
        juce::juce_audio_basics
        juce::juce_audio_formats)