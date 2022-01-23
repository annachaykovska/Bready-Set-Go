#pragma once

#include <AL/al.h>

#include "../Component.h"

struct AudioSource : Component
{
	ALuint source;
	glm::vec3 pos;
	float gain;
	float pitch;
	bool loop;

	AudioSource()
	{
		alGenSources(1, &source);
		gain = 1;
		pitch = 1;
		loop = false;
	}

	AudioSource(float gain, float pitch, bool loop)
	{
		alGenSources(1, &source);
		this->gain = gain;
		this->pitch = pitch;
		this->loop = loop;
		this->pos = glm::vec3(-10.0f, 0, -2.0f);
	}

	~AudioSource()
	{
		alDeleteSources(1, &source);
	}

	void updatePos(glm::vec3 pos)
	{
		alSource3f(source, AL_POSITION, pos.x, pos.y, pos.z);
	}

	// Plays an AudioClip from this source
	bool play(AudioClip* clip)
	{
		// Check if this AudioSoure is already playing a sound
		ALint value;
		alGetSourcei(source, AL_SOURCE_STATE, &value);
		if (value == AL_PLAYING)
			return false;

		// Set source attributes
		alSourcef(source, AL_PITCH, pitch);
		alSourcef(source, AL_GAIN, gain);
		alSource3f(source, AL_POSITION, pos.x, pos.y, pos.z);
		alSource3f(source, AL_VELOCITY, 0, 0, 0); // Just pitch shifting
		alSourcei(source, AL_LOOPING, loop);
		
		// Attach the buffer to a source to play
		alSourcei(source, AL_BUFFER, clip->buffer);

		// Play the audio in the buffer at the attached source
		alSourcePlay(source);

		return true;
	}

	// Stops the AudioClip currently playing from this source
	bool stop()
	{
		alSourceStop(source);

		ALint state;
		alGetSourcei(source, AL_SOURCE_STATE, &state);
		if (state == AL_STOPPED)
			return true;
		else
			return false;
	}
};