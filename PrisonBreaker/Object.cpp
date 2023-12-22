#include "pch.h"
#include "Object.h"

#include "AssetManager.h"

#include "Player.h"
#include "Guard.h"
#include "UI.h"
#include "Trigger.h"

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

	// ��� ��ü�� Transform ������Ʈ�� ������.
	CreateComponent(COMPONENT_TYPE::TRANSFORM);
}

CObject::~CObject()
{
	// AssetManager�� ���� ���͸����� �����ϰ� �ְ�, ��� ������Ʈ�� �ش� ���͸����� ������ �ν��Ͻ��� ������ ������, �� Ŭ�������� �Ҹ�����־�� �Ѵ�.
	Utility::SafeDelete(m_materials);
	Utility::SafeDelete(m_components);
	Utility::SafeDelete(m_children);
}

CObject* CObject::Load(const string& fileName)
{
	CObject* object = {};
	string filePath = CAssetManager::GetInstance()->GetAssetPath() + "Model\\" + fileName;
	ifstream in(filePath, ios::binary);
	string str;

	while (true)
	{
		File::ReadStringFromFile(in, str);

		if (str == "<Frames>")
		{
			cout << fileName << " �� �ε� ����...\n";
			object = CObject::LoadFrame(in);
		}
		else if (str == "</Frames>")
		{
			cout << fileName << " �� �ε� �Ϸ�...\n";
			break;
		}
	}

	File::ReadStringFromFile(in, str);

	if (str == "<Animator>")
	{
		CAnimator* animator = static_cast<CAnimator*>(object->CreateComponent(COMPONENT_TYPE::ANIMATOR));

		animator->Load(in);
	}

	return object;
}

CObject* CObject::LoadFrame(ifstream& in)
{
	CObject* object = nullptr;
	string str;

	while (true)
	{
		File::ReadStringFromFile(in, str);

		if (str == "<ClassType>")
		{
			int classType = 0;

			in.read(reinterpret_cast<char*>(&classType), sizeof(int));

			switch (classType)
			{
			// Character
			case 0: object = new CObject(); break;
			case 1: object = new CPlayer(); break;
			case 2: object = new CGuard(); break;

			// Trigger
			case 3: object = new COpenDoorTrigger(); break;
			case 4: object = new CPowerDownTrigger(); break;
			case 5: object = new COperateSirenTrigger(); break;
			case 6: object = new COpenGateTrigger(); break;
			}
		}
		else if (str == "<Name>")
		{
			File::ReadStringFromFile(in, object->m_name);
		}
		else if (str == "<IsActive>")
		{
			in.read(reinterpret_cast<char*>(&object->m_isActive), sizeof(int));
		}
		else if (str == "<Transform>")
		{
			// localPosition, localRotation, localScale
			XMFLOAT3 t[3] = {};

			in.read(reinterpret_cast<char*>(&t[0]), 3 * sizeof(XMFLOAT3));

			CTransform* transform = static_cast<CTransform*>(object->GetComponent(COMPONENT_TYPE::TRANSFORM));

			transform->SetLocalPosition(t[0]);
			transform->SetLocalRotation(t[1]);
			transform->SetLocalScale(t[2]);
		}
		else if (str == "<Mesh>")
		{
			File::ReadStringFromFile(in, str);
			
			CMesh* mesh = CAssetManager::GetInstance()->GetMesh(str);

			object->SetMesh(mesh);
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
					
					// ���͸��� �ν��Ͻ��� �����ϰ� �߰��Ѵ�.
					CMaterial* material = CAssetManager::GetInstance()->CreateMaterialInstance(str);

					object->AddMaterial(material);
				}
			}
		}
		else if (str == "<Collider>")
		{
			XMFLOAT3 center = {};
			XMFLOAT3 extents = {};

			in.read(reinterpret_cast<char*>(&center), sizeof(XMFLOAT3));
			in.read(reinterpret_cast<char*>(&extents), sizeof(XMFLOAT3));

			// �޽��� ���� �������� �ݶ��̴� ������Ʈ�� �����Ѵ�.
			object->CreateComponent(COMPONENT_TYPE::COLLIDER);
			
			CCollider* collider = static_cast<CCollider*>(object->GetComponent(COMPONENT_TYPE::COLLIDER));

			collider->SetBoundingBox(center, extents);
		}
		else if (str == "<ChildCount>")
		{
			int childCount = 0;

			in.read(reinterpret_cast<char*>(&childCount), sizeof(int));

			if (childCount > 0)
			{
				for (int i = 0; i < childCount; ++i)
				{
					CObject* child = CObject::LoadFrame(in);

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
	// �̹� �ش� ������Ʈ�� ������ �־��ٸ�, ���� �� ���� �����Ѵ�.
	if (m_components[static_cast<int>(componentType)] != nullptr)
	{
		delete m_components[static_cast<int>(componentType)];
		m_components[static_cast<int>(componentType)] = nullptr;
	}

	switch (componentType)
	{
	case COMPONENT_TYPE::STATE_MACHINE:
		m_components[static_cast<int>(componentType)] = new CStateMachine();
		break;
	case COMPONENT_TYPE::RIGIDBODY:
		m_components[static_cast<int>(componentType)] = new CRigidBody();
		break;
	case COMPONENT_TYPE::ANIMATOR:
		m_components[static_cast<int>(componentType)] = new CSkinningAnimator();
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

	return m_components[static_cast<int>(componentType)];
}

CComponent* CObject::GetComponent(COMPONENT_TYPE componentType)
{
	return m_components[static_cast<int>(componentType)];
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

void CObject::CreateShaderVariables()
{
}

void CObject::UpdateShaderVariables()
{
	for (const auto& component : m_components)
	{
		if (component != nullptr)
		{
			component->UpdateShaderVariables();
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
		// ������ �ٿ���ڽ��� ������ �˻��Ѵ�.
		bool isIntersected = collider->GetBoundingBox().Intersects(XMLoadFloat3(&rayOrigin), XMLoadFloat3(&rayDirection), hitDistance);

		if (isIntersected)
		{
			// ������ �ٿ�� �ڽ� ������ �Ÿ��� MaxDistance���� �۰ų� ���ٸ� ������ �޽�(�ﰢ��)�� ������ �˻��Ѵ�.
			if (hitDistance <= maxDistance)
			{
				CTransform* transform = static_cast<CTransform*>(GetComponent(COMPONENT_TYPE::TRANSFORM));

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
	cout << "OnCollisionEnter()\n";
}

void CObject::OnCollision(CObject* collidedObject)
{
}

void CObject::OnCollisionExit(CObject* collidedObject)
{
	cout << "OnCollisionExit()\n";
}

void CObject::Update()
{
	for (const auto& component : m_components)
	{
		if (component != nullptr)
		{
			component->Update();
		}
	}

	for (const auto& child : m_children)
	{
		if ((child->m_isActive) && (!child->m_isDeleted))
		{
			child->Update();
		}
	}
}

void CObject::PreRender(CCamera* camera)
{
	UpdateShaderVariables();

	// DepthWrite�� ���, ���� ������ȯ ����� ����ϱ� ������ �������� �ø��� ������ �� ����.
	if (m_mesh != nullptr)
	{
		for (int i = 0; i < m_materials.size(); ++i)
		{
			m_materials[i]->SetPipelineState(RENDER_TYPE::DEPTH_WRITE);
			m_mesh->Render(i);
		}
	}

	for (const auto& child : m_children)
	{
		if (child->IsActive() && !child->IsDeleted())
		{
			child->PreRender(camera);
		}
	}
}

void CObject::Render(CCamera* camera)
{
	UpdateShaderVariables();

	if (IsVisible(camera))
	{
		if (m_mesh != nullptr)
		{
			for (int i = 0; i < m_materials.size(); ++i)
			{
				m_materials[i]->SetPipelineState(RENDER_TYPE::STANDARD);
				m_materials[i]->UpdateShaderVariables();
				m_mesh->Render(i);
			}
		}
	}

	// [Debug] Render BoundingBox
	CComponent* collider = GetComponent(COMPONENT_TYPE::COLLIDER);

	if (collider != nullptr)
	{
		collider->Render(camera);
	}

	for (const auto& child : m_children)
	{
		if (child->IsActive() && !child->IsDeleted())
		{
			child->Render(camera);
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