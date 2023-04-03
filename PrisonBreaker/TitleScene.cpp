#include "pch.h"
#include "TitleScene.h"

CTitleScene::CTitleScene()
{
}

CTitleScene::~CTitleScene()
{
}

void CTitleScene::Enter()
{
	//switch (MsgType)
	//{
	//case MSG_TYPE_DISCONNECTION:
	//	m_QuadObjects[5]->SetActive(true);
	//	break;
	//}

	//if (MsgType != MSG_TYPE_NONE)
	//{
	//	ShowCursor(TRUE);

	//	CCore::GetInstance()->DisconnectServer();
	//}

	//CSoundManager::GetInstance()->Stop(SOUND_TYPE_ENDING_BGM);
	//CSoundManager::GetInstance()->Play(SOUND_TYPE_TITLE_BGM, 0.3f, false);
}

void CTitleScene::Exit()
{
	//UINT BilboardObjectCount{ static_cast<UINT>(m_QuadObjects.size()) };

	//for (UINT i = 3; i < BilboardObjectCount; ++i)
	//{
	//	if (m_QuadObjects[i])
	//	{
	//		m_QuadObjects[i]->SetActive(false);
	//	}
	//}

	//CSoundManager::GetInstance()->Stop(SOUND_TYPE_TITLE_BGM);
}

void CTitleScene::Init(ID3D12Device* d3d12Device, ID3D12GraphicsCommandList* d3d12GraphicsCommandList)
{
	//// 렌더링에 필요한 셰이더 객체(PSO)를 생성한다.
	//shared_ptr<CGraphicsShader> Shader{ make_shared<CQuadShader>() };

	//Shader->CreatePipelineStates(d3d12Device, D3D12RootSignature, 3);
	//CShaderManager::GetInstance()->RegisterShader(TEXT("QuadShader"), Shader);

	//// 파일로부터 UI 객체들을 생성하고 배치한다.
	//LoadUIInfoFromFile(d3d12Device, d3d12GraphicsCommandList, TEXT("Scenes/TitleScene_UI.bin"));
}

//void CTitleScene::ProcessPacket()
//{
//	SOCKET_INFO SocketInfo{ CCore::GetInstance()->GetSocketInfo() };
//
//	if (SocketInfo.m_Socket)
//	{
//		MSG_TYPE MsgType{ MSG_TYPE_TITLE };
//		int ReturnValue{ send(SocketInfo.m_Socket, (char*)&MsgType, sizeof(MsgType), 0) };
//
//		if (ReturnValue == SOCKET_ERROR)
//		{
//			Server::ErrorDisplay("send()");
//		}
//
//		ReturnValue = recv(SocketInfo.m_Socket, (char*)&MsgType, sizeof(MsgType), MSG_WAITALL);
//
//		if (ReturnValue == SOCKET_ERROR)
//		{
//			Server::ErrorDisplay("recv()");
//		}
//		else if (ReturnValue == 0)
//		{
//			tcout << "서버가 종료되었습니다." << endl;
//			closesocket(SocketInfo.m_Socket);
//		}
//		else
//		{
//			if (MsgType == MSG_TYPE_INGAME)
//			{
//				CSceneManager::GetInstance()->ReserveScene(TEXT("GameScene"));
//				CCore::GetInstance()->GetPostProcessingShader()->SetPostProcessingType(POST_PROCESSING_TYPE_FADE_OUT);
//			}
//		}
//	}
//}
