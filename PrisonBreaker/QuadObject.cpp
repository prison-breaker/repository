#include "stdafx.h"
#include "UIObjects.h"
#include "Material.h"
#include "Texture.h"
#include "UIAnimationController.h"

shared_ptr<CQuadObject> CQuadObject::LoadObjectInfoFromFile(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, tifstream& InFile, unordered_map<tstring, shared_ptr<CMaterial>>& MaterialCaches)
{
	tstring Token{};

	shared_ptr<CQuadObject> NewObject{};

	UINT Type{};
	XMUINT2 CellCount{};

	while (true)
	{
		File::ReadStringFromFile(InFile, Token);

		if (Token == TEXT("<Type>"))
		{
			Type = File::ReadIntegerFromFile(InFile);

			switch (Type)
			{
			case 0:
				NewObject = make_shared<CQuadObject>();
				break;
			case 1:
				NewObject = make_shared<CBackgroundUI>();
				break;
			case 2:
				NewObject = make_shared<CMissionUI>();
				break;
			case 3:
				NewObject = make_shared<CKeyUI>();
				break;
			case 4:
				NewObject = make_shared<CHitUI>();
				break;
			case 5:
				NewObject = make_shared<CMainButtonUI>();
				break;
			case 6:
				NewObject = make_shared<CPanelButtonUI>();
				break;
			case 7:
				NewObject = make_shared<CLoadingIconUI>();
				break;
			case 8:
				NewObject = make_shared<CGameOverUI>();
				break;
			case 9:
				NewObject = make_shared<CEndingCreditUI>();
				break;
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
		
			if (MaterialCaches.count(Token))
			{
				NewObject->SetMaterial(MaterialCaches[Token]);
			}
			else
			{
				shared_ptr<CMaterial> Material{ make_shared<CMaterial>() };
				shared_ptr<CTexture> Texture{ make_shared<CTexture>() };
				shared_ptr<CShader> Shader{ CShaderManager::GetInstance()->GetShader("QuadShader") };

				Texture->LoadTextureFromDDSFile(D3D12Device, D3D12GraphicsCommandList, TEXTURE_TYPE_ALBEDO_MAP, Token);
				Material->SetStateNum(2);
				Material->RegisterTexture(Texture);
				Material->RegisterShader(Shader);
				CTextureManager::GetInstance()->RegisterTexture(Token, Texture);
				MaterialCaches.emplace(Token, Material);

				NewObject->SetMaterial(Material);
			}
		}
		else if (Token == TEXT("<CellInfo>"))
		{
			CellCount = { File::ReadIntegerFromFile(InFile), File::ReadIntegerFromFile(InFile) };
		}
		else if (Token == TEXT("<RectTransform>"))
		{
			NewObject->m_MaxVertexCount = NewObject->m_VertexCount = File::ReadIntegerFromFile(InFile);

			NewObject->m_D3D12VertexBuffer = DX::CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, nullptr, sizeof(QUAD_INFO) * NewObject->m_VertexCount, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, nullptr);
			DX::ThrowIfFailed(NewObject->m_D3D12VertexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&NewObject->m_MappedQuadInfo)));

			NewObject->m_D3D12VertexBufferView.BufferLocation = NewObject->m_D3D12VertexBuffer->GetGPUVirtualAddress();
			NewObject->m_D3D12VertexBufferView.StrideInBytes = sizeof(QUAD_INFO);
			NewObject->m_D3D12VertexBufferView.SizeInBytes = sizeof(QUAD_INFO) * NewObject->m_VertexCount;

			// 버튼 영역이나, 크레딧의 초기 위치 등을 저장한다.
			shared_ptr<CButtonUI> ButtonUI{};
			shared_ptr<CEndingCreditUI> EndingCreditUI{};

			if (typeid(*NewObject) == typeid(CMainButtonUI) || typeid(*NewObject) == typeid(CPanelButtonUI))
			{
				ButtonUI = static_pointer_cast<CButtonUI>(NewObject);
			}
			else if (typeid(*NewObject) == typeid(CEndingCreditUI))
			{
				EndingCreditUI = static_pointer_cast<CEndingCreditUI>(NewObject);
			}

			for (UINT i = 0; i < NewObject->m_MaxVertexCount; ++i)
			{
				QUAD_INFO* MappedQuadInfo{ NewObject->m_MappedQuadInfo + i };
				XMFLOAT3 Position{};
				XMFLOAT2 Size{};
				float CellIndex{};

				InFile.read(reinterpret_cast<TCHAR*>(&Position), sizeof(XMFLOAT2));
				InFile.read(reinterpret_cast<TCHAR*>(&CellIndex), sizeof(float));
				InFile.read(reinterpret_cast<TCHAR*>(&Size), sizeof(XMFLOAT2));

				MappedQuadInfo->m_Position = Position;
				MappedQuadInfo->m_Size = Size;
				MappedQuadInfo->m_CellCount = CellCount;
				MappedQuadInfo->m_CellIndex = CellIndex;

				if (ButtonUI)
				{
					// x: XMin, y: XMax, z: YMin, w: YMax
					XMFLOAT4 ButtonArea{ Position.x - 0.5f * Size.x, Position.x + 0.5f * Size.x, Position.y - 0.5f * Size.y, Position.y + 0.5f * Size.y };

					ButtonUI->SetButtonArea(i, ButtonArea);
				}
				else if (EndingCreditUI)
				{
					EndingCreditUI->SetInitPosition(i, Position);
				}
			}
		}
		else if (Token == TEXT("<AlphaColor>"))
		{
			for (UINT i = 0; i < NewObject->m_VertexCount; ++i)
			{
				QUAD_INFO* MappedQuadInfo{ NewObject->m_MappedQuadInfo + i };
				float AlphaColor{};

				InFile.read(reinterpret_cast<TCHAR*>(&AlphaColor), sizeof(float));

				MappedQuadInfo->m_AlphaColor = AlphaColor;
			}
		}
		else if (Token == TEXT("<Animation>"))
		{
			CQuadObject::LoadAnimationInfoFromFile(InFile, NewObject);
		}
		else if (Token == TEXT("<ChildCount>"))
		{
			UINT ChildCount{ File::ReadIntegerFromFile(InFile) };

			if (ChildCount > 0)
			{
				for (UINT i = 0; i < ChildCount; ++i)
				{
					shared_ptr<CQuadObject> ChildObject{ CQuadObject::LoadObjectInfoFromFile(D3D12Device, D3D12GraphicsCommandList, InFile, MaterialCaches) };

					if (ChildObject)
					{
						NewObject->SetChild(ChildObject);
					}
				}
			}
		}
		else if (Token == TEXT("</UIObject>"))
		{
			break;
		}
	}

	return NewObject;
}

void CQuadObject::LoadAnimationInfoFromFile(tifstream& InFile, const shared_ptr<CQuadObject>& Model)
{
	tstring Token{};

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

				Model->m_AnimationController = make_shared<CUIAnimationController>(Model, UIAnimationClips);
				Model->Initialize();
			}
		}
		else if (Token == TEXT("</AnimationClips>"))
		{
			break;
		}
	}
}

void CQuadObject::Initialize()
{
	m_IsActive = true;
}

void CQuadObject::Reset()
{
	m_IsActive = true;
	m_VertexCount = m_MaxVertexCount;

	for (const auto& ChildObjects : m_ChildObjects)
	{
		if (ChildObjects)
		{
			ChildObjects->Reset();
		}
	}
}

void CQuadObject::ProcessMouseMessage(UINT Message, const XMINT2& ScreenPosition, UINT RootFrameIndex)
{
	if (m_IsActive)
	{
		for (const auto& ChildObjects : m_ChildObjects)
		{
			if (ChildObjects)
			{
				ChildObjects->ProcessMouseMessage(Message, ScreenPosition, RootFrameIndex);
			}
		}
	}
}

void CQuadObject::Animate(float ElapsedTime)
{
	if (m_IsActive)
	{
		for (const auto& ChildObjects : m_ChildObjects)
		{
			if (ChildObjects)
			{
				ChildObjects->Animate(ElapsedTime);
			}
		}
	}
}

void CQuadObject::Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera, RENDER_TYPE RenderType)
{
	if (m_IsActive)
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

		for (const auto& ChildObject : m_ChildObjects)
		{
			if (ChildObject)
			{
				ChildObject->Render(D3D12GraphicsCommandList, Camera, RenderType);
			}
		}
	}
}

void CQuadObject::ReleaseUploadBuffers()
{
	if (m_D3D12VertexUploadBuffer)
	{
		m_D3D12VertexUploadBuffer.Reset();
	}

	for (const auto& ChildObject : m_ChildObjects)
	{
		if (ChildObject)
		{
			ChildObject->ReleaseUploadBuffers();
		}
	}
}

bool CQuadObject::IsActive() const
{
	return m_IsActive;
}

void CQuadObject::SetActive(bool IsActive)
{
	m_IsActive = IsActive;
}

UINT CQuadObject::GetMaxVertexCount() const
{
	return m_MaxVertexCount;
}

void CQuadObject::SetVertexCount(UINT VertexCount)
{
	if (VertexCount < 0 || VertexCount > m_MaxVertexCount)
	{
		return;
	}

	m_VertexCount = VertexCount;
}

UINT CQuadObject::GetVertexCount() const
{
	return m_VertexCount;
}

void CQuadObject::SetPosition(UINT Index, const XMFLOAT3& Position)
{
	if (Index < 0 || Index > m_MaxVertexCount)
	{
		return;
	}

	m_MappedQuadInfo[Index].m_Position = Position;
}

const XMFLOAT3& CQuadObject::GetPosition(UINT Index) const
{
	return m_MappedQuadInfo[Index].m_Position;
}

void CQuadObject::SetSize(UINT Index, const XMFLOAT2& Size)
{
	if (Index < 0 || Index > m_MaxVertexCount)
	{
		return;
	}

	m_MappedQuadInfo[Index].m_Size = Size;
}

const XMFLOAT2& CQuadObject::GetSize(UINT Index) const
{
	return m_MappedQuadInfo[Index].m_Size;
}

void CQuadObject::SetAlphaColor(UINT Index, float AlphaColor)
{
	if (Index < 0 || Index > m_MaxVertexCount)
	{
		return;
	}

	m_MappedQuadInfo[Index].m_AlphaColor = AlphaColor;
}

float CQuadObject::GetAlphaColor(UINT Index) const
{
	return m_MappedQuadInfo[Index].m_AlphaColor;
}

void CQuadObject::SetCellIndex(UINT Index, float CellIndex)
{
	if (Index < 0 || Index > m_MaxVertexCount)
	{
		return;
	}

	m_MappedQuadInfo[Index].m_CellIndex = CellIndex;
}

float CQuadObject::GetCellIndex(UINT Index) const
{
	return m_MappedQuadInfo[Index].m_CellIndex;
}

void CQuadObject::SetMaterial(const shared_ptr<CMaterial>& Material)
{
	if (Material)
	{
		m_Materials.push_back(Material);
	}
}

shared_ptr<CUIAnimationController> CQuadObject::GetAnimationController() const
{
	return m_AnimationController;
}

void CQuadObject::SetChild(const shared_ptr<CQuadObject>& ChildObject)
{
	if (ChildObject)
	{
		m_ChildObjects.push_back(ChildObject);
	}
}
