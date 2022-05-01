#include "stdafx.h"
#include "EventTriggers.h"
#include "GameScene.h"

COpenDoorEventTrigger::COpenDoorEventTrigger()
{
	m_IsActive = true;
	m_ActiveFOV = 70.0f;
}

bool COpenDoorEventTrigger::CanPassTriggerArea(const XMFLOAT3& NewPosition)
{
	// 문이 모두 열리지 않은 상태에서는 문 너머로 갈 수 없다.
	if (m_DoorAngle < 70.0f)
	{
		// 문 너머로 넘어가는 것을 계산하는 판별식(D > 0)
		if (Math::Discriminant(m_TriggerAreas[0], m_TriggerAreas[3], NewPosition) > 0)
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

void COpenDoorEventTrigger::GenerateEventTrigger(float ElapsedTime)
{
	if (IsActive() && IsInteracted())
	{
		if (m_DoorAngle < 70.0f)
		{
			const XMFLOAT3 WorldUp{ 0.0f, 1.0f, 0.0f };

			m_EventObjects[0]->Rotate(WorldUp, 45.0f * ElapsedTime);
			m_EventObjects[1]->Rotate(WorldUp, -45.0f * ElapsedTime);
			m_DoorAngle += 45.0f * ElapsedTime;
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

void CSirenEventTrigger::GenerateEventTrigger(float ElapsedTime)
{

}

//=========================================================================================================================

COpenGateEventTrigger::COpenGateEventTrigger()
{
	m_IsActive = true;
	m_ActiveFOV = 70.0f;
}

bool COpenGateEventTrigger::CanPassTriggerArea(const XMFLOAT3& NewPosition)
{
	// 게이트가 모두 열리지 않은 상태에서는 게이트 너머로 갈 수 없다.
	if (m_GateAngle < 120.0f)
	{
		// 게이트 너머로 넘어가는 것을 계산하는 판별식(D > 0)
		if (Math::Discriminant(m_TriggerAreas[0], m_TriggerAreas[3], NewPosition) > 0)
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

void  CGetPistolEventTrigger::ShowInteractionUI()
{
	if (m_InteractionUI)
	{
		m_InteractionUI->SetActive(true);
		m_InteractionUI->SetCellIndex(0, 4);
	}
}

//=========================================================================================================================

void CGetKeyEventTrigger::ShowInteractionUI()
{
	if (m_InteractionUI)
	{
		m_InteractionUI->SetActive(true);
		m_InteractionUI->SetCellIndex(0, 5);
	}
}
