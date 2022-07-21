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

	const XMFLOAT3 WorldUp{ 0.0f, 1.0f, 0.0f };

	m_EventObjects[0]->Rotate(WorldUp, -m_DoorAngle);
	m_EventObjects[1]->Rotate(WorldUp, m_DoorAngle);
	m_DoorAngle = 0.0f;
}

bool COpenDoorEventTrigger::CanPassTriggerArea(const XMFLOAT3& Position, const XMFLOAT3& NewPosition)
{
	// 문이 모두 열리지 않은 상태에서는 문 너머로 갈 수 없다.
	if (m_DoorAngle < 70.0f)
	{
		// 문 너머로 넘어가는 것을 계산한다.
		if (Math::LineIntersection(m_TriggerArea[0], m_TriggerArea[3], Position, NewPosition))
		{
			return false;
		}
	}

	// 위 경우가 아니라면, 이동이 가능하다.
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

bool COpenDoorEventTrigger::InteractEventTrigger(UINT CallerIndex)
{
	if (!m_IsInteracted)
	{
		m_IsInteracted = true;

		shared_ptr<CGameScene> GameScene{ static_pointer_cast<CGameScene>(CSceneManager::GetInstance()->GetCurrentScene()) };
		vector<vector<shared_ptr<CGameObject>>>& GameObjects{ GameScene->GetGameObjects() };
		shared_ptr<CPlayer> Player{ static_pointer_cast<CPlayer>(GameObjects[OBJECT_TYPE_PLAYER][CallerIndex]) };
		shared_ptr<CPlayer> OtherPlayer{ static_pointer_cast<CPlayer>(GameObjects[OBJECT_TYPE_PLAYER][abs(int(CallerIndex - 1))]) };

		if (CFramework::GetInstance()->GetSocketInfo().m_ID == CallerIndex)
		{
			CSoundManager::GetInstance()->Play(SOUND_TYPE_OPEN_DOOR, 0.65f);
		}
		else
		{
			float Distance{ Math::Distance(Player->GetPosition(), OtherPlayer->GetPosition()) };

			CSoundManager::GetInstance()->Play(SOUND_TYPE_OPEN_DOOR, GameScene->MultiSound(Distance, 0.65f, 30.0f));
		}

		return true;
	}

	return false;
}

void COpenDoorEventTrigger::Update(float ElapsedTime)
{
	if (m_IsActive && m_IsInteracted)
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

	vector<LIGHT>& Lights{ static_pointer_cast<CGameScene>(CSceneManager::GetInstance()->GetScene(TEXT("GameScene")))->GetLights() };
	const XMFLOAT3 WorldUp{ 0.0f, 1.0f, 0.0f };

	Lights[1].m_IsActive = true;

	m_EventObjects[0]->Rotate(WorldUp, m_PanelAngle);
	m_IsOpened = false;
	m_PanelAngle = 0.0f;
}

void CPowerDownEventTrigger::ShowInteractionUI()
{
	if (m_InteractionUI)
	{
		// 감시탑 차단 상호작용 UI는 0번 플레이어에게만 보여진다.
		if (CFramework::GetInstance()->GetSocketInfo().m_ID == 0)
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
}

bool CPowerDownEventTrigger::InteractEventTrigger(UINT CallerIndex)
{
	if (!m_IsInteracted)
	{
		m_IsInteracted = true;

		if (m_IsOpened)
		{
			shared_ptr<CGameScene> GameScene{ static_pointer_cast<CGameScene>(CSceneManager::GetInstance()->GetCurrentScene()) };
			vector<LIGHT>& Lights{ GameScene->GetLights() };

			// 감시탑의 조명을 끈다.
			Lights[1].m_IsActive = false;

			// 0번 플레이어의 감시탑 차단 미션UI를 완료상태로 변경한다.
			if (CFramework::GetInstance()->GetSocketInfo().m_ID == 0)
			{
				vector<vector<shared_ptr<CBilboardObject>>>& BilboardObjects{ GameScene->GetBilboardObjects() };

				BilboardObjects[BILBOARD_OBJECT_TYPE_UI][0]->SetCellIndex(0, 1);

				CSoundManager::GetInstance()->Play(SOUND_TYPE_POWER_DOWN, 0.65f);
			}
			
		}
		else
		{
			if (CFramework::GetInstance()->GetSocketInfo().m_ID == 0)
			{
				CSoundManager::GetInstance()->Play(SOUND_TYPE_OPEN_EP, 0.65f);
			}
		}

		return true;
	}

	return false;
}

void CPowerDownEventTrigger::Update(float ElapsedTime)
{
	if (m_IsActive && m_IsInteracted)
	{
		if (m_PanelAngle < 120.0f)
		{
			const XMFLOAT3 WorldUp{ 0.0f, 1.0f, 0.0f };

			m_EventObjects[0]->Rotate(WorldUp, -70.0f * ElapsedTime);
			m_PanelAngle += 70.0f * ElapsedTime;
		}
		else
		{
			if (!m_IsOpened)
			{
				m_IsInteracted = false;
				m_IsOpened = true;
				m_PanelAngle = 120.0f;
			}
		}
	}
}

//=========================================================================================================================

CSirenEventTrigger::CSirenEventTrigger()
{
	m_IsActive = true;
	m_ActiveFOV = 40.0f;
}

void CSirenEventTrigger::ShowInteractionUI()
{
	if (m_InteractionUI)
	{
		// 사이렌 작동 상호작용 UI는 1번 플레이어에게만 보여진다.
		if (CFramework::GetInstance()->GetSocketInfo().m_ID == 1)
		{
			m_InteractionUI->SetActive(true);
			m_InteractionUI->SetCellIndex(0, 3);
		}
	}
}

bool CSirenEventTrigger::InteractEventTrigger(UINT CallerIndex)
{
	if (!m_IsInteracted)
	{
		m_IsInteracted = true;

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
						// 사이렌을 작동시킬 경우 주변 범위에 있는 경찰들이 플레이어를 쫒기 시작한다.
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

		// 1번 플레이어의 사이렌 작동 미션UI를 완료상태로 변경한다.
		if (CFramework::GetInstance()->GetSocketInfo().m_ID == 1)
		{
			vector<vector<shared_ptr<CBilboardObject>>>& BilboardObjects{ GameScene->GetBilboardObjects() };

			BilboardObjects[BILBOARD_OBJECT_TYPE_UI][0]->SetCellIndex(0, 3);
		}

		CSoundManager::GetInstance()->Play(SOUND_TYPE_SIREN, 0.25f);

		return true;
	}

	return false;
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

	const XMFLOAT3 WorldUp{ 0.0f, 1.0f, 0.0f };

	m_EventObjects[0]->Rotate(WorldUp, m_GateAngle);
	m_EventObjects[1]->Rotate(WorldUp, -m_GateAngle);
	m_UsedKeyIndices[0] = m_UsedKeyIndices[1] = false;
	m_GateAngle = 0.0f;
}

bool COpenGateEventTrigger::CanPassTriggerArea(const XMFLOAT3& Position, const XMFLOAT3& NewPosition)
{
	// 게이트가 모두 열리지 않은 상태에서는 게이트 너머로 갈 수 없다.
	if (m_GateAngle < 120.0f)
	{
		// 게이트 너머로 넘어가는 것을 계산한다.
		if (Math::LineIntersection(m_TriggerArea[0], m_TriggerArea[3], Position, NewPosition))
		{
			return false;
		}
	}

	// 위 경우가 아니라면, 이동이 가능하다.
	return true;
}

void COpenGateEventTrigger::ShowInteractionUI()
{
	if (m_InteractionUI)
	{
		UINT PlayerID{ CFramework::GetInstance()->GetSocketInfo().m_ID };
		vector<vector<shared_ptr<CGameObject>>>& GameObjects{ static_pointer_cast<CGameScene>(CSceneManager::GetInstance()->GetCurrentScene())->GetGameObjects() };
		shared_ptr<CPlayer> Player{ static_pointer_cast<CPlayer>(GameObjects[OBJECT_TYPE_PLAYER][PlayerID]) };

		if (Player->HasKey() && !m_UsedKeyIndices[PlayerID])
		{
			m_InteractionUI->SetActive(true);
			m_InteractionUI->SetCellIndex(0, 6);
		}
	}
}

bool COpenGateEventTrigger::InteractEventTrigger(UINT CallerIndex)
{
	if (!m_IsInteracted)
	{
		vector<vector<shared_ptr<CGameObject>>>& GameObjects{ static_pointer_cast<CGameScene>(CSceneManager::GetInstance()->GetCurrentScene())->GetGameObjects() };
		shared_ptr<CPlayer> Player{ static_pointer_cast<CPlayer>(GameObjects[OBJECT_TYPE_PLAYER][CallerIndex]) };

		if (Player->HasKey() && !m_UsedKeyIndices[CallerIndex])
		{
			m_UsedKeyIndices[CallerIndex] = true;

			if (m_UsedKeyIndices[0] && m_UsedKeyIndices[1])
			{
				m_IsInteracted = true;
				CSoundManager::GetInstance()->Play(SOUND_TYPE_OPEN_GATE, 0.35f);
			}

			return true;
		}
	}

	return false;
}

void COpenGateEventTrigger::Update(float ElapsedTime)
{
	if (m_IsActive && m_IsInteracted)
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

	m_IsActive = false;
}

void CGetPistolEventTrigger::ShowInteractionUI()
{
	if (m_InteractionUI)
	{
		m_InteractionUI->SetActive(true);
		m_InteractionUI->SetCellIndex(0, 4);
	}
}

bool CGetPistolEventTrigger::InteractEventTrigger(UINT CallerIndex)
{
	if (!m_IsInteracted)
	{
		m_IsInteracted = true;

		shared_ptr<CGameScene> GameScene{ static_pointer_cast<CGameScene>(CSceneManager::GetInstance()->GetCurrentScene()) };
		vector<vector<shared_ptr<CGameObject>>>& GameObjects{ GameScene->GetGameObjects() };

		// 권총을 획득한 경우, 권총으로 무기를 교체하고 UI 또한 주먹에서 권총으로 변경시킨다.
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

		return true;
	}

	return false;
}

//=========================================================================================================================

CGetKeyEventTrigger::CGetKeyEventTrigger()
{
	m_ActiveFOV = 360.0f;
}

void CGetKeyEventTrigger::Reset()
{
	CEventTrigger::Reset();

	m_IsActive = false;
}

void CGetKeyEventTrigger::ShowInteractionUI()
{
	if (m_InteractionUI)
	{
		m_InteractionUI->SetActive(true);
		m_InteractionUI->SetCellIndex(0, 5);
	}
}

bool CGetKeyEventTrigger::InteractEventTrigger(UINT CallerIndex)
{
	if (!m_IsInteracted)
	{
		m_IsInteracted = true;

		shared_ptr<CGameScene> GameScene{ static_pointer_cast<CGameScene>(CSceneManager::GetInstance()->GetScene("GameScene")) };
		vector<vector<shared_ptr<CGameObject>>>& GameObjects{ GameScene->GetGameObjects() };
		shared_ptr<CPlayer> Player{ static_pointer_cast<CPlayer>(GameObjects[OBJECT_TYPE_PLAYER][CallerIndex]) };

		Player->ManageKey(true);

		if (CFramework::GetInstance()->GetSocketInfo().m_ID == CallerIndex)
		{
			vector<vector<shared_ptr<CBilboardObject>>>& BilboardObjects{ static_pointer_cast<CGameScene>(CSceneManager::GetInstance()->GetScene("GameScene"))->GetBilboardObjects()};

			// 열쇠 획득 애니메이션을 출력하도록 CKeyUIActivationState 상태로 전이한다.
			static_pointer_cast<CKeyUI>(BilboardObjects[BILBOARD_OBJECT_TYPE_UI][5])->GetStateMachine()->SetCurrentState(CKeyUIActivationState::GetInstance());

			// 열쇠 획득 미션UI를 완료상태로 변경한다.
			BilboardObjects[BILBOARD_OBJECT_TYPE_UI][0]->SetCellIndex(1, 5);
		}

		return true;
	}

	return false;
}
