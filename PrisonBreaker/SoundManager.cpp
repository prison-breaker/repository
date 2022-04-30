#include "stdafx.h"
#include "SoundManager.h"

CSoundManager* CSoundManager::GetInstance()
{
	static CSoundManager Instance{};

	return &Instance;
}

void CSoundManager::Initialize()
{
	FMOD_RESULT Result{};

	Result = FMOD::System_Create(&m_System);

	if (Result != FMOD_OK)
	{
		return;
	}

	Result = m_System->init(3, FMOD_INIT_NORMAL, nullptr);

	if (Result != FMOD_OK)
	{
		return;
	}

	Result = m_System->createSound("Sounds/Beginning.mp3", FMOD_LOOP_NORMAL, 0, &m_Sounds[SOUND_TYPE_TITLE_BGM]);
	Result = m_System->createSound("Sounds/OnTheRun.mp3", FMOD_LOOP_NORMAL, 0, &m_Sounds[SOUND_TYPE_INGAME_BGM]);
	Result = m_System->createSound("Sounds/ButtonOver.wav", FMOD_LOOP_OFF, 0, &m_Sounds[SOUND_TYPE_BUTTON_OVER]);

	if (Result != FMOD_OK)
	{
		return;
	}

	Result = m_System->playSound(FMOD_CHANNEL_FREE, m_Sounds[SOUND_TYPE_TITLE_BGM], false, &m_Channels[SOUND_TYPE_TITLE_BGM]);
	
	if (Result != FMOD_OK)
	{
		return;
	}
	
	Result = m_Channels[SOUND_TYPE_TITLE_BGM]->setVolume(0.75f);

	if (Result != FMOD_OK)
	{
		return;
	}
}

void CSoundManager::Play(SOUND_TYPE SoundType, float Volume)
{
	if (SoundType < 0 || SoundType > SOUND_TYPE_BUTTON_OVER)
	{
		return;
	}

	m_System->playSound(FMOD_CHANNEL_FREE, m_Sounds[SoundType], false, &m_Channels[SoundType]);
	m_Channels[SoundType]->setVolume(Volume);
}

void CSoundManager::Stop(SOUND_TYPE SoundType)
{
	if (SoundType < 0 || SoundType > SOUND_TYPE_BUTTON_OVER)
	{
		return;
	}

	m_Channels[SoundType]->stop();
}

void CSoundManager::Pause(SOUND_TYPE SoundType)
{
	if (SoundType < 0 || SoundType > SOUND_TYPE_BUTTON_OVER)
	{
		return;
	}

	m_Channels[SoundType]->setPaused(true);
}

void CSoundManager::Resume(SOUND_TYPE SoundType)
{
	if (SoundType < 0 || SoundType > SOUND_TYPE_BUTTON_OVER)
	{
		return;
	}

	m_Channels[SoundType]->setPaused(false);
}

void CSoundManager::Update()
{
	if (m_System->update() != FMOD_OK)
	{
		return;
	}
}
