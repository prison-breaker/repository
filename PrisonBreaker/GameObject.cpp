#include "stdafx.h"
#include "GameObject.h"
#include "AnimationController.h"

shared_ptr<LOADED_MODEL_INFO> CGameObject::LoadObjectFromFile(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, const tstring& FileName)
{
	tstring Token{};

	shared_ptr<LOADED_MODEL_INFO> ModelInfo{ make_shared<LOADED_MODEL_INFO>() };
	unordered_map<tstring, shared_ptr<CMesh>> MeshCaches{};
	unordered_map<tstring, vector<shared_ptr<CMaterial>>> MaterialCaches{};

#ifdef READ_BINARY_FILE
	tifstream InFile{ FileName, ios::binary };

	while (true)
	{
		File::ReadStringFromFile(InFile, Token);

		if (Token == TEXT("<Hierarchy>"))
		{
			tcout << FileName << TEXT(" 로드 시작...") << endl;
			NewObject = CGameObject::LoadObjectInfoFromFile(D3D12Device, D3D12GraphicsCommandList, InFile, MeshCaches, MaterialCaches);
		}
		else if (Token == TEXT("</Hierarchy>"))
		{
			tcout << FileName << TEXT(" 로드 완료...") << endl;
			break;
		}
	}
#else
	tifstream InFile{ FileName };

	while (InFile >> Token)
	{
		if (Token == TEXT("<Hierarchy>"))
		{
			tcout << FileName << TEXT(" 로드 시작...") << endl;
			ModelInfo->m_Model = CGameObject::LoadObjectInfoFromFile(D3D12Device, D3D12GraphicsCommandList, InFile, MeshCaches, MaterialCaches);
		}
		else if (Token == TEXT("</Hierarchy>"))
		{
			tcout << FileName << TEXT(" 로드 완료...") << endl;
			break;
		}
	}

	if (InFile >> Token)
	{
		if (Token == TEXT("<Animation>"))
		{
			tcout << FileName << TEXT(" 애니메이션 로드 시작...") << endl;
			CGameObject::LoadAnimationInfoFromFile(InFile, ModelInfo);
		}
	}
#endif

	return ModelInfo;
}

shared_ptr<CGameObject> CGameObject::LoadObjectInfoFromFile(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, tifstream& InFile, unordered_map<tstring, shared_ptr<CMesh>>& MeshCaches, unordered_map<tstring, vector<shared_ptr<CMaterial>>>& MaterialCaches)
{
	tstring Token{};
	shared_ptr<CGameObject> NewObject{};
	bool IsSkinnedMesh{};

#ifdef READ_BINARY_FILE
	while (true)
	{
		File::ReadStringFromFile(InFile, Token);

		if (Token == TEXT("<Frame>"))
		{
			File::ReadStringFromFile(InFile, Token);

			NewObject = make_shared<CGameObject>();
			NewObject->SetActive(true);
			NewObject->m_FrameName = Token;

			if (MeshCaches.count(NewObject->m_FrameName))
			{
				NewObject->SetMesh(MeshCaches[NewObject->m_FrameName]);
			}

			if (MaterialCaches.count(NewObject->m_FrameName))
			{
				copy(MaterialCaches[NewObject->m_FrameName].begin(), MaterialCaches[NewObject->m_FrameName].end(), back_inserter(NewObject->m_Materials));
			}
		}
		else if (Token == TEXT("<TransformMatrix>"))
		{
			InFile.read(reinterpret_cast<TCHAR*>(&NewObject->m_TransformMatrix), sizeof(XMFLOAT4X4));
		}
		else if (Token == TEXT("<Mesh>"))
		{
			File::ReadStringFromFile(InFile, Token);

			if (Token != TEXT("Null"))
			{
				shared_ptr<CMesh> Mesh{ make_shared<CMesh>() };

				Mesh->LoadMeshInfoFromFile(D3D12Device, D3D12GraphicsCommandList, InFile);
				NewObject->SetMesh(Mesh);
				MeshCaches.emplace(NewObject->m_FrameName, Mesh);
			}
		}
		else if (Token == TEXT("<Materials>"))
		{
			UINT MaterialCount{ File::ReadIntegerFromFile(InFile) };

			if (MaterialCount > 0)
			{
				NewObject->m_Materials.reserve(MaterialCount);

				for (UINT i = 0; i < MaterialCount; ++i)
				{
					shared_ptr<CMaterial> Material{ make_shared<CMaterial>() };

					Material->LoadMaterialFromFile(D3D12Device, D3D12GraphicsCommandList, InFile);
					NewObject->m_Materials.push_back(Material);
				}

				MaterialCaches.emplace(NewObject->m_FrameName, NewObject->m_Materials);
			}
		}
		else if (Token == TEXT("<ChildCount>"))
		{
			UINT ChildCount{ File::ReadIntegerFromFile(InFile) };

			if (ChildCount > 0)
			{
				for (UINT i = 0; i < ChildCount; ++i)
				{
					shared_ptr<CGameObject> ChildObject{ CGameObject::LoadObjectInfoFromFile(D3D12Device, D3D12GraphicsCommandList, InFile, MeshCaches, MaterialCaches) };

					if (ChildObject)
					{
						NewObject->SetChild(ChildObject);
					}
				}
			}
		}
		else if (Token == TEXT("</Frame>"))
		{
			break;
		}
	}
#else
	while (InFile >> Token)
	{
		if (Token == TEXT("<Frame>"))
		{
			NewObject = make_shared<CGameObject>();
			NewObject->SetActive(true);

			InFile >> NewObject->m_FrameName;

			if (MeshCaches.count(NewObject->m_FrameName))
			{
				NewObject->SetMesh(MeshCaches[NewObject->m_FrameName]);
			}

			if (MaterialCaches.count(NewObject->m_FrameName))
			{
				copy(MaterialCaches[NewObject->m_FrameName].begin(), MaterialCaches[NewObject->m_FrameName].end(), back_inserter(NewObject->m_Materials));
			}
		}
		else if (Token == TEXT("<TransformMatrix>"))
		{
			InFile >> NewObject->m_TransformMatrix._11 >> NewObject->m_TransformMatrix._12 >> NewObject->m_TransformMatrix._13 >> NewObject->m_TransformMatrix._14;
			InFile >> NewObject->m_TransformMatrix._21 >> NewObject->m_TransformMatrix._22 >> NewObject->m_TransformMatrix._23 >> NewObject->m_TransformMatrix._24;
			InFile >> NewObject->m_TransformMatrix._31 >> NewObject->m_TransformMatrix._32 >> NewObject->m_TransformMatrix._33 >> NewObject->m_TransformMatrix._34;
			InFile >> NewObject->m_TransformMatrix._41 >> NewObject->m_TransformMatrix._42 >> NewObject->m_TransformMatrix._43 >> NewObject->m_TransformMatrix._44;
		}
		else if (Token == TEXT("<SkinInfo>"))
		{
			InFile >> Token;

			if (Token != TEXT("Null"))
			{
				shared_ptr<CSkinnedMesh> SkinnedMesh{ make_shared<CSkinnedMesh>() };

				SkinnedMesh->LoadSkinInfoFromFile(D3D12Device, D3D12GraphicsCommandList, InFile);

				InFile >> Token;

				if (Token == TEXT("<Mesh>"))
				{
					SkinnedMesh->LoadMeshInfoFromFile(D3D12Device, D3D12GraphicsCommandList, InFile);
				}

				NewObject->SetMesh(SkinnedMesh);
			 	NewObject->SetBoundingBox(make_shared<BoundingBox>());
				MeshCaches.emplace(NewObject->m_FrameName, SkinnedMesh);

				IsSkinnedMesh = true;
			}
		}
		else if (Token == TEXT("<Mesh>"))
		{
			shared_ptr<CMesh> Mesh{ make_shared<CMesh>() };

			Mesh->LoadMeshInfoFromFile(D3D12Device, D3D12GraphicsCommandList, InFile);
			NewObject->SetMesh(Mesh);
			NewObject->SetBoundingBox(make_shared<BoundingBox>());
			MeshCaches.emplace(NewObject->m_FrameName, Mesh);
		}
		else if (Token == TEXT("<Materials>"))
		{
			UINT MaterialCount{};

			InFile >> MaterialCount;

			if (MaterialCount > 0)
			{
				NewObject->m_Materials.reserve(MaterialCount);

				for (UINT i = 0; i < MaterialCount; ++i)
				{
					shared_ptr<CMaterial> Material{ make_shared<CMaterial>() };

					Material->LoadMaterialFromFile(D3D12Device, D3D12GraphicsCommandList, InFile, IsSkinnedMesh);
					NewObject->m_Materials.push_back(Material);
				}

				MaterialCaches.emplace(NewObject->m_FrameName, NewObject->m_Materials);
			}
		}
		else if (Token == TEXT("<ChildCount>"))
		{
			UINT ChildCount{};

			InFile >> ChildCount;

			if (ChildCount > 0)
			{
				for (UINT i = 0; i < ChildCount; ++i)
				{
					shared_ptr<CGameObject> ChildObject{ CGameObject::LoadObjectInfoFromFile(D3D12Device, D3D12GraphicsCommandList, InFile, MeshCaches, MaterialCaches) };

					if (ChildObject)
					{
						NewObject->SetChild(ChildObject);
					}
				}
			}
		}
		else if (Token == TEXT("</Frame>"))
		{
			break;
		}
	}
#endif

	return NewObject;
}

void CGameObject::LoadAnimationInfoFromFile(tifstream& InFile, const shared_ptr<LOADED_MODEL_INFO>& ModelInfo)
{
	tstring Token{};
	unordered_map<tstring, shared_ptr<CGameObject>> BoneFrameCaches{};

	while (InFile >> Token)
	{
		if (Token == TEXT("<FrameNames>"))
		{
			UINT SkinnedMeshFrameCount{};

			InFile >> SkinnedMeshFrameCount;
			ModelInfo->m_SkinnedMeshCaches.reserve(SkinnedMeshFrameCount);
			ModelInfo->m_BoneFrameCaches.resize(SkinnedMeshFrameCount);

			for (UINT i = 0; i < SkinnedMeshFrameCount; ++i)
			{
				InFile >> Token;
				ModelInfo->m_SkinnedMeshCaches.push_back(ModelInfo->m_Model->FindSkinnedMesh(Token));

				UINT BoneCount{};

				InFile >> BoneCount;
				ModelInfo->m_BoneFrameCaches[i].reserve(BoneCount);

				for (UINT j = 0; j < BoneCount; ++j)
				{
					InFile >> Token;
					
					if (BoneFrameCaches.count(Token))
					{
						ModelInfo->m_BoneFrameCaches[i].push_back(BoneFrameCaches[Token]);
					}
					else
					{
						shared_ptr<CGameObject> Frame{ ModelInfo->m_Model->FindFrame(Token) };

						ModelInfo->m_BoneFrameCaches[i].push_back(Frame);
						BoneFrameCaches.emplace(Token, Frame);
					}
				}

				ModelInfo->m_SkinnedMeshCaches.back()->SetBoneFrameCaches(ModelInfo->m_BoneFrameCaches[i]);
			}
		}
		else if (Token == TEXT("<AnimationClips>"))
		{
			UINT AnimationClipCount{};

			InFile >> AnimationClipCount;
			ModelInfo->m_AnimationClips.reserve(AnimationClipCount);

			for (UINT i = 0; i < AnimationClipCount; ++i)
			{
				shared_ptr<CAnimationClip> AnimationClip{ make_shared<CAnimationClip>() };

				AnimationClip->LoadAnimationClipInfoFromFile(InFile, ModelInfo);
				ModelInfo->m_AnimationClips.push_back(AnimationClip);
			}
		}
		else if (Token == TEXT("</Animation>"))
		{
			break;
		}
	}
}

shared_ptr<CGameObject> CGameObject::FindFrame(const tstring& FrameName)
{
	shared_ptr<CGameObject> Object{};

	if (m_FrameName == FrameName)
	{
		return shared_from_this();
	}

	if (m_SiblingObject)
	{
		if (Object = m_SiblingObject->FindFrame(FrameName))
		{
			return Object;
		}
	}

	if (m_ChildObject)
	{
		if (Object = m_ChildObject->FindFrame(FrameName))
		{
			return Object;
		}
	}

	return Object;
}

shared_ptr<CSkinnedMesh> CGameObject::FindSkinnedMesh(const tstring& SkinnedMeshName)
{
	shared_ptr<CSkinnedMesh> SkinnedMesh{};

	if (m_Mesh)
	{
		if (typeid(*m_Mesh) == typeid(CSkinnedMesh))
		{
			if (m_Mesh->GetName() == SkinnedMeshName)
			{
				return static_pointer_cast<CSkinnedMesh>(m_Mesh);
			}
		}
	}

	if (m_SiblingObject)
	{
		if (SkinnedMesh = m_SiblingObject->FindSkinnedMesh(SkinnedMeshName))
		{
			return SkinnedMesh;
		}
	}

	if (m_ChildObject)
	{
		if (SkinnedMesh = m_ChildObject->FindSkinnedMesh(SkinnedMeshName))
		{
			return SkinnedMesh;
		}
	}

	return SkinnedMesh;
}

void CGameObject::CreateShaderVariables(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{

}

void CGameObject::UpdateShaderVariables(ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	XMFLOAT4X4 WorldMatrix{};

	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_WorldMatrix)));
	D3D12GraphicsCommandList->SetGraphicsRoot32BitConstants(ROOT_PARAMETER_TYPE_OBJECT, 16, &WorldMatrix, 0);
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

	if (m_SiblingObject)
	{
		m_SiblingObject->ReleaseUploadBuffers();
	}

	if (m_ChildObject)
	{
		m_ChildObject->ReleaseUploadBuffers();
	}
}

void CGameObject::Move(const XMFLOAT3& Direction, float Distance)
{
	XMFLOAT3 Shift{ Vector3::ScalarProduct(Distance, Direction, false) };

	SetPosition(Vector3::Add(GetPosition(), Shift));
}

void CGameObject::Animate(float ElapsedTime)
{
	if (IsActive())
	{
		if (m_AnimationController)
		{
			m_AnimationController->UpdateAnimationClip(ElapsedTime, shared_from_this());
		}
	}
}

void CGameObject::Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera, RENDER_TYPE RenderType)
{
	if (IsActive())
	{
		if (m_AnimationController)
		{
			m_AnimationController->UpdateShaderVariables(D3D12GraphicsCommandList);
		}

		if (IsVisible(Camera))
		{
			if (m_Mesh)
			{
				UpdateShaderVariables(D3D12GraphicsCommandList);

				UINT MaterialCount{ static_cast<UINT>(m_Materials.size()) };

				for (UINT i = 0; i < MaterialCount; ++i)
				{
					if (m_Materials[i])
					{
						m_Materials[i]->SetPipelineState(D3D12GraphicsCommandList, RenderType);
						m_Materials[i]->UpdateShaderVariables(D3D12GraphicsCommandList);
					}

					m_Mesh->Render(D3D12GraphicsCommandList, i);
				}
			}
		}

		if (m_SiblingObject)
		{
			m_SiblingObject->Render(D3D12GraphicsCommandList, Camera, RenderType);
		}

		if (m_ChildObject)
		{
			m_ChildObject->Render(D3D12GraphicsCommandList, Camera, RenderType);
		}
	}
}

bool CGameObject::IsActive() const
{
	return m_IsActive;
}

void CGameObject::SetActive(bool IsActive)
{
	m_IsActive = IsActive;
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

void CGameObject::SetMaterial(const shared_ptr<CMaterial>& Material)
{
	if (Material)
	{
		m_Materials.push_back(Material);
	}
}

void CGameObject::SetBoundingBox(const shared_ptr<BoundingBox>& BoundingBox)
{
	if (BoundingBox)
	{
		m_BoundingBox = BoundingBox;
	}
}

shared_ptr<BoundingBox> CGameObject::GetBoundingBox()
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

void CGameObject::SetAnimationController(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, const shared_ptr<LOADED_MODEL_INFO>& ModelInfo)
{
	if (ModelInfo)
	{
		m_AnimationController = make_shared<CAnimationController>(D3D12Device, D3D12GraphicsCommandList, ModelInfo);
	}
}

void CGameObject::SetAnimationClip(UINT ClipNum)
{
	if (m_AnimationController)
	{
		m_AnimationController->ChangeAnimationClip(ClipNum);
	}
}

bool CGameObject::IsVisible(CCamera* Camera) const
{
	bool Visible{};

	if (Camera)
	{
		if (m_Mesh)
		{
			BoundingBox BoundingBox{ m_Mesh->GetBoundingBox() };

			BoundingBox.Transform(BoundingBox, XMLoadFloat4x4(&m_WorldMatrix));
			Visible = Camera->IsInBoundingFrustum(BoundingBox);
		}
	}

	return Visible;
}

void CGameObject::UpdateBoundingBox()
{
	if (m_Mesh && m_BoundingBox)
	{
		m_Mesh->GetBoundingBox().Transform(*m_BoundingBox, XMLoadFloat4x4(&m_WorldMatrix));
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

	UpdateBoundingBox();
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

void CGameObject::RenderBoundingBox(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera)
{
	if (IsActive())
	{
		if (IsVisible(Camera))
		{
			if (m_Mesh)
			{
				UpdateShaderVariables(D3D12GraphicsCommandList);
				m_Mesh->RenderBoundingBox(D3D12GraphicsCommandList);
			}
		}

		if (m_SiblingObject)
		{
			m_SiblingObject->RenderBoundingBox(D3D12GraphicsCommandList, Camera);
		}

		if (m_ChildObject)
		{
			m_ChildObject->RenderBoundingBox(D3D12GraphicsCommandList, Camera);
		}
	}
}
