#include "pch.h"
#include "UI.h"

#include "AssetManager.h"
#include "InputManager.h"

#include "Mesh.h"
#include "Material.h"

#include "Animator.h"
#include "SpriteRenderer.h"

CUI::CUI() :
    m_isFixed(true),
    m_isCursorOver()
{
}

CUI::~CUI()
{
}

CUI* CUI::Load(ID3D12Device* d3d12Device, ID3D12GraphicsCommandList* d3d12GraphicsCommandList, ifstream& in)
{
	CUI* ui = nullptr;
	string str;

	while (true)
	{
		File::ReadStringFromFile(in, str);

		if (str == "<Type>")
		{
			int type = 0;

			in.read((char*)&type, sizeof(int));

			CMesh* mesh = CAssetManager::GetInstance()->GetMesh("Quad");

			ui = new CUI();
			ui->SetMesh(mesh);
		}
		else if (str == "<Name>")
		{
			File::ReadStringFromFile(in, str);
			ui->SetName(str);
		}
		else if (str == "<IsActive>")
		{
			int isActive = false;

			in.read((char*)&isActive, sizeof(int));
			ui->SetActive(isActive);
		}
		else if (str == "<Position>")
		{
			XMFLOAT2 position = {};

			in.read((char*)&position, sizeof(XMFLOAT2));
			ui->SetPosition(position);
		}
		else if (str == "<Size>")
		{
			XMFLOAT2 size = {};

			in.read((char*)&size, sizeof(XMFLOAT2));
			ui->SetSize(size);
		}
		else if (str == "<Material>")
		{
			File::ReadStringFromFile(in, str);

			CMaterial* material = CAssetManager::GetInstance()->GetMaterial(str);

			ui->AddMaterial(material);
		}
		else if (str == "<SpriteSize>")
		{
			CSpriteRenderer* spriteRenderer = static_cast<CSpriteRenderer*>(ui->CreateComponent(COMPONENT_TYPE::SPRITE_RENDERER));
			XMINT2 spriteSize = {};

			in.read((char*)&spriteSize, sizeof(XMINT2));
			spriteRenderer->SetSpriteSize(spriteSize);
		}
		else if (str == "<FrameIndex>")
		{
			CSpriteRenderer* spriteRenderer = ui->GetComponent<CSpriteRenderer>();
			int frameIndex = 0;

			in.read((char*)&frameIndex, sizeof(int));
			spriteRenderer->SetFrameIndex(frameIndex);
		}
		else if (str == "<ChildCount>")
		{
			int childCount = 0;

			in.read(reinterpret_cast<char*>(&childCount), sizeof(int));

			for (int i = 0; i < childCount; ++i)
			{
				CUI* child = CUI::Load(d3d12Device, d3d12GraphicsCommandList, in);

				if (child != nullptr)
				{
					ui->AddChild(child);
				}
			}
		}
		else if (str == "</Frame>")
		{
			cout << endl;
			break;
		}
	}

	return ui;
}

void CUI::SetFixed(bool isFixed)
{
    m_isFixed = isFixed;
}

bool CUI::IsFixed()
{
    return m_isFixed;
}

void CUI::CheckCursorOver()
{
    const XMFLOAT2& cursor = CURSOR;
    XMFLOAT2 position = GetPosition();
    XMFLOAT2 size = GetSize();

    if ((position.x - 0.5f * size.x <= cursor.x) && (cursor.x <= position.x + 0.5f * size.x) &&
        (position.y - 0.5f * size.y <= cursor.y) && (cursor.y <= position.y + 0.5f * size.y))
    {
        m_isCursorOver = true;
    }
    else
    {
        m_isCursorOver = false;
    }
}

bool CUI::IsCursorOver()
{
    return m_isCursorOver;
}

void CUI::SetPosition(const XMFLOAT2& position)
{
	XMFLOAT4X4 transformMatrix = GetTransformMatrix();

	transformMatrix._41 = position.x;
	transformMatrix._42 = position.y;

	SetTransformMatrix(transformMatrix);
	UpdateTransform(false);
}

XMFLOAT2 CUI::GetPosition()
{
	const XMFLOAT4X4& worldMatrix = GetWorldMatrix();

	return XMFLOAT2(worldMatrix._41, worldMatrix._42);
}

void CUI::SetSize(const XMFLOAT2& size)
{
	XMFLOAT4X4 transformMatrix = GetTransformMatrix();

	transformMatrix._11 = size.x;
	transformMatrix._22 = size.y;

	SetTransformMatrix(transformMatrix);
	UpdateTransform(false);
}

XMFLOAT2 CUI::GetSize()
{
    const XMFLOAT4X4& worldMatrix = GetWorldMatrix();

    return XMFLOAT2(worldMatrix._11, worldMatrix._22);
}

void CUI::OnCursorOver()
{
}

void CUI::OnCursorLeftButtonDown()
{
}

void CUI::OnCursorLeftButtonUp()
{
}

void CUI::OnCursorLeftButtonClick()
{
}

void CUI::LateUpdate()
{
    CObject::LateUpdate();
    CheckCursorOver();
}

void CUI::Render(ID3D12GraphicsCommandList* d3d12GraphicsCommandList, CCamera* camera)
{
	const vector<CComponent*>& components = GetComponents();

	for (const auto& component : components)
	{
		if (component != nullptr)
		{
			component->UpdateShaderVariables(d3d12GraphicsCommandList);
		}
	}

	CMesh* mesh = GetMesh();

	if (mesh != nullptr)
	{
		UpdateShaderVariables(d3d12GraphicsCommandList);

		const vector<CMaterial*>& materials = GetMaterials();

		for (int i = 0; i < materials.size(); ++i)
		{
			materials[i]->SetPipelineState(d3d12GraphicsCommandList, RENDER_TYPE::STANDARD);
			materials[i]->UpdateShaderVariables(d3d12GraphicsCommandList);
			mesh->Render(d3d12GraphicsCommandList, i);
		}
	}
	
	const vector<CObject*>& children = GetChildren();

	for (const auto& child : children)
	{
		if (child->IsActive() && !child->IsDeleted())
		{
			child->Render(d3d12GraphicsCommandList, camera);
		}
	}
}
