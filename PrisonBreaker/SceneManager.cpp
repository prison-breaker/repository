#include "pch.h"
#include "SceneManager.h"

#include "TitleScene.h"
#include "GameScene.h"

CSceneManager::CSceneManager() :
	m_scenes(),
	m_currentScene()
{
}

CSceneManager::~CSceneManager()
{
	Utility::SafeDelete(m_scenes);
}

void CSceneManager::ChangeScene(SCENE_TYPE sceneType)
{
	m_currentScene->Exit();
	m_currentScene = m_scenes[(int)sceneType];
	m_currentScene->Enter();
}

CScene* CSceneManager::GetCurrentScene()
{
	return m_currentScene;
}

void CSceneManager::Init(ID3D12Device* d3d12Device, ID3D12GraphicsCommandList* d3d12GraphicsCommandList)
{
	// ¾À »ý¼º
	m_scenes.resize((int)SCENE_TYPE::COUNT);

	//m_scenes[(int)SCENE_TYPE::TITLE] = new CTitleScene();
	//m_scenes[(int)SCENE_TYPE::TITLE]->Init(d3d12Device, d3d12GraphicsCommandList, D3D12RootSignature);

	m_scenes[(int)SCENE_TYPE::GAME] = new CGameScene();
	m_scenes[(int)SCENE_TYPE::GAME]->Init(d3d12Device, d3d12GraphicsCommandList);

	// ÇöÀç ¾À ¼³Á¤
	m_currentScene = m_scenes[(int)SCENE_TYPE::GAME];
	m_currentScene->Enter();
}

void CSceneManager::ReleaseUploadBuffers()
{
	for (int i = 0; i < (int)SCENE_TYPE::COUNT; ++i)
	{
		if (m_scenes[i] != nullptr)
		{
			m_scenes[i]->ReleaseUploadBuffers();
		}
	}
}

void CSceneManager::Update()
{
	m_currentScene->Update();
}

void CSceneManager::PreRender(ID3D12GraphicsCommandList* d3d12GraphicsCommandList)
{
	m_currentScene->PreRender(d3d12GraphicsCommandList);
}

void CSceneManager::Render(ID3D12GraphicsCommandList* d3d12GraphicsCommandList)
{
	m_currentScene->Render(d3d12GraphicsCommandList);
}

void CSceneManager::PostRender(ID3D12GraphicsCommandList* d3d12GraphicsCommandList)
{
	m_currentScene->PostRender(d3d12GraphicsCommandList);
}

//void CSceneManager::ProcessPacket()
//{
//	m_currentScene->ProcessPacket();
//}
