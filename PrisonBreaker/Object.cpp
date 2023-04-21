#include "pch.h"
#include "Object.h"

#include "AssetManager.h"

#include "Mesh.h"
#include "Shader.h"
#include "Material.h"

#include "StateMachine.h"
#include "RigidBody.h"
#include "Animator.h"
#include "Transform.h"
#include "SpriteRenderer.h"
#include "Collider.h"

#include "Camera.h"

UINT CObject::m_nextInstanceID = 0;

CObject::CObject() :
	m_instanceID(m_nextInstanceID++),
	m_name(),
	m_isActive(),
	m_isDeleted(),
	m_mesh(),
	m_materials(),
	m_components(),
	m_parent(),
	m_children()
{
	m_components.resize(static_cast<size_t>(COMPONENT_TYPE::COUNT));

	// 모든 객체는 Transform 컴포넌트를 가진다.
	CreateComponent(COMPONENT_TYPE::TRANSFORM);
}

CObject::~CObject()
{
	Utility::SafeDelete(m_components);
	Utility::SafeDelete(m_children);
}

LoadedModel CObject::Load(ID3D12Device* d3d12Device, ID3D12GraphicsCommandList* d3d12GraphicsCommandList, const string& fileName)
{
	LoadedModel loadedModel = {};
	string filePath = CAssetManager::GetInstance()->GetAssetPath() + "Model\\" + fileName;
	ifstream in(filePath, ios::binary);
	string str;

	while (true)
	{
		File::ReadStringFromFile(in, str);

		if (str == "<Frames>")
		{
			cout << fileName << " 모델 로드 시작...\n";
			loadedModel.m_rootFrame = CObject::LoadFrame(d3d12Device, d3d12GraphicsCommandList, in);
		}
		else if (str == "</Frames>")
		{
			cout << fileName << " 모델 로드 완료...\n";
			break;
		}
	}

	File::ReadStringFromFile(in, str);

	if (str == "<Animator>")
	{
		loadedModel.m_animator = new CAnimator();

		// 임시적으로 rootFrame을 owner로 지정하여 뼈정보를 저장시켜 놓는다.
		loadedModel.m_animator->SetOwner(loadedModel.m_rootFrame);
		loadedModel.m_animator->Load(d3d12Device, d3d12GraphicsCommandList, in);
	}

	return loadedModel;
}

CObject* CObject::LoadFrame(ID3D12Device* d3d12Device, ID3D12GraphicsCommandList* d3d12GraphicsCommandList, ifstream& in)
{
	CObject* object = nullptr;
	string str;

	while (true)
	{
		File::ReadStringFromFile(in, str);

		if (str == "<Frame>")
		{
			object = new CObject();
		}
		else if (str == "<Name>")
		{
			File::ReadStringFromFile(in, object->m_name);
		}
		else if (str == "<IsActive>")
		{
			int isActive = 0;

			in.read(reinterpret_cast<char*>(&isActive), sizeof(int));
			object->SetActive(isActive);
		}
		else if (str == "<Transform>")
		{
			// <LocalPosition>
			XMFLOAT3 localPosition = {};

			File::ReadStringFromFile(in, str);
			in.read(reinterpret_cast<char*>(&localPosition), sizeof(XMFLOAT3));

			// <LocalRotation>
			XMFLOAT3 localRotation = {};

			File::ReadStringFromFile(in, str);
			in.read(reinterpret_cast<char*>(&localRotation), sizeof(XMFLOAT3));

			// <LocalScale>
			XMFLOAT3 localScale = {};

			File::ReadStringFromFile(in, str);
			in.read(reinterpret_cast<char*>(&localScale), sizeof(XMFLOAT3));

			CTransform* transform = object->GetComponent<CTransform>();

			transform->SetLocalPosition(localPosition);
			transform->SetLocalRotation(localRotation);
			transform->SetLocalScale(localScale);
		}
		else if (str == "<Mesh>")
		{
			File::ReadStringFromFile(in, str);
			
			CMesh* mesh = CAssetManager::GetInstance()->GetMesh(str);

			object->SetMesh(mesh);

			// 메쉬를 가진 프레임은 콜라이더 컴포넌트를 생성한다.
			object->CreateComponent(COMPONENT_TYPE::COLLIDER);
		}
		else if (str == "<Materials>")
		{
			int materialCount = 0;

			in.read(reinterpret_cast<char*>(&materialCount), sizeof(int));

			if (materialCount > 0)
			{
				object->m_materials.reserve(materialCount);

				// <Material>
				File::ReadStringFromFile(in, str);

				for (int i = 0; i < materialCount; ++i)
				{
					File::ReadStringFromFile(in, str);
					
					CMaterial* material = CAssetManager::GetInstance()->GetMaterial(str);

					object->AddMaterial(material);
				}
			}
		}
		else if (str == "<ChildCount>")
		{
			int childCount = 0;

			in.read(reinterpret_cast<char*>(&childCount), sizeof(int));

			if (childCount > 0)
			{
				for (int i = 0; i < childCount; ++i)
				{
					CObject* child = CObject::LoadFrame(d3d12Device, d3d12GraphicsCommandList, in);

					if (child != nullptr)
					{
						object->AddChild(child);
					}
				}
			}
		}
		else if (str == "</Frame>")
		{
			break;
		}
	}

	return object;
}

UINT CObject::GetInstanceID()
{
	return m_instanceID;
}

void CObject::SetName(const string& name)
{
	m_name = name;
}

const string& CObject::GetName()
{
	return m_name;
}

void CObject::SetActive(bool isActive)
{
	m_isActive = isActive;
}

bool CObject::IsActive()
{
	return m_isActive;
}

void CObject::SetDeleted(bool isDeleted)
{
	m_isDeleted = isDeleted;
}

bool CObject::IsDeleted()
{
	return m_isDeleted;
}

void CObject::SetMesh(CMesh* mesh)
{
	m_mesh = mesh;
}

CMesh* CObject::GetMesh()
{
	return m_mesh;
}

void CObject::AddMaterial(CMaterial* material)
{
	if (material != nullptr)
	{
		m_materials.push_back(material);
	}
}

const vector<CMaterial*>& CObject::GetMaterials()
{
	return m_materials;
}

CComponent* CObject::CreateComponent(COMPONENT_TYPE componentType)
{
	if (m_components[static_cast<int>(componentType)] == nullptr)
	{
		switch (componentType)
		{
		case COMPONENT_TYPE::STATE_MACHINE:
			m_components[static_cast<int>(componentType)] = new CStateMachine();
			break;
		case COMPONENT_TYPE::RIGIDBODY:
			m_components[static_cast<int>(componentType)] = new CRigidBody();
			break;
		case COMPONENT_TYPE::ANIMATOR:
			m_components[static_cast<int>(componentType)] = new CAnimator();
			break;
		case COMPONENT_TYPE::TRANSFORM:
			m_components[static_cast<int>(componentType)] = new CTransform();
			break;
		case COMPONENT_TYPE::COLLIDER:
			m_components[static_cast<int>(componentType)] = new CCollider();
			break;
		case COMPONENT_TYPE::SPRITE_RENDERER:
			m_components[static_cast<int>(componentType)] = new CSpriteRenderer();
			break;
		}

		m_components[static_cast<int>(componentType)]->SetOwner(this);
	}

	return m_components[static_cast<int>(componentType)];
}

void CObject::SetComponent(COMPONENT_TYPE componentType, CComponent* newComponent)
{
	// 기존에 해당 타입의 컴포넌트를 가지고 있었다면 소멸시킨다.
	if (m_components[static_cast<int>(componentType)] != nullptr)
	{
		delete m_components[static_cast<int>(componentType)];
		m_components[static_cast<int>(componentType)] = nullptr;
	}

	if (newComponent != nullptr)
	{
		// newComponent의 owner가 있었다면, 연결을 해지시킨다.
		CObject* owner = newComponent->GetOwner();

		if (owner != nullptr)
		{
			owner->SetComponent(componentType, nullptr);
		}

		newComponent->SetOwner(this);
	}
	
	m_components[static_cast<int>(componentType)] = newComponent;
}

const vector<CComponent*>& CObject::GetComponents()
{
	return m_components;
}

CObject* CObject::GetParent()
{
	return m_parent;
}

void CObject::AddChild(CObject* object)
{
	if (object != nullptr)
	{
		m_children.push_back(object);
		object->m_parent = this;
	}
}

const vector<CObject*>& CObject::GetChildren()
{
	return m_children;
}

void CObject::Init()
{
}

void CObject::CreateShaderVariables(ID3D12Device* d3d12Device, ID3D12GraphicsCommandList* d3d12GraphicsCommandList)
{
}

void CObject::UpdateShaderVariables(ID3D12GraphicsCommandList* d3d12GraphicsCommandList)
{
	for (const auto& component : m_components)
	{
		if (component != nullptr)
		{
			component->UpdateShaderVariables(d3d12GraphicsCommandList);
		}
	}
}

void CObject::ReleaseShaderVariables()
{
}

void CObject::ReleaseUploadBuffers()
{
}

CObject* CObject::FindFrame(const string& name)
{
	CObject* object = nullptr;

	if (m_name == name)
	{
		return this;
	}

	for (const auto& child : m_children)
	{
		object = child->FindFrame(name);

		if (object != nullptr)
		{
			break;
		}
	}

	return object;
}

CObject* CObject::CheckRayIntersection(const XMFLOAT3& rayOrigin, const XMFLOAT3& rayDirection, float& hitDistance, float maxDistance)
{
	CCollider* collider = static_cast<CCollider*>(m_components[static_cast<int>(COMPONENT_TYPE::COLLIDER)]);

	if ((m_mesh != nullptr) && (collider != nullptr))
	{
		// 광선과 바운딩박스의 교차를 검사한다.
		bool isIntersected = collider->GetBoundingBox().Intersects(XMLoadFloat3(&rayOrigin), XMLoadFloat3(&rayDirection), hitDistance);

		if (isIntersected)
		{
			// 광원과 바운딩 박스 사이의 거리가 MaxDistance보다 작거나 같다면 광선과 메쉬(삼각형)의 교차를 검사한다.
			if (hitDistance <= maxDistance)
			{
				CTransform* transform = GetComponent<CTransform>();

				if (m_mesh->CheckRayIntersection(rayOrigin, rayDirection, XMLoadFloat4x4(&transform->GetWorldMatrix()), hitDistance))
				{
					return this;
				}
			}
		}
	}

	CObject* nearestIntersectedObject = nullptr;
	float nearestHitDistance = FLT_MAX;

	for (const auto& child : m_children)
	{
		CObject* intersectedObject = child->CheckRayIntersection(rayOrigin, rayDirection, hitDistance, maxDistance);

		if ((intersectedObject != nullptr) && (hitDistance < nearestHitDistance))
		{
			nearestIntersectedObject = intersectedObject;
			nearestHitDistance = hitDistance;
		}
	}

	if (nearestIntersectedObject != nullptr)
	{
		hitDistance = nearestHitDistance;
	}

	return nearestIntersectedObject;
}

bool CObject::IsVisible(CCamera* camera)
{
	CCollider* collider = static_cast<CCollider*>(m_components[static_cast<int>(COMPONENT_TYPE::COLLIDER)]);

	if ((camera != nullptr) && (collider != nullptr))
	{
		return camera->IsInBoundingFrustum(collider->GetBoundingBox());
	}

	return false;
}

void CObject::OnCollisionEnter(CObject* collidedObject)
{
}

void CObject::OnCollision(CObject* collidedObject)
{
}

void CObject::OnCollisionExit(CObject* collidedObject)
{
}

void CObject::Update()
{
	//if (m_components[static_cast<int>(COMPONENT_TYPE::STATE_MACHINE)] != nullptr)
	//{
	//	m_components[static_cast<int>(COMPONENT_TYPE::STATE_MACHINE)]->Update();
	//}

	//for (const auto& child : m_children)
	//{
	//	if (child->m_isActive && !child->m_isDeleted)
	//	{
	//		child->Update();
	//	}
	//}
}

void CObject::LateUpdate()
{
	for (int i = static_cast<int>(COMPONENT_TYPE::RIGIDBODY); i < static_cast<int>(COMPONENT_TYPE::COUNT); ++i)
	{
		if (m_components[i] != nullptr)
		{
			m_components[i]->Update();
		}
	}

	for (const auto& child : m_children)
	{
		if (child->m_isActive && !child->m_isDeleted)
		{
			child->LateUpdate();
		}
	}
}

void CObject::PreRender(ID3D12GraphicsCommandList* d3d12GraphicsCommandList, CCamera* camera)
{
	UpdateShaderVariables(d3d12GraphicsCommandList);

	// DepthWrite의 경우, 직교 투영변환 행렬을 사용하기 때문에 프러스텀 컬링을 수행할 수 없다.
	if (m_mesh != nullptr)
	{
		for (int i = 0; i < m_materials.size(); ++i)
		{
			m_materials[i]->SetPipelineState(d3d12GraphicsCommandList, RENDER_TYPE::DEPTH_WRITE);
			m_mesh->Render(d3d12GraphicsCommandList, i);
		}
	}

	for (const auto& child : m_children)
	{
		if (child->IsActive() && !child->IsDeleted())
		{
			child->PreRender(d3d12GraphicsCommandList, camera);
		}
	}
}

void CObject::Render(ID3D12GraphicsCommandList* d3d12GraphicsCommandList, CCamera* camera)
{
	UpdateShaderVariables(d3d12GraphicsCommandList);

	if (IsVisible(camera))
	{
		if (m_mesh != nullptr)
		{
			for (int i = 0; i < m_materials.size(); ++i)
			{
				m_materials[i]->SetPipelineState(d3d12GraphicsCommandList, RENDER_TYPE::STANDARD);
				m_materials[i]->UpdateShaderVariables(d3d12GraphicsCommandList);
				m_mesh->Render(d3d12GraphicsCommandList, i);
			}

			// [Debug] Render BoundingBox
			//CAssetManager::GetInstance()->GetShader("WireFrame")->SetPipelineState(d3d12GraphicsCommandList, 0);
			//m_mesh->RenderBoundingBox(d3d12GraphicsCommandList);
		}
	}

	for (const auto& child : m_children)
	{
		if (child->IsActive() && !child->IsDeleted())
		{
			child->Render(d3d12GraphicsCommandList, camera);
		}
	}
}

//void CObject::PlaySound(SOUND_TYPE SoundType, float Volume, float MaxHearingDistance)
//{
//	shared_ptr<CGameScene> GameScene{ static_pointer_cast<CGameScene>(CSceneManager::GetInstance()->GetScene("GameScene")) };
//	vector<vector<shared_ptr<CGameObject>>>& GameObjects{ GameScene->GetGameObjects() };
//	shared_ptr<CGameObject> MyPlayer{ GameObjects[OBJECT_TYPE_PLAYER][CCore::GetInstance()->GetSocketInfo().m_ID] };
//
//	float Distance{ Math::Distance(GetPosition(), MyPlayer->GetPosition()) };
//
//	Volume = Volume + ((2.0f - Distance) * (Volume / MaxHearingDistance));
//
//	CSoundManager::GetInstance()->Play(SoundType, Volume, true);
//}
