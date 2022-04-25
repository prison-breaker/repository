#include "stdafx.h"
#include "BilboardObject.h"
#include "Material.h"
#include "UIAnimationController.h"
#include "MissionUI.h"
#include "KeyUI.h"

shared_ptr<CBilboardObject> CBilboardObject::LoadObjectInfoFromFile(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, tifstream& InFile)
{
	tstring Token{};

	shared_ptr<CBilboardObject> NewObject{};

	UINT Type{};
	XMUINT2 CellCount{};

#ifdef READ_BINARY_FILE
	while (true)
	{
		File::ReadStringFromFile(InFile, Token);

		if (Token == TEXT("<Type>"))
		{
			Type = File::ReadIntegerFromFile(InFile);

			switch (Type)
			{
			case 1:
				NewObject = make_shared<CMissionUI>();
				break;
			case 2:
				NewObject = make_shared<CKeyUI>();
				break;
			default:
				NewObject = make_shared<CBilboardObject>();
			}
		}
		else if (Token == TEXT("<IsActive>"))
		{
			bool IsActive{ static_cast<bool>(File::ReadIntegerFromFile(InFile)) };

			NewObject->SetActive(IsActive);
		}
		else if (Token == TEXT("<TextureName>"))
		{
			File::ReadStringFromFile(InFile, Token);
			
			tcout << Token << endl;

			shared_ptr<CMaterial> Material{ make_shared<CMaterial>() };
			shared_ptr<CTexture> Texture{ make_shared<CTexture>() };
			shared_ptr<CShader> Shader{ CShaderManager::GetInstance()->GetShader("UIShader") };

			Texture->LoadTextureFromDDSFile(D3D12Device, D3D12GraphicsCommandList, TEXTURE_TYPE_ALBEDO_MAP, Token);
			Material->RegisterTexture(Texture);
			Material->RegisterShader(Shader);
			CTextureManager::GetInstance()->RegisterTexture(Token, Texture);

			NewObject->SetMaterial(Material);
		}
		else if (Token == TEXT("<CellInfo>"))
		{
			CellCount = { File::ReadIntegerFromFile(InFile), File::ReadIntegerFromFile(InFile) };
		}
		else if (Token == TEXT("<RectTransform>"))
		{
			NewObject->m_MaxVertexCount = NewObject->m_VertexCount = File::ReadIntegerFromFile(InFile);

			NewObject->m_D3D12VertexBuffer = DX::CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, nullptr, sizeof(CBilboardMesh) * NewObject->m_VertexCount, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, nullptr);
			DX::ThrowIfFailed(NewObject->m_D3D12VertexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&NewObject->m_MappedImageInfo)));

			NewObject->m_D3D12VertexBufferView.BufferLocation = NewObject->m_D3D12VertexBuffer->GetGPUVirtualAddress();
			NewObject->m_D3D12VertexBufferView.StrideInBytes = sizeof(CBilboardMesh);
			NewObject->m_D3D12VertexBufferView.SizeInBytes = sizeof(CBilboardMesh) * NewObject->m_VertexCount;

			for (UINT i = 0; i < NewObject->m_VertexCount; ++i)
			{
				CBilboardMesh* MappedInfo{ NewObject->m_MappedImageInfo + i };
				XMFLOAT3 Position{};
				XMFLOAT2 Size{};
				float CellIndex{};

				InFile.read(reinterpret_cast<TCHAR*>(&Position), sizeof(XMFLOAT2));
				InFile.read(reinterpret_cast<TCHAR*>(&CellIndex), sizeof(float));
				InFile.read(reinterpret_cast<TCHAR*>(&Size), sizeof(XMFLOAT2));

				MappedInfo->SetPosition(Position);
				MappedInfo->SetSize(Size);
				MappedInfo->SetCellCount(CellCount);
				MappedInfo->SetCellIndex(static_cast<UINT>(CellIndex));
			}
		}
		else if (Token == TEXT("<AlphaColor>"))
		{
			for (UINT i = 0; i < NewObject->m_VertexCount; ++i)
			{
				CBilboardMesh* MappedInfo{ NewObject->m_MappedImageInfo + i };
				float AlphaColor{};

				InFile.read(reinterpret_cast<TCHAR*>(&AlphaColor), sizeof(float));

				MappedInfo->SetAlphaColor(AlphaColor);
			}
		}
		else if (Token == TEXT("<Animation>"))
		{
			CBilboardObject::LoadAnimationInfoFromFile(InFile, NewObject);
		}
		else if (Token == TEXT("</UIObject>"))
		{
			break;
		}
	}
#else
	while (InFile >> Token)
	{
		if (Token == TEXT("<IsActive>"))
		{
			InFile >> Token;

			NewObject->SetActive(stoi(Token));
		}
		else if (Token == TEXT("<TextureName>"))
		{
			InFile >> Token;

			shared_ptr<CMaterial> Material{ make_shared<CMaterial>() };
			shared_ptr<CTexture> Texture{ make_shared<CTexture>() };
			shared_ptr<CShader> Shader{ CShaderManager::GetInstance()->GetShader("UIShader") };

			Texture->LoadTextureFromDDSFile(D3D12Device, D3D12GraphicsCommandList, TEXTURE_TYPE_ALBEDO_MAP, Token);
			Material->RegisterTexture(Texture);
			Material->RegisterShader(Shader);
			CTextureManager::GetInstance()->RegisterTexture(Token, Texture);

			NewObject->SetMaterial(Material);
		}
		else if (Token == TEXT("<CellInfo>"))
		{
			InFile >> CellCount.x >> CellCount.y;
		}
		else if (Token == TEXT("<RectTransform>"))
		{
			InFile >> NewObject->m_MaxVertexCount;

			NewObject->m_VertexCount = NewObject->m_MaxVertexCount;

			NewObject->m_D3D12VertexBuffer = DX::CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, nullptr, sizeof(CBilboardMesh) * NewObject->m_VertexCount, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, nullptr);
			DX::ThrowIfFailed(NewObject->m_D3D12VertexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&NewObject->m_MappedImageInfo)));

			NewObject->m_D3D12VertexBufferView.BufferLocation = NewObject->m_D3D12VertexBuffer->GetGPUVirtualAddress();
			NewObject->m_D3D12VertexBufferView.StrideInBytes = sizeof(CBilboardMesh);
			NewObject->m_D3D12VertexBufferView.SizeInBytes = sizeof(CBilboardMesh) * NewObject->m_VertexCount;

			for (UINT i = 0; i < NewObject->m_VertexCount; ++i)
			{
				XMFLOAT3 Position{};
				XMFLOAT2 Size{};
				float CellIndex{};

				InFile >> Position.x >> Position.y;
				InFile >> CellIndex;
				InFile >> Size.x >> Size.y;

				*(NewObject->m_MappedImageInfo + i) = CBilboardMesh{ Position, Size, CellCount, static_cast<UINT>(CellIndex) };
			}
		}
		else if (Token == TEXT("<Animation>"))
		{
			CBilboardObject::LoadAnimationInfoFromFile(InFile, NewObject);
		}
		else if (Token == TEXT("</UIObject>"))
		{
			break;
		}
	}
#endif

	return NewObject;
}

void CBilboardObject::LoadAnimationInfoFromFile(tifstream& InFile, const shared_ptr<CBilboardObject>& Model)
{
	tstring Token{};

#ifdef READ_BINARY_FILE
	while (true)
	{
		File::ReadStringFromFile(InFile, Token);

		if (Token == TEXT("<AnimationClips>"))
		{
			UINT ClipCount{ File::ReadIntegerFromFile(InFile) };

			if (ClipCount > 0)
			{
				vector<shared_ptr<CUIAnimationClip>> UIAnimationClips{};

				UIAnimationClips.reserve(ClipCount);

				for (UINT i = 0; i < ClipCount; ++i)
				{
					shared_ptr<CUIAnimationClip> UIAnimationClip{ make_shared<CUIAnimationClip>() };

					UIAnimationClip->LoadAnimationClipInfoFromFile(InFile, Model->m_VertexCount);
					UIAnimationClips.push_back(UIAnimationClip);
				}

				Model->m_UIAnimationController = make_shared<CUIAnimationController>(Model, UIAnimationClips);
				Model->Initialize();
			}
		}
		else if (Token == TEXT("</AnimationClips>"))
		{
			break;
		}
	}
#else
	while (InFile >> Token)
	{
		if (Token == TEXT("<AnimationClips>"))
		{
			UINT ClipCount{};

			InFile >> ClipCount;

			if (ClipCount > 0)
			{
				vector<shared_ptr<CUIAnimationClip>> UIAnimationClips{};

				UIAnimationClips.reserve(ClipCount);

				for (UINT i = 0; i < ClipCount; ++i)
				{
					shared_ptr<CUIAnimationClip> UIAnimationClip{ make_shared<CUIAnimationClip>() };

					UIAnimationClip->LoadAnimationClipInfoFromFile(InFile, Model->m_VertexCount);
					UIAnimationClips.push_back(UIAnimationClip);
				}

				Model->m_UIAnimationController = make_shared<CUIAnimationController>(UIAnimationClips);
			}
		}
		else if (Token == TEXT("</AnimationClips>"))
		{
			break;
		}
	}
#endif
}

void CBilboardObject::Initialize()
{
	SetActive(true);
}

void CBilboardObject::Animate(float ElapsedTime)
{

}

void CBilboardObject::Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera, RENDER_TYPE RenderType)
{
	if (IsActive())
	{
		D3D12_VERTEX_BUFFER_VIEW VertexBufferViews[] = { m_D3D12VertexBufferView };

		D3D12GraphicsCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
		D3D12GraphicsCommandList->IASetVertexBuffers(0, 1, VertexBufferViews);

		UINT MaterialCount{ static_cast<UINT>(m_Materials.size()) };

		for (UINT i = 0; i < MaterialCount; ++i)
		{
			if (m_Materials[i])
			{
				m_Materials[i]->SetPipelineState(D3D12GraphicsCommandList, RenderType);
				m_Materials[i]->UpdateShaderVariables(D3D12GraphicsCommandList);
			}

			D3D12GraphicsCommandList->DrawInstanced(m_VertexCount, 1, 0, 0);
		}
	}
}

void CBilboardObject::ReleaseUploadBuffers()
{
	if (m_D3D12VertexUploadBuffer)
	{
		m_D3D12VertexUploadBuffer.ReleaseAndGetAddressOf();
	}
}

bool CBilboardObject::IsActive() const
{
	return m_IsActive;
}

void CBilboardObject::SetActive(bool IsActive)
{
	m_IsActive = IsActive;
}

void CBilboardObject::SetMaterial(const shared_ptr<CMaterial>& Material)
{
	if (Material)
	{
		m_Materials.push_back(Material);
	}
}

UINT CBilboardObject::GetMaxVertexCount() const
{
	return m_MaxVertexCount;
}

void CBilboardObject::SetVertexCount(UINT VertexCount)
{
	if (VertexCount < 0 || VertexCount > m_MaxVertexCount)
	{
		return;
	}

	m_VertexCount = VertexCount;
}

UINT CBilboardObject::GetVertexCount() const
{
	return m_VertexCount;
}

void CBilboardObject::SetPosition(UINT Index, const XMFLOAT3& Position)
{
	if (Index < 0 || Index > m_VertexCount)
	{
		return;
	}

	m_MappedImageInfo[Index].SetPosition(Position);
}

void CBilboardObject::SetSize(UINT Index, const XMFLOAT2& Size)
{
	if (Index < 0 || Index > m_VertexCount)
	{
		return;
	}

	m_MappedImageInfo[Index].SetSize(Size);
}

void CBilboardObject::SetAlphaColor(UINT Index, float AlphaColor)
{
	if (Index < 0 || Index > m_VertexCount)
	{
		return;
	}

	m_MappedImageInfo[Index].SetAlphaColor(AlphaColor);
}

void CBilboardObject::SetCellIndex(UINT Index, UINT CellIndex)
{
	if (Index < 0 || Index > m_VertexCount)
	{
		return;
	}

	m_MappedImageInfo[Index].SetCellIndex(CellIndex);
}

CUIAnimationController* CBilboardObject::GetUIAnimationController() const
{
	return m_UIAnimationController.get();
}

void CBilboardObject::SetAnimationClip(UINT ClipNum)
{
	if (m_UIAnimationController)
	{
		m_UIAnimationController->SetAnimationClip(ClipNum);
	}
}

void CBilboardObject::SetKeyFrameIndex(UINT ClipNum, UINT KeyFrameIndex)
{
	if (m_UIAnimationController)
	{
		m_UIAnimationController->SetKeyFrameIndex(ClipNum, KeyFrameIndex);
	}
}
