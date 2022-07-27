#include "stdafx.h"
#include "TitleScene.h"
#include "Framework.h"
#include "QuadObject.h"
#include "QuadShader.h"
#include "PostProcessingShader.h"
#include "UILayer.h"

void CTitleScene::Initialize()
{

}

void CTitleScene::OnCreate(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, ID3D12RootSignature* D3D12RootSignature)
{
	BuildObjects(D3D12Device, D3D12GraphicsCommandList, D3D12RootSignature);
}

void CTitleScene::OnDestroy()
{

}

void CTitleScene::BuildObjects(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, ID3D12RootSignature* D3D12RootSignature)
{
	// �������� �ʿ��� ���̴� ��ü(PSO)�� �����Ѵ�.
	shared_ptr<CGraphicsShader> Shader{ make_shared<CQuadShader>() };

	Shader->CreatePipelineStates(D3D12Device, D3D12RootSignature, 3);
	CShaderManager::GetInstance()->RegisterShader(TEXT("QuadShader"), Shader);

	// ���Ϸκ��� UI ��ü���� �����ϰ� ��ġ�Ѵ�.
	LoadUIInfoFromFile(D3D12Device, D3D12GraphicsCommandList, TEXT("Scenes/TitleScene_UI.bin"));
}

void CTitleScene::ReleaseObjects()
{

}

void CTitleScene::Enter(MSG_TYPE MsgType)
{
	switch (MsgType)
	{
	case MSG_TYPE_DISCONNECTION:
		m_QuadObjects[5]->SetActive(true);
		break;
	}

	if (MsgType != MSG_TYPE_NONE)
	{
		ShowCursor(TRUE);

		CFramework::GetInstance()->DisconnectServer();
	}

	CSoundManager::GetInstance()->Stop(SOUND_TYPE_ENDING_BGM);
	CSoundManager::GetInstance()->Play(SOUND_TYPE_TITLE_BGM, 0.3f);
}

void CTitleScene::Exit()
{
	UINT BilboardObjectCount{ static_cast<UINT>(m_QuadObjects.size()) };

	for (UINT i = 3; i < BilboardObjectCount; ++i)
	{
		if (m_QuadObjects[i])
		{
			m_QuadObjects[i]->SetActive(false);
		}
	}

	CSoundManager::GetInstance()->Stop(SOUND_TYPE_TITLE_BGM);
}

void CTitleScene::LoadSceneInfoFromFile(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, const tstring& FileName)
{

}

void CTitleScene::LoadUIInfoFromFile(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, const tstring& FileName)
{
	tstring Token{};

	unordered_map<tstring, shared_ptr<CMaterial>> MaterialCaches{};
	shared_ptr<CQuadObject> Object{};

	tcout << FileName << TEXT(" �ε� ����...") << endl;

	tifstream InFile{ FileName, ios::binary };

	while (true)
	{
		File::ReadStringFromFile(InFile, Token);

		if (Token == TEXT("<UIObject>"))
		{
			Object = CQuadObject::LoadObjectInfoFromFile(D3D12Device, D3D12GraphicsCommandList, InFile, MaterialCaches);

			m_QuadObjects.push_back(Object);
		}
		else if (Token == TEXT("</UI>"))
		{
			break;
		}
	}

	tcout << FileName << TEXT(" �ε� �Ϸ�...") << endl << endl;
}

void CTitleScene::CreateShaderVariables(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{

}

void CTitleScene::UpdateShaderVariables(ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{

}

void CTitleScene::ReleaseShaderVariables()
{

}

void CTitleScene::ReleaseUploadBuffers()
{
	for (const auto& QuadObject : m_QuadObjects)
	{
		if (QuadObject)
		{
			QuadObject->ReleaseUploadBuffers();
		}
	}
}

void CTitleScene::ProcessMouseMessage(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch (Message)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
	{
		UINT BilboardObjectCount{ static_cast<UINT>(m_QuadObjects.size()) };

		for (UINT i = 0; i < BilboardObjectCount; ++i)
		{
			if (m_QuadObjects[i])
			{
				m_QuadObjects[i]->ProcessMouseMessage(Message, XMINT2{ LOWORD(lParam), HIWORD(lParam) }, i);
			}
		}

		// m_BilboardObject[3]: ���� IP�ּ� �Է� �г�
		CFramework::GetInstance()->GetUILayer()->SetActive(m_QuadObjects[3]->IsActive());
	}
	break;
	}
}

void CTitleScene::ProcessKeyboardMessage(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch (Message)
	{
	case WM_CHAR:
		CFramework::GetInstance()->GetUILayer()->UpdateText(wParam);
		break;
	}
}

void CTitleScene::ProcessInput(HWND hWnd, float ElapsedTime)
{

}

void CTitleScene::Animate(float ElapsedTime)
{
	for (const auto& QuadObject : m_QuadObjects)
	{
		if (QuadObject)
		{
			QuadObject->Animate(ElapsedTime);
		}
	}
}

void CTitleScene::PreRender(ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{

}

void CTitleScene::Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	RSSetViewportsAndScissorRects(D3D12GraphicsCommandList);

	for (const auto& QuadObject : m_QuadObjects)
	{
		if (QuadObject)
		{
			QuadObject->Render(D3D12GraphicsCommandList, nullptr, RENDER_TYPE_STANDARD);
		}
	}
}

void CTitleScene::PostRender(ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{

}

void CTitleScene::ProcessPacket()
{
	SOCKET_INFO SocketInfo{ CFramework::GetInstance()->GetSocketInfo() };

	if (SocketInfo.m_Socket)
	{
		MSG_TYPE MsgType{ MSG_TYPE_TITLE };
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
			tcout << "������ ����Ǿ����ϴ�." << endl;
			closesocket(SocketInfo.m_Socket);
		}
		else
		{
			if (MsgType == MSG_TYPE_INGAME)
			{
				CSceneManager::GetInstance()->ReserveScene(TEXT("GameScene"));
				CFramework::GetInstance()->GetPostProcessingShader()->SetPostProcessingType(POST_PROCESSING_TYPE_FADE_OUT);
			}
		}
	}
}

vector<shared_ptr<CQuadObject>>& CTitleScene::GetQuadObjects()
{
	return m_QuadObjects;
}

void CTitleScene::RSSetViewportsAndScissorRects(ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	D3D12GraphicsCommandList->RSSetViewports(1, &m_ViewPort);
	D3D12GraphicsCommandList->RSSetScissorRects(1, &m_ScissorRect);
}
