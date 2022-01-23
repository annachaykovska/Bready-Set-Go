#pragma once

#include <string>

#include <AL/al.h>

struct AudioClip
{
	ALuint buffer;
	ALenum format;
	uint32_t frequency;
	std::string fileName;

	AudioClip(std::string fileName)
	{
		this->fileName = fileName;
	}

	AudioClip(ALenum format, uint32_t frequency, std::string fileName)
	{
		this->buffer = NULL;
		this->format = format;
		this->frequency = frequency;
		this->fileName = fileName;
	}

	~AudioClip()
	{
		alDeleteBuffers(1, &buffer);
	}
};