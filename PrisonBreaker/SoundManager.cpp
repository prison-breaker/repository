#include "stdafx.h"
#include "SoundManager.h"

CSoundManager::~CSoundManager()
{
	if (m_System)
	{
		m_System->release();
	}
}

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

	Result = m_System->init(MAX_BGM_SOUNDS + MAX_SFX_SOUNDS, FMOD_INIT_NORMAL, nullptr);

	if (Result != FMOD_OK)
	{
		return;
	}

	// BGM
	Result = m_System->createSound("Sounds/01. Beginning.mp3", FMOD_LOOP_NORMAL, nullptr, &m_Sounds[SOUND_TYPE_TITLE_BGM]);
	Result = m_System->createSound("Sounds/04. On The Run.mp3", FMOD_LOOP_NORMAL, nullptr, &m_Sounds[SOUND_TYPE_INGAME_BGM_1]);
	Result = m_System->createSound("Sounds/05. Getaway.mp3", FMOD_LOOP_NORMAL, nullptr, &m_Sounds[SOUND_TYPE_INGAME_BGM_2]);

	// SFX
	Result = m_System->createSound("Sounds/ButtonOver.wav", FMOD_LOOP_OFF, nullptr, &m_Sounds[SOUND_TYPE_BUTTON_OVER]);
	Result = m_System->createSound("Sounds/OpenDoor.wav", FMOD_LOOP_OFF, nullptr, &m_Sounds[SOUND_TYPE_OPEN_DOOR]);
	Result = m_System->createSound("Sounds/OpenElectricalPanel.wav", FMOD_LOOP_OFF, nullptr, &m_Sounds[SOUND_TYPE_OPEN_EP]);
	Result = m_System->createSound("Sounds/OpenMetalDoor.wav", FMOD_LOOP_OFF, nullptr, &m_Sounds[SOUND_TYPE_OPEN_GATE]);
	Result = m_System->createSound("Sounds/PowerDown.wav", FMOD_LOOP_OFF, nullptr, &m_Sounds[SOUND_TYPE_POWER_DOWN]);
	Result = m_System->createSound("Sounds/Breath.wav", FMOD_LOOP_OFF, nullptr, &m_Sounds[SOUND_TYPE_BREATH]);
	Result = m_System->createSound("Sounds/Grunt_1.wav", FMOD_LOOP_OFF, nullptr, &m_Sounds[SOUND_TYPE_GRUNT_1]);
	Result = m_System->createSound("Sounds/Grunt_2.wav", FMOD_LOOP_OFF, nullptr, &m_Sounds[SOUND_TYPE_GRUNT_2]);
	Result = m_System->createSound("Sounds/PistolShot.wav", FMOD_LOOP_OFF, nullptr, &m_Sounds[SOUND_TYPE_PISTOL_SHOT]);
	Result = m_System->createSound("Sounds/PistolDryFire.wav", FMOD_LOOP_OFF, nullptr, &m_Sounds[SOUND_TYPE_PISTOL_EMPTY]);
	Result = m_System->createSound("Sounds/EvacuationSiren.wav", FMOD_LOOP_NORMAL, nullptr, &m_Sounds[SOUND_TYPE_SIREN]);

	if (Result != FMOD_OK)
	{
		return;
	}

	Result = m_System->playSound(FMOD_CHANNEL_FREE, m_Sounds[SOUND_TYPE_TITLE_BGM], false, &m_Channels[SOUND_TYPE_TITLE_BGM]);
	
	if (Result != FMOD_OK)
	{
		return;
	}
	
	Result = m_Channels[SOUND_TYPE_TITLE_BGM]->setVolume(0.65f);

	if (Result != FMOD_OK)
	{
		return;
	}
}

void CSoundManager::Play(SOUND_TYPE SoundType, float Volume)
{
	if (SoundType < 0)
	{
		return;
	}

	bool IsPlaying{};

	m_Channels[SoundType]->isPlaying(&IsPlaying);

	if (!IsPlaying)
	{
		m_System->playSound(FMOD_CHANNEL_FREE, m_Sounds[SoundType], false, &m_Channels[SoundType]);
		m_Channels[SoundType]->setVolume(Volume);
	}
}

void CSoundManager::Stop(SOUND_TYPE SoundType)
{
	if (SoundType < 0)
	{
		return;
	}

	m_Channels[SoundType]->stop();
}

void CSoundManager::Pause(SOUND_TYPE SoundType)
{
	if (SoundType < 0)
	{
		return;
	}

	m_Channels[SoundType]->setPaused(true);
}

void CSoundManager::Resume(SOUND_TYPE SoundType)
{
	if (SoundType < 0)
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
