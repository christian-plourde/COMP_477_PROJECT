#pragma once
#include <string>
#include <map>
#include <SFML/Audio.hpp>

enum SoundEffects
{
	Pacman_Hit,
	Pacman_Chomp,
	Pacman_EatGhost,
	Pacman_PowerUp,
	Pacman_Intro
};

class SoundManager
{
public:

	//Make a singleton
	static SoundManager* Instance()
	{
		if (instance == 0)
		{
			instance = new SoundManager();
			return instance;
		}
		return instance;
	}

	void loadSound(SoundEffects sound, const std::string& filename)
	{
		sf::SoundBuffer buffer;
		if(!buffer.loadFromFile("sounds/" + filename))
			return;

		soundBuffers.emplace(sound, buffer);

		sf::Sound soundEffect;
		sounds.emplace(sound, soundEffect);
		sounds[sound].setBuffer(soundBuffers[sound]);
	}

	void playSound(SoundEffects sound)
	{
		sounds[sound].play();
	}

	sf::Sound& getSound(SoundEffects sound)
	{
		return sounds[sound];
	}


private:
	SoundManager() {}
	~SoundManager() {}

	static SoundManager* instance;

	std::map<SoundEffects, sf::SoundBuffer> soundBuffers;
	std::map<SoundEffects, sf::Sound> sounds;

};

typedef SoundManager SoundManager;

