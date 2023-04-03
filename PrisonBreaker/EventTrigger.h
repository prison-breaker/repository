#pragma once

class CObject;
class CQuadObject;

class CEventTrigger : public enable_shared_from_this<CEventTrigger>
{
protected:
	bool							m_IsActive{};
	bool							m_IsInteracted{};

	XMFLOAT3						m_ToTrigger{};
	float							m_ActiveFOV{};

	XMFLOAT3						m_TriggerArea[4]{}; // Vertices

	vector<shared_ptr<CObject>> m_EventObjects{};
	shared_ptr<CQuadObject>		m_InteractionUI{};

public:
	CEventTrigger() = default;
	virtual ~CEventTrigger() = default;

	virtual void Reset();

	virtual bool CanPassTriggerArea(const XMFLOAT3& Position, const XMFLOAT3& NewPosition);

	virtual void ShowInteractionUI();

	virtual bool InteractEventTrigger(UINT CallerIndex);
	virtual void Update(float ElapsedTime);

	void LoadEventTriggerFromFile(ifstream& in);

	void SetActive(bool IsActive);
	bool IsActive() const;

	void SetInteracted(bool IsInteracted);
	bool IsInteracted() const;

	void CalculateTriggerAreaByGuard(const XMFLOAT3& Position);

	void InsertEventObject(const shared_ptr<CObject>& EventObject);
	shared_ptr<CObject> GetEventObject(UINT Index);

	void SetInteractionUI(const shared_ptr<CQuadObject>& InteractionUI);
	shared_ptr<CQuadObject> GetInteractionUI() const;

	bool IsInTriggerArea(const XMFLOAT3& Position, const XMFLOAT3& LookDirection);

	void HideInteractionUI();
};
