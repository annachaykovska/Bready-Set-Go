#pragma once

#include <iostream>
#include <vector>
#include <cassert>
#include <string>
#include <fstream>
#include <stdio.h>
#include <inttypes.h>

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/AudioFile.h>
#include <glm/glm.hpp>

#include "AudioSource.h"
#include "AudioClip.h"

class AudioSystem
{
public:

	AudioSystem();
	~AudioSystem();

	AudioSystem(AudioSystem& rhs) = delete; // No copy constructor
	void operator=(const AudioSystem&) = delete; // No assignment operator

	AudioClip* getAudioClip(std::string filePath);
	bool load(std::string filePath);
	AudioSource* createAudioSource();

	void playGameMusic(AudioSource* source);
	void playMainMenuMusic(AudioSource* source);
	void stopMusic(AudioSource* source);
	void playSlurp(AudioSource* source);
	void endSlurp(AudioSource* source, bool playBanner, bool success);
	void ingredientSuck(AudioSource* source);
	void vacuumReady(AudioSource* source);
	void turnOffAllAudio();
	void playMenuAudio(std::string clipName);

	void update(const float dt, int gameStage);

private:

	std::unordered_map<std::string, AudioClip> audioClips;
	std::vector<AudioSource> audioSources;

	AudioSource* menuAudio;

	ALCdevice* device;
	ALCcontext* context;
	std::string dir;

	float accumulator;

	struct  WavHeader
	{
		// RIFF chunk
		uint8_t         RIFF[4];        // RIFF header
		uint32_t        fileSize;		// Total file size (bytes)
		uint8_t         WAVE[4];        // WAVE header
		// FMT chunk
		uint8_t         FMT[4];         // FMT header
		uint32_t        fmtSize;		// Size of the fmt chunk (bytes)
		uint16_t        format;			// Audio format (1=PCM)
		uint16_t        channels;		// Number of channels (1=Mono, 2=Stereo)
		uint32_t        frequency;		// Sampling frequency in Hz
		uint32_t        bytesPerSecond; // Bytes per second
		uint16_t        blockAlign;     // 1=8-bit mono, 2=16-bit mono, 3=8-bit stereo, 4=16-bit stereo
		uint16_t        bitDepth;		// Number of bits in each sample
		// Data chunk
		uint8_t         DATA[4];		// DATA header
		uint32_t        dataSize;		// Size of the data chunk (bytes)

		void summary()
		{
			printf("RIFF: %c%c%c%c\n", RIFF[0], RIFF[1], RIFF[2], RIFF[3]);
			printf("File size: %" PRIu32 "\n", fileSize);
			printf("WAVE: %c%c%c%c\n", WAVE[0], WAVE[1], WAVE[2], WAVE[3]);
			printf("FMT: %c%c%c%c\n", FMT[0], FMT[1], FMT[2], FMT[3]);
			printf("Format: %" PRIu32 "\n", fmtSize);
			printf("Audio format: %" PRIu16 "\n", format);
			printf("Channels: %" PRIu16 "\n", channels);
			printf("Frequency: %" PRIu32 " Hz\n", frequency);
			printf("Bytes per Second: %" PRIu32 "\n", bytesPerSecond);
			printf("Block Alignment: %" PRIu16 "\n", blockAlign);
			printf("Bit depth: %" PRIu16 "\n", bitDepth);
			printf("DATA: %c%c%c%c\n", DATA[0], DATA[1], DATA[2], DATA[3]);
			printf("Data size: %" PRIu32 "\n", dataSize);
		}
	};
};