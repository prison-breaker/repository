#pragma once
#include "fmod.hpp"

class CSoundManager
{
private:
	FMOD::System*  m_System{};

	FMOD::Channel* m_Channels[MAX_BGM_SOUNDS + MAX_SFX_SOUNDS]{};
	FMOD::Sound*   m_Sounds[MAX_BGM_SOUNDS + MAX_SFX_SOUNDS]{};

private:
	CSoundManager() = default;
	~CSoundManager();

public:
	static CSoundManager* GetInstance();

	void Initialize();

	void Play(SOUND_TYPE SoundType, float Volume);
	void Stop(SOUND_TYPE SoundType);
	void Pause(SOUND_TYPE SoundType);
	void Resume(SOUND_TYPE SoundType);

	void Update();
};
