#include "stdafx.h"
#include "GameScene.h"

void CGameScene::OnCreate(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	CreateRootSignature(D3D12Device);

	BuildLights();
	BuildObjects(D3D12Device, D3D12GraphicsCommandList);
}

void CGameScene::OnDestroy()
{

}
	
void CGameScene::BuildObjects(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	// NavMesh 객체를 생성한다.
	m_NavMesh = make_shared<CNavMesh>();
	m_NavMesh->LoadNavNodeFromFile(TEXT("Navigation/NavMesh.txt"));

	// 렌더링에 필요한 셰이더 객체(PSO)를 생성한다.
	shared_ptr<CGraphicsShader> Shader{ make_shared<CDepthWriteShader>(D3D12Device, D3D12GraphicsCommandList) };

	Shader->CreatePipelineState(D3D12Device, m_D3D12RootSignature.Get(), 2);
	CShaderManager::GetInstance()->RegisterShader(TEXT("DepthWriteShader"), Shader);

	Shader = make_shared<CShadowMapShader>();
	Shader->CreatePipelineState(D3D12Device, m_D3D12RootSignature.Get(), 2);
	CShaderManager::GetInstance()->RegisterShader(TEXT("ShadowMapShader"), Shader);

	Shader = make_shared<CSkyBoxShader>();
	Shader->CreatePipelineState(D3D12Device, m_D3D12RootSignature.Get(), 0);
	CShaderManager::GetInstance()->RegisterShader(TEXT("SkyBoxShader"), Shader);

	Shader = make_shared<CUIShader>();
	Shader->CreatePipelineState(D3D12Device, m_D3D12RootSignature.Get(), 0);
	CShaderManager::GetInstance()->RegisterShader(TEXT("UIShader"), Shader);

	Shader = make_shared<CDebugShader>();
	Shader->CreatePipelineState(D3D12Device, m_D3D12RootSignature.Get(), 0);
	CShaderManager::GetInstance()->RegisterShader(TEXT("DebugShader"), Shader);

	// 타입 수만큼 각 벡터의 크기를 재할당한다.
	m_GameObjects.resize(OBJECT_TYPE_STRUCTURE + 1);
	m_BilboardObjects.resize(BILBOARD_OBJECT_TYPE_UI + 1);

	// 스카이박스 객체를 생성한다.
	m_BilboardObjects[BILBOARD_OBJECT_TYPE_SKYBOX].push_back(make_shared<CSkyBox>(D3D12Device, D3D12GraphicsCommandList));

	// 파일로부터 씬 객체들을 생성하고 배치한다.
#ifdef READ_BINARY_FILE
	LoadSceneInfoFromFile(D3D12Device, D3D12GraphicsCommandList, TEXT("Scenes/GameScene.bin"));
#else
	LoadSceneInfoFromFile(D3D12Device, D3D12GraphicsCommandList, TEXT("Scenes/GameScene.txt"));
#endif

	// 파일로부터 UI 객체들을 생성하고 배치한다.
#ifdef READ_BINARY_FILE
	LoadUIInfoFromFile(D3D12Device, D3D12GraphicsCommandList, TEXT("Scenes/GameScene_UI.bin"));
#else
	LoadUIInfoFromFile(D3D12Device, D3D12GraphicsCommandList, TEXT("Scenes/GameScene_UI.txt"));
#endif

	// 모든 텍스처를 저장하는 힙과 각 텍스처의 SRV 리소스를 생성한다.
	CTextureManager::GetInstance()->CreateCbvSrvUavDescriptorHeaps(D3D12Device);
	CTextureManager::GetInstance()->CreateShaderResourceViews(D3D12Device);

	CreateShaderVariables(D3D12Device, D3D12GraphicsCommandList);
}

void CGameScene::ReleaseObjects()
{
	ReleaseShaderVariables();
}

void CGameScene::LoadSceneInfoFromFile(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, const tstring& FileName)
{
	tstring Token{};

	shared_ptr<LOADED_MODEL_INFO> ModelInfo{};
	UINT ObjectType{};

	unordered_map<tstring, shared_ptr<CMesh>> MeshCaches{};
	unordered_map<tstring, shared_ptr<CMaterial>> MaterialCaches{};

#ifdef READ_BINARY_FILE
	LoadMeshCachesFromFile(D3D12Device, D3D12GraphicsCommandList, TEXT("MeshesAndMaterials/Meshes.bin"), MeshCaches);
	LoadMaterialCachesFromFile(D3D12Device, D3D12GraphicsCommandList, TEXT("MeshesAndMaterials/Materials.bin"), MaterialCaches);

	tifstream InFile{ FileName, ios::binary };

	while (true)
	{
		File::ReadStringFromFile(InFile, Token);

		if (Token == TEXT("<Name>"))
		{
			File::ReadStringFromFile(InFile, Token);

			ModelInfo = CGameObject::LoadObjectFromFile(D3D12Device, D3D12GraphicsCommandList, Token, MeshCaches, MaterialCaches);
		}
		else if (Token == TEXT("<Type>"))
		{
			ObjectType = File::ReadIntegerFromFile(InFile);
		}
		else if (Token == TEXT("<TransformMatrix>"))
		{
			XMFLOAT4X4 TransformMatrix{};

			InFile.read(reinterpret_cast<TCHAR*>(&TransformMatrix), sizeof(XMFLOAT4X4));

			switch (ObjectType)
			{
			case OBJECT_TYPE_PLAYER:
			{
				// 플레이어 객체를 생성한다.
				shared_ptr<CPlayer> Player{ make_shared<CPlayer>(D3D12Device, D3D12GraphicsCommandList) };

				Player->SetChild(ModelInfo->m_Model);
				Player->SetTransformMatrix(TransformMatrix);
				Player->SetAnimationController(D3D12Device, D3D12GraphicsCommandList, ModelInfo);
				Player->Initialize();

				m_GameObjects[ObjectType].push_back(Player);
			}
				break;
			case OBJECT_TYPE_NPC:
			{
				XMFLOAT3 TargetPosition{};

				// <TargetPosition>
				File::ReadStringFromFile(InFile, Token);
				InFile.read(reinterpret_cast<TCHAR*>(&TargetPosition), sizeof(XMFLOAT3));

				tcout << TargetPosition.x << ", " << TargetPosition.y << ", " << TargetPosition.z << endl;

				// 교도관 객체를 생성한다.
				shared_ptr<CGuard> Guard{ make_shared<CGuard>() };

				Guard->SetChild(ModelInfo->m_Model);
				Guard->SetTransformMatrix(TransformMatrix);
				Guard->UpdateTransform(Matrix4x4::Identity());
				Guard->SetAnimationController(D3D12Device, D3D12GraphicsCommandList, ModelInfo);
				Guard->SetTargetPosition(TargetPosition);
				//Guard->FindPatrolNavPath(m_NavMesh);
				Guard->Initialize();

				m_GameObjects[ObjectType].push_back(Guard);
			}
				break;
			case OBJECT_TYPE_TERRAIN:
			case OBJECT_TYPE_STRUCTURE:
			{
				// 지형 및 구조물 객체를 생성한다.
				shared_ptr<CGameObject> Architecture{ make_shared<CGameObject>() };

				Architecture->SetChild(ModelInfo->m_Model);
				Architecture->SetTransformMatrix(TransformMatrix);
				Architecture->Initialize();

				m_GameObjects[ObjectType].push_back(Architecture);
			}
				break;
			}
		}
		else if (Token == TEXT("</GameScene>"))
		{
			break;
		}
	}
#else
	LoadMeshCachesFromFile(D3D12Device, D3D12GraphicsCommandList, TEXT("MeshesAndMaterials/Meshes.txt"), MeshCaches);
	LoadMaterialCachesFromFile(D3D12Device, D3D12GraphicsCommandList, TEXT("MeshesAndMaterials/Materials.txt"), MaterialCaches);

	tifstream InFile{ FileName };

	while (InFile >> Token)
	{
		if (Token == TEXT("<Name>"))
		{
			InFile >> Token;
			ModelInfo = CGameObject::LoadObjectFromFile(D3D12Device, D3D12GraphicsCommandList, Token, MeshCaches, MaterialCaches);
		}
		else if (Token == TEXT("<Type>"))
		{
			InFile >> ObjectType;
		}
		else if (Token == TEXT("<TransformMatrix>"))
		{
			XMFLOAT4X4 TransformMatrix{};

			InFile >> TransformMatrix._11 >> TransformMatrix._12 >> TransformMatrix._13 >> TransformMatrix._14;
			InFile >> TransformMatrix._21 >> TransformMatrix._22 >> TransformMatrix._23 >> TransformMatrix._24;
			InFile >> TransformMatrix._31 >> TransformMatrix._32 >> TransformMatrix._33 >> TransformMatrix._34;
			InFile >> TransformMatrix._41 >> TransformMatrix._42 >> TransformMatrix._43 >> TransformMatrix._44;

			switch (ObjectType)
			{
			case OBJECT_TYPE_PLAYER:
			{
				// 플레이어 객체를 생성한다.
				shared_ptr<CPlayer> Player{ make_shared<CPlayer>(D3D12Device, D3D12GraphicsCommandList) };

				Player->Initialize();
				Player->SetChild(ModelInfo->m_Model);
				Player->SetTransformMatrix(TransformMatrix);
				Player->SetAnimationController(D3D12Device, D3D12GraphicsCommandList, ModelInfo);
				Player->FindFrame(TEXT("gun_pr_1"))->SetActive(false);

				m_GameObjects[ObjectType].push_back(Player);
			}
			break;
			case OBJECT_TYPE_NPC:
			{
				// 교도관 객체를 생성한다.
				shared_ptr<CGuard> Guard{ make_shared<CGuard>() };

				Guard->Initialize();
				Guard->SetChild(ModelInfo->m_Model);
				Guard->SetTransformMatrix(TransformMatrix);
				Guard->SetAnimationController(D3D12Device, D3D12GraphicsCommandList, ModelInfo);
				Guard->SetAnimationClip(static_cast<UINT>(rand() % 4));

				m_GameObjects[ObjectType].push_back(Guard);
			}
			break;
			case OBJECT_TYPE_TERRAIN:
			case OBJECT_TYPE_STRUCTURE:
			{
				// 지형 및 구조물 객체를 생성한다.
				shared_ptr<CGameObject> Architecture{ make_shared<CGameObject>() };

				Architecture->Initialize();
				Architecture->SetChild(ModelInfo->m_Model);
				Architecture->SetTransformMatrix(TransformMatrix);

				m_GameObjects[ObjectType].push_back(Architecture);
			}
			break;
			}
		}
		else if (Token == TEXT("</GameScene>"))
		{
			break;
		}
	}
#endif
}

void CGameScene::LoadUIInfoFromFile(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, const tstring& FileName)
{
	tstring Token{};

	shared_ptr<CBilboardObject> Object{};

#ifdef READ_BINARY_FILE
	tifstream InFile{ FileName, ios::binary };

	while (true)
	{
		File::ReadStringFromFile(InFile, Token);

		if (Token == TEXT("<UIObject>"))
		{
			Object = CBilboardObject::LoadObjectInfoFromFile(D3D12Device, D3D12GraphicsCommandList, InFile);

			m_BilboardObjects[BILBOARD_OBJECT_TYPE_UI].push_back(Object);
		}
		else if (Token == TEXT("</UI>"))
		{
			break;
		}
	}
#else
	tifstream InFile{ FileName };

	while (InFile >> Token)
	{
		if (Token == TEXT("<UIObject>"))
		{
			Object = CBilboardObject::LoadObjectInfoFromFile(D3D12Device, D3D12GraphicsCommandList, InFile);

			m_BilboardObjects[BILBOARD_OBJECT_TYPE_UI].push_back(Object);
		}
		else if (Token == TEXT("</UI>"))
		{
			break;
		}
	}
#endif

	LoadEventTriggerFromFile(TEXT("Triggers/EventTriggers.bin"));
}

void CGameScene::CreateRootSignature(ID3D12Device* D3D12Device)
{
	CD3DX12_DESCRIPTOR_RANGE D3D12DescriptorRanges[4]{};

	D3D12DescriptorRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
	D3D12DescriptorRanges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);
	D3D12DescriptorRanges[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2);
	D3D12DescriptorRanges[3].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 3);

	CD3DX12_ROOT_PARAMETER D3D12RootParameters[11]{};

	D3D12RootParameters[ROOT_PARAMETER_TYPE_FRAMEWORK_INFO].InitAsConstantBufferView(0);					   // 프레임워크 정보(b0)
	D3D12RootParameters[ROOT_PARAMETER_TYPE_CAMERA].InitAsConstantBufferView(1);							   // 카메라 정보(b1)
	D3D12RootParameters[ROOT_PARAMETER_TYPE_LIGHT].InitAsConstantBufferView(2);							       // 조명 정보(b2)
	D3D12RootParameters[ROOT_PARAMETER_TYPE_FOG].InitAsConstantBufferView(3);								   // 안개 정보(b3)
	D3D12RootParameters[ROOT_PARAMETER_TYPE_OBJECT].InitAsConstants(23, 4);								       // 오브젝트 정보(b4)
	D3D12RootParameters[ROOT_PARAMETER_TYPE_BONE_OFFSET].InitAsConstantBufferView(5);						   // 스키닝 애니메이션(오프셋 행렬) 정보(b5)
	D3D12RootParameters[ROOT_PARAMETER_TYPE_BONE_TRANSFORM].InitAsConstantBufferView(6);					   // 스키닝 애니메이션(변환된 뼈들의 행렬) 정보(b6)
	D3D12RootParameters[ROOT_PARAMETER_TYPE_ALBEDO_MAP].InitAsDescriptorTable(1, &D3D12DescriptorRanges[0]);   // 텍스처 정보(AlbedoMap : t0)
	D3D12RootParameters[ROOT_PARAMETER_TYPE_METALLIC_MAP].InitAsDescriptorTable(1, &D3D12DescriptorRanges[1]); // 텍스처 정보(MetallicMap : t1)
	D3D12RootParameters[ROOT_PARAMETER_TYPE_NORMAL_MAP].InitAsDescriptorTable(1, &D3D12DescriptorRanges[2]);   // 텍스처 정보(NormalMap : t2)
	D3D12RootParameters[ROOT_PARAMETER_TYPE_SHADOW_MAP].InitAsDescriptorTable(1, &D3D12DescriptorRanges[3]);   // 텍스처 정보(ShadowMap : t3)
	
	D3D12_ROOT_SIGNATURE_FLAGS D3D12RootSignatureFlags{ D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_ALLOW_STREAM_OUTPUT }; // IA단계를 허용, 스트림 출력 단계를 허용
	CD3DX12_STATIC_SAMPLER_DESC D3D12SamplerDesc[2]{};

	D3D12SamplerDesc[0].Init(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		0.0f, 1, D3D12_COMPARISON_FUNC_ALWAYS, D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE, 0.0f, D3D12_FLOAT32_MAX, D3D12_SHADER_VISIBILITY_PIXEL, 0);
	D3D12SamplerDesc[1].Init(1, D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT, D3D12_TEXTURE_ADDRESS_MODE_BORDER, D3D12_TEXTURE_ADDRESS_MODE_BORDER, D3D12_TEXTURE_ADDRESS_MODE_BORDER,
		0.0f, 1, D3D12_COMPARISON_FUNC_LESS_EQUAL, D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE, 0.0f, D3D12_FLOAT32_MAX, D3D12_SHADER_VISIBILITY_PIXEL, 0);

	CD3DX12_ROOT_SIGNATURE_DESC D3D12RootSignatureDesc{};

	D3D12RootSignatureDesc.Init(_countof(D3D12RootParameters), D3D12RootParameters, _countof(D3D12SamplerDesc), D3D12SamplerDesc, D3D12RootSignatureFlags);

	ComPtr<ID3DBlob> D3D12SignatureBlob{}, D3D12ErrorBlob{};

	DX::ThrowIfFailed(D3D12SerializeRootSignature(&D3D12RootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, D3D12SignatureBlob.GetAddressOf(), D3D12ErrorBlob.GetAddressOf()));
	DX::ThrowIfFailed(D3D12Device->CreateRootSignature(0, D3D12SignatureBlob->GetBufferPointer(), D3D12SignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), reinterpret_cast<void**>(m_D3D12RootSignature.GetAddressOf())));
}

void CGameScene::CreateShaderVariables(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	UINT Bytes{ (sizeof(CB_LIGHT) + 255) & ~255 };

	m_D3D12Lights = DX::CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, nullptr, Bytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, nullptr);
	DX::ThrowIfFailed(m_D3D12Lights->Map(0, nullptr, reinterpret_cast<void**>(&m_MappedLights)));

	Bytes = (sizeof(CB_FOG) + 255) & ~255;

	m_D3D12Fog = DX::CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, nullptr, Bytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, nullptr);
	DX::ThrowIfFailed(m_D3D12Fog->Map(0, nullptr, reinterpret_cast<void**>(&m_MappedFog)));

	BuildFog();
}

void CGameScene::UpdateShaderVariables(ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	memcpy(m_MappedLights->m_Lights, m_Lights.data(), sizeof(CB_LIGHT) * (UINT)m_Lights.size());
	D3D12GraphicsCommandList->SetGraphicsRootConstantBufferView(ROOT_PARAMETER_TYPE_LIGHT, m_D3D12Lights->GetGPUVirtualAddress());
	D3D12GraphicsCommandList->SetGraphicsRootConstantBufferView(ROOT_PARAMETER_TYPE_FOG, m_D3D12Fog->GetGPUVirtualAddress());
}

void CGameScene::ReleaseShaderVariables()
{
	if (m_D3D12Lights)
	{
		m_D3D12Lights->Unmap(0, nullptr);
	}
}
	
void CGameScene::ReleaseUploadBuffers()
{
	for (UINT i = OBJECT_TYPE_PLAYER; i <= OBJECT_TYPE_STRUCTURE; ++i)
	{
		for (const auto& GameObject : m_GameObjects[i])
		{
			if (GameObject)
			{
				GameObject->ReleaseUploadBuffers();
			}
		}
	}

	for (UINT i = BILBOARD_OBJECT_TYPE_SKYBOX; i <= BILBOARD_OBJECT_TYPE_UI; ++i)
	{
		for (const auto& BilboardObject : m_BilboardObjects[i])
		{
			if (BilboardObject)
			{
				BilboardObject->ReleaseUploadBuffers();
			}
		}
	}
}

void CGameScene::ProcessMouseMessage(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{

}

void CGameScene::ProcessKeyboardMessage(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case 'r': // 권총 획득
	case 'R':
		static_pointer_cast<CPlayer>(m_GameObjects[OBJECT_TYPE_PLAYER].back())->AcquirePistol();
		if (static_pointer_cast<CPlayer>(m_GameObjects[OBJECT_TYPE_PLAYER].back())->SwapWeapon(WEAPON_TYPE_PISTOL))
		{
			m_BilboardObjects[BILBOARD_OBJECT_TYPE_UI][4]->SetActive(false); // 4: Punch UI
			m_BilboardObjects[BILBOARD_OBJECT_TYPE_UI][5]->SetActive(true);	 // 5: Pistol UI
			m_BilboardObjects[BILBOARD_OBJECT_TYPE_UI][6]->SetActive(true);	 // 6: Bullet UI
		}
		break;
	case '1': // 무기 스왑 - 펀치
		if (static_pointer_cast<CPlayer>(m_GameObjects[OBJECT_TYPE_PLAYER].back())->SwapWeapon(WEAPON_TYPE_PUNCH))
		{
			m_BilboardObjects[BILBOARD_OBJECT_TYPE_UI][4]->SetActive(true);  // 4: Punch UI
			m_BilboardObjects[BILBOARD_OBJECT_TYPE_UI][5]->SetActive(false); // 5: Pistol UI
			m_BilboardObjects[BILBOARD_OBJECT_TYPE_UI][6]->SetActive(false); // 6: Bullet UI
		}
		break;
	case '2': // 무기 스왑 - 권총
		if (static_pointer_cast<CPlayer>(m_GameObjects[OBJECT_TYPE_PLAYER].back())->SwapWeapon(WEAPON_TYPE_PISTOL))
		{
			m_BilboardObjects[BILBOARD_OBJECT_TYPE_UI][4]->SetActive(false); // 4: Punch UI
			m_BilboardObjects[BILBOARD_OBJECT_TYPE_UI][5]->SetActive(true);	 // 5: Pistol UI
			m_BilboardObjects[BILBOARD_OBJECT_TYPE_UI][6]->SetActive(true);	 // 6: Bullet UI
		}
		break;
	case '3': // 미션 UI 완료 디버깅
		m_BilboardObjects[BILBOARD_OBJECT_TYPE_UI][0]->SetCellIndex(0, 1);
		m_BilboardObjects[BILBOARD_OBJECT_TYPE_UI][0]->SetCellIndex(1, 5);
		break;
	case 'b': // 바운딩 박스 렌더링 ON/OFF
	case 'B':
		(m_RenderBoundingBox) ? m_RenderBoundingBox = false : m_RenderBoundingBox = true;
		break;
	case 'p': // 안개 ON/OFF
	case 'P':
		(m_MappedFog->m_Fog.m_Density > 0.0f) ? m_MappedFog->m_Fog.m_Density = 0.0f : m_MappedFog->m_Fog.m_Density = 0.025f;
		break;
	case 'e': // 현재 플레이어가 있는 위치를 향해 길찾기 시작
	case 'E':
		static_pointer_cast<CGuard>(m_GameObjects[OBJECT_TYPE_NPC][11])->FindPath(m_NavMesh, m_GameObjects[OBJECT_TYPE_PLAYER].back()->GetPosition());
		break;
	case 'q': // 플레이어를 감옥 밖으로 이동시키고 'e'키를 통해 길찾기를 하는 NPC를 운동장 근처로 소환
	case 'Q':
		m_GameObjects[OBJECT_TYPE_PLAYER].back()->SetPosition(m_NavMesh->GetNavNodes()[500]->GetTriangle().m_Centroid);
		break;
	}
}

void CGameScene::ProcessInput(HWND hWnd, float ElapsedTime)
{
	m_RayCasting = false;

	// 방향성 조명 방향 변경
	static float Angle = XMConvertToRadians(90.0f);

	if (GetAsyncKeyState(VK_NEXT) & 0x8000)
	{
		Angle += ElapsedTime;

		m_Lights[1].m_Position.x = 100.0f * cosf(Angle);
		m_Lights[1].m_Position.z = 100.0f * sinf(Angle) + 50.0f;
		m_Lights[1].m_Direction.x = cosf(Angle);
		m_Lights[1].m_Direction.z = sinf(Angle);
	}

	if (GetAsyncKeyState(VK_PRIOR) & 0x8000)
	{
		Angle -= ElapsedTime;

		m_Lights[1].m_Position.x = 100.0f * cosf(Angle);
		m_Lights[1].m_Position.z = 100.0f * sinf(Angle) + 50.0f;
		m_Lights[1].m_Direction.x = cosf(Angle);
		m_Lights[1].m_Direction.z = sinf(Angle);
	}

	RECT Rect{};

	GetWindowRect(hWnd, &Rect);

	POINT NewCursorPos{};
	POINT OldCursorPos{ Rect.right / 2, Rect.bottom / 2 };

	GetCursorPos(&NewCursorPos);
	SetCursorPos(OldCursorPos.x, OldCursorPos.y);

	XMFLOAT2 Delta{ 10.0f * ElapsedTime * (NewCursorPos.x - OldCursorPos.x), 10.0f * ElapsedTime * (NewCursorPos.y - OldCursorPos.y) };
	shared_ptr<CPlayer> Player{ static_pointer_cast<CPlayer>(m_GameObjects[OBJECT_TYPE_PLAYER][0]) };

	// 1인칭 모드
	//Player->GetCamera()->Rotate(Delta.y, Delta.x, 0.0f);

	//if (GetAsyncKeyState('W') & 0x8000) Player->GetCamera()->Move(Vector3::ScalarProduct(15.0f * ElapsedTime, Player->GetCamera()->GetLook(), false));
	//if (GetAsyncKeyState('S') & 0x8000) Player->GetCamera()->Move(Vector3::ScalarProduct(-15.0f * ElapsedTime, Player->GetCamera()->GetLook(), false));
	//if (GetAsyncKeyState('A') & 0x8000) Player->GetCamera()->Move(Vector3::ScalarProduct(-15.0f * ElapsedTime, Player->GetCamera()->GetRight(), false));
	//if (GetAsyncKeyState('D') & 0x8000) Player->GetCamera()->Move(Vector3::ScalarProduct(15.0f * ElapsedTime, Player->GetCamera()->GetRight(), false));

	//return;

	// 3인칭 모드
	Player->Rotate(Delta.y, Delta.x, 0.0f, ElapsedTime);

	UINT InputMask{};

	if (GetAsyncKeyState('W') & 0x8000)
	{
		InputMask |= INPUT_MASK_W;
	}

	if (GetAsyncKeyState('S') & 0x8000)
	{
		InputMask |= INPUT_MASK_S;
	}

	if (GetAsyncKeyState('A') & 0x8000)
	{
		InputMask |= INPUT_MASK_A;
	}

	if (GetAsyncKeyState('D') & 0x8000)
	{
		InputMask |= INPUT_MASK_D;
	}

	if (GetAsyncKeyState('F') & 0x0001)
	{
		XMFLOAT3 Position{ Player->GetPosition() };
		XMFLOAT3 LookDirection{ Player->GetLook() };

		for (const auto& EventTrigger : m_EventTriggers)
		{
			if (EventTrigger)
			{
				if (EventTrigger->IsInTriggerArea(Position, LookDirection))
				{
					EventTrigger->SetActive(true);
					break;
				}
			}
		}
	}

	if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
	{
		InputMask |= INPUT_MASK_SHIFT;
	}

	// 미션 UI ON/OFF
	if (GetAsyncKeyState(VK_TAB) & 0x0001)
	{
		InputMask |= INPUT_MASK_TAB;
		m_BilboardObjects[BILBOARD_OBJECT_TYPE_UI][0]->GetStateMachine()->ProcessInput(INPUT_MASK_TAB, ElapsedTime);
	}

	if (GetAsyncKeyState(VK_RBUTTON) & 0x8000)
	{
		InputMask |= INPUT_MASK_RMB;
		m_RayCasting = true;
	}

	if (GetAsyncKeyState(VK_LBUTTON) & 0x0001)
	{
		InputMask |= INPUT_MASK_LMB;

		if (m_RayCasting)
		{
			shared_ptr<CGameObject> NearestIntersectedObject{};
			float NearestHitDistance = FLT_MAX;
			float HitDistance{};

			for (UINT i = OBJECT_TYPE_NPC; i <= OBJECT_TYPE_STRUCTURE; ++i)
			{
				for (const auto& GameObject : m_GameObjects[i])
				{
					if (GameObject)
					{
						// 모델을 공유하기 때문에, 월드 변환 행렬을 객체마다 갱신시켜주어야 한다.
						CAnimationController* AnimationController{ GameObject->GetAnimationController() };

						if (AnimationController)
						{
							AnimationController->UpdateShaderVariables();
						}

						shared_ptr<CGameObject> IntersectedObject{ GameObject->PickObjectByRayIntersection(Player->GetCamera()->GetPosition(),Player->GetCamera()->GetLook(), HitDistance) };

						if (IntersectedObject)
						{
							tcout << TEXT("광선을 맞은 객체명 : ") << IntersectedObject->GetName() << TEXT(" (거리 : ") << HitDistance << TEXT(")") << endl;
							tcout << TEXT("- 해당 객체의 위치 : ") << IntersectedObject->GetPosition().x << ", " << IntersectedObject->GetPosition().y << ", " << IntersectedObject->GetPosition().z << endl;
							tcout << TEXT("- 해당 객체 중심까지의 거리 : ") << Vector3::Length(Vector3::Subtract(IntersectedObject->GetPosition(), Player->GetCamera()->GetPosition())) << endl;
						}

						if (IntersectedObject && (HitDistance < NearestHitDistance))
						{
							NearestIntersectedObject = IntersectedObject;
							NearestHitDistance = HitDistance;
						}
					}
				}
			}

			if (NearestIntersectedObject)
			{
				tcout << TEXT("★ 가장 먼저 광선을 맞은 객체명 : ") << NearestIntersectedObject->GetName() << TEXT(" (거리 : ") << NearestHitDistance << TEXT(")") << endl << endl;
			}
		}
	}

	Player->ProcessInput(InputMask, ElapsedTime, m_NavMesh);
	(Player->GetCamera()->IsZoomIn()) ? m_BilboardObjects[BILBOARD_OBJECT_TYPE_UI][7]->SetActive(true) : m_BilboardObjects[BILBOARD_OBJECT_TYPE_UI][7]->SetActive(false);
}

void CGameScene::Animate(float ElapsedTime)
{
	for (const auto& GameObject : m_GameObjects[OBJECT_TYPE_NPC])
	{
		shared_ptr<CGuard> Guard{ static_pointer_cast<CGuard>(GameObject) };

		if (Guard)
		{
			if (Guard->IsFoundPlayer(m_GameObjects[OBJECT_TYPE_PLAYER].back()->GetPosition()))
			{
				Guard->GetStateMachine()->ChangeState(CGuardChaseState::GetInstance());
				Guard->FindPath(m_NavMesh, m_GameObjects[OBJECT_TYPE_PLAYER].back()->GetPosition());
			}

			//if (Guard->GetStateMachine()->IsInState(CGuardChaseState::GetInstance()))
			//{
			//	Guard->FindPath(m_NavMesh, m_GameObjects[OBJECT_TYPE_PLAYER].back()->GetPosition());
			//}
		}
	}

	for (UINT i = OBJECT_TYPE_PLAYER; i <= OBJECT_TYPE_STRUCTURE; ++i)
	{
		for (const auto& GameObject : m_GameObjects[i])
		{
			if (GameObject)
			{
				GameObject->Animate(ElapsedTime);
			}
		}
	}

	for (UINT i = BILBOARD_OBJECT_TYPE_SKYBOX; i <= BILBOARD_OBJECT_TYPE_UI; ++i)
	{
		for (const auto& BilboardObject : m_BilboardObjects[i])
		{
			if (BilboardObject)
			{
				BilboardObject->Animate(ElapsedTime);
			}
		}
	}

	for (const auto& EventTrigger : m_EventTriggers)
	{
		if (EventTrigger)
		{
			XMFLOAT3 Position{ m_GameObjects[OBJECT_TYPE_PLAYER].back()->GetPosition() };
			XMFLOAT3 LookDirection{ m_GameObjects[OBJECT_TYPE_PLAYER].back()->GetLook() };

			for (const auto& EventTrigger : m_EventTriggers)
			{
				if (EventTrigger)
				{
					if (EventTrigger->IsInTriggerArea(Position, LookDirection))
					{
						EventTrigger->GenerateEventTrigger(ElapsedTime);
						break;
					}
				}
			}
		}
	}
}

void CGameScene::PreRender(ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	if (m_D3D12RootSignature)
	{
		D3D12GraphicsCommandList->SetGraphicsRootSignature(m_D3D12RootSignature.Get());
	}

	CTextureManager::GetInstance()->SetDescriptorHeap(D3D12GraphicsCommandList);
	static_pointer_cast<CDepthWriteShader>(CShaderManager::GetInstance()->GetShader(TEXT("DepthWriteShader")))->PrepareShadowMap(D3D12GraphicsCommandList, m_Lights, m_GameObjects);

	UpdateShaderVariables(D3D12GraphicsCommandList);
}

void CGameScene::Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	shared_ptr<CPlayer> Player{ static_pointer_cast<CPlayer>(m_GameObjects[OBJECT_TYPE_PLAYER].back()) };

	Player->GetCamera()->RSSetViewportsAndScissorRects(D3D12GraphicsCommandList);
	Player->GetCamera()->UpdateShaderVariables(D3D12GraphicsCommandList);

	CTextureManager::GetInstance()->GetTexture(TEXT("ShadowMap"))->UpdateShaderVariable(D3D12GraphicsCommandList);

	for (UINT i = OBJECT_TYPE_PLAYER; i <= OBJECT_TYPE_STRUCTURE; ++i)
	{
		for (const auto& GameObject : m_GameObjects[i])
		{
			if (GameObject)
			{
				GameObject->Render(D3D12GraphicsCommandList, Player->GetCamera(), RENDER_TYPE_STANDARD);
			}
		}
	}
	
	for (UINT i = BILBOARD_OBJECT_TYPE_SKYBOX; i <= BILBOARD_OBJECT_TYPE_UI; ++i)
	{
		for (const auto& BilboardObject : m_BilboardObjects[i])
		{
			if (BilboardObject)
			{
				BilboardObject->Render(D3D12GraphicsCommandList, Player->GetCamera(), RENDER_TYPE_STANDARD);
			}
		}
	}

	if (m_RenderBoundingBox)
	{
		static_pointer_cast<CDebugShader>(CShaderManager::GetInstance()->GetShader(TEXT("DebugShader")))->Render(D3D12GraphicsCommandList, Player->GetCamera(), m_GameObjects, 0);
	}
}

void CGameScene::PostRender(ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{

}

void CGameScene::LoadMeshCachesFromFile(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, const tstring& FileName, unordered_map<tstring, shared_ptr<CMesh>>& MeshCaches)
{
	tstring Token{};

#ifdef READ_BINARY_FILE
	tifstream InFile{ FileName, ios::binary };

	while (true)
	{
		File::ReadStringFromFile(InFile, Token);

		if (Token == TEXT("<Meshes>"))
		{
			UINT MeshCount{ File::ReadIntegerFromFile(InFile) };

			if (MeshCount > 0)
			{
				tcout << TEXT("총 ") << MeshCount << TEXT("개의 메쉬를 읽어오는 중입니다.") << endl;
				MeshCaches.reserve(MeshCount);
			}
		}
		else if (Token == TEXT("<Mesh>"))
		{
			shared_ptr<CMesh> Mesh{ make_shared<CMesh>() };

			Mesh->LoadMeshInfoFromFile(D3D12Device, D3D12GraphicsCommandList, InFile);
			MeshCaches.emplace(Mesh->GetName(), Mesh);
		}
		else if (Token == TEXT("<SkinnedMesh>"))
		{
			shared_ptr<CSkinnedMesh> SkinnedMesh{ make_shared<CSkinnedMesh>() };

			SkinnedMesh->LoadMeshInfoFromFile(D3D12Device, D3D12GraphicsCommandList, InFile);
			MeshCaches.emplace(SkinnedMesh->GetName(), SkinnedMesh);
		}
		else if (Token == TEXT("</Meshes>"))
		{
			tcout << TEXT("총 ") << MeshCaches.size() << TEXT("개의 메쉬를 읽어왔습니다.") << endl;
			break;
		}
	}
#else
	tifstream InFile{ FileName };

	while (InFile >> Token)
	{
		if (Token == TEXT("<Meshes>"))
		{
			UINT MeshCount{};

			InFile >> MeshCount;

			if (MeshCount > 0)
			{
				tcout << TEXT("총 ") << MeshCount << TEXT("개의 메쉬를 읽어오는 중입니다.") << endl;
				MeshCaches.reserve(MeshCount);
			}
		}
		else if (Token == TEXT("<Mesh>"))
		{
			shared_ptr<CMesh> Mesh{ make_shared<CMesh>() };

			Mesh->LoadMeshInfoFromFile(D3D12Device, D3D12GraphicsCommandList, InFile);
			MeshCaches.emplace(Mesh->GetName(), Mesh);
		}
		else if (Token == TEXT("<SkinnedMesh>"))
		{
			shared_ptr<CSkinnedMesh> SkinnedMesh{ make_shared<CSkinnedMesh>() };

			SkinnedMesh->LoadMeshInfoFromFile(D3D12Device, D3D12GraphicsCommandList, InFile);
			MeshCaches.emplace(SkinnedMesh->GetName(), SkinnedMesh);
		}
		else if (Token == TEXT("</Meshes>"))
		{
			tcout << TEXT("총 ") << MeshCaches.size() << TEXT("개의 메쉬를 읽어왔습니다.") << endl;
			break;
		}
	}
#endif
}

void CGameScene::LoadMaterialCachesFromFile(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, const tstring& FileName, unordered_map<tstring, shared_ptr<CMaterial>>& MaterialCaches)
{
	tstring Token{};

#ifdef READ_BINARY_FILE
	tifstream InFile{ FileName, ios::binary };

	while (true)
	{
		File::ReadStringFromFile(InFile, Token);

		if (Token == TEXT("<Materials>"))
		{
			UINT MaterialCount{ File::ReadIntegerFromFile(InFile) };

			if (MaterialCount > 0)
			{
				tcout << TEXT("총 ") << MaterialCount << TEXT("개의 메터리얼을 읽어오는 중입니다.") << endl;
				MaterialCaches.reserve(MaterialCount);
			}
		}
		else if (Token == TEXT("<Material>"))
		{
			shared_ptr<CMaterial> Material{ make_shared<CMaterial>() };

			Material->LoadMaterialInfoFromFile(D3D12Device, D3D12GraphicsCommandList, InFile);
			MaterialCaches.emplace(Material->GetName(), Material);
		}
		else if (Token == TEXT("</Materials>"))
		{
			tcout << TEXT("총 ") << MaterialCaches.size() << TEXT("개의 메터리얼을 읽어왔습니다.") << endl;
			break;
		}
	}

#else
	tifstream InFile{ FileName };

	while (InFile >> Token)
	{
		if (Token == TEXT("<Materials>"))
		{
			UINT MaterialCount{};

			InFile >> MaterialCount;

			if (MaterialCount > 0)
			{
				tcout << TEXT("총 ") << MaterialCount << TEXT("개의 메터리얼을 읽어오는 중입니다.") << endl;
				MaterialCaches.reserve(MaterialCount);
			}
		}
		else if (Token == TEXT("<Material>"))
		{
			shared_ptr<CMaterial> Material{ make_shared<CMaterial>() };

			Material->LoadMaterialInfoFromFile(D3D12Device, D3D12GraphicsCommandList, InFile);
			MaterialCaches.emplace(Material->GetName(), Material);
		}
		else if (Token == TEXT("</Materials>"))
		{
			tcout << TEXT("총 ") << MaterialCaches.size() << TEXT("개의 메터리얼을 읽어왔습니다.") << endl;
			break;
		}
	}
#endif
}

void CGameScene::LoadEventTriggerFromFile(const tstring& FileName)
{
	tstring Token{};

#ifdef READ_BINARY_FILE
	tifstream InFile{ FileName, ios::binary };

	while (true)
	{
		File::ReadStringFromFile(InFile, Token);

		if (Token == TEXT("<EventTriggers>"))
		{
			UINT TriggerCount{ File::ReadIntegerFromFile(InFile) };

			m_EventTriggers.reserve(TriggerCount);
		}
		else if (Token == TEXT("<EventTrigger>"))
		{
			// <Type>
			File::ReadStringFromFile(InFile, Token);

			UINT TriggerType{File::ReadIntegerFromFile(InFile)};
			shared_ptr<CEventTrigger> EventTrigger{};

			switch (TriggerType)
			{
			case 0:
				EventTrigger = make_shared<COpenDoorEventTrigger>();
				break;
			case 1:
				EventTrigger = make_shared<CPowerDownEventTrigger>();
				break;
			case 2:
				EventTrigger = make_shared<CSirenEventTrigger>();
				break;
			case 3:
				EventTrigger = make_shared<COpenGateEventTrigger>();
				break;
			}

			EventTrigger->LoadEventTriggerFromFile(InFile);

			// <TargetRootIndex>
			File::ReadStringFromFile(InFile, Token);

			UINT TargetRootIndex{ File::ReadIntegerFromFile(InFile) };

			// <TargetObject>
			File::ReadStringFromFile(InFile, Token);

			UINT TargetObjectCount{ File::ReadIntegerFromFile(InFile) };
			
			if (TargetObjectCount > 0)
			{
				for (UINT i = 0; i < TargetObjectCount; ++i)
				{
					File::ReadStringFromFile(InFile, Token);

					shared_ptr<CGameObject> TargetObject{ m_GameObjects[OBJECT_TYPE_STRUCTURE][TargetRootIndex]->FindFrame(Token) };

					EventTrigger->InsertEventObject(TargetObject);
				}
			}

			m_EventTriggers.push_back(EventTrigger);
		}
		else if (Token == TEXT("</EventTriggers>"))
		{
			break;
		}
	}
#else
	tifstream InFile{ FileName };
#endif

	UINT TriggerCount{ static_cast<UINT>(m_EventTriggers.size()) };

	for (const auto& EventTrigger : m_EventTriggers)
	{
		if (EventTrigger)
		{
			EventTrigger->SetInteractionUI(m_BilboardObjects[BILBOARD_OBJECT_TYPE_UI].back());
		}
	}
}

void CGameScene::BuildLights()
{
	LIGHT Lights[MAX_LIGHTS]{};

	Lights[0].m_IsActive = true;
	Lights[0].m_ShadowMapping = false;
	Lights[0].m_Type = LIGHT_TYPE_DIRECTIONAL;
	Lights[0].m_Position = XMFLOAT3(0.0f, 500.0f, 100.0f);
	Lights[0].m_Direction = XMFLOAT3(0.0f, -1.0f, -1.0f);
	Lights[0].m_Color = XMFLOAT4(0.1f, 0.1f, 0.1f, 0.0f);

	Lights[1].m_IsActive = true;
	Lights[1].m_ShadowMapping = true;
	Lights[1].m_Type = LIGHT_TYPE_SPOT;
	Lights[1].m_Position = XMFLOAT3(0.0f, 18.0f, 150.0f);
	Lights[1].m_Direction = XMFLOAT3(0.0f, -1.0f, 1.0f);
	Lights[1].m_Color = XMFLOAT4(0.05f, 0.05f, 0.01f, 0.0f);
	Lights[1].m_Attenuation = XMFLOAT3(3.0f, 2.0f, 1.0f);
	Lights[1].m_Falloff = 15.0f;
	Lights[1].m_Range = 500.0f;
	Lights[1].m_Theta = cosf(XMConvertToRadians(30.0f));
	Lights[1].m_Phi = cosf(XMConvertToRadians(60.0f));

	m_Lights.reserve(MAX_LIGHTS);
	m_Lights.push_back(Lights[0]);
	m_Lights.push_back(Lights[1]);
}

void CGameScene::BuildFog()
{
	m_MappedFog->m_Fog.m_Color = XMFLOAT4(0.0f, 0.015f, 0.03f, 1.0f);
	m_MappedFog->m_Fog.m_Density = 0.025f;
}
