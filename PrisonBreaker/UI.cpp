#include "pch.h"
#include "UI.h"

#include "AssetManager.h"
#include "InputManager.h"

#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"
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
			ui = new CUI();
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
			ui->SetPosition(XMFLOAT3(position.x, position.y, 0.0f));
		}
		else if (str == "<Size>")
		{
			XMFLOAT2 size = {};

			in.read((char*)&size, sizeof(XMFLOAT2));
			ui->Scale(size.x, size.y, 1.0f);
		}
		else if (str == "<Material>")
		{
			File::ReadStringFromFile(in, str);
		}
		else if (str == "<SpriteSize>")
		{
			CSpriteRenderer* spriteRenderer = static_cast<CSpriteRenderer*>(ui->CreateComponent(COMPONENT_TYPE::SPRITE_RENDERER));
			XMFLOAT2 spriteSize = {};

			in.read((char*)&spriteSize, sizeof(XMFLOAT2));
			spriteRenderer->SetSpriteSize(spriteSize);
		}
		else if (str == "<SpriteIndex>")
		{
			CSpriteRenderer* spriteRenderer = static_cast<CSpriteRenderer*>(ui->GetComponent(COMPONENT_TYPE::SPRITE_RENDERER));
			int frameIndex = 0;

			in.read((char*)&frameIndex, sizeof(int));
			spriteRenderer->SetFrameIndex(frameIndex);
		}
		else if (str == "</UI>")	
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
    XMFLOAT3 position = GetPosition();
    XMFLOAT2 size = GetSize();

    if ((position.x <= cursor.x) && (cursor.x <= position.x + size.x) &&
        (position.y <= cursor.y) && (cursor.y <= position.y + size.y))
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
