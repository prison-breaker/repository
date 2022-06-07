#include "stdafx.h"
#include "TitleScene.h"
#include "Framework.h"

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
#ifdef READ_BINARY_FILE
	LoadUIInfoFromFile(D3D12Device, D3D12GraphicsCommandList, TEXT("Scenes/TitleScene_UI.bin"));
#else
	LoadUIInfoFromFile(D3D12Device, D3D12GraphicsCommandList, TEXT("Scenes/TitleScene_UI.txt"));
#endif

	CreateShaderVariables(D3D12Device, D3D12GraphicsCommandList);
}

void CTitleScene::ReleaseObjects()
{

}

void CTitleScene::LoadSceneInfoFromFile(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, const tstring& FileName)
{

}

void CTitleScene::LoadUIInfoFromFile(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, const tstring& FileName)
{
	tstring Token{};

	shared_ptr<CBilboardObject> Object{};

	tcout << FileName << TEXT(" 로드 시작...") << endl;

#ifdef READ_BINARY_FILE
	tifstream InFile{ FileName, ios::binary };

	while (true)
	{
		File::ReadStringFromFile(InFile, Token);

		if (Token == TEXT("<UIObject>"))
		{
			Object = CBilboardObject::LoadObjectInfoFromFile(D3D12Device, D3D12GraphicsCommandList, InFile);

			m_BilboardObjects.push_back(Object);
		}
		else if (Token == TEXT("</UI>"))
		{
			break;
		}
	}
#else
	tifstream InFile{ FileName };

	while (InFile >> Token)
	{
		if (Token == TEXT("<UIObject>"))
		{
			Object = CBilboardObject::LoadObjectInfoFromFile(D3D12Device, D3D12GraphicsCommandList, InFile);

			m_BilboardObjects[BILBOARD_OBJECT_TYPE_UI].push_back(Object);
		}
		else if (Token == TEXT("</UI>"))
		{
			break;
		}
	}
#endif
	tcout << FileName << TEXT(" 로드 완료...") << endl << endl;

	UINT VertexCount{ static_cast<UINT>(m_BilboardObjects[2]->GetVertexCount()) };

	for (UINT i = 0; i < VertexCount; ++i)
	{
		// 버튼의 영역이 필요하기 때문에 어쩔 수 없이 MappedData를 읽어들임(한번만 하므로 사용)
		XMFLOAT3 Position{ m_BilboardObjects[2]->GetPosition(i) };
		XMFLOAT2 Size{ m_BilboardObjects[2]->GetSize(i) };

		// x: XMin, y: XMax, z: YMin, w: YMax
		m_ButtonArea[i].x = Position.x - 0.5f * Size.x;
		m_ButtonArea[i].y = Position.x + 0.5f * Size.x;
		m_ButtonArea[i].z = Position.y - 0.5f * Size.y;
		m_ButtonArea[i].w = Position.y + 0.5f * Size.y;
	}
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

}

void CTitleScene::ProcessKeyboardMessage(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{

}

void CTitleScene::ProcessInput(HWND hWnd, float ElapsedTime)
{
	POINT CursorPos{};

	GetCursorPos(&CursorPos);

	UINT VertexCount{ static_cast<UINT>(m_BilboardObjects[2]->GetVertexCount()) };

	for (UINT i = 0; i < VertexCount; ++i)
	{
		if ((m_ButtonArea[i].x <= CursorPos.x) && (CursorPos.x <= m_ButtonArea[i].y) &&
			(m_ButtonArea[i].z <= CursorPos.y) && (CursorPos.y <= m_ButtonArea[i].w))
		{
			m_BilboardObjects[2]->SetCellIndex(i, 2 * i + 1);

			if (!m_ButtonOver)
			{
				m_ButtonOver = true;

				CSoundManager::GetInstance()->Play(SOUND_TYPE_BUTTON_OVER, 0.7f);
			}

			if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
			{
				switch (i)
				{
				case 0:
				{	
					// 게임시작시 커서가 중앙에 위치하도록 하여 카메라가 돌아가지 않도록 한다.
					// 나중에 이 코드는 옮겨야 한다.
					RECT Rect{};

					GetWindowRect(hWnd, &Rect);

					POINT OldCursorPos{ Rect.right / 2, Rect.bottom / 2 };

					SetCursorPos(OldCursorPos.x, OldCursorPos.y);
					ShowCursor(FALSE);

					// GameStart Button
					CSceneManager::GetInstance()->ReserveChangeScene(TEXT("GameScene"));
					CSoundManager::GetInstance()->Stop(SOUND_TYPE_TITLE_BGM);
					CSoundManager::GetInstance()->Play(SOUND_TYPE_INGAME_BGM_1, 0.3f);
				}
					break;
				case 1:
					// Help Button
					break;
				case 2:
					// Exit Button
					PostQuitMessage(0);
					break;
				}
			}
			return;
		}
		else
		{
			m_BilboardObjects[2]->SetCellIndex(i, 2 * i);

			if (i == 2)
			{
				m_ButtonOver = false;
			}
		}
	}

	CFramework::GetInstance()->SendPacket(CLIENT_TO_SERVER_DATA{ 0, 0, Matrix4x4::Identity() });
	CFramework::GetInstance()->ReceivePacket();
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

void CTitleScene::RSSetViewportsAndScissorRects(ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	D3D12GraphicsCommandList->RSSetViewports(1, &m_ViewPort);
	D3D12GraphicsCommandList->RSSetScissorRects(1, &m_ScissorRect);
}

void CTitleScene::ApplyPacketData(const SERVER_TO_CLIENT_DATA& PacketData)
{

}
