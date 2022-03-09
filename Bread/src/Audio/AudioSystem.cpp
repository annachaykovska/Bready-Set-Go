#include "AudioSystem.h"
#include "../Scene/Scene.h"
#include "../Scene/Entity.h"

extern Scene g_scene;

AudioSystem::AudioSystem()
{
	// Reserve space for AudioSource Components in contiguous memory
	audioSources.reserve(g_scene.count());

	// Set audio device as default
	device = alcOpenDevice(NULL); // Create device
	if (!device) std::cout << "ERROR: Default audio device not found.\n";

	// Create context
	context = alcCreateContext(device, NULL); // Create context
	if (!context) std::cout << "ERROR: Audio context could not be created.\n";

	float orientation[] = { 0, 0, 1, 0, 1, 0 };

	alListeneri(AL_DISTANCE_MODEL, AL_INVERSE_DISTANCE_CLAMPED);
	alListenerf(AL_GAIN, 0.5f);
	alListener3f(AL_POSITION, 0, 0, 0);
	alListener3f(AL_VELOCITY, 0, 0, 0);
	alListenerfv(AL_ORIENTATION, orientation);

	// Set the new context as current
	if (!alcMakeContextCurrent(context))
		std::cout << "ERROR: Audio context could not be made current.\n";

	// Set directory
	this->dir = "resources/audio/";

	// Load audio library
	load("bg.wav");
	load("horn.wav");
	load("bread.wav");
	load("tone.wav");
	load("ding.wav");
	load("pickup.wav");
	load("thump.wav");
	//load("thumpdull.wav");
	//load("thump2.wav");

	// Create AudioSource Components
	AudioSource* countertopAudioSource = createAudioSource();
	AudioSource* p1AudioSource = createAudioSource();

	// Attach AudioSource Components to Entities
	Entity* countertop = g_scene.getEntity("countertop");
	countertop->attachComponent(countertopAudioSource, "audio");

	Entity* player1 = g_scene.getEntity("player1");
	player1->attachComponent(p1AudioSource, "audio");
}

AudioSystem::~AudioSystem()
{
	for (auto& it : audioSources)
	{
		it.stop();
		alDeleteSources(1, &(it.source));
	}

	for (auto& it : audioClips)
		alDeleteBuffers(1, &(it.second.buffer));

	context = alcGetCurrentContext();
	device = alcGetContextsDevice(context);
	alcMakeContextCurrent(NULL);
	alcDestroyContext(context);
	alcCloseDevice(device);
}

// Returns the AudioClip with the specified file name
AudioClip* AudioSystem::getAudioClip(std::string fileName)
{
	std::string filePath = dir + fileName;

	auto result = audioClips.find(filePath);

	if (result != audioClips.end())
		return &(result->second);
	else
		return nullptr;
}

bool AudioSystem::load(std::string fileName)
{
	std::string filePath = dir + fileName;

	// Load wav file from disk
	std::ifstream file(filePath, std::ios::binary);
	if (file.bad())
	{
		std::cout << "Bad audio file path: " << filePath.c_str() << std::endl;
		return false;
	}

	// Read wav file header
	WavHeader header;
	file.read((char*)&header, sizeof(header));

	// Loop until the data header is found
	while (header.DATA[0] != 'D' && header.DATA[0] != 'd')
	{
		// Erase old data
		char* buf = new char[header.dataSize];
		file.read(&buf[0], header.dataSize);
		delete[](buf);

		// Read new data
		char buffer[4];
		file.read(buffer, 4);
		header.DATA[0] = buffer[0];
		header.DATA[1] = buffer[1];
		header.DATA[2] = buffer[2];
		header.DATA[3] = buffer[3];
		file.read(buffer, 4);

		// Copy the data to header
		std::int32_t temp = 0;
		std::memcpy(&temp, buffer, 4);
		header.dataSize = temp;

		// Test for end of file to prevent infinite loop
		if (file.eof())
			return false;
	}

	// Print the wav header data for debugging
	//std::cout << "Filename: " << fileName << std::endl;
	//header.summary();

	// Copy relevant header data to AudioClip
	uint32_t frequency = header.frequency;

	// Read wav file data
	char* data = new char[header.dataSize];
	file.read(&data[0], header.dataSize);

	// Determine audio format from header
	ALenum format;
	if (header.channels == 1 && header.bitDepth == 8)
		format = AL_FORMAT_MONO8;
	else if (header.channels == 1 && header.bitDepth == 16)
		format = AL_FORMAT_MONO16;
	else if (header.channels == 2 && header.bitDepth == 8)
		format = AL_FORMAT_STEREO8;
	else if (header.channels == 2 && header.bitDepth == 16)
		format = AL_FORMAT_STEREO16;
	else
	{
		std::cout << "ERROR: unreadable wave format: "
				  << header.channels << " channels, "
				  << header.bitDepth << " bps.\n";
		return false;
	}

	// Add new AudioClip to masterlist
	audioClips.emplace(std::make_pair(std::string(filePath), AudioClip(format, frequency, filePath)));

	// Set AudioClip data
	alGenBuffers(1, &(*audioClips.find(filePath)).second.buffer);

	// Copy wav data into audio buffer
	alBufferData((*audioClips.find(filePath)).second.buffer, format, data, header.dataSize, header.frequency);

	// Unload wav from RAM
	delete[](data);

	return true;
}

/// <summary>
/// Creates a new AudioSource Component in the Audio System's vector.
/// </summary>
/// <returns>
/// A reference to the new AudioSource Component if successful, nullptr otherwise.
/// </returns>
AudioSource* AudioSystem::createAudioSource()
{
	// Used to see if the new AudioSource is added successfully
	int size = audioSources.size();

	// Add the new AudioSource to the container of AudioSource Components
	audioSources.emplace_back(AudioSource());

	// Check if size got bigger, if it did return a ref to the new AudioSource
	if (size < audioSources.size())
		return &audioSources.back();
	else
		return nullptr;
}