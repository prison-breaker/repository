#include "stdafx.h"
#include "EventTriggers.h"
#include "GameObject.h"
#include "BilboardObject.h"

void COpenDoorEventTrigger::GenerateEventTrigger(float ElapsedTime)
{
	if (m_InteractionUI)
	{
		m_InteractionUI->SetCellIndex(0, 0);
	}

	if (IsActive())
	{
		if (m_DoorAngle < 70.0f)
		{
			m_EventObjects[0]->Rotate(XMFLOAT3(0.0f, 1.0f, 0.0f), 45.0f * ElapsedTime);
			m_EventObjects[1]->Rotate(XMFLOAT3(0.0f, 1.0f, 0.0f), -45.0f * ElapsedTime);

			m_DoorAngle += 45.0f * ElapsedTime;
		}
		else
		{
			SetActive(false);

			if (m_InteractionUI)
			{
				m_InteractionUI->SetActive(false);
			}
		}
	}
}

//=========================================================================================================================

void CPowerDownEventTrigger::GenerateEventTrigger(float ElapsedTime)
{
	if (m_PanelAngle <= 0.0f)
	{
		if (m_InteractionUI)
		{
			m_InteractionUI->SetCellIndex(0, 1);
		}
	}
	else if (m_PanelAngle < 120.0f)
	{
		if (m_InteractionUI)
		{
			m_InteractionUI->SetCellIndex(0, 2);
		}
	}

	if (IsActive())
	{
		if (m_PanelAngle < 120.0f)
		{
			m_EventObjects[0]->Rotate(XMFLOAT3(0.0f, 1.0f, 0.0f), -15.0f * ElapsedTime);

			m_PanelAngle += 15.0f * ElapsedTime;
		}
		else
		{
			SetActive(false);

			if (m_InteractionUI)
			{
				m_InteractionUI->SetActive(false);
			}
		}
	}
}

//=========================================================================================================================

void CSirenEventTrigger::GenerateEventTrigger(float ElapsedTime)
{
	if (m_InteractionUI)
	{
		m_InteractionUI->SetCellIndex(0, 3);
	}
}

//=========================================================================================================================

void COpenGateEventTrigger::GenerateEventTrigger(float ElapsedTime)
{
	if (m_InteractionUI)
	{
		m_InteractionUI->SetCellIndex(0, 4);
	}

	if (IsActive())
	{
		if (m_DoorAngle < 120.0f)
		{
			m_EventObjects[0]->Rotate(XMFLOAT3(0.0f, 1.0f, 0.0f), -20.0f * ElapsedTime);
			m_EventObjects[1]->Rotate(XMFLOAT3(0.0f, 1.0f, 0.0f), 20.0f * ElapsedTime);

			m_DoorAngle += 20.0f * ElapsedTime;
		}
		else
		{
			SetActive(false);

			if (m_InteractionUI)
			{
				m_InteractionUI->SetActive(false);
			}
		}
	}
}
