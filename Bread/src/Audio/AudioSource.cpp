#include "../SystemManager.h"
#include "../Scene/Entity.h"
#include "AudioClip.h"
#include "AudioSystem.h"

extern SystemManager g_systems;

// TODO add audio streaming
// TODO add .ogg file format compatibility
// TODO add update method that updates audio source positions and velocities
// TODO add audio event queuing system?

AudioSource::AudioSource()
{	
	alGenSources(1, &source);
	this->gain = 1;
	this->pitch = 1;
	this->loop = false;
}

AudioSource::AudioSource(const AudioSource& rhs)
{
	alGenSources(1, &source);
	this->gain = rhs.gain;
	this->pitch = rhs.pitch;
	this->loop = rhs.loop;
}

AudioSource::~AudioSource()
{
	alDeleteSources(1, &source);
}

void AudioSource::setSource()
{
	alDeleteSources(1, &(this->source));
	alGenSources(1, &(this->source));
}

// Plays an AudioClip from this source
bool AudioSource::play(std::string clipName)
{
	// Get the relevant AudioClip from the audio system
	AudioClip* clip = g_systems.audio->getAudioClip(clipName);

	// Check if this AudioSoure is already playing a sound
	ALint value;

	alGetSourcei(source, AL_SOURCE_STATE, &value);
	if (value == AL_PLAYING)
		return false;

	// Get a reference to this Component's owning Entity's Transform Component
	Transform* trans = this->owner->getTransform();

	// Ensure this Component's owning Entity has a Transform Component
	float x, y, z;
	if (trans)
	{
		x = trans->position.x;
		y = trans->position.y;
		z = trans->position.z;
	}
	else
	{
		x = 0;
		y = 0;
		z = 0;
	}

	// Set source attributes
	alSourcef(source, AL_PITCH, pitch);
	alSourcef(source, AL_GAIN, gain);
	alSource3f(source, AL_POSITION, x, y, z);
	alSource3f(source, AL_VELOCITY, 0, 0, 0); // Just pitch shifting
	alSourcei(source, AL_LOOPING, loop);

	// Attach the buffer to a source to play
	alSourcei(source, AL_BUFFER, clip->buffer);

	// Play the audio in the buffer at the attached source
	alSourcePlay(source);

	return true;
}

// Stops the AudioClip currently playing from this source
bool AudioSource::stop()
{
	alSourceStop(source);

	ALint state;
	alGetSourcei(source, AL_SOURCE_STATE, &state);
	if (state == AL_STOPPED)
		return true;
	else
		return false;
}