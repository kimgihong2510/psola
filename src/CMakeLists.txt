# PSOLA
add_library(psola_lib
    STATIC
        ${CMAKE_CURRENT_LIST_DIR}/psola.cpp
        ${CMAKE_CURRENT_LIST_DIR}/sola.cpp)

target_link_libraries(psola_lib
    PUBLIC
        juce::juce_audio_basics
        pyin_lib)

target_compile_features(psola_lib 
    PRIVATE cxx_std_14)

target_include_directories(psola_lib PUBLIC ${CMAKE_CURRENT_LIST_DIR})