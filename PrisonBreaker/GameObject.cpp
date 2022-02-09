#include "stdafx.h"
#include "GameObject.h"

shared_ptr<CGameObject> CGameObject::LoadObjectFromFile(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, const tstring& FileName)
{
	tifstream InFile{ FileName };
	tstring Token{};
	shared_ptr<CGameObject> NewObject{};

	while (InFile >> Token)
	{
		if (!Token.compare(TEXT("<Hierarchy>")))
		{
			NewObject = CGameObject::LoadObjectInfoFromFile(D3D12Device, D3D12GraphicsCommandList, InFile);
			tcout << FileName << TEXT(" 로드 시작...") << endl;
		}
		else if (!Token.compare(TEXT("</Hierarchy>")))
		{
			tcout << FileName << TEXT(" 로드 완료...") << endl;
			break;
		}
	}

	return NewObject;
}

shared_ptr<CGameObject> CGameObject::LoadObjectInfoFromFile(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, tifstream& InFile)
{
	tstring Token{};
	shared_ptr<CGameObject> NewObject{};

	while (InFile >> Token)
	{
		if (!Token.compare(TEXT("<Frame>")))
		{
			NewObject = make_shared<CGameObject>();
			NewObject->SetAlive(true);

			InFile >> NewObject->m_FrameName;
		}
		else if (!Token.compare(TEXT("<TransformMatrix>")))
		{
			InFile >> NewObject->m_TransformMatrix._11 >> NewObject->m_TransformMatrix._12 >> NewObject->m_TransformMatrix._13 >> NewObject->m_TransformMatrix._14;
			InFile >> NewObject->m_TransformMatrix._21 >> NewObject->m_TransformMatrix._22 >> NewObject->m_TransformMatrix._23 >> NewObject->m_TransformMatrix._24;
			InFile >> NewObject->m_TransformMatrix._31 >> NewObject->m_TransformMatrix._32 >> NewObject->m_TransformMatrix._33 >> NewObject->m_TransformMatrix._34;
			InFile >> NewObject->m_TransformMatrix._41 >> NewObject->m_TransformMatrix._42 >> NewObject->m_TransformMatrix._43 >> NewObject->m_TransformMatrix._44;
		}
		else if (!Token.compare(TEXT("<Mesh>")))
		{
			InFile >> Token;

			if (Token.compare(TEXT("Null")))
			{
				shared_ptr<CMesh> Mesh{ make_shared<CMesh>() };

				Mesh->LoadMeshFromFile(D3D12Device, D3D12GraphicsCommandList, InFile);
				NewObject->SetMesh(Mesh);
			}
		}
		else if (!Token.compare(TEXT("<Texture>")))
		{
			shared_ptr<CTexture> Texture{};

			while (InFile >> Token)
			{
				if (!Token.compare(TEXT("<AlbedoMap>")))
				{
					InFile >> Token;

					if (Token.compare(TEXT("Null")))
					{
						Texture = CTextureManager::GetInstance()->GetTexture(Token);

						if (!Texture)
						{
							Texture = make_shared<CTexture>();
							Texture->LoadTextureFromDDSFile(D3D12Device, D3D12GraphicsCommandList, TEXTURE_TYPE_DIFFUSEMAP, Token.c_str());
							CTextureManager::GetInstance()->RegisterTexture(Token, Texture);
						}
						else
						{
							break;
						}
					}
				}
				else if (!Token.compare(TEXT("<NormalMap>")))
				{
					InFile >> Token;

					if (Token.compare(TEXT("Null")))
					{
						Texture->LoadTextureFromDDSFile(D3D12Device, D3D12GraphicsCommandList, TEXTURE_TYPE_NORMALMAP, Token.c_str());
					}
				}
				else if (!Token.compare(TEXT("</Texture>")))
				{
					break;
				}
			}

			NewObject->SetTexture(Texture);
		}
		else if (!Token.compare(TEXT("<ChildCount>")))
		{
			UINT ChildCount{};

			InFile >> ChildCount;

			if (ChildCount > 0)
			{
				for (UINT i = 0; i < ChildCount; ++i)
				{
					shared_ptr<CGameObject> ChildObject = CGameObject::LoadObjectInfoFromFile(D3D12Device, D3D12GraphicsCommandList, InFile);

					if (ChildObject)
					{
						NewObject->SetChild(ChildObject);
					}
				}
			}
		}
		else if (!Token.compare(TEXT("</Frame>")))
		{
			break;
		}
	}

	return NewObject;
}

void CGameObject::CreateShaderVariables(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{

}

void CGameObject::UpdateShaderVariables(ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	XMFLOAT4X4 WorldMatrix{};

	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_WorldMatrix)));
	D3D12GraphicsCommandList->SetGraphicsRoot32BitConstants(ROOT_PARAMETER_OBJECT, 16, &WorldMatrix, 0);

	if (m_Texture)
	{
		m_Texture->UpdateShaderVariables(D3D12GraphicsCommandList);
	}
}

void CGameObject::ReleaseShaderVariables()
{

}

void CGameObject::ReleaseUploadBuffers()
{
	if (m_Mesh)
	{
		m_Mesh->ReleaseUploadBuffers();
	}
}

void CGameObject::Move(const XMFLOAT3& Direction, float Distance)
{
	XMFLOAT3 Shift{ Vector3::ScalarProduct(Distance, Direction, false) };

	SetPosition(Vector3::Add(GetPosition(), Shift));
}

void CGameObject::Animate(float ElapsedTime)
{

}

void CGameObject::Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera)
{
	if (IsAlive())
	{
		//if (IsVisible(Camera))
		{
			UpdateShaderVariables(D3D12GraphicsCommandList);

			if (m_Mesh)
			{
				m_Mesh->Render(D3D12GraphicsCommandList);
			}
		}
		
		if (m_SiblingObject)
		{
			m_SiblingObject->Render(D3D12GraphicsCommandList, Camera);
		}

		if (m_ChildObject)
		{
			m_ChildObject->Render(D3D12GraphicsCommandList, Camera);
		}
	}
}

bool CGameObject::IsAlive() const
{
	return m_IsAlive;
}

void CGameObject::SetAlive(bool IsAlive)
{
	m_IsAlive = IsAlive;
}

void CGameObject::SetWorldMatrix(const XMFLOAT4X4 WorldMatrix)
{
	m_WorldMatrix = WorldMatrix;
}

const XMFLOAT4X4& CGameObject::GetWorldMatrix() const
{
	return m_WorldMatrix;
}

void CGameObject::SetTransformMatrix(const XMFLOAT4X4& TransformMatrix)
{
	m_TransformMatrix = TransformMatrix;
}

const XMFLOAT4X4& CGameObject::GetTransformMatrix() const
{
	return m_TransformMatrix;
}

void CGameObject::SetRight(const XMFLOAT3& Right)
{
	m_TransformMatrix._11 = Right.x;
	m_TransformMatrix._12 = Right.y;
	m_TransformMatrix._13 = Right.z;

	UpdateTransform(Matrix4x4::Identity());
}

XMFLOAT3 CGameObject::GetRight() const
{
	return XMFLOAT3(m_WorldMatrix._11, m_WorldMatrix._12, m_WorldMatrix._13);
}

void CGameObject::SetUp(const XMFLOAT3& Up)
{
	m_TransformMatrix._21 = Up.x;
	m_TransformMatrix._22 = Up.y;
	m_TransformMatrix._23 = Up.z;

	UpdateTransform(Matrix4x4::Identity());
}

XMFLOAT3 CGameObject::GetUp() const
{
	return XMFLOAT3(m_WorldMatrix._21, m_WorldMatrix._22, m_WorldMatrix._23);
}

void CGameObject::SetLook(const XMFLOAT3& Look)
{
	m_TransformMatrix._31 = Look.x;
	m_TransformMatrix._32 = Look.y;
	m_TransformMatrix._33 = Look.z;

	UpdateTransform(Matrix4x4::Identity());
}

XMFLOAT3 CGameObject::GetLook() const
{
	return XMFLOAT3(m_WorldMatrix._31, m_WorldMatrix._32, m_WorldMatrix._33);
}

void CGameObject::SetPosition(const XMFLOAT3& Position)
{
	m_TransformMatrix._41 = Position.x;
	m_TransformMatrix._42 = Position.y;
	m_TransformMatrix._43 = Position.z;

	UpdateTransform(Matrix4x4::Identity());
}

XMFLOAT3 CGameObject::GetPosition() const
{
	return XMFLOAT3(m_WorldMatrix._41, m_WorldMatrix._42, m_WorldMatrix._43);
}

void CGameObject::SetMesh(const shared_ptr<CMesh>& Mesh)
{
	if (Mesh)
	{
		m_Mesh = Mesh;
	}
}

void CGameObject::SetTexture(const shared_ptr<CTexture>& Texture)
{
	if (Texture)
	{
		m_Texture = Texture;
	}
}

void CGameObject::SetBoundingBox(const BoundingOrientedBox& BoundingBox)
{
	m_BoundingBox = BoundingBox;
}

const BoundingOrientedBox& CGameObject::GetBoundingBox() const
{
	return m_BoundingBox;
}

void CGameObject::SetChild(const shared_ptr<CGameObject>& ChildObject)
{
	if (m_ChildObject)
	{
		if (ChildObject)
		{
			ChildObject->m_SiblingObject = m_ChildObject;
			m_ChildObject = ChildObject;
		}
	}
	else
	{
		m_ChildObject = ChildObject;
	}
}

bool CGameObject::IsVisible(CCamera* Camera) const
{
	bool Visible{};

	if (Camera)
	{
		if (m_Mesh)
		{
			BoundingOrientedBox BoundingBox{ m_Mesh->GetBoundingBox() };

			BoundingBox.Transform(BoundingBox, XMLoadFloat4x4(&m_WorldMatrix));
			Visible = Camera->IsInFrustum(BoundingBox);
		}
	}

	return Visible;
}

void CGameObject::UpdateBoundingBox()
{
	if (m_Mesh)
	{
		m_Mesh->GetBoundingBox().Transform(m_BoundingBox, XMLoadFloat4x4(&m_WorldMatrix));
		XMStoreFloat4(&m_BoundingBox.Orientation, XMQuaternionNormalize(XMLoadFloat4(&m_BoundingBox.Orientation)));
	}
}

void CGameObject::UpdateTransform(const XMFLOAT4X4& ParentMatrix)
{
	m_WorldMatrix = Matrix4x4::Multiply(m_TransformMatrix, ParentMatrix);

	if (m_SiblingObject)
	{
		m_SiblingObject->UpdateTransform(ParentMatrix);
	}

	if (m_ChildObject)
	{
		m_ChildObject->UpdateTransform(m_WorldMatrix);
	}
}

void CGameObject::Scale(float Pitch, float Yaw, float Roll)
{
	XMFLOAT4X4 ScaleMatrix{ Matrix4x4::Scale(Pitch, Yaw, Roll) };

	m_TransformMatrix = Matrix4x4::Multiply(ScaleMatrix, m_TransformMatrix);

	UpdateTransform(Matrix4x4::Identity());
}

void CGameObject::Rotate(float Pitch, float Yaw, float Roll)
{
	XMFLOAT4X4 RotationMatrix{ Matrix4x4::RotationYawPitchRoll(Pitch, Yaw, Roll) };

	m_TransformMatrix = Matrix4x4::Multiply(RotationMatrix, m_TransformMatrix);

	UpdateTransform(Matrix4x4::Identity());
}

void CGameObject::Rotate(const XMFLOAT3& Axis, float Angle)
{
	XMFLOAT4X4 RotationMatrix{ Matrix4x4::RotationAxis(Axis, Angle) };

	m_TransformMatrix = Matrix4x4::Multiply(RotationMatrix, m_TransformMatrix);

	UpdateTransform(Matrix4x4::Identity());
}
