#include "pch.h"
#include "CreditScene.h"
#include "Core.h"
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

void CCreditScene::OnCreate(ID3D12Device* d3d12Device, ID3D12GraphicsCommandList* d3d12GraphicsCommandList, ID3D12RootSignature* D3D12RootSignature)
{
	BuildObjects(d3d12Device, d3d12GraphicsCommandList, D3D12RootSignature);
}

void CCreditScene::OnDestroy()
{

}

void CCreditScene::BuildObjects(ID3D12Device* d3d12Device, ID3D12GraphicsCommandList* d3d12GraphicsCommandList, ID3D12RootSignature* D3D12RootSignature)
{
	// 파일로부터 UI 객체들을 생성하고 배치한다.
	LoadUIInfoFromFile(d3d12Device, d3d12GraphicsCommandList, TEXT("Scenes/CreditScene_UI.bin"));
}

void CCreditScene::ReleaseObjects()
{

}

void CCreditScene::Enter(MSG_TYPE MsgType)
{
	CCore::GetInstance()->DisconnectServer();
}

void CCreditScene::Exit()
{
	CSoundManager::GetInstance()->Stop(SOUND_TYPE_ENDING_BGM);
}

void CCreditScene::LoadSceneInfoFromFile(ID3D12Device* d3d12Device, ID3D12GraphicsCommandList* d3d12GraphicsCommandList, const string& FileName)
{

}

void CCreditScene::LoadUIInfoFromFile(ID3D12Device* d3d12Device, ID3D12GraphicsCommandList* d3d12GraphicsCommandList, const string& FileName)
{
	string str{};

	unordered_map<string, shared_ptr<CMaterial>> MaterialCaches{};
	shared_ptr<CQuadObject> Object{};

	tcout << FileName << TEXT(" 로드 시작...") << endl;

	ifstream in{ FileName, ios::binary };

	while (true)
	{
		File::ReadStringFromFile(in, str);

		if (str == TEXT("<UIObject>"))
		{
			Object = CQuadObject::LoadObjectInfoFromFile(d3d12Device, d3d12GraphicsCommandList, in, MaterialCaches);

			m_QuadObjects.push_back(Object);
		}
		else if (str == TEXT("</UI>"))
		{
			break;
		}
	}

	tcout << FileName << TEXT(" 로드 완료...") << endl << endl;
}

void CCreditScene::CreateShaderVariables(ID3D12Device* d3d12Device, ID3D12GraphicsCommandList* d3d12GraphicsCommandList)
{

}

void CCreditScene::UpdateShaderVariables(ID3D12GraphicsCommandList* d3d12GraphicsCommandList)
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
		CCore::GetInstance()->GetPostProcessingShader()->SetPostProcessingType(POST_PROCESSING_TYPE_FADE_OUT);
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

void CCreditScene::PreRender(ID3D12GraphicsCommandList* d3d12GraphicsCommandList)
{

}

void CCreditScene::Render(ID3D12GraphicsCommandList* d3d12GraphicsCommandList)
{
	RSSetViewportsAndScissorRects(d3d12GraphicsCommandList);

	for (const auto& QuadObject : m_QuadObjects)
	{
		if (QuadObject)
		{
			QuadObject->Render(d3d12GraphicsCommandList, nullptr, RENDER_TYPE_STANDARD);
		}
	}
}

void CCreditScene::PostRender(ID3D12GraphicsCommandList* d3d12GraphicsCommandList)
{

}

void CCreditScene::ProcessPacket()
{

}

void CCreditScene::RSSetViewportsAndScissorRects(ID3D12GraphicsCommandList* d3d12GraphicsCommandList)
{
	d3d12GraphicsCommandList->RSSetViewports(1, &m_ViewPort);
	d3d12GraphicsCommandList->RSSetScissorRects(1, &m_ScissorRect);
}
