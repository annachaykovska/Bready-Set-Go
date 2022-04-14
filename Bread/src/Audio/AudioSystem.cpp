#include "AudioSystem.h"
#include "../Scene/Scene.h"
#include "../Scene/Entity.h"
#include "../SystemManager.h"

extern Scene g_scene;
extern SystemManager g_systems;

AudioSystem::AudioSystem()
{
	// Reserve space for AudioSource Components in contiguous memory
	audioSources.reserve(g_scene.entityCount() * 2);

	// Set audio device as default
	device = alcOpenDevice(NULL); // Create device
	if (!device) std::cout << "ERROR: Default audio device not found.\n";

	// Create context
	context = alcCreateContext(device, NULL); // Create context
	if (!context) std::cout << "ERROR: Audio context could not be created.\n";

	float orientation[] = { 0, 0, 1, 0, 1, 0 };

	alListeneri(AL_DISTANCE_MODEL, AL_INVERSE_DISTANCE_CLAMPED);
	alListenerf(AL_GAIN, 1.0f);
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
	load("idle.wav");
	load("le_festin.wav");
	load("slurp.wav");
	load("fart.wav");
	load("suck.wav");
	load("power.wav");
	load("banner_popup.wav");
	load("knife.wav");
	load("chop.wav");
	load("lose.wav");
	load("carioca.wav");
	load("menu.wav");
	load("wrong.wav");
	load("drum.wav");
	//load("thumpdull.wav");
	//load("thump2.wav");

	// Create AudioSource Components
	AudioSource* menuAudio = createAudioSource(); // keep this first
	AudioSource* countertopAudioSource = createAudioSource();
	AudioSource* p1AudioSource = createAudioSource();
	AudioSource* bgMusic = createAudioSource();
	AudioSource* p1Engine = createAudioSource();
	AudioSource* p1Vacuum = createAudioSource();
	AudioSource* p2AudioSource = createAudioSource();
	AudioSource* p2Engine = createAudioSource();
	AudioSource* p2Vacuum = createAudioSource();
	AudioSource* p3AudioSource = createAudioSource();
	AudioSource* p3Engine = createAudioSource();
	AudioSource* p3Vacuum = createAudioSource();
	AudioSource* p4AudioSource = createAudioSource();
	AudioSource* p4Engine = createAudioSource();
	AudioSource* p4Vacuum = createAudioSource();

	this->menuAudio = menuAudio;

	// Attach AudioSource Components to Entities
	Entity* countertop = g_scene.getEntity("countertop");
	countertop->attachComponent(countertopAudioSource, "audio");
	Entity* player1 = g_scene.getEntity("player1");
	player1->attachComponent(p1AudioSource, "audio");
	player1->attachComponent(bgMusic, "bg");
	player1->attachComponent(menuAudio, "menuAudio");
	player1->attachComponent(p1Engine, "engineAudio");
	player1->attachComponent(p1Vacuum, "vacuumAudio");
	Entity* player2 = g_scene.getEntity("player2");
	player2->attachComponent(p2AudioSource, "audio");
	player2->attachComponent(p2Engine, "engineAudio");
	player2->attachComponent(p2Vacuum, "vacuumAudio");
	Entity* player3 = g_scene.getEntity("player3");
	player3->attachComponent(p3AudioSource, "audio");
	player3->attachComponent(p3Engine, "engineAudio");
	player3->attachComponent(p3Vacuum, "vacuumAudio");
	Entity* player4 = g_scene.getEntity("player4");
	player4->attachComponent(p4AudioSource, "audio");
	player4->attachComponent(p4Engine, "engineAudio");
	player4->attachComponent(p4Vacuum, "vacuumAudio");

	bgMusic->loop = true;
	bgMusic->gain = 0.05f;
	bgMusic->pitch = 1.0f;

	p1Engine->loop = true;
	p1Engine->gain = 0.7f;
	p1Vacuum->gain = 0.5f;

	p2Engine->loop = true;
	p3Engine->loop = true;
	p4Engine->loop = true;

	this->accumulator = 0.0f;
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

// Updates audio sources each frame as necessary (i.e. as players move)
void AudioSystem::update(const float dt, int gameStage)
{
	AudioSource* bgMusic = (AudioSource*)g_scene.getEntity("player1")->getComponent("bg");
	if (gameStage != 5) {
		bgMusic->gain = 0.2;
		bgMusic->play("carioca.wav");
	}
	else {
		bgMusic->gain = 0.5;
		bgMusic->play("le_festin.wav");
	}

	// Update listener position to player1's new position
	Transform* trans = g_scene.getEntity("player1")->getTransform();
	glm::vec3 pos = trans->position;
	alListener3f(AL_POSITION, pos.x, pos.y, pos.z);
	glm::vec3 heading = glm::normalize(g_scene.getEntity("player1")->orientation);
	float orientation[] = { -heading.x, -heading.y, -heading.z, 0, 1, 0 };
	alListenerfv(AL_ORIENTATION, orientation);
	glm::vec3 velocity = heading * g_scene.getEntity("player1")->speed;
	alListener3f(AL_VELOCITY, velocity.x, velocity.y, velocity.z);

	// Update each player's engine audio location
	AudioSource* p1Engine = (AudioSource*)g_scene.getEntity("player1")->getComponent("engineAudio");
	p1Engine->update();
	AudioSource* p2Engine = (AudioSource*)g_scene.getEntity("player2")->getComponent("engineAudio");
	p2Engine->update();
	AudioSource* p3Engine = (AudioSource*)g_scene.getEntity("player3")->getComponent("engineAudio");
	p3Engine->update();
	AudioSource* p4Engine = (AudioSource*)g_scene.getEntity("player4")->getComponent("engineAudio");
	p4Engine->update();

	// Change pitch of engine noises wrt player speed
	if (accumulator > 0.25f)
	{
		p1Engine->pitch = glm::abs(g_systems.physics->getPlayerSpeed(1) / 60.0f) + 1.0f;
		p1Engine->stop();
		p1Engine->play("idle.wav");

		p2Engine->pitch = glm::abs(g_systems.physics->getPlayerSpeed(2) / 60.0f) + 1.0f;
		p2Engine->stop();
		p2Engine->play("idle.wav");

		p3Engine->pitch = glm::abs(g_systems.physics->getPlayerSpeed(3) / 60.0f) + 1.0f;
		p3Engine->stop();
		p3Engine->play("idle.wav");

		p4Engine->pitch = glm::abs(g_systems.physics->getPlayerSpeed(4) / 60.0f) + 1.0f;
		p4Engine->stop();
		p4Engine->play("idle.wav");

		accumulator = 0.0f;
	}

	accumulator += dt;
}

void AudioSystem::playGameMusic(AudioSource* source) {
	source->gain = 0.1f; // Volume control
	source->loop = true;
	source->play("carioca.wav"); 
}

void AudioSystem::playMainMenuMusic(AudioSource* source) {
	source->gain = 0.6f; // Volume control
	source->loop = true;
	source->play("menu.wav");
}

void AudioSystem::stopMusic(AudioSource* source) {
	source->stop();
}

void AudioSystem::playSlurp(AudioSource* source) {
	source->gain = 1.f; // Volume control
	//source->stop(); // NOTE: not sure if needed but i tried it to make it play consistently
	source->loop = true;
	source->play("slurp.wav"); // Comment this out to turn off the music on load
}

void AudioSystem::endSlurp(AudioSource* source, bool playBanner, bool success) {
	source->gain = 1.f; // Volume control
	source->stop();
	source->loop = false;
	if (success) {
		if (playBanner) {
			source->play("banner_popup.wav");
		}
		else {
			source->play("ding.wav");
		}
	}
	else {
		source->play("fart.wav");
	}
}

void AudioSystem::ingredientSuck(AudioSource* source)
{
	source->stop();
	source->loop = false;
	source->play("suck.wav");
}

void AudioSystem::powerReady(AudioSource* source)
{
	source->gain = .5f; // Volume control
	source->stop();
	source->loop = false;
	source->play("power.wav");
}

void AudioSystem::turnOffAllAudio() {
	// Starts from 1 to avoid the menu noises
	for (int i = 1; i < audioSources.size(); i++) {
		audioSources[i].stop();
	}
}

void AudioSystem::playMenuAudio(std::string clipName)
{
	this->menuAudio->stop();
	this->menuAudio->play(clipName);
}
