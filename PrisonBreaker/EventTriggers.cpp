#include "stdafx.h"
#include "EventTriggers.h"
#include "GameObject.h"
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
			m_InteractionUI->SetCellIndex(0, 4);
		}
	}
}
