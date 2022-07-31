#include "stdafx.h"
#include "CreditScene.h"
#include "Framework.h"
#include "QuadObject.h"
#include "Texture.h"
#include "PostProcessingShader.h"

void CCreditScene::Initialize()
{
	for (const auto& QuadObject : m_QuadObjects)
	{
		if (QuadObject)
		{
			QuadObject->Reset();
		}
	}
}

void CCreditScene::OnCreate(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, ID3D12RootSignature* D3D12RootSignature)
{
	BuildObjects(D3D12Device, D3D12GraphicsCommandList, D3D12RootSignature);
}

void CCreditScene::OnDestroy()
{

}

void CCreditScene::BuildObjects(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, ID3D12RootSignature* D3D12RootSignature)
{
	// 파일로부터 UI 객체들을 생성하고 배치한다.
	LoadUIInfoFromFile(D3D12Device, D3D12GraphicsCommandList, TEXT("Scenes/CreditScene_UI.bin"));
}

void CCreditScene::ReleaseObjects()
{

}

void CCreditScene::Enter(MSG_TYPE MsgType)
{
	CFramework::GetInstance()->DisconnectServer();
}

void CCreditScene::Exit()
{
	CSoundManager::GetInstance()->Stop(SOUND_TYPE_ENDING_BGM);
}

void CCreditScene::LoadSceneInfoFromFile(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, const tstring& FileName)
{

}

void CCreditScene::LoadUIInfoFromFile(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, const tstring& FileName)
{
	tstring Token{};

	unordered_map<tstring, shared_ptr<CMaterial>> MaterialCaches{};
	shared_ptr<CQuadObject> Object{};

	tcout << FileName << TEXT(" 로드 시작...") << endl;

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

	tcout << FileName << TEXT(" 로드 완료...") << endl << endl;
}

void CCreditScene::CreateShaderVariables(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{

}

void CCreditScene::UpdateShaderVariables(ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{

}

void CCreditScene::ReleaseShaderVariables()
{

}

void CCreditScene::ReleaseUploadBuffers()
{
	for (const auto& QuadObject : m_QuadObjects)
	{
		if (QuadObject)
		{
			QuadObject->ReleaseUploadBuffers();
		}
	}
}

void CCreditScene::ProcessMouseMessage(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{

}

void CCreditScene::ProcessKeyboardMessage(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{

}

void CCreditScene::ProcessInput(HWND hWnd, float ElapsedTime)
{

}

void CCreditScene::Animate(float ElapsedTime)
{
	m_ElapsedTime += ElapsedTime;

	if (m_ElapsedTime >= m_TimeToTitleScene)
	{
		m_ElapsedTime = 0.0f;

		CSceneManager::GetInstance()->ReserveScene(TEXT("TitleScene"), MSG_TYPE_TITLE);
		CFramework::GetInstance()->GetPostProcessingShader()->SetPostProcessingType(POST_PROCESSING_TYPE_FADE_OUT);
	}
	else
	{
		for (const auto& QuadObject : m_QuadObjects)
		{
			if (QuadObject)
			{
				QuadObject->Animate(ElapsedTime);
			}
		}
	}
}

void CCreditScene::PreRender(ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{

}

void CCreditScene::Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
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

void CCreditScene::PostRender(ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{

}

void CCreditScene::ProcessPacket()
{

}

void CCreditScene::RSSetViewportsAndScissorRects(ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	D3D12GraphicsCommandList->RSSetViewports(1, &m_ViewPort);
	D3D12GraphicsCommandList->RSSetScissorRects(1, &m_ScissorRect);
}
