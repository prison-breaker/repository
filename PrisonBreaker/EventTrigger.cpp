#include "stdafx.h"
#include "EventTrigger.h"
#include "BilboardObject.h"
#include "GameObject.h"

bool CEventTrigger::CanPassTriggerArea(const XMFLOAT3& Position, const XMFLOAT3& NewPosition)
{
	return true;
}

void CEventTrigger::ShowInteractionUI()
{
	if (m_InteractionUI)
	{
		m_InteractionUI->SetActive(true);
	}
}

void CEventTrigger::InteractEventTrigger()
{
	SetInteracted(true);
}

void CEventTrigger::Update(float ElapsedTime)
{

}

void CEventTrigger::LoadEventTriggerFromFile(tifstream& InFile)
{
	tstring Token{};

#ifdef READ_BINARY_FILE
	while (true)
	{
		File::ReadStringFromFile(InFile, Token);

		if (Token == TEXT("<TriggerAreas>"))
		{
			InFile.read(reinterpret_cast<TCHAR*>(m_TriggerArea), 4 * sizeof(XMFLOAT3));
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

void CEventTrigger::SetActive(bool IsActive)
{
	m_IsActive = IsActive;
}

bool CEventTrigger::IsActive() const
{
	return m_IsActive;
}

void CEventTrigger::SetInteracted(bool IsInteracted)
{
	m_IsInteracted = IsInteracted;
}

bool CEventTrigger::IsInteracted() const
{
	return m_IsInteracted;
}

void CEventTrigger::CalculateTriggerAreaByGuard(const XMFLOAT3& Position)
{
	m_TriggerArea[0].x = Position.x - 3.0f;
	m_TriggerArea[0].z = Position.z - 3.0f;

	m_TriggerArea[1].x = Position.x - 3.0f;
	m_TriggerArea[1].z = Position.z + 3.0f;

	m_TriggerArea[2].x = Position.x + 3.0f;
	m_TriggerArea[2].z = Position.z + 3.0f;

	m_TriggerArea[3].x = Position.x + 3.0f;
	m_TriggerArea[3].z = Position.z - 3.0f;
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

bool CEventTrigger::IsInTriggerArea(const XMFLOAT3& Position, const XMFLOAT3& LookDirection)
{
	if (IsActive() && !IsInteracted())
	{
		for (UINT i = 0; i < 2; ++i)
		{
			if (Math::IsInTriangle(m_TriggerArea[0], m_TriggerArea[i + 1], m_TriggerArea[i + 2], Position))
			{
				if (Vector3::Angle(LookDirection, m_ToTrigger) <= m_ActiveFOV)
				{
					// 각도가 일정 범위안에 있다면 상호작용 UI를 렌더링하도록 만든다.
					ShowInteractionUI();
				}
				else
				{
					HideInteractionUI();
				}

				return true;
			}
		}
	}

	return false;
}

void CEventTrigger::HideInteractionUI()
{
	if (m_InteractionUI)
	{
		m_InteractionUI->SetActive(false);
	}
}
