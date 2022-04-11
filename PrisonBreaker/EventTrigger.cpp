#include "stdafx.h"
#include "EventTrigger.h"

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
		else if (Token == TEXT("</EventTrigger>"))
		{
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

bool CEventTrigger::IsInTriggerArea(const XMFLOAT3& Position)
{
	float XMin{ m_TriggerArea.x };
	float XMax{ m_TriggerArea.y };
	float ZMin{ m_TriggerArea.z };
	float ZMax{ m_TriggerArea.w };

	if ((Position.x > XMin) && (Position.x < XMax) && (Position.z > ZMin) && (Position.z < ZMax))
	{
		SetActive(true);

		return true;
	}

	return false;
}

void CEventTrigger::InsertEventObject(const shared_ptr<CGameObject>& EventObject)
{
	m_EventObjects.push_back(EventObject);
}
