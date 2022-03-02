#include "stdafx.h"
#include "SceneManager.h"
#include "Scene.h"

CSceneManager* CSceneManager::GetInstance()
{
	static CSceneManager Instance{};

	return &Instance;
}

void CSceneManager::RegisterScene(const tstring& SceneName, const shared_ptr<CScene>& Scene)
{
	if (Scene)
	{
		m_Scenes.emplace(SceneName, Scene);
	}
}

void CSceneManager::ReserveChangeScene(const tstring& SceneName)
{
	if (m_Scenes.count(SceneName))
	{
		m_ReservedScene = m_Scenes[SceneName];
	}
}

void CSceneManager::ChangeScene(const tstring& SceneName)
{
	if (m_Scenes.count(SceneName))
	{
		m_CurrentScene = m_Scenes[SceneName];
	}
}

void CSceneManager::ChangeToReservedScene()
{
	if (m_ReservedScene)
	{
		m_CurrentScene = m_ReservedScene;
		m_ReservedScene = nullptr;
	}
}

void CSceneManager::ProcessMouseMessage(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	if (m_CurrentScene)
	{
		m_CurrentScene->ProcessMouseMessage(hWnd, Message, wParam, lParam);
	}
}

void CSceneManager::ProcessKeyboardMessage(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	if (m_CurrentScene)
	{
		m_CurrentScene->ProcessKeyboardMessage(hWnd, Message, wParam, lParam);
	}
}

void CSceneManager::ProcessInput(HWND hWnd, float ElapsedTime)
{
	if (m_CurrentScene)
	{
		m_CurrentScene->ProcessInput(hWnd, ElapsedTime);
	}
}

void CSceneManager::Animate(float ElapsedTime)
{
	if (m_CurrentScene)
	{
		m_CurrentScene->Animate(ElapsedTime);
	}
}

void CSceneManager::PreRender(ID3D12GraphicsCommandList* D3D12GraphicsCommandList) const
{
	if (m_CurrentScene)
	{
		m_CurrentScene->PreRender(D3D12GraphicsCommandList);
	}
}

void CSceneManager::Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList) const
{
	if (m_CurrentScene)
	{
		m_CurrentScene->Render(D3D12GraphicsCommandList);
	}
}
