#pragma once

class CGameObject;
class CBilboardObject;

class CEventTrigger
{
protected:
	bool							m_IsActive{};

	XMFLOAT4						m_TriggerArea{};

	vector<shared_ptr<CGameObject>> m_EventObjects{};
	shared_ptr<CBilboardObject>		m_InteractionUI{};

public:
	CEventTrigger() = default;
	virtual ~CEventTrigger() = default;

	virtual void GenerateEventTrigger(float ElapsedTime);

	void LoadEventTriggerFromFile(tifstream& InFile);

	void SetActive(bool IsActive);
	bool IsActive() const;

	bool IsInTriggerArea(const XMFLOAT3& Position, const XMFLOAT3& LookDirection);

	void InsertEventObject(const shared_ptr<CGameObject>& EventObject);

	void SetInteractionUI(const shared_ptr<CBilboardObject>& InteractionUI);
	CBilboardObject* GetInteractionUI() const;
};
