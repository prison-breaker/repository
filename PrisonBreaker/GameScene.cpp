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
				m_GameObjects[ObjectType].push_back(make_shared<CPlayer>(D3D12Device, D3D12GraphicsCommandList));
				m_GameObjects[ObjectType].back()->SetActive(true);
				m_GameObjects[ObjectType].back()->SetChild(ModelInfo->m_Model);
				m_GameObjects[ObjectType].back()->SetTransformMatrix(TransformMatrix);
				m_GameObjects[ObjectType].back()->SetAnimationController(D3D12Device, D3D12GraphicsCommandList, ModelInfo);
				m_GameObjects[ObjectType].back()->FindFrame(TEXT("gun_pr_1"))->SetActive(false);
				break;
			case OBJECT_TYPE_NPC:
				m_GameObjects[ObjectType].push_back(make_shared<CGuard>());
				m_GameObjects[ObjectType].back()->SetActive(true);
				m_GameObjects[ObjectType].back()->SetChild(ModelInfo->m_Model);
				m_GameObjects[ObjectType].back()->SetTransformMatrix(TransformMatrix);
				m_GameObjects[ObjectType].back()->SetAnimationController(D3D12Device, D3D12GraphicsCommandList, ModelInfo);
				m_GameObjects[ObjectType].back()->SetAnimationClip(static_cast<UINT>(Random::Random(0.0f, 4.0f)));
				break;
			case OBJECT_TYPE_TERRAIN:
			case OBJECT_TYPE_STRUCTURE:
				m_GameObjects[ObjectType].push_back(make_shared<CGameObject>());
				m_GameObjects[ObjectType].back()->SetActive(true);
				m_GameObjects[ObjectType].back()->SetChild(ModelInfo->m_Model);
				m_GameObjects[ObjectType].back()->SetTransformMatrix(TransformMatrix);
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
				m_GameObjects[ObjectType].push_back(make_shared<CPlayer>(D3D12Device, D3D12GraphicsCommandList));
				m_GameObjects[ObjectType].back()->SetActive(true);
				m_GameObjects[ObjectType].back()->SetChild(ModelInfo->m_Model);
				m_GameObjects[ObjectType].back()->SetTransformMatrix(TransformMatrix);
				m_GameObjects[ObjectType].back()->SetAnimationController(D3D12Device, D3D12GraphicsCommandList, ModelInfo);
				m_GameObjects[ObjectType].back()->FindFrame(TEXT("gun_pr_1"))->SetActive(false);
				break;
			case OBJECT_TYPE_NPC:
				m_GameObjects[ObjectType].push_back(make_shared<CGuard>());
				m_GameObjects[ObjectType].back()->SetActive(true);
				m_GameObjects[ObjectType].back()->SetChild(ModelInfo->m_Model);
				m_GameObjects[ObjectType].back()->SetTransformMatrix(TransformMatrix);
				m_GameObjects[ObjectType].back()->SetAnimationController(D3D12Device, D3D12GraphicsCommandList, ModelInfo);
				m_GameObjects[ObjectType].back()->SetAnimationClip(static_cast<UINT>(Random::Random(0.0f, 4.0f)));
				break;
			case OBJECT_TYPE_TERRAIN:
			case OBJECT_TYPE_STRUCTURE:
				m_GameObjects[ObjectType].push_back(make_shared<CGameObject>());
				m_GameObjects[ObjectType].back()->SetActive(true);
				m_GameObjects[ObjectType].back()->SetChild(ModelInfo->m_Model);
				m_GameObjects[ObjectType].back()->SetTransformMatrix(TransformMatrix);
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
}

void CGameScene::CreateRootSignature(ID3D12Device* D3D12Device)
{
	CD3DX12_DESCRIPTOR_RANGE D3D12DescriptorRanges[4]{};

	D3D12DescriptorRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
	D3D12DescriptorRanges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);
	D3D12DescriptorRanges[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2);
	D3D12DescriptorRanges[3].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 3);

	CD3DX12_ROOT_PARAMETER D3D12RootParameters[10]{};

	D3D12RootParameters[ROOT_PARAMETER_TYPE_FRAMEWORK_INFO].InitAsConstantBufferView(0);					   // 프레임워크 정보(b0)
	D3D12RootParameters[ROOT_PARAMETER_TYPE_CAMERA].InitAsConstantBufferView(1);							   // 카메라 정보(b1)
	D3D12RootParameters[ROOT_PARAMETER_TYPE_LIGHT].InitAsConstantBufferView(2);							       // 조명 정보(b2)
	D3D12RootParameters[ROOT_PARAMETER_TYPE_OBJECT].InitAsConstants(23, 3);								       // 오브젝트 정보(b3)
	D3D12RootParameters[ROOT_PARAMETER_TYPE_BONE_OFFSET].InitAsConstantBufferView(4);						   // 조명 정보(b4)
	D3D12RootParameters[ROOT_PARAMETER_TYPE_BONE_TRANSFORM].InitAsConstantBufferView(5);					   // 조명 정보(b5)
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
}

void CGameScene::UpdateShaderVariables(ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	memcpy(m_MappedLights->m_Lights, m_Lights.data(), sizeof(CB_LIGHT) * (UINT)m_Lights.size());
	D3D12GraphicsCommandList->SetGraphicsRootConstantBufferView(ROOT_PARAMETER_TYPE_LIGHT, m_D3D12Lights->GetGPUVirtualAddress());
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
	case '0': // 권총 획득
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
	case 'b':
	case 'B':
		(m_RenderBoundingBox) ? m_RenderBoundingBox = false : m_RenderBoundingBox = true;
		break;
	}
}

void CGameScene::ProcessInput(HWND hWnd, float ElapsedTime)
{
	// 방향성 조명 방향 변경
	static float Angle = XMConvertToRadians(90.0f);

	if (GetAsyncKeyState(VK_NUMPAD4) & 0x8000)
	{
		Angle += ElapsedTime;

		m_Lights[1].m_Position.x = 100.0f * cosf(Angle);
		m_Lights[1].m_Position.z = 100.0f * sinf(Angle) + 50.0f;
		m_Lights[1].m_Direction.x = cosf(Angle);
		m_Lights[1].m_Direction.z = sinf(Angle);
	}

	if (GetAsyncKeyState(VK_NUMPAD6) & 0x8000)
	{
		Angle -= ElapsedTime;

		m_Lights[1].m_Position.x = 100.0f * cosf(Angle);
		m_Lights[1].m_Position.z = 100.0f * sinf(Angle) + 50.0f;
		m_Lights[1].m_Direction.x = cosf(Angle);
		m_Lights[1].m_Direction.z = sinf(Angle);
	}

	// 미션UI ON
	if (GetAsyncKeyState(VK_TAB) & 0x8000)
	{
		m_BilboardObjects[BILBOARD_OBJECT_TYPE_UI][0]->SetKeyFrameIndex(0, 0);
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

	UINT Action{};
	XMFLOAT3 Direction{};
	float Speed{ 3.15f };

	if (GetAsyncKeyState('W') & 0x8000) Action |= MOVE_FORWARD;
	if (GetAsyncKeyState('S') & 0x8000) Action |= MOVE_BACKWARD;
	if (GetAsyncKeyState('A') & 0x8000) Action |= MOVE_LEFT_STRAFE;
	if (GetAsyncKeyState('D') & 0x8000) Action |= MOVE_RIGHT_STRAFE;
	if (GetAsyncKeyState(VK_SHIFT) & 0x8000) Action |= RUNNING;
	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) Action |= PUNCHING;
	if (GetAsyncKeyState(VK_RBUTTON) & 0x8000) Action |= SHOOTING;

	switch (Action)
	{
	case IDLE:
	case RUNNING:
		Player->SetAnimationClip(0); // 0: Idle
		return;

	case PUNCHING:
		Player->SetAnimationClip(7); // 7: Punching
		return;

	case SHOOTING:
		Player->SetAnimationClip(8); // 8: Shooting
		return;

	case MOVE_FORWARD:
		Direction = Player->GetLook();
		Player->SetAnimationClip(1); // 1: Crouched Walk
		break;
	case MOVE_FORWARD | MOVE_LEFT_STRAFE:
		Direction = Vector3::Add(Player->GetLook(), XMFLOAT3(-Player->GetRight().x, Player->GetRight().y, -Player->GetRight().z));
		Player->SetAnimationClip(1); // 1: Crouched Walk
		break;
	case MOVE_FORWARD | MOVE_RIGHT_STRAFE:
		Direction = Vector3::Add(Player->GetLook(), Player->GetRight());
		Player->SetAnimationClip(1); // 1: Crouched Walk
		break;
	case MOVE_FORWARD | RUNNING:
		Direction = Player->GetLook();
		Speed = 12.6f;
		Player->SetAnimationClip(4); // 4: Running
		break;
	case MOVE_FORWARD | MOVE_LEFT_STRAFE | RUNNING:
		Direction = Vector3::Add(Player->GetLook(), XMFLOAT3(-Player->GetRight().x, Player->GetRight().y, -Player->GetRight().z));
		Speed = 12.6f;
		Player->SetAnimationClip(4); // 4: Running
		break;
	case MOVE_FORWARD | MOVE_RIGHT_STRAFE | RUNNING:
		Direction = Vector3::Add(Player->GetLook(), Player->GetRight());
		Speed = 12.6f;
		Player->SetAnimationClip(4); // 4: Running
		break;

	case MOVE_BACKWARD:
	case MOVE_BACKWARD | RUNNING:
		Direction = Player->GetLook();
		Speed = -3.15f;
		Player->SetAnimationClip(1); // 1: Crouched Walk
		break;
	case MOVE_BACKWARD | MOVE_LEFT_STRAFE:
	case MOVE_BACKWARD | MOVE_LEFT_STRAFE | RUNNING:
		Direction = Vector3::Add(Player->GetLook(), Player->GetRight());
		Speed = -3.15f;
		Player->SetAnimationClip(1); // 1: Crouched Walk
		break;
	case MOVE_BACKWARD | MOVE_RIGHT_STRAFE:
	case MOVE_BACKWARD | MOVE_RIGHT_STRAFE | RUNNING:
		Direction = Vector3::Add(Player->GetLook(), XMFLOAT3(-Player->GetRight().x, Player->GetRight().y, -Player->GetRight().z));
		Speed = -3.15f;
		Player->SetAnimationClip(1); // 1: Crouched Walk
		break;

	case MOVE_LEFT_STRAFE:
		Direction = Player->GetRight();
		Speed = -3.15f;
		Player->SetAnimationClip(2); // 2: Left Strafe Walk
		break;
	case MOVE_LEFT_STRAFE | RUNNING:
		Direction = Player->GetRight();
		Speed = -12.6f;
		Player->SetAnimationClip(5); // 5: Left Strafe Running
		break;

	case MOVE_RIGHT_STRAFE:
		Direction = Player->GetRight();
		Player->SetAnimationClip(3); // 3: Right Strafe Walk
		break;
	case MOVE_RIGHT_STRAFE | RUNNING:
		Direction = Player->GetRight();
		Speed = 12.6f;
		Player->SetAnimationClip(6); // 6: Right Strafe Running
		break;
	}

	Player->Move(Direction, Speed * ElapsedTime);
}

void CGameScene::Animate(float ElapsedTime)
{
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
