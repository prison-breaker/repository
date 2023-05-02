#include "pch.h"
#include "EventTrigger.h"
#include "GameScene.h"
#include "GameObject.h"
#include "QuadObject.h"

void CEventTrigger::Reset()
{
	m_isActive = true;
	m_IsInteracted = false;
}

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

bool CEventTrigger::InteractEventTrigger(UINT CallerIndex)
{
	if (!m_IsInteracted)
	{
		m_IsInteracted = true;

		return true;
	}

	return false;
}

void CEventTrigger::Update(float ElapsedTime)
{

}

void CEventTrigger::LoadEventTriggerFromFile(ifstream& in)
{
	string str{};
	UINT TargetRootIndex{};

	while (true)
	{
		File::ReadStringFromFile(in, str);

		if (str == TEXT("<TriggerAreas>"))
		{
			in.read(reinterpret_cast<char*>(m_TriggerArea), 4 * sizeof(XMFLOAT3));
		}
		else if (str == TEXT("<ToTrigger>"))
		{
			in.read(reinterpret_cast<char*>(&m_ToTrigger), sizeof(XMFLOAT3));
		}
		else if (str == TEXT("<TargetRootIndex>"))
		{
			TargetRootIndex = File::ReadIntegerFromFile(in);
		}
		else if (str == TEXT("<TargetObject>"))
		{
			UINT TargetObjectCount{ File::ReadIntegerFromFile(in) };
			vector<vector<shared_ptr<CObject>>>& GameObjects{ static_pointer_cast<CGameScene>(CSceneManager::GetInstance()->GetScene(TEXT("GameScene")))->GetGameObjects()};

			m_EventObjects.reserve(TargetObjectCount);

			for (UINT i = 0; i < TargetObjectCount; ++i)
			{
				File::ReadStringFromFile(in, str);

				shared_ptr<CObject> TargetObject{ GameObjects[OBJECT_TYPE::STRUCTURE][TargetRootIndex]->FindFrame(str) };

				if (TargetObject)
				{
					m_EventObjects.push_back(TargetObject);
				}
			}
			break;
		}
	}
}

void CEventTrigger::SetActive(bool IsActive)
{
	m_isActive = IsActive;
}

bool CEventTrigger::IsActive() const
{
	return m_isActive;
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

void CEventTrigger::InsertEventObject(const shared_ptr<CObject>& EventObject)
{
	if (EventObject)
	{
		m_EventObjects.push_back(EventObject);
	}
}

shared_ptr<CObject> CEventTrigger::GetEventObject(UINT Index)
{
	if (Index < 0 || Index >= m_EventObjects.size())
	{
		return nullptr;
	}

	return m_EventObjects[Index];
}

void CEventTrigger::SetInteractionUI(const shared_ptr<CQuadObject>& InteractionUI)
{
	if (InteractionUI)
	{
		m_InteractionUI = InteractionUI;
	}
}

shared_ptr<CQuadObject> CEventTrigger::GetInteractionUI() const
{
	return m_InteractionUI;
}

bool CEventTrigger::IsInTriggerArea(const XMFLOAT3& Position, const XMFLOAT3& LookDirection)
{
	if (m_isActive && !m_IsInteracted)
	{
		for (UINT i = 0; i < 2; ++i)
		{
			if (Math::IsInTriangle(m_TriggerArea[0], m_TriggerArea[i + 1], m_TriggerArea[i + 2], Position))
			{
				// 각도가 일정 범위안에 있다면 상호작용 UI를 렌더링하도록 만든다.
				if (Vector3::Angle(LookDirection, m_ToTrigger) <= m_ActiveFOV)
				{
					ShowInteractionUI();

					return true;
				}
			}
		}
	}

	HideInteractionUI();

	return false;
}

void CEventTrigger::HideInteractionUI()
{
	if (m_InteractionUI)
	{
		m_InteractionUI->SetActive(false);
	}
}
