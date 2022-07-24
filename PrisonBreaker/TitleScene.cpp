#include "stdafx.h"
#include "TitleScene.h"
#include "Framework.h"

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
	// 렌더링에 필요한 셰이더 객체(PSO)를 생성한다.
	shared_ptr<CGraphicsShader> Shader{ make_shared<CUIShader>() };

	Shader->CreatePipelineState(D3D12Device, D3D12RootSignature, 0);
	CShaderManager::GetInstance()->RegisterShader(TEXT("UIShader"), Shader);

	// 파일로부터 UI 객체들을 생성하고 배치한다.
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
		m_BilboardObjects[5]->SetActive(true);
		break;
	}

	CSoundManager::GetInstance()->Play(SOUND_TYPE_TITLE_BGM, 0.3f);
}

void CTitleScene::Exit()
{
	ShowCursor(FALSE);

	UINT BilboardObjectCount{ static_cast<UINT>(m_BilboardObjects.size()) };

	for (UINT i = 3; i < BilboardObjectCount; ++i)
	{
		if (m_BilboardObjects[i])
		{
			m_BilboardObjects[i]->SetActive(false);
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
	shared_ptr<CBilboardObject> Object{};

	tcout << FileName << TEXT(" 로드 시작...") << endl;

	tifstream InFile{ FileName, ios::binary };

	while (true)
	{
		File::ReadStringFromFile(InFile, Token);

		if (Token == TEXT("<UIObject>"))
		{
			Object = CBilboardObject::LoadObjectInfoFromFile(D3D12Device, D3D12GraphicsCommandList, InFile, MaterialCaches);

			m_BilboardObjects.push_back(Object);
		}
		else if (Token == TEXT("</UI>"))
		{
			break;
		}
	}

	tcout << FileName << TEXT(" 로드 완료...") << endl << endl;
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
	for (const auto& BilboardObject : m_BilboardObjects)
	{
		if (BilboardObject)
		{
			BilboardObject->ReleaseUploadBuffers();
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
		UINT BilboardObjectCount{ static_cast<UINT>(m_BilboardObjects.size()) };

		for (UINT i = 0; i < BilboardObjectCount; ++i)
		{
			if (m_BilboardObjects[i])
			{
				m_BilboardObjects[i]->ProcessMouseMessage(Message, XMINT2{ LOWORD(lParam), HIWORD(lParam) }, i);
			}
		}

		// m_BilboardObject[3]: 서버 IP주소 입력 패널
		CFramework::GetInstance()->GetUILayer()->SetActive(m_BilboardObjects[3]->IsActive());
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
	for (const auto& BilboardObject : m_BilboardObjects)
	{
		if (BilboardObject)
		{
			BilboardObject->Animate(ElapsedTime);
		}
	}
}

void CTitleScene::PreRender(ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{

}

void CTitleScene::Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	RSSetViewportsAndScissorRects(D3D12GraphicsCommandList);
	CTextureManager::GetInstance()->SetDescriptorHeap(D3D12GraphicsCommandList);

	for (const auto& BilboardObject : m_BilboardObjects)
	{
		if (BilboardObject)
		{
			BilboardObject->Render(D3D12GraphicsCommandList, nullptr, RENDER_TYPE_STANDARD);
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
			tcout << "서버가 종료되었습니다." << endl;
			closesocket(SocketInfo.m_Socket);
		}
		else
		{
			if (MsgType == MSG_TYPE_INGAME)
			{
				CSceneManager::GetInstance()->ReserveScene(TEXT("GameScene"));
				CFramework::GetInstance()->SetPostProcessingType(POST_PROCESSING_TYPE_FADE_OUT);
			}
		}
	}
}

vector<shared_ptr<CBilboardObject>>& CTitleScene::GetBilboardObjects()
{
	return m_BilboardObjects;
}

void CTitleScene::RSSetViewportsAndScissorRects(ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	D3D12GraphicsCommandList->RSSetViewports(1, &m_ViewPort);
	D3D12GraphicsCommandList->RSSetScissorRects(1, &m_ScissorRect);
}
