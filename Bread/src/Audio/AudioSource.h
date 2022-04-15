#pragma once

#include <iostream>

#include <AL/al.h>

#include "../Scene/Component.h"

class AudioSource : public Component
{
public:

	float gain;
	float pitch;
	bool loop;
	ALuint source;

	AudioSource();
	~AudioSource();
	AudioSource(const AudioSource& rhs); // Copy constructor
	void operator=(const AudioSource&) =  delete; // No assignment operator

	void setSource();
	bool play(std::string clipName);
	bool isPlaying();
	bool stop();

	void update();
	void update(glm::vec3 pos);

private:

};