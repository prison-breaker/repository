#pragma once
#include "fmod.hpp"

class CSoundManager
{
private:
	FMOD::System*  m_System{};

	FMOD::Channel* m_Channels[10]{};
	FMOD::Sound*   m_Sounds[10]{};

private:
	CSoundManager() = default;
	~CSoundManager();

public:
	static CSoundManager* GetInstance();

	void Initialize();

	void Play(SOUND_TYPE SoundType, float Volume = 1.0f);
	void Stop(SOUND_TYPE SoundType);
	void Pause(SOUND_TYPE SoundType);
	void Resume(SOUND_TYPE SoundType);

	void Update();
};
