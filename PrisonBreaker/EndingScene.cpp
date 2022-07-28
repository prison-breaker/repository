#include "stdafx.h"
#include "EndingScene.h"
#include "Framework.h"
#include "Player.h"
#include "State_Player.h"
#include "BilboardObjects.h"
#include "StateMachine.h"
#include "AnimationController.h"
#include "Camera.h"
#include "PostProcessingShader.h"

CEndingScene::CEndingScene(vector<vector<shared_ptr<CGameObject>>>& GameObjects, shared_ptr<CQuadObject>& SkyBox) :
	m_GameObjects{ GameObjects }
{
	// 0: Trees, 1: SkyBox
	m_QuadObjects.resize(2);
	
	// 오버 드로우를 하지 않기 위해서는 스카이박스를 제일 마지막에 그려야하기 때문에 인덱스 1번에 추가한다.
	m_QuadObjects[1] = SkyBox;
}

void CEndingScene::Initialize()
{	

}

void CEndingScene::OnCreate(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, ID3D12RootSignature* D3D12RootSignature)
{
	BuildObjects(D3D12Device, D3D12GraphicsCommandList, D3D12RootSignature);
}

void CEndingScene::OnDestroy()
{

}

void CEndingScene::BuildObjects(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, ID3D12RootSignature* D3D12RootSignature)
{
	shared_ptr<CTree> Tree{ make_shared<CTree>(D3D12Device, D3D12GraphicsCommandList) };

	m_QuadObjects[0] = Tree;
}

void CEndingScene::ReleaseObjects()
{

}

void CEndingScene::Enter(MSG_TYPE MsgType)
{
	shared_ptr<CPlayer> Player{ static_pointer_cast<CPlayer>(m_GameObjects[OBJECT_TYPE_PLAYER][CFramework::GetInstance()->GetSocketInfo().m_ID]) };

	Player->GetCamera()->GenerateViewMatrix(XMFLOAT3(0.0f, 2.0f, -20.0f), XMFLOAT3(0.0f, 0.0f, 1.0f));
	Player->GetCamera()->Rotate(3.0f, 0.0f, 0.0f);

	for (const auto& GameObject : m_GameObjects[OBJECT_TYPE_PLAYER])
	{
		if (GameObject)
		{
			Player = static_pointer_cast<CPlayer>(GameObject);
			Player->GetStateMachine()->SetCurrentState(CPlayerRunningState::GetInstance());
			Player->GetAnimationController()->SetAnimationClipType(ANIMATION_CLIP_TYPE_PLAYER_RUN_FORWARD);
		}
	}

	if (m_GameObjects[OBJECT_TYPE_TERRAIN][1])
	{
		m_GameObjects[OBJECT_TYPE_TERRAIN][1]->SetActive(true);
	}

	CSoundManager::GetInstance()->Play(SOUND_TYPE_ENDING_BGM, 1.0f);
}

void CEndingScene::Exit()
{
	if (m_GameObjects[OBJECT_TYPE_TERRAIN][1])
	{
		m_GameObjects[OBJECT_TYPE_TERRAIN][1]->SetActive(false);
	}
}

void CEndingScene::LoadSceneInfoFromFile(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, const tstring& FileName)
{

}

void CEndingScene::LoadUIInfoFromFile(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, const tstring& FileName)
{

}

void CEndingScene::CreateShaderVariables(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{

}

void CEndingScene::UpdateShaderVariables(ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{

}

void CEndingScene::ReleaseShaderVariables()
{

}

void CEndingScene::ReleaseUploadBuffers()
{
	for (UINT i = OBJECT_TYPE_PLAYER; i <= OBJECT_TYPE_STRUCTURE; ++i)
	{
		for (const auto& GameObject : m_GameObjects[i])
		{
			if (GameObject)
			{
				GameObject->ReleaseUploadBuffers();
			}
		}
	}

	for (const auto& QuadObject : m_QuadObjects)
	{
		if (QuadObject)
		{
			QuadObject->ReleaseUploadBuffers();
		}
	}
}

void CEndingScene::ProcessMouseMessage(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{

}

void CEndingScene::ProcessKeyboardMessage(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{

}

void CEndingScene::ProcessInput(HWND hWnd, float ElapsedTime)
{

}

void CEndingScene::Animate(float ElapsedTime)
{
	m_ElapsedTime += ElapsedTime;

	if (m_ElapsedTime >= m_TimeToCreditScene)
	{
		m_ElapsedTime = 0.0f;

		CSceneManager::GetInstance()->ReserveScene(TEXT("CreditScene"));
		CFramework::GetInstance()->GetPostProcessingShader()->SetPostProcessingType(POST_PROCESSING_TYPE_FADE_OUT);
	}
	else
	{
		shared_ptr<CPlayer> Player{ static_pointer_cast<CPlayer>(m_GameObjects[OBJECT_TYPE_PLAYER][CFramework::GetInstance()->GetSocketInfo().m_ID]) };

		Player->GetCamera()->Rotate(-2.5f * ElapsedTime, 0.0f, 0.0f);
	}
}

void CEndingScene::PreRender(ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{

}

void CEndingScene::Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	shared_ptr<CPlayer> Player{ static_pointer_cast<CPlayer>(m_GameObjects[OBJECT_TYPE_PLAYER][CFramework::GetInstance()->GetSocketInfo().m_ID]) };

	Player->GetCamera()->RSSetViewportsAndScissorRects(D3D12GraphicsCommandList);
	Player->GetCamera()->UpdateShaderVariables(D3D12GraphicsCommandList);

	shared_ptr<CScene> GameScene{ CSceneManager::GetInstance()->GetScene(TEXT("GameScene")) };

	GameScene->UpdateShaderVariables(D3D12GraphicsCommandList);

	for (const auto& GameObject : m_GameObjects[OBJECT_TYPE_PLAYER])
	{
		if (GameObject)
		{
			GameObject->Render(D3D12GraphicsCommandList, Player->GetCamera().get(), RENDER_TYPE_STANDARD);
		}
	}

	if (m_GameObjects[OBJECT_TYPE_TERRAIN][1])
	{
		m_GameObjects[OBJECT_TYPE_TERRAIN][1]->Render(D3D12GraphicsCommandList, Player->GetCamera().get(), RENDER_TYPE_STANDARD);
	}

	for (const auto& QuadObject : m_QuadObjects)
	{
		if (QuadObject)
		{
			QuadObject->Render(D3D12GraphicsCommandList, Player->GetCamera().get(), RENDER_TYPE_STANDARD);
		}
	}
}

void CEndingScene::PostRender(ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{

}

void CEndingScene::ProcessPacket()
{
	SOCKET_INFO SocketInfo{ CFramework::GetInstance()->GetSocketInfo() };

	if (SocketInfo.m_Socket)
	{
		MSG_TYPE MsgType{ MSG_TYPE_ENDING };
		int ReturnValue{ send(SocketInfo.m_Socket, (char*)&MsgType, sizeof(MsgType), 0) };

		if (ReturnValue == SOCKET_ERROR)
		{
			Server::ErrorDisplay("send()");
		}

		ReturnValue = recv(SocketInfo.m_Socket, (char*)&MsgType, sizeof(MsgType), MSG_WAITALL);

		if (ReturnValue == SOCKET_ERROR)
		{
			Server::ErrorDisplay("recv()");
		}
		else if (ReturnValue == 0)
		{
			tcout << "서버가 종료되었습니다." << endl;
			closesocket(SocketInfo.m_Socket);
		}
		else
		{
			switch (MsgType)
			{
			case MSG_TYPE_DISCONNECTION:
				CSceneManager::GetInstance()->ChangeScene(TEXT("TitleScene"), MsgType);
				return;
			}

			XMFLOAT4X4 TransformMatrixes[MAX_PLAYER_CAPACITY]{};

			ReturnValue = recv(SocketInfo.m_Socket, (char*)TransformMatrixes, sizeof(TransformMatrixes), MSG_WAITALL);

			if (ReturnValue == SOCKET_ERROR)
			{
				Server::ErrorDisplay("recv()");
			}
			else if (ReturnValue == 0)
			{
				tcout << "서버가 종료되었습니다." << endl;
				closesocket(SocketInfo.m_Socket);
			}
			else
			{
				for (UINT i = 0; i < MAX_PLAYER_CAPACITY; ++i)
				{
					shared_ptr<CPlayer> Player{ static_pointer_cast<CPlayer>(m_GameObjects[OBJECT_TYPE_PLAYER][i]) };

					Player->SetTransformMatrix(TransformMatrixes[i]);
					Player->UpdateTransform(Matrix4x4::Identity());
					Player->GetAnimationController()->UpdateAnimationClip(ANIMATION_TYPE_LOOP);
				}
			}
		}
	}
}
