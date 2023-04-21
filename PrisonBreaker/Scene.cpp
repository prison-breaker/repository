#include "pch.h"
#include "Scene.h"

#include "AssetManager.h"
#include "CameraManager.h"

#include "Player.h"
#include "Guard.h"
#include "UI.h"

#include "Animator.h"
#include "Transform.h"

#include "Camera.h"

CScene::CScene() :
	m_name(),
	m_objects()
{
}

CScene::~CScene()
{
	ReleaseShaderVariables();

	for (int i = 0; i < (int)GROUP_TYPE::COUNT; ++i)
	{
		Utility::SafeDelete(m_objects[i]);
	}
}

void CScene::Load(ID3D12Device* d3d12Device, ID3D12GraphicsCommandList* d3d12GraphicsCommandList, const string& fileName)
{
	string filePath = CAssetManager::GetInstance()->GetAssetPath() + "Scene\\" + fileName;
	ifstream in(filePath, ios::binary);
	string str, modelFileName;
	GROUP_TYPE groupType;

	while (true)
	{
		File::ReadStringFromFile(in, str);

		if (str == "<GroupType>")
		{
			in.read(reinterpret_cast<char*>(&groupType), sizeof(int));
		}
		else if (str == "<FileName>")
		{
			File::ReadStringFromFile(in, modelFileName);
		}
		else if (str == "<Instance>")
		{
			int instanceCount = 0;

			in.read(reinterpret_cast<char*>(&instanceCount), sizeof(int));

			for (int i = 0; i < instanceCount; ++i)
			{
				LoadedModel loadedModel = CObject::Load(d3d12Device, d3d12GraphicsCommandList, modelFileName);

				// <IsActive>
				File::ReadStringFromFile(in, str);

				int isActive = 0;
				in.read(reinterpret_cast<char*>(&isActive), sizeof(int));

				// <Transform>
				File::ReadStringFromFile(in, str);

				// <Position>
				XMFLOAT3 position = {};

				File::ReadStringFromFile(in, str);
				in.read(reinterpret_cast<char*>(&position), sizeof(XMFLOAT3));

				// <Rotation>
				XMFLOAT3 rotation = {};

				File::ReadStringFromFile(in, str);
				in.read(reinterpret_cast<char*>(&rotation), sizeof(XMFLOAT3));

				// <Scale>
				XMFLOAT3 scale = {};

				File::ReadStringFromFile(in, str);
				in.read(reinterpret_cast<char*>(&scale), sizeof(XMFLOAT3));

				switch (groupType)
				{
				case GROUP_TYPE::TERRAIN:
				case GROUP_TYPE::STRUCTURE:
				{
					CObject* object = new CObject();
					CTransform* transform = object->GetComponent<CTransform>();

					object->SetActive(isActive);
					transform->SetPosition(position);
					transform->SetRotation(rotation);
					transform->SetScale(scale);
					object->AddChild(loadedModel.m_rootFrame);
					object->Init();
					AddObject(groupType, object);
				}
					break;
				case GROUP_TYPE::ENEMY:
				{
					CGuard* guard = new CGuard();
					CTransform* transform = guard->GetComponent<CTransform>();

					guard->SetActive(isActive);
					guard->SetComponent(COMPONENT_TYPE::ANIMATOR, loadedModel.m_animator);
					transform->SetPosition(position);
					transform->SetRotation(rotation);
					transform->SetScale(scale);
					guard->AddChild(loadedModel.m_rootFrame);
					guard->Init();

					// <TargetPosition>
					XMFLOAT3 targetPosition = {};

					File::ReadStringFromFile(in, str);
					in.read(reinterpret_cast<char*>(&targetPosition), sizeof(XMFLOAT3));
					guard->CreatePatrolPath(targetPosition);
					AddObject(groupType, guard);
				}
					break;
				case GROUP_TYPE::PLAYER:
				{
					CPlayer* player = new CPlayer();
					CTransform* transform = player->GetComponent<CTransform>();

					player->SetActive(isActive);
					player->SetComponent(COMPONENT_TYPE::ANIMATOR, loadedModel.m_animator);
					transform->SetPosition(position);
					transform->SetRotation(rotation);
					transform->SetScale(scale);
					player->AddChild(loadedModel.m_rootFrame);
					player->Init();
					AddObject(groupType, player);
				}
					break;
				}
			}
		}
		else if (str == "</Scene>")
		{
			cout << endl;
			break;
		}
	}
}

void CScene::LoadUI(ID3D12Device* d3d12Device, ID3D12GraphicsCommandList* d3d12GraphicsCommandList, const string& fileName)
{
	string filePath = CAssetManager::GetInstance()->GetAssetPath() + "Scene\\" + fileName;
	ifstream in(filePath, ios::binary);
	string str;

	while (true)
	{
		File::ReadStringFromFile(in, str);

		if (str == "<UI>")
		{
			//CUI* ui = CUI::Load(d3d12Device, d3d12GraphicsCommandList, in);

			//AddObject(GROUP_TYPE::UI, ui);
		}
		else if (str == "</UIs>")
		{
			cout << endl;
			break;
		}
	}
}

void CScene::CreateShaderVariables(ID3D12Device* d3d12Device, ID3D12GraphicsCommandList* d3d12GraphicsCommandList)
{
}

void CScene::UpdateShaderVariables(ID3D12GraphicsCommandList* d3d12GraphicsCommandList)
{
}

void CScene::ReleaseShaderVariables()
{
}

void CScene::SetName(const string& name)
{
	m_name = name;
}

const string& CScene::GetName()
{
	return m_name;
}

void CScene::AddObject(GROUP_TYPE groupType, CObject* object)
{
	if (object != nullptr)
	{
		m_objects[static_cast<int>(groupType)].push_back(object);
	}
}

const vector<CObject*>& CScene::GetGroupObject(GROUP_TYPE groupType)
{
	return m_objects[static_cast<int>(groupType)];
}

void CScene::DeleteGroupObject(GROUP_TYPE groupType)
{
	Utility::SafeDelete(m_objects[static_cast<int>(groupType)]);
}

void CScene::ReleaseUploadBuffers()
{
	for (int i = 0; i < (int)GROUP_TYPE::COUNT; ++i)
	{
		for (const auto& object : m_objects[i])
		{
			object->ReleaseUploadBuffers();
		}
	}
}

void CScene::Update()
{
	for (int i = 0; i < (int)GROUP_TYPE::COUNT; ++i)
	{
		for (const auto& object : m_objects[i])
		{
			if (object->IsActive() && !object->IsDeleted())
			{
				object->Update();
			}
		}
	}
}

void CScene::LateUpdate()
{
	for (int i = 0; i < (int)GROUP_TYPE::COUNT; ++i)
	{
		for (const auto& object : m_objects[i])
		{
			if (object->IsActive() && !object->IsDeleted())
			{
				object->LateUpdate();
			}
		}
	}
}

void CScene::PreRender(ID3D12GraphicsCommandList* d3d12GraphicsCommandList)
{
}

void CScene::Render(ID3D12GraphicsCommandList* d3d12GraphicsCommandList)
{
	CCamera* camera = CCameraManager::GetInstance()->GetMainCamera();

	camera->RSSetViewportsAndScissorRects(d3d12GraphicsCommandList);
	camera->UpdateShaderVariables(d3d12GraphicsCommandList);

	for (int i = 0; i <= (int)GROUP_TYPE::BILBOARD; ++i)
	{
		for (const auto& object : m_objects[i])
		{
			if (object->IsActive() && !object->IsDeleted())
			{
				object->Render(d3d12GraphicsCommandList, camera);
			}
		}
	}

	camera = CCameraManager::GetInstance()->GetUICamera();
	camera->RSSetViewportsAndScissorRects(d3d12GraphicsCommandList);
	camera->UpdateShaderVariables(d3d12GraphicsCommandList);

	for (const auto& object : m_objects[(int)GROUP_TYPE::UI])
	{
		if (object->IsActive() && !object->IsDeleted())
		{
			object->Render(d3d12GraphicsCommandList, camera);
		}
	}
}

void CScene::PostRender(ID3D12GraphicsCommandList* d3d12GraphicsCommandList)
{
}
