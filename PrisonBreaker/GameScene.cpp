#include "stdafx.h"
#include "GameScene.h"
#include "Framework.h"

void CGameScene::OnCreate(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, ID3D12RootSignature* D3D12RootSignature)
{
	BuildObjects(D3D12Device, D3D12GraphicsCommandList, D3D12RootSignature);
	BuildLights();
	BuildFog();
}

void CGameScene::OnDestroy()
{

}
	
void CGameScene::BuildObjects(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, ID3D12RootSignature* D3D12RootSignature)
{
	// 렌더링에 필요한 셰이더 객체(PSO)를 생성한다.
	shared_ptr<CGraphicsShader> Shader{ make_shared<CDepthWriteShader>(D3D12Device, D3D12GraphicsCommandList) };

	Shader->CreatePipelineState(D3D12Device, D3D12RootSignature, 2);
	CShaderManager::GetInstance()->RegisterShader(TEXT("DepthWriteShader"), Shader);

	Shader = make_shared<CShadowMapShader>();
	Shader->CreatePipelineState(D3D12Device, D3D12RootSignature, 2);
	CShaderManager::GetInstance()->RegisterShader(TEXT("ShadowMapShader"), Shader);

	Shader = make_shared<CSkyBoxShader>();
	Shader->CreatePipelineState(D3D12Device, D3D12RootSignature, 0);
	CShaderManager::GetInstance()->RegisterShader(TEXT("SkyBoxShader"), Shader);

	//Shader = make_shared<CUIShader>();
	//Shader->CreatePipelineState(D3D12Device, D3D12RootSignature, 0);
	//CShaderManager::GetInstance()->RegisterShader(TEXT("UIShader"), Shader);

	Shader = make_shared<CDebugShader>();
	Shader->CreatePipelineState(D3D12Device, D3D12RootSignature, 0);
	CShaderManager::GetInstance()->RegisterShader(TEXT("DebugShader"), Shader);

	// 파일로부터 NavMesh 객체를 생성한다.
	m_NavMesh = make_shared<CNavMesh>();

#ifdef READ_BINARY_FILE
	m_NavMesh->LoadNavMeshFromFile(TEXT("Navigation/NavMesh.bin"));
#else
	m_NavMesh->LoadNavMeshFromFile(TEXT("Navigation/NavMesh.txt"));
#endif

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
				Player->UpdateTransform(Matrix4x4::Identity());
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
				Architecture->UpdateTransform(Matrix4x4::Identity());
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

void CGameScene::CreateShaderVariables(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	UINT Bytes{ (sizeof(CB_LIGHT) + 255) & ~255 };

	m_D3D12Lights = DX::CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, nullptr, Bytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, nullptr);
	DX::ThrowIfFailed(m_D3D12Lights->Map(0, nullptr, reinterpret_cast<void**>(&m_MappedLights)));

	Bytes = (sizeof(CB_FOG) + 255) & ~255;

	m_D3D12Fog = DX::CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, nullptr, Bytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, nullptr);
	DX::ThrowIfFailed(m_D3D12Fog->Map(0, nullptr, reinterpret_cast<void**>(&m_MappedFog)));
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
	shared_ptr<CPlayer> Player{ static_pointer_cast<CPlayer>(m_GameObjects[OBJECT_TYPE_PLAYER][CFramework::GetInstance()->GetSocketInfo().m_ID]) };

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
		Player->SetPosition(m_NavMesh->GetNavNodes()[750]->GetTriangle().m_Centroid);
		break;
	case 'i':
	case 'I':
		(m_InvincibleMode) ? m_InvincibleMode = false : m_InvincibleMode = true;
		break;
	}
}

void CGameScene::ProcessInput(HWND hWnd, float ElapsedTime)
{	
	shared_ptr<CPlayer> Player{ static_pointer_cast<CPlayer>(m_GameObjects[OBJECT_TYPE_PLAYER][CFramework::GetInstance()->GetSocketInfo().m_ID]) };

	UpdatePerspective(hWnd, ElapsedTime, Player);

	// 1인칭 모드
	//if (GetAsyncKeyState('W') & 0x8000) Player->GetCamera()->Move(Vector3::ScalarProduct(15.0f * ElapsedTime, Player->GetCamera()->GetLook(), false));
	//if (GetAsyncKeyState('S') & 0x8000) Player->GetCamera()->Move(Vector3::ScalarProduct(-15.0f * ElapsedTime, Player->GetCamera()->GetLook(), false));
	//if (GetAsyncKeyState('A') & 0x8000) Player->GetCamera()->Move(Vector3::ScalarProduct(-15.0f * ElapsedTime, Player->GetCamera()->GetRight(), false));
	//if (GetAsyncKeyState('D') & 0x8000) Player->GetCamera()->Move(Vector3::ScalarProduct(15.0f * ElapsedTime, Player->GetCamera()->GetRight(), false));
	//return;

	m_InputMask = INPUT_MASK_NONE;

	if (GetAsyncKeyState('W') & 0x8000)
	{
		m_InputMask |= INPUT_MASK_W;
	}

	if (GetAsyncKeyState('S') & 0x8000)
	{
		m_InputMask |= INPUT_MASK_S;
	}

	if (GetAsyncKeyState('A') & 0x8000)
	{
		m_InputMask |= INPUT_MASK_A;
	}

	if (GetAsyncKeyState('D') & 0x8000)
	{
		m_InputMask |= INPUT_MASK_D;
	}

	if (GetAsyncKeyState('F') & 0x0001)
	{
		m_InputMask |= INPUT_MASK_F;
	}

	if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
	{
		m_InputMask |= INPUT_MASK_SHIFT;
	}

	// 미션 UI ON/OFF
	if (GetAsyncKeyState(VK_TAB) & 0x0001)
	{
		m_InputMask |= INPUT_MASK_TAB;

		static_pointer_cast<CMissionUI>(m_BilboardObjects[BILBOARD_OBJECT_TYPE_UI][0])->GetStateMachine()->ProcessInput(ElapsedTime, m_InputMask);
	}

	if (GetAsyncKeyState(VK_LBUTTON) & 0x0001)
	{
		m_InputMask |= INPUT_MASK_LMB;
	}

	if (GetAsyncKeyState(VK_RBUTTON) & 0x8000)
	{
		m_InputMask |= INPUT_MASK_RMB;
	}

	//Player->ProcessInput(ElapsedTime, m_InputMask);
	(Player->GetCamera()->IsZoomIn()) ? m_BilboardObjects[BILBOARD_OBJECT_TYPE_UI][8]->SetActive(true) : m_BilboardObjects[BILBOARD_OBJECT_TYPE_UI][8]->SetActive(false);
}

void CGameScene::Animate(float ElapsedTime)
{
	for (const auto& BilboardObject : m_BilboardObjects[BILBOARD_OBJECT_TYPE_UI])
	{
		if (BilboardObject)
		{
			BilboardObject->Animate(ElapsedTime);
		}
	}
	
	for (const auto& EventTrigger : m_EventTriggers)
	{
		if (EventTrigger)
		{
			EventTrigger->Update(ElapsedTime);
		}
	}
}

void CGameScene::PreRender(ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	CTextureManager::GetInstance()->SetDescriptorHeap(D3D12GraphicsCommandList);
	
	static_pointer_cast<CDepthWriteShader>(CShaderManager::GetInstance()->GetShader(TEXT("DepthWriteShader")))->PrepareShadowMap(D3D12GraphicsCommandList, m_Lights, m_GameObjects);

	UpdateShaderVariables(D3D12GraphicsCommandList);
}

void CGameScene::Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	shared_ptr<CPlayer> Player{ static_pointer_cast<CPlayer>(m_GameObjects[OBJECT_TYPE_PLAYER][CFramework::GetInstance()->GetSocketInfo().m_ID]) };

	Player->GetCamera()->RSSetViewportsAndScissorRects(D3D12GraphicsCommandList);
	Player->GetCamera()->UpdateShaderVariables(D3D12GraphicsCommandList);

	CTextureManager::GetInstance()->GetTexture(TEXT("ShadowMap"))->UpdateShaderVariable(D3D12GraphicsCommandList);

	for (UINT i = OBJECT_TYPE_PLAYER; i <= OBJECT_TYPE_STRUCTURE; ++i)
	{
		for (const auto& GameObject : m_GameObjects[i])
		{
			if (GameObject)
			{
				GameObject->Render(D3D12GraphicsCommandList, Player->GetCamera().get(), RENDER_TYPE_STANDARD);
			}
		}
	}

	for (UINT i = BILBOARD_OBJECT_TYPE_SKYBOX; i <= BILBOARD_OBJECT_TYPE_UI; ++i)
	{
		for (const auto& BilboardObject : m_BilboardObjects[i])
		{
			if (BilboardObject)
			{
				BilboardObject->Render(D3D12GraphicsCommandList, Player->GetCamera().get(), RENDER_TYPE_STANDARD);
			}
		}
	}

	if (m_RenderBoundingBox)
	{
		static_pointer_cast<CDebugShader>(CShaderManager::GetInstance()->GetShader(TEXT("DebugShader")))->Render(D3D12GraphicsCommandList, Player->GetCamera().get(), m_GameObjects, 0);
	}
}

void CGameScene::PostRender(ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{

}

void CGameScene::ProcessPacket()
{
	// Send to msg data.
	SOCKET_INFO SocketInfo{ CFramework::GetInstance()->GetSocketInfo() };
	MSG_TYPE MsgType{ MSG_TYPE_INGAME };
	int ReturnValue{ send(SocketInfo.m_Socket, (char*)&MsgType, sizeof(MsgType), 0) };

	if (ReturnValue == SOCKET_ERROR)
	{
		Server::ErrorDisplay("send()");
	}

	// Send to packet data.
	CLIENT_TO_SERVER_DATA SendedPacketData{ m_InputMask, m_GameObjects[OBJECT_TYPE_PLAYER][SocketInfo.m_ID]->GetWorldMatrix() };

	ReturnValue = send(SocketInfo.m_Socket, (char*)&SendedPacketData, sizeof(SendedPacketData), 0);

	if (ReturnValue == SOCKET_ERROR)
	{
		Server::ErrorDisplay("send()");
	}

	// Receive updated packet data.
	SERVER_TO_CLIENT_DATA ReceivedPacketData{};

	ReturnValue = recv(SocketInfo.m_Socket, (char*)&ReceivedPacketData, sizeof(ReceivedPacketData), MSG_WAITALL);

	if (ReturnValue == SOCKET_ERROR)
	{
		Server::ErrorDisplay("recv()");
	}
	else if (ReturnValue == 0)
	{
		tcout << "서버가 종료되었습니다." << endl;
		closesocket(SocketInfo.m_Socket);
	}
	else
	{
		shared_ptr<CPlayer> Player{};

		for (UINT i = 0; i < MAX_CLIENT_CAPACITY; ++i)
		{
			if (m_GameObjects[OBJECT_TYPE_PLAYER][i]->IsActive())
			{
				Player = static_pointer_cast<CPlayer>(m_GameObjects[OBJECT_TYPE_PLAYER][i]);
				Player->GetAnimationController()->UpdateAnimationClip(ANIMATION_TYPE_LOOP);

				if (Player->GetAnimationController()->GetAnimationClipType() != ReceivedPacketData.m_PlayerAnimationClipTypes[i])
				{
					Player->GetAnimationController()->SetAnimationClipType(ReceivedPacketData.m_PlayerAnimationClipTypes[i]);
				}

				Player->SetTransformMatrix(ReceivedPacketData.m_PlayerWorldMatrices[i]);
				Player->UpdateTransform(Matrix4x4::Identity());
			}
		}

		for (UINT i = 0; i < MAX_NPC_COUNT; ++i)
		{
			if (m_GameObjects[OBJECT_TYPE_NPC][i]->IsActive())
			{
				shared_ptr<CGuard> Guard{ static_pointer_cast<CGuard>(m_GameObjects[OBJECT_TYPE_NPC][i]) };

				Guard->GetAnimationController()->UpdateAnimationClip(ANIMATION_TYPE_LOOP);

				if (Guard->GetAnimationController()->GetAnimationClipType() != ReceivedPacketData.m_NPCAnimationClipTypes[i])
				{
					Guard->GetAnimationController()->SetAnimationClipType(ReceivedPacketData.m_NPCAnimationClipTypes[i]);
				}

				Guard->SetTransformMatrix(ReceivedPacketData.m_NPCWorldMatrices[i]);
				Guard->UpdateTransform(Matrix4x4::Identity());
			}
		}

		// 메세지 타입에 트리거와 관련된 내용이 포함돼있다면 상호작용을 수행한다.
		for (const auto& EventTrigger : m_EventTriggers)
		{
			if (EventTrigger)
			{
				if (EventTrigger->GetType() & ReceivedPacketData.m_MsgType)
				{
					EventTrigger->InteractEventTrigger();
				}
			}
		}

		Player = static_pointer_cast<CPlayer>(m_GameObjects[OBJECT_TYPE_PLAYER][SocketInfo.m_ID]);
		Player->IsCollidedByEventTrigger(Player->GetPosition());

		m_Lights[1].m_Direction = ReceivedPacketData.m_TowerLightDirection;
	}
}

vector<vector<shared_ptr<CGameObject>>>& CGameScene::GetGameObjects()
{
	return m_GameObjects;
}

vector<vector<shared_ptr<CBilboardObject>>>& CGameScene::GetBilboardObjects()
{
	return m_BilboardObjects;
}

vector<shared_ptr<CEventTrigger>>& CGameScene::GetEventTriggers()
{
	return m_EventTriggers;
}

vector<LIGHT>& CGameScene::GetLights()
{
	return m_Lights;
}

shared_ptr<CNavMesh>& CGameScene::GetNavMesh()
{
	return m_NavMesh;
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
		else if (Token == TEXT("<Type>"))
		{
			MSG_TYPE TriggerType{ static_cast<MSG_TYPE>(File::ReadIntegerFromFile(InFile)) };

			switch (TriggerType)
			{
			case MSG_TYPE_TRIGGER_OPEN_PRISON_DOOR:
			case MSG_TYPE_TRIGGER_OPEN_GUARDPOST_DOOR:
				EventTrigger = make_shared<COpenDoorEventTrigger>(TriggerType);
				break;
			case MSG_TYPE_TRIGGER_OPEN_ELEC_PANEL:
				EventTrigger = make_shared<CPowerDownEventTrigger>(TriggerType);
				break;
			case MSG_TYPE_TRIGGER_SIREN:
				EventTrigger = make_shared<CSirenEventTrigger>(TriggerType);
				break;
			case MSG_TYPE_TRIGGER_OPEN_GATE:
				EventTrigger = make_shared<COpenGateEventTrigger>(TriggerType);
				break;
			}

			EventTrigger->LoadEventTriggerFromFile(InFile);
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

	//// 권총을 드롭하는 트리거를 추가한다.
	//// 권총은 열쇠를 갖지 않은 임의의 교도관(Index: 2 ~ 14) 중 5명이 보유하고 있다.
	//vector<UINT> Indices{};

	//Indices.resize(m_GameObjects[OBJECT_TYPE_NPC].size() - 2);
	//iota(Indices.begin(), Indices.end(), 2);
	//shuffle(Indices.begin(), Indices.end(), default_random_engine{ random_device{}() });

	//for (UINT i = 0 ; i < 5 ; ++i)
	//{
	//	if (m_GameObjects[OBJECT_TYPE_NPC][Indices[i]])
	//	{
	//		shared_ptr<CGuard> Guard{ static_pointer_cast<CGuard>(m_GameObjects[OBJECT_TYPE_NPC][Indices[i]]) };

	//		EventTrigger = make_shared<CGetPistolEventTrigger>();
	//		Guard->SetEventTrigger(EventTrigger);

	//		m_EventTriggers.push_back(EventTrigger);
	//	}
	//}

	//// 열쇠를 드롭하는 트리거를 추가한다.
	//// 열쇠는 외형이 다른 교도관(Index: 0, 1)이 보유하고 있다.
	//for (UINT i = 0; i < 2; ++i)
	//{
	//	if (m_GameObjects[OBJECT_TYPE_NPC][i])
	//	{
	//		shared_ptr<CGuard> Guard{ static_pointer_cast<CGuard>(m_GameObjects[OBJECT_TYPE_NPC][i]) };

	//		EventTrigger = make_shared<CGetKeyEventTrigger>();
	//		Guard->SetEventTrigger(EventTrigger);

	//		m_EventTriggers.push_back(EventTrigger);
	//	}
	//}

	// 모든 트리거 객체는 상호작용 UI 객체를 공유한다.
	for (const auto& EventTrigger : m_EventTriggers)
	{
		if (EventTrigger)
		{
			// [BILBOARD_OBJECT_TYPE_UI][9]: InteractionUI
			EventTrigger->SetInteractionUI(m_BilboardObjects[BILBOARD_OBJECT_TYPE_UI][9]);
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

	if (Player->GetHealth() > 0)
	{
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
	else
	{
		XMFLOAT3 Direction{ Vector3::Inverse(Player->GetCamera()->GetLook()) };

		Player->GetCamera()->Move(Vector3::ScalarProduct(2.5f * ElapsedTime, XMFLOAT3(Direction.x, 0.3f, Direction.z), false));
	}
}

void CGameScene::InteractSpotLight(float ElapsedTime)
{
	if (m_Lights[1].m_IsActive)
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
								if (GameObject)
								{
									shared_ptr<CGuard> Guard{ static_pointer_cast<CGuard>(GameObject) };

									if (Guard->GetHealth() > 0)
									{
										// 스팟조명과 충돌 할 경우 주변 범위에 있는 경찰들이 플레이어를 쫒기 시작한다.
										if (Math::Distance(LightedPosition, Guard->GetPosition()) <= 150.0f)
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
							}

							CSoundManager::GetInstance()->Stop(SOUND_TYPE_INGAME_BGM_1);
							CSoundManager::GetInstance()->Play(SOUND_TYPE_INGAME_BGM_2, 0.3f);

							return;
						}
					}
				}
			}
		}

		m_SpotLightAngle += ElapsedTime;
		m_Lights[1].m_Direction = Vector3::Normalize(XMFLOAT3(cosf(m_SpotLightAngle), -1.0f, sinf(m_SpotLightAngle)));
	}
}

bool CGameScene::IsInvincibleMode() const
{
	return m_InvincibleMode;
}
