#pragma once

class CGameObject;

class CEventTrigger
{
protected:
	bool							m_IsActive{};

	XMFLOAT4						m_TriggerArea{};

	vector<shared_ptr<CGameObject>> m_EventObjects{};

public:
	CEventTrigger() = default;
	virtual ~CEventTrigger() = default;

	virtual void GenerateEventTrigger(float ElapsedTime);

	void LoadEventTriggerFromFile(tifstream& InFile);

	void SetActive(bool IsActive);
	bool IsActive() const;

	bool IsInTriggerArea(const XMFLOAT3& Position);

	void InsertEventObject(const shared_ptr<CGameObject>& EventObject);
};
