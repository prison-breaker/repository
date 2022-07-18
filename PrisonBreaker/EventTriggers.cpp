#include "stdafx.h"
#include "EventTriggers.h"
#include "Framework.h"
#include "GameScene.h"

COpenDoorEventTrigger::COpenDoorEventTrigger()
{
	m_IsActive = true;
	m_ActiveFOV = 70.0f;
}

void COpenDoorEventTrigger::Reset()
{
	CEventTrigger::Reset();

	SetActive(true);

	const XMFLOAT3 WorldUp{ 0.0f, 1.0f, 0.0f };

	m_EventObjects[0]->Rotate(WorldUp, -m_DoorAngle);
	m_EventObjects[1]->Rotate(WorldUp, m_DoorAngle);
	m_DoorAngle = 0.0f;
}

bool COpenDoorEventTrigger::CanPassTriggerArea(const XMFLOAT3& Position, const XMFLOAT3& NewPosition)
{
	// ���� ��� ������ ���� ���¿����� �� �ʸӷ� �� �� ����.
	if (m_DoorAngle < 70.0f)
	{
		// �� �ʸӷ� �Ѿ�� ���� ����Ѵ�.
		if (Math::LineIntersection(m_TriggerArea[0], m_TriggerArea[3], Position, NewPosition))
		{
			return false;
		}
	}

	// �� ��찡 �ƴ϶��, �̵��� �����ϴ�.
	return true;
}

void COpenDoorEventTrigger::ShowInteractionUI()
{
	if (m_InteractionUI)
	{
		m_InteractionUI->SetActive(true);
		m_InteractionUI->SetCellIndex(0, 0);
	}
}

void COpenDoorEventTrigger::InteractEventTrigger()
{
	if (!IsInteracted())
	{
		CEventTrigger::InteractEventTrigger();
		CSoundManager::GetInstance()->Play(SOUND_TYPE_OPEN_DOOR, 0.65f);
	}
}

void COpenDoorEventTrigger::Update(float ElapsedTime)
{
	if (IsActive() && IsInteracted())
	{
		if (m_DoorAngle < 70.0f)
		{
			const XMFLOAT3 WorldUp{ 0.0f, 1.0f, 0.0f };

			m_EventObjects[0]->Rotate(WorldUp, 50.0f * ElapsedTime);
			m_EventObjects[1]->Rotate(WorldUp, -50.0f * ElapsedTime);
			m_DoorAngle += 50.0f * ElapsedTime;
		}
	}
}

//=========================================================================================================================

CPowerDownEventTrigger::CPowerDownEventTrigger()
{
	m_IsActive = true;
	m_ActiveFOV = 40.0f;
}

void CPowerDownEventTrigger::Reset()
{
	CEventTrigger::Reset();

	SetActive(true);

	const XMFLOAT3 WorldUp{ 0.0f, 1.0f, 0.0f };

	shared_ptr<CGameScene> GameScene{ static_pointer_cast<CGameScene>(CSceneManager::GetInstance()->GetScene(TEXT("GameScene"))) };
	vector<LIGHT>& Lights{ GameScene->GetLights() };
	vector<vector<shared_ptr<CBilboardObject>>>& BilboardObjects{ GameScene->GetBilboardObjects() };

	// ����ž�� ������ �Ҵ�.
	Lights[1].m_IsActive = true;

	// ����ž ���� �̼�UI�� �̿Ϸ���·� �����Ѵ�.
	BilboardObjects[BILBOARD_OBJECT_TYPE_UI][0]->SetCellIndex(0, 0);

	m_IsOpened = false;
	m_EventObjects[0]->Rotate(WorldUp, m_PanelAngle);
	m_PanelAngle = 0.0f;
}

void CPowerDownEventTrigger::ShowInteractionUI()
{
	if (m_InteractionUI)
	{
		m_InteractionUI->SetActive(true);

		if (m_PanelAngle <= 0.0f)
		{
			m_InteractionUI->SetCellIndex(0, 1);
		}
		else if (m_PanelAngle <= 120.0f)
		{
			m_InteractionUI->SetCellIndex(0, 2);
		}
	}
}

void CPowerDownEventTrigger::InteractEventTrigger()
{
	if (!IsInteracted())
	{
		CEventTrigger::InteractEventTrigger();

		// �������� �����ִٸ�
		if (IsOpened())
		{
			shared_ptr<CGameScene> GameScene{ static_pointer_cast<CGameScene>(CSceneManager::GetInstance()->GetCurrentScene()) };
			vector<LIGHT>& Lights{ GameScene->GetLights() };
			vector<vector<shared_ptr<CBilboardObject>>>& BilboardObjects{ GameScene->GetBilboardObjects() };

			// ����ž�� ������ ����.
			Lights[1].m_IsActive = false;

			// ����ž ���� �̼�UI�� �Ϸ���·� �����Ѵ�.
			BilboardObjects[BILBOARD_OBJECT_TYPE_UI][0]->SetCellIndex(0, 1);

			CSoundManager::GetInstance()->Play(SOUND_TYPE_POWER_DOWN, 0.65f);
		}
		else
		{
			CSoundManager::GetInstance()->Play(SOUND_TYPE_OPEN_EP, 0.65f);
		}
	}
}

void CPowerDownEventTrigger::Update(float ElapsedTime)
{
	if (IsActive() && IsInteracted())
	{
		if (m_PanelAngle < 120.0f)
		{
			const XMFLOAT3 WorldUp{ 0.0f, 1.0f, 0.0f };

			m_EventObjects[0]->Rotate(WorldUp, -70.0f * ElapsedTime);
			m_PanelAngle += 70.0f * ElapsedTime;
		}
		else
		{
			if (!IsOpened())
			{
				m_IsOpened = true;
				m_PanelAngle = 120.0f;

				SetInteracted(false);
			}
		}
	}
}

bool CPowerDownEventTrigger::IsOpened() const
{
	return m_IsOpened;
}

//=========================================================================================================================

CSirenEventTrigger::CSirenEventTrigger()
{
	m_IsActive = true;
	m_ActiveFOV = 40.0f;
}

void CSirenEventTrigger::Reset()
{
	CEventTrigger::Reset();

	SetActive(true);

	CSoundManager::GetInstance()->Stop(SOUND_TYPE_SIREN);
}

void CSirenEventTrigger::ShowInteractionUI()
{
	if (m_InteractionUI)
	{
		m_InteractionUI->SetActive(true);
		m_InteractionUI->SetCellIndex(0, 3);
	}
}

void CSirenEventTrigger::InteractEventTrigger()
{
	if (!IsInteracted())
	{
		CEventTrigger::InteractEventTrigger();

		shared_ptr<CGameScene> GameScene{ static_pointer_cast<CGameScene>(CSceneManager::GetInstance()->GetCurrentScene()) };
		vector<vector<shared_ptr<CGameObject>>>& GameObjects{ GameScene->GetGameObjects() };
		shared_ptr<CNavMesh> NavMesh{ GameScene->GetNavMesh() };

		UINT GuardCount{ static_cast<UINT>(GameObjects[OBJECT_TYPE_NPC].size()) };
		XMFLOAT3 CenterPosition{ (m_TriggerArea[0].x + m_TriggerArea[3].x) / 2.0f, m_TriggerArea[0].y, (m_TriggerArea[0].z + m_TriggerArea[1].z) / 2.0f };

		for (UINT i = 3; i < GuardCount; ++i)
		{
			if (i == 3 || i == 5 || i == 6 || i == 8 || i == 9)
			{
				if (GameObjects[OBJECT_TYPE_NPC][i])
				{
					shared_ptr<CGuard> Guard{ static_pointer_cast<CGuard>(GameObjects[OBJECT_TYPE_NPC][i]) };

					if (Guard->GetHealth() > 0)
					{
						// ���̷��� �۵���ų ��� �ֺ� ������ �ִ� �������� �÷��̾ �i�� �����Ѵ�.
						if (Guard->GetStateMachine()->IsInState(CGuardIdleState::GetInstance()) ||
							Guard->GetStateMachine()->IsInState(CGuardPatrolState::GetInstance()) ||
							Guard->GetStateMachine()->IsInState(CGuardReturnState::GetInstance()))
						{
							Guard->FindNavPath(NavMesh, CenterPosition, GameObjects);
							Guard->GetStateMachine()->ChangeState(CGuardAssembleState::GetInstance());
						}
					}
				}
			}
		}

		CSoundManager::GetInstance()->Play(SOUND_TYPE_SIREN, 0.25f);
	}
}

//=========================================================================================================================

COpenGateEventTrigger::COpenGateEventTrigger()
{
	m_IsActive = true;
	m_ActiveFOV = 70.0f;
}

void COpenGateEventTrigger::Reset()
{
	CEventTrigger::Reset();

	SetActive(true);

	const XMFLOAT3 WorldUp{ 0.0f, 1.0f, 0.0f };

	m_EventObjects[0]->Rotate(WorldUp, m_GateAngle);
	m_EventObjects[1]->Rotate(WorldUp, -m_GateAngle);
	m_GateAngle = 0.0f;
}

bool COpenGateEventTrigger::CanPassTriggerArea(const XMFLOAT3& Position, const XMFLOAT3& NewPosition)
{
	// ����Ʈ�� ��� ������ ���� ���¿����� ����Ʈ �ʸӷ� �� �� ����.
	if (m_GateAngle < 120.0f)
	{
		// ����Ʈ �ʸӷ� �Ѿ�� ���� ����Ѵ�.
		if (Math::LineIntersection(m_TriggerArea[0], m_TriggerArea[3], Position, NewPosition))
		{
			return false;
		}
	}

	// �� ��찡 �ƴ϶��, �̵��� �����ϴ�.
	return true;
}

void COpenGateEventTrigger::ShowInteractionUI()
{
	// ���踦 ȹ���� ��쿡��, Ʈ���Ÿ� Ȱ��ȭ ��Ų��.
	vector<vector<shared_ptr<CBilboardObject>>>& BilboardObjects{ static_pointer_cast<CGameScene>(CSceneManager::GetInstance()->GetCurrentScene())->GetBilboardObjects() };

	if (static_pointer_cast<CKeyUI>(BilboardObjects[BILBOARD_OBJECT_TYPE_UI][5])->GetStateMachine()->IsInState(CKeyUIActivationState::GetInstance()))
	{
		if (m_InteractionUI)
		{
			m_InteractionUI->SetActive(true);
			m_InteractionUI->SetCellIndex(0, 6);
		}
	}
}

void COpenGateEventTrigger::InteractEventTrigger()
{
	if (!IsInteracted())
	{
		// ���踦 ȹ���� ��쿡��, Ʈ���Ÿ� Ȱ��ȭ ��Ű���� �Ѵ�.
		//vector<vector<shared_ptr<CBilboardObject>>>& BilboardObjects{ static_pointer_cast<CGameScene>(CSceneManager::GetInstance()->GetCurrentScene())->GetBilboardObjects() };

		//if (static_pointer_cast<CKeyUI>(BilboardObjects[BILBOARD_OBJECT_TYPE_UI][5])->GetStateMachine()->IsInState(CKeyUIActivationState::GetInstance()))
		//{
		//	CEventTrigger::InteractEventTrigger();
		//	CSoundManager::GetInstance()->Play(SOUND_TYPE_OPEN_GATE, 0.35f);
		//}
	}
}

void COpenGateEventTrigger::Update(float ElapsedTime)
{
	if (IsActive() && IsInteracted())
	{
		if (m_GateAngle < 120.0f)
		{
			const XMFLOAT3 WorldUp{ 0.0f, 1.0f, 0.0f };

			m_EventObjects[0]->Rotate(WorldUp, -55.0f * ElapsedTime);
			m_EventObjects[1]->Rotate(WorldUp, 55.0f * ElapsedTime);
			m_GateAngle += 55.0f * ElapsedTime;
		}
	}
}

//=========================================================================================================================

CGetPistolEventTrigger::CGetPistolEventTrigger()
{
	m_ActiveFOV = 360.0f;
}

void CGetPistolEventTrigger::Reset()
{
	CEventTrigger::Reset();

	SetActive(false);
}

void CGetPistolEventTrigger::ShowInteractionUI()
{
	if (m_InteractionUI)
	{
		m_InteractionUI->SetActive(true);
		m_InteractionUI->SetCellIndex(0, 4);
	}
}

void CGetPistolEventTrigger::InteractEventTrigger(UINT CallerIndex)
{
	if (!IsInteracted())
	{
		CEventTrigger::InteractEventTrigger();

		shared_ptr<CGameScene> GameScene{ static_pointer_cast<CGameScene>(CSceneManager::GetInstance()->GetCurrentScene()) };
		vector<vector<shared_ptr<CGameObject>>>& GameObjects{ GameScene->GetGameObjects() };

		// ������ ȹ���� ���, �������� ���⸦ ��ü�ϰ� UI ���� �ָԿ��� �������� �����Ų��.
		shared_ptr<CPlayer> Player{ static_pointer_cast<CPlayer>(GameObjects[OBJECT_TYPE_PLAYER][CallerIndex]) };

		if (!Player->HasPistol())
		{
			Player->ManagePistol(true);
		}

		Player->SwapWeapon(WEAPON_TYPE_PISTOL);

		if (CFramework::GetInstance()->GetSocketInfo().m_ID == CallerIndex)
		{
			vector<vector<shared_ptr<CBilboardObject>>>& BilboardObjects{ GameScene->GetBilboardObjects() };

			BilboardObjects[BILBOARD_OBJECT_TYPE_UI][3]->SetActive(false); // 4: Punch UI
			BilboardObjects[BILBOARD_OBJECT_TYPE_UI][4]->SetActive(true);  // 6: Pistol UI
			BilboardObjects[BILBOARD_OBJECT_TYPE_UI][4]->SetVertexCount(6);
		}
	}
}

//=========================================================================================================================

CGetKeyEventTrigger::CGetKeyEventTrigger()
{
	m_ActiveFOV = 360.0f;
}

void CGetKeyEventTrigger::Reset()
{
	CEventTrigger::Reset();

	SetActive(false);
}

void CGetKeyEventTrigger::ShowInteractionUI()
{
	if (m_InteractionUI)
	{
		m_InteractionUI->SetActive(true);
		m_InteractionUI->SetCellIndex(0, 5);
	}
}

void CGetKeyEventTrigger::InteractEventTrigger(UINT CallerIndex)
{
	if (!IsInteracted())
	{
		CEventTrigger::InteractEventTrigger();

		if (CFramework::GetInstance()->GetSocketInfo().m_ID == CallerIndex)
		{
			vector<vector<shared_ptr<CBilboardObject>>>& BilboardObjects{ static_pointer_cast<CGameScene>(CSceneManager::GetInstance()->GetCurrentScene())->GetBilboardObjects() };

			// ���� ȹ�� �ִϸ��̼��� ����ϵ��� CKeyUIActivationState ���·� �����Ѵ�.
			static_pointer_cast<CKeyUI>(BilboardObjects[BILBOARD_OBJECT_TYPE_UI][5])->GetStateMachine()->SetCurrentState(CKeyUIActivationState::GetInstance());

			// ���� ȹ�� �̼�UI�� �Ϸ���·� �����Ѵ�.
			BilboardObjects[BILBOARD_OBJECT_TYPE_UI][0]->SetCellIndex(1, 5);
		}
	}
}
