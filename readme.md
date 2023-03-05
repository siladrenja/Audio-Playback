# Intro
This is my personal project aimed at easing the basic waveform audio playback. 
It wraps the windows' multimedia API and provides a simple interface to play audio files.
# Usage
The library is header-only, so you can just include the header file and use it.

The library is Windows only, but I have prepared the code for the future possibility of Unix port. 
I am not sure if I will do it, as I'm not familiar with unix libraries,
but I will try to keep the code as portable as possible. (pull requests that include linux functionality are always welcome)

## Library contents
the library is contained inside the AP namespace.
The library contains the AudioPlayer class, and it defines a WaveFormat, which is just a macro for the WAVEFORMATEX structure. (I will probably change it to my own struct in the future for easier UNIX support)

To get started you need to create a WaveFormat struct and an AudioPlayer object.

The constructor for AudioPlayer goes like this:
```cpp
 AudioPlayer(const WaveFormat& waveForm, size_t BufferSize, std::function<size_t(char*, size_t)> PrepFunc);
```
Where:
- waveForm = the format of the audio file you want to play
- BufferSize = the size of the buffer (in bytes) that will be used to play the audio file
- PrepFunc = a function that will be called when the buffer is ready to be filled with data. It should return the number of bytes that were written to the buffer. (returning 0 will assume the end of audio has been reached)

Other AudioPlayer methods include:
```cpp
void ChangeWaveFormat(const WaveFormat& waveForm);
```
Where:
- waveForm = the new format of the audio file you want to play

```cpp
void FinalLoop();
```
Which doesn't return until the end of playback (aka until the PrepFunc returns 0), and it handles the actual playback.

## Example
```cpp
#include "AudioPlayback.hpp"
#include <cmath>

int main(){
//creating the WaveFormat
    AP::WaveFormat wfx;
    wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nChannels = 2;
    wfx.nSamplesPerSec = 128*1024;
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nChannels * 2;
    wfx.nBlockAlign = wfx.nChannels * 2;
    wfx.wBitsPerSample = 16;

    int EndAfter = 16; //after how many buffers played it should end

//creating the AudioPlayer object
    AP::AudioPlayer ap(wfx, 4096, [](char* buffer, size_t size){

    		if(!(--EndAfter)) return 0; //if we played enough buffers, we return 0

    		//filling the buffer with sinusoidal wave
		for(size_t i = 0; i < size; i++){
			buffer[i] = std::sin(i);
		}

        //we filled whole buffer, so we return the size of it
        return size;
	});
}
```

# Future plans
I plan to add a few more features to the library, such as:
- better cross compatibility for UNIX

# Idea behind the library
I had this idea for the library as I wanted to write a series of libraries that would make it easier to write audio applications.
This library is the first of many and serves as a lowest level layer for the other libraries.
The intended way is to release a multiple of libraries, each with different level of abstraction, each higher one using the lower level libraries
The user will not be required to use a lower level library than the level they chose.
Currently this library is for Audio playback, but I also plan to make a minimp3 wrapper, and also a wrapper for some opus decoder (I am still researching opus decoders)
Next level will be a library that will auto detect if a file is mp3 or opus and will use the appropriate decoder.
Final layer is going to be a library that will use a single class that both reads, decodes and plays the audio using all of the lower libraries.
I will edit this readme as I release other libraries
