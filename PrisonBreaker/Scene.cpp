#include "pch.h"
#include "Scene.h"

#include "AssetManager.h"
#include "CameraManager.h"

#include "Player.h"
#include "Guard.h"
#include "UI.h"

#include "Animator.h"

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

		if (str == "<FileName>")
		{
			File::ReadStringFromFile(in, modelFileName);
		}
		else if (str == "<GroupType>")
		{
			in.read(reinterpret_cast<char*>(&groupType), sizeof(int));
		}
		else if (str == "<Instance>")
		{
			int instanceCount = 0;

			in.read(reinterpret_cast<char*>(&instanceCount), sizeof(int));

			// <Active>
			File::ReadStringFromFile(in, str);

			vector<int> isActive(instanceCount);

			in.read(reinterpret_cast<char*>(isActive.data()), sizeof(int) * instanceCount);

			// <TransformMatrix>
			File::ReadStringFromFile(in, str);

			vector<XMFLOAT4X4> transformMatrixes(instanceCount);

			in.read(reinterpret_cast<char*>(transformMatrixes.data()), sizeof(XMFLOAT4X4) * instanceCount);

			switch (groupType)
			{
			case GROUP_TYPE::TERRAIN:
			case GROUP_TYPE::STRUCTURE:
				for (int i = 0; i < instanceCount; ++i)
				{
					CObject* object = new CObject();
					LoadedModel loadedModel = CObject::Load(d3d12Device, d3d12GraphicsCommandList, modelFileName);

					object->SetActive(isActive[i]);
					object->SetTransformMatrix(transformMatrixes[i]);
					object->AddChild(loadedModel.m_rootFrame);
					object->UpdateTransform();
					object->Init();
					AddObject(groupType, object);
				}
				break;
			case GROUP_TYPE::ENEMY:
			{
				// <TargetPosition>
				File::ReadStringFromFile(in, str);

				vector<XMFLOAT3> targetPositions(instanceCount);

				in.read(reinterpret_cast<char*>(targetPositions.data()), sizeof(XMFLOAT3) * instanceCount);

				for (int i = 0; i < instanceCount; ++i)
				{
					CGuard* guard = new CGuard();
					LoadedModel loadedModel = CObject::Load(d3d12Device, d3d12GraphicsCommandList, modelFileName);

					guard->SetActive(isActive[i]);
					guard->SetTransformMatrix(transformMatrixes[i]);
					guard->SetComponent(COMPONENT_TYPE::ANIMATOR, loadedModel.m_animator);
					guard->AddChild(loadedModel.m_rootFrame);
					guard->UpdateTransform();
					guard->CreatePatrolPath(targetPositions[i]);
					guard->Init();
					AddObject(groupType, guard);

					//m_InitGameData.m_NPCInitTransformMatrixes.push_back(TransformMatrixes[i]);
				}
			}
				break;
			case GROUP_TYPE::PLAYER:
				for (int i = 0; i < instanceCount; ++i)
				{
					CPlayer* player = new CPlayer();
					LoadedModel loadedModel = CObject::Load(d3d12Device, d3d12GraphicsCommandList, modelFileName);

					player->SetActive(isActive[i]);
					player->SetTransformMatrix(transformMatrixes[i]);
					player->SetComponent(COMPONENT_TYPE::ANIMATOR, loadedModel.m_animator);
					player->AddChild(loadedModel.m_rootFrame);
					player->UpdateTransform();
					player->Init();
					AddObject(groupType, player);

					//object->SetID(playerID++);
					//m_InitGameData.m_PlayerInitTransformMatrixes.push_back(TransformMatrixes[i]);
				}
				break;
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
			CUI* ui = CUI::Load(d3d12Device, d3d12GraphicsCommandList, in);

			AddObject(GROUP_TYPE::UI, ui);
		}
		else if (str == "<UIs>")
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

void CScene::AddObject(GROUP_TYPE group, CObject* object)
{
	if (object != nullptr)
	{
		m_objects[(int)group].push_back(object);
	}
}

const vector<CObject*>& CScene::GetGroupObject(GROUP_TYPE group)
{
	return m_objects[(int)group];
}

void CScene::DeleteGroupObject(GROUP_TYPE group)
{
	Utility::SafeDelete(m_objects[(int)group]);
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

	camera->UpdateShaderVariables(d3d12GraphicsCommandList);
	camera->RSSetViewportsAndScissorRects(d3d12GraphicsCommandList);

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
	camera->UpdateShaderVariables(d3d12GraphicsCommandList);
	camera->RSSetViewportsAndScissorRects(d3d12GraphicsCommandList);

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
