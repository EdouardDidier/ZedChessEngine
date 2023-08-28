#include "Audio.h"

map<int, string> Audio::mIndexToName = {
	{Audio::gameStart, "game-start.wav"},
	{Audio::gameEnd, "game-end.wav"},
	{Audio::moveSelf, "move-self.wav"},
	{Audio::moveOpponent, "move-opponent.wav"},
	{Audio::moveCheck, "move-check.wav"},
	{Audio::castle, "castle.wav"},
	{Audio::promote, "promote.wav"},
	{Audio::capture, "capture.wav"},
	{Audio::tenSecondsAlert, "tenseconds.wav"}
};

Audio::Audio() {
	mpSounds = new Mix_Chunk * [SOUNDS_SIZE];
}

Audio::~Audio() {
	for (int i = 0; i < SOUNDS_SIZE; i++) {
		Mix_FreeChunk(mpSounds[i]);
	}

	delete mpSounds;
}

bool Audio::init() {	
	// Initialize SDL audio. If failed, end program.
	if (SDL_Init(SDL_INIT_AUDIO) < 0) {
		cout << "Error initializing SDL: " << SDL_GetError() << endl;
		system("pause");
		return false;
	}

	// Initialize SDL_mixer with our audio format
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) < 0) {
		cout << "Error initializing SDL_mixer: " << Mix_GetError() << endl;
		return false;
	}

	if (!this->loadSounds())
		return false;
  
	return true;
}

void Audio::kill() {

}

void Audio::playSound(int soundIndex) {
	Mix_PlayChannel(-1, mpSounds[soundIndex], 0);
}

bool Audio::loadSounds() {
	string path = PATH_SOUNDS;

	for (int i = 0; i < SOUNDS_SIZE; i++) {
		mpSounds[i] = Mix_LoadWAV((path + mIndexToName[i]).data());

		// Check load
		if (!mpSounds[i]) {
			cout << "Failed to load music or sound: " << Mix_GetError() << endl;
			system("pause");
			return false;
		}
	}

	return true;
}
