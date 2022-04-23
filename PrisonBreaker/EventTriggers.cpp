#include "stdafx.h"
#include "EventTriggers.h"
#include "Player.h"
#include "Guard.h"
#include "BilboardObject.h"

void COpenDoorEventTrigger::GenerateEventTrigger(float ElapsedTime)
{
	if (IsInteracted())
	{
		if (m_DoorAngle < 70.0f)
		{
			const XMFLOAT3 WorldUp{ 0.0f, 1.0f, 0.0f };

			m_EventObjects[0]->Rotate(WorldUp, 45.0f * ElapsedTime);
			m_EventObjects[1]->Rotate(WorldUp, -45.0f * ElapsedTime);
			m_DoorAngle += 45.0f * ElapsedTime;
		}
	}
	else
	{
		if (m_InteractionUI)
		{
			m_InteractionUI->SetCellIndex(0, 0);
		}
	}
}

//=========================================================================================================================

void CPowerDownEventTrigger::GenerateEventTrigger(float ElapsedTime)
{
	if (IsInteracted())
	{
		if (m_PanelAngle < 120.0f)
		{
			const XMFLOAT3 WorldUp{ 0.0f, 1.0f, 0.0f };

			m_EventObjects[0]->Rotate(WorldUp, -20.0f * ElapsedTime);
			m_PanelAngle += 20.0f * ElapsedTime;
		}
	}
	else
	{
		if (m_InteractionUI)
		{
			if (m_PanelAngle <= 0.0f)
			{
				m_InteractionUI->SetCellIndex(0, 1);
			}
			else if (m_PanelAngle < 120.0f)
			{
				m_InteractionUI->SetCellIndex(0, 2);
			}
		}
	}
}

//=========================================================================================================================

void CSirenEventTrigger::GenerateEventTrigger(float ElapsedTime)
{
	if (IsInteracted())
	{

	}
	else
	{
		if (m_InteractionUI)
		{
			m_InteractionUI->SetCellIndex(0, 3);
		}
	}
}

//=========================================================================================================================

void COpenGateEventTrigger::GenerateEventTrigger(float ElapsedTime)
{
	if (IsInteracted())
	{
		if (m_DoorAngle < 120.0f)
		{
			const XMFLOAT3 WorldUp{ 0.0f, 1.0f, 0.0f };

			m_EventObjects[0]->Rotate(WorldUp, -20.0f * ElapsedTime);
			m_EventObjects[1]->Rotate(WorldUp, 20.0f * ElapsedTime);
			m_DoorAngle += 20.0f * ElapsedTime;
		}
	}
	else
	{
		if (m_InteractionUI)
		{
			m_InteractionUI->SetCellIndex(0, 6);
		}
	}
}

//=========================================================================================================================

bool CGetPistolEventTrigger::IsInTriggerArea(const XMFLOAT3& Position, const XMFLOAT3& LookDirection)
{
	// 0: Guard
	shared_ptr<CGuard> Guard{ static_pointer_cast<CGuard>(m_EventObjects[0]) };

	if (Guard->GetHealth() == 0 && Guard->GetElapsedTime() < 10.0f)
	{
		float XMin{ m_TriggerArea.x };
		float XMax{ m_TriggerArea.y };
		float ZMin{ m_TriggerArea.z };
		float ZMax{ m_TriggerArea.w };

		if ((Position.x > XMin) && (Position.x < XMax) && (Position.z > ZMin) && (Position.z < ZMax))
		{
			if (!IsInteracted())
			{
				if (m_InteractionUI)
				{
					m_InteractionUI->SetActive(true);
				}
			}

			return true;
		}
		else
		{
			// 나중에 이 트리거를 this 처리하여 매 프레임마다 트리거 영역을 구하는 연산을 수행하는 것을 해결할 예정
			CalculateTriggerAreaByPoint(Guard->GetPosition(), 5.0f, 5.0f);
		}
	}

	if (m_InteractionUI)
	{
		m_InteractionUI->SetActive(false);
	}

	return false;
}

void CGetPistolEventTrigger::GenerateEventTrigger(float ElapsedTime)
{
	if (IsInteracted())
	{
		shared_ptr<CPlayer> Player{ static_pointer_cast<CPlayer>(m_EventObjects[1]) };

		if (!Player->HasPistol())
		{
			Player->AcquirePistol();
			Player->SwapWeapon(WEAPON_TYPE_PISTOL);
		}
	}
	else
	{
		if (m_InteractionUI)
		{
			m_InteractionUI->SetCellIndex(0, 4);
		}
	}
}

//=========================================================================================================================

bool CGetKeyEventTrigger::IsInTriggerArea(const XMFLOAT3& Position, const XMFLOAT3& LookDirection)
{
	// 0: Guard
	shared_ptr<CGuard> Guard{ static_pointer_cast<CGuard>(m_EventObjects[0]) };

	if (Guard->GetHealth() == 0)
	{
		float XMin{ m_TriggerArea.x };
		float XMax{ m_TriggerArea.y };
		float ZMin{ m_TriggerArea.z };
		float ZMax{ m_TriggerArea.w };

		if ((Position.x > XMin) && (Position.x < XMax) && (Position.z > ZMin) && (Position.z < ZMax))
		{
			if (!IsInteracted())
			{
				if (m_InteractionUI)
				{
					m_InteractionUI->SetActive(true);
				}

				return true;
			}
		}
		else
		{
			// 나중에 이 트리거를 this 처리하여 매 프레임마다 트리거 영역을 구하는 연산을 수행하는 것을 해결할 예정
			CalculateTriggerAreaByPoint(Guard->GetPosition(), 5.0f, 5.0f);
		}
	}

	if (m_InteractionUI)
	{
		m_InteractionUI->SetActive(false);
	}

	return false;
}

void CGetKeyEventTrigger::GenerateEventTrigger(float ElapsedTime)
{
	if (IsInteracted())
	{

	}
	else
	{
		if (m_InteractionUI)
		{
			m_InteractionUI->SetCellIndex(0, 5);
		}
	}
}
