#pragma once

class CGameObject;
class CBilboardObject;

class CEventTrigger
{
protected:
	bool							m_IsActive{};
	bool							m_IsInteracted{};

	XMFLOAT3						m_ToTrigger{};
	float							m_ActiveFOV{};

	XMFLOAT3						m_TriggerAreas[4]{}; // Vertice

	vector<shared_ptr<CGameObject>> m_EventObjects{};
	shared_ptr<CBilboardObject>		m_InteractionUI{};

public:
	CEventTrigger() = default;
	virtual ~CEventTrigger() = default;

	virtual bool CanPassTriggerArea(const XMFLOAT3& Position, const XMFLOAT3& NewPosition);

	virtual void ShowInteractionUI();

	virtual void InteractEventTrigger();
	virtual void GenerateEventTrigger(float ElapsedTime);

	void LoadEventTriggerFromFile(tifstream& InFile);

	void SetActive(bool IsActive);
	bool IsActive() const;

	void SetInteracted(bool IsInteracted);
	bool IsInteracted() const;

	void CalculateTriggerAreasByGuardPosition(const XMFLOAT3& Position);

	void InsertEventObject(const shared_ptr<CGameObject>& EventObject);
	shared_ptr<CGameObject> GetEventObject(UINT Index);

	void SetInteractionUI(const shared_ptr<CBilboardObject>& InteractionUI);
	shared_ptr<CBilboardObject> GetInteractionUI() const;

	bool IsInTriggerArea(const XMFLOAT3& Position, const XMFLOAT3& LookDirection);

	void HideInteractionUI();
};
