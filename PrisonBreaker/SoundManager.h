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
	virtual ~CSoundManager();

public:
	static CSoundManager* GetInstance();

	void Initialize();

	bool IsPlaying(SOUND_TYPE SoundType);

	void Play(SOUND_TYPE SoundType, float Volume, bool Overlap);
	void Stop(SOUND_TYPE SoundType);
	void Pause(SOUND_TYPE SoundType);
	void Resume(SOUND_TYPE SoundType);

	void Update();
};
