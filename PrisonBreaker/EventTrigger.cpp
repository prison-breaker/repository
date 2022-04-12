#include "stdafx.h"
#include "EventTrigger.h"
#include "BilboardObject.h"

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
			break;
		}
	}
#else
#endif
}

void CEventTrigger::SetActive(bool IsActive)
{
	m_IsActive = IsActive;
}

bool CEventTrigger::IsActive() const
{
	return m_IsActive;
}

bool CEventTrigger::IsInTriggerArea(const XMFLOAT3& Position, const XMFLOAT3& LookDirection)
{
	float XMin{ m_TriggerArea.x };
	float XMax{ m_TriggerArea.y };
	float ZMin{ m_TriggerArea.z };
	float ZMax{ m_TriggerArea.w };

	if ((Position.x > XMin) && (Position.x < XMax) && (Position.z > ZMin) && (Position.z < ZMax))
	{
		m_InteractionUI->SetActive(true);

		return true;
	}
	
	m_InteractionUI->SetActive(false);

	return false;
}

void CEventTrigger::InsertEventObject(const shared_ptr<CGameObject>& EventObject)
{
	if (EventObject)
	{
		m_EventObjects.push_back(EventObject);
	}
}

void CEventTrigger::SetInteractionUI(const shared_ptr<CBilboardObject>& InteractionUI)
{
	if (InteractionUI)
	{
		m_InteractionUI = InteractionUI;
	}
}

CBilboardObject* CEventTrigger::GetInteractionUI() const
{
	return m_InteractionUI.get();
}
