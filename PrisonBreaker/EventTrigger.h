#pragma once

class CGameObject;
class CBilboardObject;

class CEventTrigger
{
protected:
	bool							m_IsInteracted{};

	XMFLOAT3						m_ToTrigger{};
	XMFLOAT4						m_TriggerArea{};

	vector<shared_ptr<CGameObject>> m_EventObjects{};
	shared_ptr<CBilboardObject>		m_InteractionUI{};

public:
	CEventTrigger() = default;
	virtual ~CEventTrigger() = default;

	virtual bool IsInTriggerArea(const XMFLOAT3& Position, const XMFLOAT3& LookDirection);

	virtual void ActivateInteractionUI();

	virtual void GenerateEventTrigger(float ElapsedTime);

	void LoadEventTriggerFromFile(tifstream& InFile);

	void SetInteracted(bool IsInteracted);
	bool IsInteracted() const;

	void CalculateTriggerAreaByPoint(const XMFLOAT3& Position, float XWidth, float ZWidth);

	void InsertEventObject(const shared_ptr<CGameObject>& EventObject);
	shared_ptr<CGameObject> GetEventObject(UINT Index);

	void SetInteractionUI(const shared_ptr<CBilboardObject>& InteractionUI);
	shared_ptr<CBilboardObject> GetInteractionUI() const;
};
