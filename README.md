# PSOLA C++ implementation
Real-time C++ implementation of [PSOLA], monophonic time domain pitch shifter    
(Based on JUCE & C++ Standard Library only)   

**You can check out the demo [here]!**

### Requirements
- C++ 14 or later
- CMake 3.22 or later
- JUCE Framework 8.0.7 or later
## Usage
### How to integrate into your project
1. Clone this repository into a dependency folder (e.g., `your_project/dep`)
```bash
$ mkdir dep
$ cd dep
$ git clone https://github.com/kimgihong2510/psola.git
```
2. Add the directory to your main `CMakeLists.txt` file
```CMake
add_subdirectory(dep/psola)

target_link_libraries(YourTarget PRIVATE psola_lib)
```
### Code Example
```C++
#include "psola.h"

// Instantiate psola 
TD_pitch_shift::Psola psola(
    44100.0,    // sample rate
    numChannels,// number of audio channels
    70.0f,      // min frequency of input audio
    1500.0f)    // max frequency of input audio

// Get audio block to process
int frameSize = psola.getFrameSize();
juce::AudioBuffer<float> audioFrame(numChannels, frameSize);

// Process audio
// Takes frameSize samples as input and stores the output in audioFrame
double semitones = -7.0; // pitch shift -7.0 semitones up (i.e., 7.0 semitones down)
psola.Process(audioFrame, semitones);
```

### Try it Out
We've included a ready-to-run example in `example/psola_example.cpp`.  
You can build and run it directly to experiment with PSOLA, no library setup required.

#### 1. Build
```bash
$ mkdir build
$ cd build
$ cmake ..
$ make
```

#### 2. Run psola_example
```bash
$ ./build/example/psola_example <semitone> <input.wav> <output.wav>
```
This will generate a pitch-shifted version of "input.wav" according to the "semitone" value and save it as "output.wav".

## TODO
- Implement upsampling to prevent aliasing when pitch shifted up

[PSOLA]: https://en.wikipedia.org/wiki/PSOLA
[here]: https://drive.google.com/drive/folders/1bQvE_J-PoF_5sEzngos_lbAcrw1xGbqu?usp=share_link 
