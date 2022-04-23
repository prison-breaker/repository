#include "stdafx.h"
#include "GameScene.h"
#include "State_KeyUI.h"

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

				// 교도관 객체를 생성한다.
				shared_ptr<CGuard> Guard{ make_shared<CGuard>() };

				Guard->SetChild(ModelInfo->m_Model);
				Guard->SetTransformMatrix(TransformMatrix);
				Guard->UpdateTransform(Matrix4x4::Identity());
				Guard->SetAnimationController(D3D12Device, D3D12GraphicsCommandList, ModelInfo);
				Guard->FindPatrolNavPath(m_NavMesh, TargetPosition);
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
			tcout << endl;
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
				InFile >> Token;
				InFile >> TargetPosition.x >> TargetPosition.y >> TargetPosition.z;

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
			tcout << endl;
			break;
		}
	}
#endif
}

void CGameScene::LoadUIInfoFromFile(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, const tstring& FileName)
{
	tstring Token{};

	shared_ptr<CBilboardObject> Object{};

	tcout << FileName << TEXT(" 로드 시작...") << endl;

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
	tcout << FileName << TEXT(" 로드 완료...") << endl << endl;

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
	shared_ptr<CPlayer> Player{ static_pointer_cast<CPlayer>(m_GameObjects[OBJECT_TYPE_PLAYER].back()) };

	switch (wParam)
	{
	case '1': // 무기 스왑 - 펀치
		if (Player->SwapWeapon(WEAPON_TYPE_PUNCH))
		{
			m_BilboardObjects[BILBOARD_OBJECT_TYPE_UI][4]->SetActive(true);  // 4: Punch UI
			m_BilboardObjects[BILBOARD_OBJECT_TYPE_UI][6]->SetActive(false); // 6: Pistol UI
			m_BilboardObjects[BILBOARD_OBJECT_TYPE_UI][7]->SetActive(false); // 7: Bullet UI
		}
		break;
	case '2': // 무기 스왑 - 권총
		if (Player->SwapWeapon(WEAPON_TYPE_PISTOL))
		{
			m_BilboardObjects[BILBOARD_OBJECT_TYPE_UI][4]->SetActive(false); // 4: Punch UI
			m_BilboardObjects[BILBOARD_OBJECT_TYPE_UI][6]->SetActive(true);	 // 6: Pistol UI
			m_BilboardObjects[BILBOARD_OBJECT_TYPE_UI][7]->SetActive(true);	 // 7: Bullet UI
		}
		break;
	case 'b': // 바운딩 박스 렌더링 ON/OFF
	case 'B':
		(m_RenderBoundingBox) ? m_RenderBoundingBox = false : m_RenderBoundingBox = true;
		break;
	case 'p': // 안개 ON/OFF
	case 'P':
		(m_MappedFog->m_Fog.m_Density > 0.0f) ? m_MappedFog->m_Fog.m_Density = 0.0f : m_MappedFog->m_Fog.m_Density = 0.025f;
		break;
	case 'q': // 플레이어를 감옥 밖으로 이동
	case 'Q':
		Player->SetPosition(m_NavMesh->GetNavNodes()[500]->GetTriangle().m_Centroid);
		break;
	}
}

void CGameScene::ProcessInput(HWND hWnd, float ElapsedTime)
{	
	shared_ptr<CPlayer> Player{ static_pointer_cast<CPlayer>(m_GameObjects[OBJECT_TYPE_PLAYER].back()) };

	UpdatePerspective(hWnd, ElapsedTime, Player);

	// 1인칭 모드
	//if (GetAsyncKeyState('W') & 0x8000) Player->GetCamera()->Move(Vector3::ScalarProduct(15.0f * ElapsedTime, Player->GetCamera()->GetLook(), false));
	//if (GetAsyncKeyState('S') & 0x8000) Player->GetCamera()->Move(Vector3::ScalarProduct(-15.0f * ElapsedTime, Player->GetCamera()->GetLook(), false));
	//if (GetAsyncKeyState('A') & 0x8000) Player->GetCamera()->Move(Vector3::ScalarProduct(-15.0f * ElapsedTime, Player->GetCamera()->GetRight(), false));
	//if (GetAsyncKeyState('D') & 0x8000) Player->GetCamera()->Move(Vector3::ScalarProduct(15.0f * ElapsedTime, Player->GetCamera()->GetRight(), false));
	//return;

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
		InteractTrigger();
	}

	if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
	{
		InputMask |= INPUT_MASK_SHIFT;
	}

	// 미션 UI ON/OFF
	if (GetAsyncKeyState(VK_TAB) & 0x0001)
	{
		InputMask |= INPUT_MASK_TAB;
		static_pointer_cast<CMissionUI>(m_BilboardObjects[BILBOARD_OBJECT_TYPE_UI][0])->GetStateMachine()->ProcessInput(m_GameObjects, m_NavMesh, ElapsedTime, INPUT_MASK_TAB);
	}

	if (GetAsyncKeyState(VK_RBUTTON) & 0x8000)
	{
		InputMask |= INPUT_MASK_RMB;

	}

	if (GetAsyncKeyState(VK_LBUTTON) & 0x0001)
	{
		InputMask |= INPUT_MASK_LMB;
	}

	Player->ProcessInput(m_GameObjects, m_NavMesh, ElapsedTime, InputMask);
	(Player->GetCamera()->IsZoomIn()) ? m_BilboardObjects[BILBOARD_OBJECT_TYPE_UI][8]->SetActive(true) : m_BilboardObjects[BILBOARD_OBJECT_TYPE_UI][8]->SetActive(false);
}

void CGameScene::Animate(float ElapsedTime)
{
	for (UINT i = OBJECT_TYPE_PLAYER; i <= OBJECT_TYPE_STRUCTURE; ++i)
	{
		for (const auto& GameObject : m_GameObjects[i])
		{
			if (GameObject)
			{				
				GameObject->Animate(m_GameObjects, m_NavMesh, ElapsedTime);
			}
		}
	}

	for (UINT i = BILBOARD_OBJECT_TYPE_SKYBOX; i <= BILBOARD_OBJECT_TYPE_UI; ++i)
	{
		for (const auto& BilboardObject : m_BilboardObjects[i])
		{
			if (BilboardObject)
			{
				BilboardObject->Animate(m_GameObjects, m_NavMesh, ElapsedTime);
			}
		}
	}

	for (const auto& EventTrigger : m_EventTriggers) //문열기, 감시탑열기+배전함끄기, 문열기, 사이렌울리기, 권총습득(5), 열쇠습득(2), 열쇠로 열고나가기 = 
	{
		if (EventTrigger)
		{
			XMFLOAT3 Position{ m_GameObjects[OBJECT_TYPE_PLAYER].back()->GetPosition() };
			XMFLOAT3 LookDirection{ m_GameObjects[OBJECT_TYPE_PLAYER].back()->GetLook() };

			if (EventTrigger->IsInTriggerArea(Position, LookDirection))
			{
				EventTrigger->GenerateEventTrigger(ElapsedTime);
				break;
			}
		}
	}

	InteractSpotLight(ElapsedTime);
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
				MeshCaches.reserve(MeshCount);
				tcout << FileName << TEXT(" 로드 시작...") << endl;
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
			tcout << FileName << TEXT(" 로드 완료...(메쉬 수: ") << MeshCaches.size() << ")" << endl << endl;
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
				tcout << FileName << TEXT(" 로드 시작...") << endl;
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
			tcout << FileName << TEXT(" 로드 완료...(메쉬 수: ") << MeshCaches.size() << ")" << endl << endl;
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
				tcout << FileName << TEXT(" 로드 시작...") << endl;
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
			tcout << FileName << TEXT(" 로드 완료...(메터리얼 수: ") << MaterialCaches.size() << ")" << endl << endl;
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
				tcout << FileName << TEXT(" 로드 시작...") << endl;
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
			tcout << FileName << TEXT(" 로드 완료...(메터리얼 수: ") << MaterialCaches.size() << ")" << endl << endl;
			break;
		}
	}
#endif
}

void CGameScene::LoadEventTriggerFromFile(const tstring& FileName)
{
	tstring Token{};
	shared_ptr<CEventTrigger> EventTrigger{};

	tcout << FileName << TEXT(" 로드 시작...") << endl;

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
	tcout << FileName << TEXT(" 로드 완료...") << endl << endl;

	// 권총을 드롭하는 트리거를 추가한다.
	// 권총은 임의의 인덱스 교도관(Index: 2 ~ 14) 중 5명이 보유하고 있다.
	shared_ptr<bool[]> isVisited{ new bool[m_GameObjects[OBJECT_TYPE_NPC].size() - 2]{}, [](bool* p) { delete[] p; } };

	for (UINT i = 0; i < 5;)
	{
		UINT RandomIndex{ static_cast<UINT>((rand() % 13) + 2) };

		if (!isVisited[RandomIndex])
		{
			EventTrigger = make_shared<CGetPistolEventTrigger>();
			EventTrigger->InsertEventObject(m_GameObjects[OBJECT_TYPE_NPC][RandomIndex]);
			EventTrigger->InsertEventObject(m_GameObjects[OBJECT_TYPE_PLAYER].back());
			m_EventTriggers.push_back(EventTrigger);

			isVisited[RandomIndex] = true;
			++i;
		}
	}

	// 열쇠를 드롭하는 트리거를 추가한다.
	// 열쇠는 외형이 다른 교도관(Index: 0, 1)이 보유하고 있다.
	EventTrigger = make_shared<CGetKeyEventTrigger>();
	EventTrigger->InsertEventObject(m_GameObjects[OBJECT_TYPE_NPC][0]);
	m_EventTriggers.push_back(EventTrigger);

	EventTrigger = make_shared<CGetKeyEventTrigger>();
	EventTrigger->InsertEventObject(m_GameObjects[OBJECT_TYPE_NPC][1]);
	m_EventTriggers.push_back(EventTrigger);

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
	Lights[1].m_Position = XMFLOAT3(0.0f, 50.0f, 0.0f);

	m_SpotLightAngle = XMConvertToRadians(90.0f);
	Lights[1].m_Direction = Vector3::Normalize(XMFLOAT3(cosf(m_SpotLightAngle), -1.0f, sinf(m_SpotLightAngle)));
	Lights[1].m_Color = XMFLOAT4(1.0f, 1.0f, 0.3f, 0.0f);
	Lights[1].m_Attenuation = XMFLOAT3(0.5f, 0.01f, 0.0f);
	Lights[1].m_Falloff = 1.0f;
	Lights[1].m_Range = 500.0f;
	Lights[1].m_Theta = cosf(XMConvertToRadians(5.0f));
	Lights[1].m_Phi = cosf(XMConvertToRadians(10.0f));

	m_Lights.reserve(MAX_LIGHTS);
	m_Lights.push_back(Lights[0]);
	m_Lights.push_back(Lights[1]);
}

void CGameScene::BuildFog()
{
	m_MappedFog->m_Fog.m_Color = XMFLOAT4(0.0f, 0.015f, 0.03f, 1.0f);
	m_MappedFog->m_Fog.m_Density = 0.025f;
}

void CGameScene::UpdatePerspective(HWND hWnd, float ElapsedTime, const shared_ptr<CPlayer>& Player)
{
	RECT Rect{};

	GetWindowRect(hWnd, &Rect);

	POINT NewCursorPos{};
	POINT OldCursorPos{ Rect.right / 2, Rect.bottom / 2 };

	GetCursorPos(&NewCursorPos);
	SetCursorPos(OldCursorPos.x, OldCursorPos.y);

	XMFLOAT2 Delta{ 10.0f * ElapsedTime * (NewCursorPos.x - OldCursorPos.x), 10.0f * ElapsedTime * (NewCursorPos.y - OldCursorPos.y) };

	float NearestHitDistance{ FLT_MAX };
	float HitDistance{};

	XMFLOAT3 RayOrigin{ Player->GetPosition().x, Player->GetCamera()->GetPosition().y, Player->GetPosition().z };
	XMFLOAT3 RayDirection{ Vector3::Inverse(Player->GetLook()) };

	for (const auto& GameObject : m_GameObjects[OBJECT_TYPE_STRUCTURE])
	{
		if (GameObject)
		{
			shared_ptr<CGameObject> IntersectedObject{ GameObject->PickObjectByRayIntersection(RayOrigin, RayDirection, HitDistance, 3.0f) };

			if (IntersectedObject && (HitDistance < NearestHitDistance))
			{
				NearestHitDistance = HitDistance;
			}
		}
	}

	// 1인칭 모드
	//Player->GetCamera()->Rotate(Delta.y, Delta.x, 0.0f);

	Player->Rotate(Delta.y, Delta.x, 0.0f, ElapsedTime, NearestHitDistance);
}

void CGameScene::InteractTrigger()
{
	shared_ptr<CPlayer> Player{ static_pointer_cast<CPlayer>(m_GameObjects[OBJECT_TYPE_PLAYER].back()) };
	XMFLOAT3 Position{ Player->GetPosition() };
	XMFLOAT3 LookDirection{ Player->GetLook() };

	for (const auto& EventTrigger : m_EventTriggers)
	{
		if (EventTrigger)
		{
			if (!EventTrigger->IsInteracted())
			{
				if (EventTrigger->IsInTriggerArea(Position, LookDirection))
				{
					EventTrigger->SetInteracted(true);

					if (typeid(*EventTrigger) == typeid(CSirenEventTrigger))
					{
						tcout << TEXT("플레이어가 사이렌을 작동시켰습니다. 잠시 뒤 5명의 경찰이 이곳으로 올 것입니다!") << endl;

						UINT GuardCount{ static_cast<UINT>(m_GameObjects[OBJECT_TYPE_NPC].size()) };

						for (UINT i = 0; i < 5; ++i)
						{
							UINT Index{ rand() % GuardCount };
							shared_ptr<CGuard> Guard{ static_pointer_cast<CGuard>(m_GameObjects[OBJECT_TYPE_NPC][Index]) };

							if (Guard)
							{
								if (Guard->GetStateMachine()->IsInState(CGuardIdleState::GetInstance()) ||
									Guard->GetStateMachine()->IsInState(CGuardPatrolState::GetInstance()) ||
									Guard->GetStateMachine()->IsInState(CGuardReturnState::GetInstance()))
								{
									Guard->FindNavPath(m_NavMesh, Player->GetPosition(), m_GameObjects);
									Guard->GetStateMachine()->ChangeState(CGuardAssembleState::GetInstance());
								}
							}
						}
					}
					else if (typeid(*EventTrigger) == typeid(CGetPistolEventTrigger))
					{
						// 권총을 획득한 경우, UI 또한 주먹에서 권총으로 변경시킨다.
						m_BilboardObjects[BILBOARD_OBJECT_TYPE_UI][4]->SetActive(false); // 4: Punch UI
						m_BilboardObjects[BILBOARD_OBJECT_TYPE_UI][6]->SetActive(true);	 // 6: Pistol UI
						m_BilboardObjects[BILBOARD_OBJECT_TYPE_UI][7]->SetActive(true);	 // 7: Bullet UI
					}
					else if (typeid(*EventTrigger) == typeid(CGetKeyEventTrigger))
					{
						// 열쇠 획득 애니메이션을 출력하도록 CKeyUIActivationState 상태로 전이한다.
						static_pointer_cast<CKeyUI>(m_BilboardObjects[BILBOARD_OBJECT_TYPE_UI][5])->GetStateMachine()->SetCurrentState(CKeyUIActivationState::GetInstance());

						// 열쇠 획득 미션UI를 완료상태로 변경
						m_BilboardObjects[BILBOARD_OBJECT_TYPE_UI][0]->SetCellIndex(1, 5);
					}
					break;
				}
			}
		}
	}
}

void CGameScene::InteractSpotLight(float ElapsedTime)
{
	// 광원 포지션과 방향벡터를 활용해 평면에 도달하는 중심점을 계산한다. 
	float LightAngle{ Vector3::Angle(m_Lights[1].m_Direction, XMFLOAT3(0.0f, -1.0f, 0.0f)) };  // 빗변과 변의 각도 계산
	float HypotenuseLength{ m_Lights[1].m_Position.y / cosf(XMConvertToRadians(LightAngle)) }; // 빗변의 길이 계산
	float Radian{ HypotenuseLength * tanf(XMConvertToRadians(10.0f)) };                        // 광원이 쏘아지는 원의 반지름

	// 평면에 도달하는 점 계산
	XMFLOAT3 LightedPosition{ Vector3::Add(m_Lights[1].m_Position , Vector3::ScalarProduct(HypotenuseLength, m_Lights[1].m_Direction, false)) }; 

	for (const auto& Player : m_GameObjects[OBJECT_TYPE_PLAYER])
	{
		if (Player)
		{
			if (Player->IsActive())
			{
				if (Math::Distance(Player->GetPosition(), LightedPosition) < Radian)
				{
					XMFLOAT3 Direction = Vector3::Normalize(Vector3::Subtract(Player->GetPosition(), m_Lights[1].m_Position));

					shared_ptr<CGameObject> NearestIntersectedObject{};

					float NearestHitDistance{ FLT_MAX };
					float HitDistance{};
					bool HitCheck{};

					for (const auto& GameObject : m_GameObjects[OBJECT_TYPE_STRUCTURE])
					{
						if (GameObject)
						{
							shared_ptr<CGameObject> IntersectedObject{ GameObject->PickObjectByRayIntersection(m_Lights[1].m_Position, Direction, HitDistance, HypotenuseLength) };

							if (IntersectedObject && HitDistance < HypotenuseLength)
							{
								HitCheck = true;
								break;
							}
						}
					}

					if (!HitCheck)
					{
						for (const auto& GameObject : m_GameObjects[OBJECT_TYPE_NPC])
						{
							shared_ptr<CGuard> Guard{ static_pointer_cast<CGuard>(GameObject) };

							// 스팟조명과 충돌 할 경우 주변 범위에 있는 경찰들이 플레이어를 쫒기 시작한다.
							if (Math::Distance(LightedPosition, Guard->GetPosition()) < 300.0f)
							{
								if (Guard->GetStateMachine()->IsInState(CGuardIdleState::GetInstance()) ||
									Guard->GetStateMachine()->IsInState(CGuardPatrolState::GetInstance()) ||
									Guard->GetStateMachine()->IsInState(CGuardReturnState::GetInstance()))
								{
									Guard->FindNavPath(m_NavMesh, Player->GetPosition(), m_GameObjects);
									Guard->GetStateMachine()->ChangeState(CGuardAssembleState::GetInstance());
									cout << "스팟 조명으로 추적 시작" << endl;
								}
							}
						}
						return;
					}
				}
			}
		}		
	}

	m_SpotLightAngle += ElapsedTime;
	m_Lights[1].m_Direction = Vector3::Normalize(XMFLOAT3(cosf(m_SpotLightAngle), -1.0f, sinf(m_SpotLightAngle)));
}
