#include "stdafx.h"
#include "SceneManager.h"
#include "Scene.h"

CSceneManager* CSceneManager::GetInstance()
{
	static CSceneManager Instance{};

	return &Instance;
}

void CSceneManager::ReleaseUploadBuffers()
{
	for (const auto& Scene : m_Scenes)
	{
		Scene.second->ReleaseUploadBuffers();
	}
}

shared_ptr<CScene> CSceneManager::GetScene(const tstring& SceneName)
{
	if (m_Scenes.count(SceneName))
	{
		return m_Scenes[SceneName];
	}

	return nullptr;
}

void CSceneManager::SetCurrentScene(const tstring& SceneName)
{
	if (m_Scenes.count(SceneName))
	{
		m_CurrentScene = m_Scenes[SceneName];
		m_CurrentScene->Enter(MSG_TYPE_NONE);
	}
}

shared_ptr<CScene> CSceneManager::GetCurrentScene() const
{
	return m_CurrentScene;
}

shared_ptr<CScene> CSceneManager::GetReservedScene() const
{
	return m_ReservedScene;
}

void CSceneManager::RegisterScene(const tstring& SceneName, const shared_ptr<CScene>& Scene)
{
	if (Scene)
	{
		m_Scenes.emplace(SceneName, Scene);
	}
}

void CSceneManager::ReserveScene(const tstring& SceneName, MSG_TYPE MsgType)
{
	if (!m_ReservedScene)
	{
		if (m_Scenes.count(SceneName))
		{
			m_ReservedScene = m_Scenes[SceneName];
			m_ReservedMsgType = MsgType;
		}
	}
}

void CSceneManager::ChangeScene(const tstring& SceneName, MSG_TYPE MsgType)
{
	if (m_Scenes.count(SceneName))
	{
		m_CurrentScene->Exit();
		m_CurrentScene = m_Scenes[SceneName];
		m_CurrentScene->Enter(MsgType);
	}
}

void CSceneManager::ChangeToReservedScene()
{
	if (m_ReservedScene)
	{
		m_CurrentScene->Exit();
		m_CurrentScene = m_ReservedScene;
		m_CurrentScene->Enter(m_ReservedMsgType);

		m_ReservedScene = nullptr;
		m_ReservedMsgType = MSG_TYPE_NONE;
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

void CSceneManager::PreRender(ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	if (m_CurrentScene)
	{
		m_CurrentScene->PreRender(D3D12GraphicsCommandList);
	}
}

void CSceneManager::Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	if (m_CurrentScene)
	{
		m_CurrentScene->Render(D3D12GraphicsCommandList);
	}
}

void CSceneManager::PostRender(ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	if (m_CurrentScene)
	{
		m_CurrentScene->PostRender(D3D12GraphicsCommandList);
	}
}

void CSceneManager::ProcessPacket()
{
	if (m_CurrentScene)
	{
		m_CurrentScene->ProcessPacket();
	}
}

