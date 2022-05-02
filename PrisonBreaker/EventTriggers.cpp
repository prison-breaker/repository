#include "stdafx.h"
#include "EventTriggers.h"
#include "GameScene.h"

COpenDoorEventTrigger::COpenDoorEventTrigger()
{
	m_IsActive = true;
	m_ActiveFOV = 70.0f;
}

bool COpenDoorEventTrigger::CanPassTriggerArea(const XMFLOAT3& Position, const XMFLOAT3& NewPosition)
{
	// 문이 모두 열리지 않은 상태에서는 문 너머로 갈 수 없다.
	if (m_DoorAngle < 70.0f)
	{
		// 문 너머로 넘어가는 것을 계산한다.
		if (Math::LineIntersection(m_TriggerAreas[0], m_TriggerAreas[3], Position, NewPosition))
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

void COpenDoorEventTrigger::InteractEventTrigger()
{
	CEventTrigger::InteractEventTrigger();
	CSoundManager::GetInstance()->Play(SOUND_TYPE_OPEN_DOOR, 0.65f);
}

void COpenDoorEventTrigger::GenerateEventTrigger(float ElapsedTime)
{
	if (IsActive() && IsInteracted())
	{
		if (m_DoorAngle < 70.0f)
		{
			const XMFLOAT3 WorldUp{ 0.0f, 1.0f, 0.0f };

			m_EventObjects[0]->Rotate(WorldUp, 40.0f * ElapsedTime);
			m_EventObjects[1]->Rotate(WorldUp, -40.0f * ElapsedTime);
			m_DoorAngle += 40.0f * ElapsedTime;
		}
		else
		{
			m_DoorAngle = 70.0f;
		}
	}
}

//=========================================================================================================================

CPowerDownEventTrigger::CPowerDownEventTrigger()
{
	m_IsActive = true;
	m_ActiveFOV = 40.0f;
}

void CPowerDownEventTrigger::ShowInteractionUI()
{
	if (m_InteractionUI)
	{
		m_InteractionUI->SetActive(true);

		if (m_PanelAngle <= 0.0f)
		{
			m_InteractionUI->SetCellIndex(0, 1);
			m_PanelAngle;
		}
		else if (m_PanelAngle <= 120.0f)
		{
			m_InteractionUI->SetCellIndex(0, 2);
			m_PanelAngle;
		}
	}
}

void CPowerDownEventTrigger::InteractEventTrigger()
{
	CEventTrigger::InteractEventTrigger();

	// 배전함이 열려있다면
	if (IsOpened())
	{
		// 감시탑의 조명을 끈다.
		static_pointer_cast<CGameScene>(CSceneManager::GetInstance()->GetCurrentScene())->GetLights()[1].m_IsActive = false;

		auto BilboardObjects{ static_pointer_cast<CGameScene>(CSceneManager::GetInstance()->GetCurrentScene())->GetBilboardObjects() };

		// 감시탑 차단 미션UI를 완료상태로 변경한다.
		BilboardObjects[BILBOARD_OBJECT_TYPE_UI][0]->SetCellIndex(0, 1);
	}
	else
	{
		CSoundManager::GetInstance()->Play(SOUND_TYPE_OPEN_EP, 0.65f);
	}
}

void CPowerDownEventTrigger::GenerateEventTrigger(float ElapsedTime)
{
	if (IsActive() && IsInteracted())
	{
		if (m_PanelAngle < 120.0f)
		{
			const XMFLOAT3 WorldUp{ 0.0f, 1.0f, 0.0f };

			m_EventObjects[0]->Rotate(WorldUp, -20.0f * ElapsedTime);
			m_PanelAngle += 20.0f * ElapsedTime;
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
	CEventTrigger::InteractEventTrigger();

	auto GameScene{ static_pointer_cast<CGameScene>(CSceneManager::GetInstance()->GetCurrentScene()) };
	auto GameObjects{ GameScene->GetGameObjects() };
	auto NavMesh{ GameScene->GetNavMesh() };

	UINT GuardCount{ static_cast<UINT>(GameObjects[OBJECT_TYPE_NPC].size())};

	for (UINT i = 0; i < 5; ++i)
	{
		UINT Index{ rand() % GuardCount };
		shared_ptr<CGuard> Guard{ static_pointer_cast<CGuard>(GameObjects[OBJECT_TYPE_NPC][Index]) };

		if (Guard)
		{
			if (Guard->GetStateMachine()->IsInState(CGuardIdleState::GetInstance()) ||
				Guard->GetStateMachine()->IsInState(CGuardPatrolState::GetInstance()) ||
				Guard->GetStateMachine()->IsInState(CGuardReturnState::GetInstance()))
			{
				Guard->FindNavPath(NavMesh, GameObjects[OBJECT_TYPE_PLAYER].back()->GetPosition(), GameObjects);
				Guard->GetStateMachine()->ChangeState(CGuardAssembleState::GetInstance());
			}
		}
	}

	CSoundManager::GetInstance()->Play(SOUND_TYPE_SIREN, 0.5f);
}

void CSirenEventTrigger::GenerateEventTrigger(float ElapsedTime)
{

}

//=========================================================================================================================

COpenGateEventTrigger::COpenGateEventTrigger()
{
	m_IsActive = true;
	m_ActiveFOV = 70.0f;
}

bool COpenGateEventTrigger::CanPassTriggerArea(const XMFLOAT3& Position, const XMFLOAT3& NewPosition)
{
	// 게이트가 모두 열리지 않은 상태에서는 게이트 너머로 갈 수 없다.
	if (m_GateAngle < 120.0f)
	{
		// 게이트 너머로 넘어가는 것을 계산한다.
		if (Math::LineIntersection(m_TriggerAreas[0], m_TriggerAreas[3], Position, NewPosition))
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
		m_InteractionUI->SetActive(true);
		m_InteractionUI->SetCellIndex(0, 6);
	}
}

void COpenGateEventTrigger::InteractEventTrigger()
{
	CEventTrigger::InteractEventTrigger();
}

void COpenGateEventTrigger::GenerateEventTrigger(float ElapsedTime)
{
	if (IsActive() && IsInteracted())
	{
		if (m_GateAngle < 120.0f)
		{
			const XMFLOAT3 WorldUp{ 0.0f, 1.0f, 0.0f };

			m_EventObjects[0]->Rotate(WorldUp, -20.0f * ElapsedTime);
			m_EventObjects[1]->Rotate(WorldUp, 20.0f * ElapsedTime);
			m_GateAngle += 20.0f * ElapsedTime;
		}
	}
}

//=========================================================================================================================

CGetPistolEventTrigger::CGetPistolEventTrigger()
{
	m_ActiveFOV = 360.0f;
}

void CGetPistolEventTrigger::ShowInteractionUI()
{
	if (m_InteractionUI)
	{
		m_InteractionUI->SetActive(true);
		m_InteractionUI->SetCellIndex(0, 4);
	}
}

void CGetPistolEventTrigger::InteractEventTrigger()
{
	CEventTrigger::InteractEventTrigger();

	auto GameScene{ static_pointer_cast<CGameScene>(CSceneManager::GetInstance()->GetCurrentScene()) };
	auto GameObjects{ GameScene->GetGameObjects() };
	auto BilboardObjects{ GameScene->GetBilboardObjects() };

	// 권총을 획득한 경우, 권총으로 무기를 교체하고 UI 또한 주먹에서 권총으로 변경시킨다.
	shared_ptr<CPlayer> Player{ static_pointer_cast<CPlayer>(GameObjects[OBJECT_TYPE_PLAYER].back()) };

	if (!Player->HasPistol())
	{
		Player->AcquirePistol();
	}

	Player->SwapWeapon(WEAPON_TYPE_PISTOL);

	BilboardObjects[BILBOARD_OBJECT_TYPE_UI][4]->SetActive(false); // 4: Punch UI
	BilboardObjects[BILBOARD_OBJECT_TYPE_UI][6]->SetActive(true);  // 6: Pistol UI
	BilboardObjects[BILBOARD_OBJECT_TYPE_UI][7]->SetActive(true);  // 7: Bullet UI
	BilboardObjects[BILBOARD_OBJECT_TYPE_UI][7]->SetVertexCount(5);
}

//=========================================================================================================================

CGetKeyEventTrigger::CGetKeyEventTrigger()
{
	m_ActiveFOV = 360.0f;
}

void CGetKeyEventTrigger::ShowInteractionUI()
{
	if (m_InteractionUI)
	{
		m_InteractionUI->SetActive(true);
		m_InteractionUI->SetCellIndex(0, 5);
	}
}

void CGetKeyEventTrigger::InteractEventTrigger()
{
	CEventTrigger::InteractEventTrigger();

	auto BilboardObjects{ static_pointer_cast<CGameScene>(CSceneManager::GetInstance()->GetCurrentScene())->GetBilboardObjects() };

	// 열쇠 획득 애니메이션을 출력하도록 CKeyUIActivationState 상태로 전이한다.
	static_pointer_cast<CKeyUI>(BilboardObjects[BILBOARD_OBJECT_TYPE_UI][5])->GetStateMachine()->SetCurrentState(CKeyUIActivationState::GetInstance());

	// 열쇠 획득 미션UI를 완료상태로 변경한다.
	BilboardObjects[BILBOARD_OBJECT_TYPE_UI][0]->SetCellIndex(1, 5);
}
