#include "pch.h"
#include "GameScene.h"

#include "Core.h"

#include "TimeManager.h"
#include "InputManager.h"
#include "AssetManager.h"
#include "CameraManager.h"
#include "CollisionManager.h"

#include "Player.h"
#include "Guard.h"
#include "Bilboard.h"
#include "UI.h"

#include "Texture.h"
#include "Material.h"
#include "Shader.h"

#include "StateMachine.h"

#include "Camera.h"

#include "NavMesh.h"
#include "NavNode.h"

#include "GuardStates.h"

CGameScene::CGameScene() :
	m_d3d12GameScene(),
	m_mappedGameScene(),
	m_towerLight(),
	m_towerLightAngle()
{
	SetName("GameScene");
}

CGameScene::~CGameScene()
{
	ReleaseShaderVariables();
}

void CGameScene::CreateShaderVariables(ID3D12Device* d3d12Device, ID3D12GraphicsCommandList* d3d12GraphicsCommandList)
{
	UINT bytes = (sizeof(CB_GameScene) + 255) & ~255;

	m_d3d12GameScene = DX::CreateBufferResource(d3d12Device, d3d12GraphicsCommandList, nullptr, bytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, nullptr);
	DX::ThrowIfFailed(m_d3d12GameScene->Map(0, nullptr, reinterpret_cast<void**>(&m_mappedGameScene)));

	m_mappedGameScene->m_fog.m_color = XMFLOAT4(0.025f, 0.025f, 0.05f, 1.0f);
	m_mappedGameScene->m_fog.m_density = 0.015f;
}

void CGameScene::UpdateShaderVariables(ID3D12GraphicsCommandList* d3d12GraphicsCommandList)
{
	d3d12GraphicsCommandList->SetGraphicsRootConstantBufferView(static_cast<UINT>(ROOT_PARAMETER_TYPE::GAME_SCENE), m_d3d12GameScene->GetGPUVirtualAddress());
}

void CGameScene::ReleaseShaderVariables()
{
	if (m_d3d12GameScene.Get() != nullptr)
	{
		m_d3d12GameScene->Unmap(0, nullptr);
	}
}

void CGameScene::Enter()
{
	ShowCursor(false);

	// ī�޶��� Ÿ�� ����
	const vector<CObject*>& objects = GetGroupObject(GROUP_TYPE::PLAYER);

	CCameraManager::GetInstance()->GetMainCamera()->SetTarget((CPlayer*)objects[0]);
	//CSoundManager::GetInstance()->Play(SOUND_TYPE_INGAME_BGM_1, 0.3f, false);
}

void CGameScene::Exit()
{
	//// ������������ ����ž�� ������ �������� �ȵǱ� ������ ���Ӿ��� ���� ��, ���ش�.
	//m_Lights[1].m_IsActive = false;
	//m_Lights[2].m_IsActive = false;

	//CSoundManager::GetInstance()->Stop(SOUND_TYPE_SIREN);
	//CSoundManager::GetInstance()->Stop(SOUND_TYPE_INGAME_BGM_1);
	//CSoundManager::GetInstance()->Stop(SOUND_TYPE_INGAME_BGM_2);
}

void CGameScene::Init(ID3D12Device* d3d12Device, ID3D12GraphicsCommandList* d3d12GraphicsCommandList)
{
	// �� �ε�
	Load(d3d12Device, d3d12GraphicsCommandList, "GameScene.bin");
	//LoadUI(d3d12Device, d3d12GraphicsCommandList, "GameScene_UI.bin");

	// �������� ��ȸ�ϸ�, ����ž�� ���� �������� ã�� �����Ѵ�.
	const vector<CObject*>& structures = GetGroupObject(GROUP_TYPE::STRUCTURE);

	for (const auto& structure : structures)
	{
		CObject* towerLight = structure->FindFrame("spotlight_pr_1");

		if (towerLight != nullptr)
		{
			m_towerLight = towerLight;
			break;
		}
	}

	// ��ī�̹ڽ� �߰�
	CObject* object = new CSkyBox(d3d12Device, d3d12GraphicsCommandList);

	AddObject(GROUP_TYPE::BILBOARD, object);

	// ���� �߰�
	object = new CTree(d3d12Device, d3d12GraphicsCommandList, SCENE_TYPE::GAME);
	AddObject(GROUP_TYPE::BILBOARD, object);

	// UI �׽�Ʈ
	//object = new CUI();
	//object->SetActive(true);
	//object->SetMesh(CAssetManager::GetInstance()->GetMesh("Plane.002"));

	//CMaterial* material = CAssetManager::GetInstance()->CreateMaterial("BOL4");
	//CTexture* texture = CAssetManager::GetInstance()->CreateTexture("BOL4");
	//CShader* shader = CAssetManager::GetInstance()->GetShader("UI");

	//texture->Load(d3d12Device, d3d12GraphicsCommandList, "BOL4.dds", TEXTURE_TYPE::ALBEDO_MAP);
	//material->SetTexture(texture);
	//material->AddShader(shader);
	//object->AddMaterial(material);
	//object->Scale(256.0f, 384.0f, 1.0f);
	//object->Rotate(90.0f, 180.0f, 0.0f);
	//object->UpdateTransform();
	//AddObject(GROUP_TYPE::UI, object);

	// �浹 �׷� ����
	CCollisionManager::GetInstance()->SetCollisionGroup(GROUP_TYPE::ENEMY, GROUP_TYPE::ENEMY);
	CCollisionManager::GetInstance()->SetCollisionGroup(GROUP_TYPE::ENEMY, GROUP_TYPE::PLAYER);
	CCollisionManager::GetInstance()->SetCollisionGroup(GROUP_TYPE::PLAYER, GROUP_TYPE::PLAYER);

	CreateShaderVariables(d3d12Device, d3d12GraphicsCommandList);

	// ����(Light) ����
	const vector<CCamera*>& cameras = CCameraManager::GetInstance()->GetCameras();

	m_mappedGameScene->m_lights[0].m_isActive = true;
	m_mappedGameScene->m_lights[0].m_shadowMapping = true;
	m_mappedGameScene->m_lights[0].m_type = (int)LIGHT_TYPE::DIRECTIONAL;
	m_mappedGameScene->m_lights[0].m_position = XMFLOAT3(0.0f, 100.0f, 130.0f);
	m_mappedGameScene->m_lights[0].m_direction = Vector3::Normalize(XMFLOAT3(0.0f, -1.0f, -1.0f));
	m_mappedGameScene->m_lights[0].m_color = XMFLOAT4(0.5f, 0.5f, 0.6f, 0.0f);
	m_mappedGameScene->m_lights[0].m_range = 2000.0f;
	cameras[2]->SetLight(&m_mappedGameScene->m_lights[0]);

	m_towerLightAngle = XMConvertToRadians(90.0f);
	m_mappedGameScene->m_lights[1].m_isActive = true;
	m_mappedGameScene->m_lights[1].m_type = (int)LIGHT_TYPE::SPOT;
	m_mappedGameScene->m_lights[1].m_position = XMFLOAT3(0.0f, 50.0f, 0.0f);
	m_mappedGameScene->m_lights[1].m_direction = Vector3::Normalize(XMFLOAT3(cosf(m_towerLightAngle), -1.0f, sinf(m_towerLightAngle)));
	m_mappedGameScene->m_lights[1].m_color = XMFLOAT4(0.7f, 0.7f, 0.3f, 0.0f);
	m_mappedGameScene->m_lights[1].m_attenuation = XMFLOAT3(0.5f, 0.01f, 0.0f);
	m_mappedGameScene->m_lights[1].m_fallOff = 1.0f;
	m_mappedGameScene->m_lights[1].m_range = 500.0f;
	m_mappedGameScene->m_lights[1].m_theta = cosf(XMConvertToRadians(5.0f));
	m_mappedGameScene->m_lights[1].m_phi = cosf(XMConvertToRadians(10.0f));

	m_mappedGameScene->m_lights[2].m_isActive = true;
	m_mappedGameScene->m_lights[2].m_type = (int)LIGHT_TYPE::POINT;
	m_mappedGameScene->m_lights[2].m_position = XMFLOAT3(7.5f * cosf(m_towerLightAngle), 37.0f, 7.5f * sinf(m_towerLightAngle));
	m_mappedGameScene->m_lights[2].m_color = XMFLOAT4(1.0f, 1.0f, 0.8f, 0.0f);
	m_mappedGameScene->m_lights[2].m_attenuation = XMFLOAT3(0.5f, 0.01f, 0.0f);
	m_mappedGameScene->m_lights[2].m_range = 7.0f;
}

void CGameScene::Update()
{
	if (KEY_TAP(KEY::Q))
	{
		CNavMesh* navMesh = (CNavMesh*)CAssetManager::GetInstance()->GetMesh("NavMesh");
		const vector<CObject*>& objects = GetGroupObject(GROUP_TYPE::PLAYER);

		objects[0]->SetPosition(navMesh->GetNavNodes()[100]->GetTriangle().m_centroid);
		objects[0]->UpdateTransform();
	}

	CScene::Update();
	UpdateLightTower();
}

void CGameScene::UpdateLightTower()
{
	if (m_mappedGameScene->m_lights[1].m_isActive)
	{
		// ���� �����ǰ� ���� ���͸� Ȱ���� ��鿡 �����ϴ� �߽����� ����Ѵ�. 
		float angle = Vector3::Angle(m_mappedGameScene->m_lights[1].m_direction, XMFLOAT3(0.0f, -1.0f, 0.0f));	// ������ ���� ����
		float hypotenuseLength = m_mappedGameScene->m_lights[1].m_position.y / cosf(XMConvertToRadians(angle)); // ������ ����
		XMFLOAT3 lightCenter = Vector3::Add(m_mappedGameScene->m_lights[1].m_position, Vector3::ScalarProduct(hypotenuseLength, m_mappedGameScene->m_lights[1].m_direction, false)); // �������� ������ �߽��� ���ϴ� ����(�غ�)
		float radius = hypotenuseLength * tanf(XMConvertToRadians(10.0f)); // ������ ������� ���� ������

		const vector<CObject*> players = GetGroupObject(GROUP_TYPE::PLAYER);
		const vector<CObject*> structures = GetGroupObject(GROUP_TYPE::STRUCTURE);

		for (const auto& object1 : players)
		{
			CPlayer* player = (CPlayer*)object1;

			if (player->GetHealth() > 0)
			{
				float dist = Math::Distance(player->GetPosition(), lightCenter);

				if (dist <= radius)
				{
					XMFLOAT3 toPlayer = Vector3::Normalize(Vector3::Subtract(player->GetPosition(), m_mappedGameScene->m_lights[1].m_position));
					float nearestHitDist = FLT_MAX;
					bool isHit = false;

					// ���� ������ ���� �÷��̾ ������ �ڿ� �ִ����� ���� ���� ���θ� �ľ��Ѵ�.
					for (const auto& object2 : structures)
					{
						float hitDist = 0.0f;
						CObject* intersectedObject = object2->CheckRayIntersection(m_mappedGameScene->m_lights[1].m_position, toPlayer, hitDist, hypotenuseLength);

						if ((intersectedObject != nullptr) && (hitDist < nearestHitDist))
						{
							isHit = true;
							break;
						}
					}

					// �������� ������ ���� �ʾҴٸ�, ����ž�� �÷��̾ �߰��� ������ ����Ѵ�.
					if (!isHit)
					{
						const vector<CObject*> guards = GetGroupObject(GROUP_TYPE::ENEMY);

						for (const auto& object3 : guards)
						{
							CGuard* guard = (CGuard*)object3;

							if (guard->GetHealth() > 0)
							{
								float dist = Math::Distance(guard->GetPosition(), lightCenter);

								// ������ �߽ɿ��� �Ÿ��� 80.0f������ ���������� �÷��̾��� ��ġ�� �ҷ����δ�.
								if (dist <= 80.0f)
								{
									CStateMachine* stateMachine = static_cast<CStateMachine*>(guard->GetComponent(COMPONENT_TYPE::STATE_MACHINE));
									CState* currentState = stateMachine->GetCurrentState();

									if (currentState == CGuardIdleState::GetInstance() ||
										currentState == CGuardPatrolState::GetInstance() ||
										currentState == CGuardReturnState::GetInstance())
									{
										guard->CreateMovePath(player->GetPosition());
										stateMachine->ChangeState(CGuardAssembleState::GetInstance());
									}
								}
							}
						}

						// ����ž�� �÷��̾ �Ѿư���.
						XMFLOAT3 toPlayer = player->GetPosition();

						toPlayer.y = 0.0f;

						if (!Vector3::IsEqual(Vector3::Normalize(lightCenter), Vector3::Normalize(toPlayer)))
						{
							float axis = Vector3::CrossProduct(lightCenter, toPlayer, false).y;

							if (axis > 0.0f)
							{
								m_towerLightAngle -= 0.25f * DT;
							}
							else if (axis < 0.0f)
							{
								m_towerLightAngle += 0.25f * DT;
							}

							m_mappedGameScene->m_lights[1].m_direction = Vector3::Normalize(XMFLOAT3(cosf(m_towerLightAngle), -1.0f, sinf(m_towerLightAngle)));
							m_mappedGameScene->m_lights[2].m_position = XMFLOAT3(7.5f * cosf(m_towerLightAngle), 37.0f, 7.5f * sinf(m_towerLightAngle));

							// ����ž ���� ȸ��
							m_towerLight->UpdateLocalCoord(Vector3::Inverse(m_mappedGameScene->m_lights[1].m_direction));
						}

						return;
					}
				}
			}
		}

		m_towerLightAngle += DT;
		m_mappedGameScene->m_lights[1].m_direction = Vector3::Normalize(XMFLOAT3(cosf(m_towerLightAngle), -1.0f, sinf(m_towerLightAngle)));
		m_mappedGameScene->m_lights[2].m_position = XMFLOAT3(7.5f * cosf(m_towerLightAngle), 37.0f, 7.5f * sinf(m_towerLightAngle));

		// ����ž ���� ȸ��
		m_towerLight->UpdateLocalCoord(Vector3::Inverse(m_mappedGameScene->m_lights[1].m_direction));
	}
}

void CGameScene::PreRender(ID3D12GraphicsCommandList* d3d12GraphicsCommandList)
{
	UpdateShaderVariables(d3d12GraphicsCommandList);

	const vector<CCamera*>& cameras = CCameraManager::GetInstance()->GetCameras();

	for (const auto& camera : cameras)
	{
		if (camera->GetType() == CAMERA_TYPE::LIGHT)
		{
			Light* light = camera->GetLight();

			if ((light != nullptr) && (light->m_isActive) && (light->m_shadowMapping))
			{
				float nearPlaneDist = 5.0f;
				float farPlaneDist = light->m_range;

				switch ((LIGHT_TYPE)light->m_type)
				{
				case LIGHT_TYPE::POINT:
					break;
				case LIGHT_TYPE::SPOT:
					camera->GeneratePerspectiveProjectionMatrix(90.0f, static_cast<float>(DEPTH_BUFFER_WIDTH) / static_cast<float>(DEPTH_BUFFER_HEIGHT), nearPlaneDist, farPlaneDist);
					break;
				case LIGHT_TYPE::DIRECTIONAL:
					camera->GenerateOrthographicsProjectionMatrix(static_cast<float>(TERRAIN_WIDTH), static_cast<float>(TERRAIN_HEIGHT), nearPlaneDist, farPlaneDist);
					break;
				}

				camera->GenerateViewMatrix(light->m_position, light->m_direction);

				XMFLOAT4X4 projectionToTexture =
				{
					0.5f,  0.0f, 0.0f, 0.0f,
					0.0f, -0.5f, 0.0f, 0.0f,
					0.0f,  0.0f, 1.0f, 0.0f,
					0.5f,  0.5f, 0.0f, 1.0f
				};

				XMStoreFloat4x4(&light->m_toTexCoord, XMMatrixTranspose(XMLoadFloat4x4(&camera->GetViewMatrix()) * XMLoadFloat4x4(&camera->GetProjectionMatrix()) * XMLoadFloat4x4(&projectionToTexture)));

				CTexture* depthTexture = CAssetManager::GetInstance()->GetTexture("DepthWrite");

				DX::ResourceTransition(d3d12GraphicsCommandList, depthTexture->GetTexture(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_RENDER_TARGET);

				CD3DX12_CPU_DESCRIPTOR_HANDLE d3d12RtvCPUDescriptorHandle(CCore::GetInstance()->GetRtvDescriptorHeap()->GetCPUDescriptorHandleForHeapStart());
				CD3DX12_CPU_DESCRIPTOR_HANDLE d3d12DsvCPUDescriptorHandle(CCore::GetInstance()->GetDsvDescriptorHeap()->GetCPUDescriptorHandleForHeapStart());

				d3d12RtvCPUDescriptorHandle.ptr += 2 * CCore::GetInstance()->GetRtvDescriptorIncrementSize();
				d3d12DsvCPUDescriptorHandle.ptr += CCore::GetInstance()->GetDsvDescriptorIncrementSize();

				d3d12GraphicsCommandList->ClearRenderTargetView(d3d12RtvCPUDescriptorHandle, Colors::White, 0, nullptr);
				d3d12GraphicsCommandList->ClearDepthStencilView(d3d12DsvCPUDescriptorHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
				d3d12GraphicsCommandList->OMSetRenderTargets(1, &d3d12RtvCPUDescriptorHandle, TRUE, &d3d12DsvCPUDescriptorHandle);

				camera->RSSetViewportsAndScissorRects(d3d12GraphicsCommandList);
				camera->UpdateShaderVariables(d3d12GraphicsCommandList);
				depthTexture->UpdateShaderVariable(d3d12GraphicsCommandList);

				for (int i = (int)GROUP_TYPE::STRUCTURE; i <= (int)GROUP_TYPE::PLAYER; ++i)
				{
					const vector<CObject*>& objects = GetGroupObject((GROUP_TYPE)i);

					for (const auto& object : objects)
					{
						if (object->IsActive() && !object->IsDeleted())
						{
							object->PreRender(d3d12GraphicsCommandList, camera);
						}
					}
				}

				DX::ResourceTransition(d3d12GraphicsCommandList, depthTexture->GetTexture(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COMMON);
			}
		}
	}
}

void CGameScene::Render(ID3D12GraphicsCommandList* d3d12GraphicsCommandList)
{
	CScene::Render(d3d12GraphicsCommandList);

	// [Debug] Render NavMesh
	//XMFLOAT4X4 worldMatrix = {}, identity = Matrix4x4::Identity();

	//XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&identity)));
	//d3d12GraphicsCommandList->SetGraphicsRoot32BitConstants((UINT)ROOT_PARAMETER_TYPE::OBJECT, 16, &worldMatrix, 0);

	//CAssetManager::GetInstance()->GetShader("WireFrame")->SetPipelineState(d3d12GraphicsCommandList, 0);
	//CAssetManager::GetInstance()->GetMesh("NavMesh")->Render(d3d12GraphicsCommandList, 0);

	// [Debug] Render DepthTexture
	//const XMFLOAT2& resolution = CCore::GetInstance()->GetResolution();
	//D3D12_VIEWPORT d3d12Viewport = { 0.0f, 0.0f, resolution.x * 0.4f, resolution.y * 0.4f, 0.0f, 1.0f };
	//D3D12_RECT d3d12ScissorRect = { 0, 0,(LONG)(resolution.x * 0.4f), (LONG)(resolution.y * 0.4f) };
	//CTexture* texture = CAssetManager::GetInstance()->GetTexture("DepthWrite");
	//CShader* shader = CAssetManager::GetInstance()->GetShader("DepthWrite");

	//texture->UpdateShaderVariable(d3d12GraphicsCommandList);
	//shader->SetPipelineState(d3d12GraphicsCommandList, 2);
	//d3d12GraphicsCommandList->RSSetViewports(1, &d3d12Viewport);
	//d3d12GraphicsCommandList->RSSetScissorRects(1, &d3d12ScissorRect);
	//d3d12GraphicsCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//d3d12GraphicsCommandList->DrawInstanced(6, 1, 0, 0);
}
