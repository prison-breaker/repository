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
	// 카메라 객체를 생성한다.
	shared_ptr<CCamera> Camera{ make_shared<CCamera>() };

	Camera->CreateShaderVariables(D3D12Device, D3D12GraphicsCommandList);
	Camera->GeneratePerspectiveProjectionMatrix(90.0f, (float)CLIENT_WIDTH / (float)CLIENT_HEIGHT, 1.0f, 500.0f);
	Camera->GenerateViewMatrix(XMFLOAT3(0.0f, 5.0f, -150.0f), XMFLOAT3(0.0f, 0.0f, 1.0f));

	// 플레이어 객체를 생성한다.
	m_Player = make_shared<CPlayer>();
	m_Player->SetCamera(Camera);

	// 파일로부터 씬 객체들을 생성하고 배치한다.
#ifdef READ_BINARY_FILE
	LoadSceneInfoFromFile(D3D12Device, D3D12GraphicsCommandList, TEXT("Model/GameScene.bin"));
#else
	LoadSceneInfoFromFile(D3D12Device, D3D12GraphicsCommandList, TEXT("Model/GameScene.txt"));
#endif

	// 그림자의 영향을 받는 객체들을 하나의 벡터에 저장한다.
	vector<shared_ptr<CGameObject>> ShadyObjects{};

	ShadyObjects.reserve(m_Guards.size() + m_Structures.size() + 2);
	ShadyObjects.push_back(m_Player);
	ShadyObjects.push_back(m_Ground);
	copy(m_Guards.begin(), m_Guards.end(), back_inserter(ShadyObjects));
	copy(m_Structures.begin(), m_Structures.end(), back_inserter(ShadyObjects));

	// 각 객체들을 렌더링하기 위한 쉐이더를 생성한다.
	m_DepthWriteShader = make_shared<CDepthWriteShader>(D3D12Device, D3D12GraphicsCommandList, m_Lights, ShadyObjects);
	m_DepthWriteShader->CreatePipelineState(D3D12Device, m_D3D12RootSignature.Get());

	m_DebugShader = make_shared<CDebugShader>();
	m_DebugShader->CreatePipelineState(D3D12Device, m_D3D12RootSignature.Get());

	shared_ptr<CShadowMapShader> ShadowMapShader{ make_shared<CShadowMapShader>(ShadyObjects) };

	ShadowMapShader->CreatePipelineState(D3D12Device, m_D3D12RootSignature.Get());
	m_Shaders.push_back(ShadowMapShader);

	shared_ptr<CSkyBoxShader> SkyBoxShader{ make_shared<CSkyBoxShader>(D3D12Device, D3D12GraphicsCommandList) };

	SkyBoxShader->CreatePipelineState(D3D12Device, m_D3D12RootSignature.Get());
	m_Shaders.push_back(SkyBoxShader);

	CTextureManager::GetInstance()->CreateCbvSrvUavDescriptorHeaps(D3D12Device);
	CTextureManager::GetInstance()->CreateShaderResourceViews(D3D12Device);

	CreateShaderVariables(D3D12Device, D3D12GraphicsCommandList);
}

void CGameScene::ReleaseObjects()
{
	ReleaseShaderVariables();
}
	
void CGameScene::CreateRootSignature(ID3D12Device* D3D12Device)
{
	CD3DX12_DESCRIPTOR_RANGE D3D12DescriptorRanges[4]{};

	D3D12DescriptorRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
	D3D12DescriptorRanges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);
	D3D12DescriptorRanges[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2);
	D3D12DescriptorRanges[3].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 3);

	CD3DX12_ROOT_PARAMETER D3D12RootParameters[8]{};

	D3D12RootParameters[ROOT_PARAMETER_TYPE_FRAMEWORK_INFO].InitAsConstantBufferView(0);					   // 프레임워크 정보(b0)
	D3D12RootParameters[ROOT_PARAMETER_TYPE_CAMERA].InitAsConstantBufferView(1);							   // 카메라 정보(b1)
	D3D12RootParameters[ROOT_PARAMETER_TYPE_LIGHT].InitAsConstantBufferView(2);							       // 조명 정보(b2)
	D3D12RootParameters[ROOT_PARAMETER_TYPE_OBJECT].InitAsConstants(21, 3);								       // 오브젝트 정보(b3)
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
	DX::ThrowIfFailed(D3D12Device->CreateRootSignature(0, D3D12SignatureBlob->GetBufferPointer(), D3D12SignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void**)m_D3D12RootSignature.GetAddressOf()));
}

void CGameScene::CreateShaderVariables(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	UINT Bytes{ (sizeof(CB_LIGHT) + 255) & ~255 };

	m_D3D12Lights = DX::CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, nullptr, Bytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, nullptr);
	DX::ThrowIfFailed(m_D3D12Lights->Map(0, nullptr, (void**)&m_MappedLights));
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
	if (m_Player)
	{
		m_Player->ReleaseUploadBuffers();
	}

	for (const auto& Guard : m_Guards)
	{
		if (Guard)
		{
			Guard->ReleaseUploadBuffers();
		}
	}

	if (m_Ground)
	{
		m_Ground->ReleaseUploadBuffers();
	}

	for (const auto& Structure : m_Structures)
	{
		if (Structure)
		{
			Structure->ReleaseShaderVariables();
		}
	}

	for (const auto& Shader : m_Shaders)
	{
		if (Shader)
		{
			Shader->ReleaseUploadBuffers();
		}
	}
}

void CGameScene::ProcessMouseMessage(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{

}

void CGameScene::ProcessKeyboardMessage(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{

}

void CGameScene::ProcessInput(HWND hWnd, float ElapsedTime)
{
	// 방향성 조명 방향 변경
	static float Angle = XMConvertToRadians(90.0f);

	if (GetAsyncKeyState(VK_NUMPAD4) & 0x8000)
	{
		Angle += ElapsedTime;

		m_Lights[0].m_Position.x = 60.0f * cosf(Angle);
		m_Lights[0].m_Position.z = 60.0f * sinf(Angle);
		m_Lights[0].m_Direction.x = cosf(Angle);
		m_Lights[0].m_Direction.z = sinf(Angle);
	}

	if (GetAsyncKeyState(VK_NUMPAD6) & 0x8000)
	{
		Angle -= ElapsedTime;

		m_Lights[0].m_Position.x = 60.0f * cosf(Angle);
		m_Lights[0].m_Position.z = 60.0f * sinf(Angle);
		m_Lights[0].m_Direction.x = cosf(Angle);
		m_Lights[0].m_Direction.z = sinf(Angle);
	}

	RECT Rect{};

	GetWindowRect(hWnd, &Rect);

	POINT NewCursorPos{};
	POINT OldCursorPos{ Rect.right / 2, Rect.bottom / 2 };

	GetCursorPos(&NewCursorPos);
	SetCursorPos(OldCursorPos.x, OldCursorPos.y);

	XMFLOAT2 Delta{ 10.0f * ElapsedTime * (NewCursorPos.x - OldCursorPos.x), 10.0f * ElapsedTime * (NewCursorPos.y - OldCursorPos.y) };

	// 1인칭 모드
	//m_Player->GetCamera()->Rotate(Delta.y, Delta.x, 0.0f);

	//if (GetAsyncKeyState('W') & 0x8000)
	//{
	//	m_Player->GetCamera()->Move(Vector3::ScalarProduct(10.0f * ElapsedTime, m_Player->GetCamera()->GetLook(), false));
	//}

	//if (GetAsyncKeyState('S') & 0x8000)
	//{
	//	m_Player->GetCamera()->Move(Vector3::ScalarProduct(-10.0f * ElapsedTime, m_Player->GetCamera()->GetLook(), false));
	//}

	//if (GetAsyncKeyState('A') & 0x8000)
	//{
	//	m_Player->GetCamera()->Move(Vector3::ScalarProduct(-10.0f * ElapsedTime, m_Player->GetCamera()->GetRight(), false));
	//}

	//if (GetAsyncKeyState('D') & 0x8000)
	//{
	//	m_Player->GetCamera()->Move(Vector3::ScalarProduct(10.0f * ElapsedTime, m_Player->GetCamera()->GetRight(), false));
	//}

	// 3인칭 모드
	m_Player->Rotate(Delta.y, Delta.x, 0.0f, ElapsedTime);

	if (GetAsyncKeyState('W') & 0x8000)
	{
		m_Player->Move(m_Player->GetLook(), 5.0f * ElapsedTime);
	}

	if (GetAsyncKeyState('S') & 0x8000)
	{
		m_Player->Move(m_Player->GetLook(), -5.0f * ElapsedTime);
	}

	if (GetAsyncKeyState('A') & 0x8000)
	{
		m_Player->Move(m_Player->GetRight(), -5.0f * ElapsedTime);
	}

	if (GetAsyncKeyState('D') & 0x8000)
	{
		m_Player->Move(m_Player->GetRight(), 5.0f * ElapsedTime);
	}
}

void CGameScene::Animate(float ElapsedTime)
{

}

void CGameScene::PreRender(ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	if (m_D3D12RootSignature)
	{
		D3D12GraphicsCommandList->SetGraphicsRootSignature(m_D3D12RootSignature.Get());
	}

	CTextureManager::GetInstance()->SetDescriptorHeap(D3D12GraphicsCommandList);

	if (m_DepthWriteShader)
	{
		m_DepthWriteShader->PrepareShadowMap(D3D12GraphicsCommandList);
	}

	UpdateShaderVariables(D3D12GraphicsCommandList);
}

void CGameScene::Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList) const
{
	m_Player->GetCamera()->RSSetViewportsAndScissorRects(D3D12GraphicsCommandList);
	m_Player->GetCamera()->UpdateShaderVariables(D3D12GraphicsCommandList);

	for (const auto& Shader : m_Shaders)
	{
		if (Shader)
		{
			Shader->Render(D3D12GraphicsCommandList, m_Player->GetCamera());
		}
	}

	if (m_DebugShader)
	{
		m_DebugShader->Render(D3D12GraphicsCommandList, m_Player->GetCamera());
		m_Player->RenderBoundingBox(D3D12GraphicsCommandList, m_Player->GetCamera());

		for (const auto& Guards : m_Guards)
		{
			Guards->RenderBoundingBox(D3D12GraphicsCommandList, m_Player->GetCamera());
		}

		for (const auto& Structure : m_Structures)
		{
			Structure->RenderBoundingBox(D3D12GraphicsCommandList, m_Player->GetCamera());
		}
	}
}

void CGameScene::LoadSceneInfoFromFile(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, const tstring& FileName)
{
	tstring Token{};

	shared_ptr<CGameObject> Object{};
	shared_ptr<CGameObject> Model{};
	UINT ObjectType{};

#ifdef READ_BINARY_FILE
	tifstream InFile{ FileName, ios::binary };

	while (true)
	{
		File::ReadStringFromFile(InFile, Token);

		if (Token == TEXT("<Name>"))
		{
			File::ReadStringFromFile(InFile, Token);

			Model = CGameObject::LoadObjectFromFile(D3D12Device, D3D12GraphicsCommandList, Token);
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
				m_Player->SetAlive(true);
				m_Player->SetChild(Model);
				m_Player->SetTransformMatrix(TransformMatrix);
				m_Player->SetPosition(XMFLOAT3(50.0f, 0.0f, -50.0f));
				break;
			case OBJECT_TYPE_NPC:
				Object = make_shared<CGameObject>();
				Object->SetAlive(true);
				Object->SetChild(Model);
				Object->SetTransformMatrix(TransformMatrix);
				m_Guards.push_back(Object);
				break;
			case OBJECT_TYPE_TERRAIN:
				m_Ground = make_shared<CGameObject>();
				m_Ground->SetAlive(true);
				m_Ground->SetChild(Model);
				m_Ground->SetTransformMatrix(TransformMatrix);
				break;
			case OBJECT_TYPE_STRUCTURE:
				Object = make_shared<CGameObject>();
				Object->SetAlive(true);
				Object->SetChild(Model);
				Object->SetTransformMatrix(TransformMatrix);
				m_Structures.push_back(Object);
				break;
			}
		}
		else if (Token == TEXT("</GameScene>"))
		{
			break;
		}
	}
#else
	tifstream InFile{ FileName };

	while (InFile >> Token)
	{
		if (Token == TEXT("<Name>"))
		{
			InFile >> Token;

			Model = CGameObject::LoadObjectFromFile(D3D12Device, D3D12GraphicsCommandList, Token);
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
				m_Player->SetAlive(true);
				m_Player->SetChild(Model);
				m_Player->SetTransformMatrix(TransformMatrix);
				break;
			case OBJECT_TYPE_NPC:
				Object = make_shared<CGameObject>();
				Object->SetAlive(true);
				Object->SetChild(Model);
				Object->SetTransformMatrix(TransformMatrix);
				m_Guards.push_back(Object);
				break;
			case OBJECT_TYPE_TERRAIN:
				m_Ground = make_shared<CGameObject>();
				m_Ground->SetAlive(true);
				m_Ground->SetChild(Model);
				m_Ground->SetTransformMatrix(TransformMatrix);
				break;
			case OBJECT_TYPE_STRUCTURE:
				Object = make_shared<CGameObject>();
				Object->SetAlive(true);
				Object->SetChild(Model);
				Object->SetTransformMatrix(TransformMatrix);
				m_Structures.push_back(Object);
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

void CGameScene::BuildLights()
{
	LIGHT Lights[MAX_LIGHTS]{};

	//Lights[0].m_IsActive = true;
	//Lights[0].m_Type = LIGHT_TYPE_DIRECTIONAL;
	//Lights[0].m_Position = XMFLOAT3(0.0f, 250.0f, 0.0f);
	//Lights[0].m_Direction = XMFLOAT3(0.0f, -0.8f, -1.0f);
	//Lights[0].m_Color = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f); // XMFLOAT4(0.45f, 0.45f, 0.45f, 1.0f);
	//Lights[0].m_Range = 500.0f;

	Lights[0].m_IsActive = true;
	Lights[0].m_Type = LIGHT_TYPE_SPOT;
	Lights[0].m_Position = XMFLOAT3(0.0f, 8.0f, 60.0f);
	Lights[0].m_Direction = XMFLOAT3(0.0f, -0.6f, 1.0f);
	Lights[0].m_Color = XMFLOAT4(0.2f, 0.2f, 0.0f, 1.0f);
	Lights[0].m_Attenuation = XMFLOAT3(0.5f, 0.1f, 0.01f);
	Lights[0].m_Falloff = 5.0f;
	Lights[0].m_Range = 500.0f;
	Lights[0].m_Theta = cosf(XMConvertToRadians(60.0f));
	Lights[0].m_Phi = cosf(XMConvertToRadians(90.0f));

	m_Lights.reserve(MAX_LIGHTS);
	m_Lights.push_back(Lights[0]);
	//m_Lights.push_back(Lights[1]);
}
