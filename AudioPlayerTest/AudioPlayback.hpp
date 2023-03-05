#pragma once
#include <functional>
#ifdef _WIN32
#include <Windows.h>
#pragma comment(lib, "winmm.lib")

#endif
namespace AP {

#ifdef _WIN32
	using WaveFormat = WAVEFORMATEX;

	
#endif

	class AudioPlayer {
	public:
		AudioPlayer(const WaveFormat& waveForm, size_t BufferSize, std::function<size_t(char*, size_t)> PrepFunc);
		void ChangeWaveFormat(const WaveFormat& waveForm);
		~AudioPlayer();
		void FinalLoop();
	protected:
		WaveFormat WaveForm = {0};
		char* BufferData[2] = { 0,0 };
		bool ActiveBuffer = 0;
		HWAVEOUT hWaveOut;
		WAVEHDR WaveHdr[2];
		size_t BufferSize = 0;
		std::function<size_t(char*, size_t)> PreparationFunction;
	};

#ifdef _WIN32

	AudioPlayer::AudioPlayer(const WaveFormat& waveForm, size_t bufferSize, std::function<size_t(char*,size_t)> PrepFunc) : BufferSize(bufferSize), WaveForm(waveForm), PreparationFunction(PrepFunc) {

		BufferData[0] = (char*)malloc(bufferSize*2);
		//BufferData[0] = (char*)calloc(bufferSize * 2, 1);
		BufferData[1] = BufferData[0] + bufferSize;

		WaveHdr[0].lpData = BufferData[0];
		WaveHdr[0].dwBufferLength = BufferSize;
		WaveHdr[1].lpData = BufferData[1];
		WaveHdr[1].dwBufferLength = BufferSize;

		waveOutOpen(&hWaveOut, WAVE_MAPPER, &WaveForm, 0, 0, 0);
	}

	void AudioPlayer::FinalLoop() {
		size_t temp = PreparationFunction(BufferData[ActiveBuffer], BufferSize);
		WaveHdr[ActiveBuffer].dwBufferLength = temp;
		waveOutPrepareHeader(hWaveOut, &WaveHdr[ActiveBuffer], sizeof(WAVEHDR));
		waveOutWrite(hWaveOut, &WaveHdr[ActiveBuffer], sizeof(WAVEHDR));
		if (!temp)return;
		while (true) {
			temp = PreparationFunction(BufferData[!ActiveBuffer], BufferSize);
			WaveHdr[!ActiveBuffer].dwBufferLength = temp;
			waveOutPrepareHeader(hWaveOut, &WaveHdr[!ActiveBuffer], sizeof(WAVEHDR));
			while (!(WaveHdr[ActiveBuffer].dwFlags & WHDR_DONE)) {
				Sleep(1);
			}
			ActiveBuffer = !ActiveBuffer;
			waveOutWrite(hWaveOut, &WaveHdr[ActiveBuffer], sizeof(WAVEHDR));
			if (!temp)return;
		}
	}

	AudioPlayer::~AudioPlayer() {
		free(BufferData[0]);
		waveOutClose(hWaveOut);
	}

	void AudioPlayer::ChangeWaveFormat(const WaveFormat& waveForm) {
		WaveForm = waveForm;
	    waveOutClose(hWaveOut);
		waveOutOpen(&hWaveOut, WAVE_MAPPER, &WaveForm, 0, 0, 0);
	}

#else

#endif
}