#include "stdafx.h"
#include "EventTrigger.h"
#include "BilboardObject.h"

bool CEventTrigger::IsInTriggerArea(const XMFLOAT3& Position, const XMFLOAT3& LookDirection)
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

	if (m_InteractionUI)
	{
		m_InteractionUI->SetActive(false);
	}

	return false;
}

void CEventTrigger::ActivateInteractionUI()
{
	if (m_InteractionUI)
	{
		m_InteractionUI->SetActive(true);
	}
}

void CEventTrigger::GenerateEventTrigger(float ElapsedTime)
{

}

void CEventTrigger::LoadEventTriggerFromFile(tifstream& InFile)
{
	tstring Token{};

#ifdef READ_BINARY_FILE
	while (true)
	{
		File::ReadStringFromFile(InFile, Token);

		if (Token == TEXT("<TriggerArea>"))
		{
			InFile.read(reinterpret_cast<TCHAR*>(&m_TriggerArea), sizeof(XMFLOAT4));
		}
		else if (Token == TEXT("<ToTrigger>"))
		{
			InFile.read(reinterpret_cast<TCHAR*>(&m_ToTrigger), sizeof(XMFLOAT3));
			break;
		}
	}
#else
#endif
}

void CEventTrigger::SetInteracted(bool IsInteracted)
{
	m_IsInteracted = IsInteracted;
}

bool CEventTrigger::IsInteracted() const
{
	return m_IsInteracted;
}

void CEventTrigger::CalculateTriggerAreaByPoint(const XMFLOAT3& Position, float XWidth, float ZWidth)
{
	m_TriggerArea.x = Position.x - 0.5f * XWidth;
	m_TriggerArea.y = Position.x + 0.5f * XWidth;
	m_TriggerArea.z = Position.z - 0.5f * ZWidth;
	m_TriggerArea.w = Position.z + 0.5f * ZWidth;
}

void CEventTrigger::InsertEventObject(const shared_ptr<CGameObject>& EventObject)
{
	if (EventObject)
	{
		m_EventObjects.push_back(EventObject);
	}
}

shared_ptr<CGameObject> CEventTrigger::GetEventObject(UINT Index)
{
	if (Index < 0 || Index >= m_EventObjects.size())
	{
		return nullptr;
	}

	return m_EventObjects[Index];
}

void CEventTrigger::SetInteractionUI(const shared_ptr<CBilboardObject>& InteractionUI)
{
	if (InteractionUI)
	{
		m_InteractionUI = InteractionUI;
	}
}

shared_ptr<CBilboardObject> CEventTrigger::GetInteractionUI() const
{
	return m_InteractionUI;
}
