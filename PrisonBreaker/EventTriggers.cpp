#include "stdafx.h"
#include "EventTriggers.h"
#include "GameScene.h"

void COpenDoorEventTrigger::ActivateInteractionUI()
{
	if (m_InteractionUI)
	{
		m_InteractionUI->SetActive(true);
		m_InteractionUI->SetCellIndex(0, 0);
	}
}

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
}

//=========================================================================================================================

void CPowerDownEventTrigger::ActivateInteractionUI()
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
		else
		{
			if (!m_IsOpened)
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

void CSirenEventTrigger::ActivateInteractionUI()
{
	if (m_InteractionUI)
	{
		m_InteractionUI->SetActive(true);
		m_InteractionUI->SetCellIndex(0, 3);
	}
}

void CSirenEventTrigger::GenerateEventTrigger(float ElapsedTime)
{

}

//=========================================================================================================================

void COpenGateEventTrigger::ActivateInteractionUI()
{
	if (m_InteractionUI)
	{
		m_InteractionUI->SetActive(true);
		m_InteractionUI->SetCellIndex(0, 6);
	}
}

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
				ActivateInteractionUI();
			}

			return true;
		}
	}

	if (m_InteractionUI)
	{
		m_InteractionUI->SetActive(false);
	}

	return false;
}

void  CGetPistolEventTrigger::ActivateInteractionUI()
{
	if (m_InteractionUI)
	{
		m_InteractionUI->SetActive(true);
		m_InteractionUI->SetCellIndex(0, 4);
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
				ActivateInteractionUI();

				return true;
			}
		}
	}

	if (m_InteractionUI)
	{
		m_InteractionUI->SetActive(false);
	}

	return false;
}

void CGetKeyEventTrigger::ActivateInteractionUI()
{
	if (m_InteractionUI)
	{
		m_InteractionUI->SetActive(true);
		m_InteractionUI->SetCellIndex(0, 5);
	}
}
