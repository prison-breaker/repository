#pragma once
#include "QuadObject.h"

template <typename>
class CStateMachine;

class CBackgroundUI : public CQuadObject
{
private:
	shared_ptr<CStateMachine<CBackgroundUI>> m_StateMachine{};

public:
	CBackgroundUI() = default;
	virtual ~CBackgroundUI() = default;

	virtual void Initialize();

	virtual void Animate(float ElapsedTime);

	shared_ptr<CStateMachine<CBackgroundUI>> GetStateMachine() const;
};

//=========================================================================================================================

class CMissionUI : public CQuadObject
{
private:
	shared_ptr<CStateMachine<CMissionUI>> m_StateMachine{};

public:
	CMissionUI() = default;
	virtual ~CMissionUI() = default;

	virtual void Initialize();
	virtual void Reset();

	virtual void Animate(float ElapsedTime);

	shared_ptr<CStateMachine<CMissionUI>> GetStateMachine() const;
};

//=========================================================================================================================

class CKeyUI : public CQuadObject
{
private:
	shared_ptr<CStateMachine<CKeyUI>> m_StateMachine{};

public:
	CKeyUI() = default;
	virtual ~CKeyUI() = default;

	virtual void Initialize();
	virtual void Reset();

	virtual void Animate(float ElapsedTime);

	shared_ptr<CStateMachine<CKeyUI>> GetStateMachine() const;
};

//=========================================================================================================================

class CHitUI : public CQuadObject
{
private:
	shared_ptr<CStateMachine<CHitUI>> m_StateMachine{};

public:
	CHitUI() = default;
	virtual ~CHitUI() = default;

	virtual void Initialize();
	virtual void Reset();

	virtual void Animate(float ElapsedTime);

	shared_ptr<CStateMachine<CHitUI>> GetStateMachine() const;
};

//=========================================================================================================================

class CButtonUI : public CQuadObject
{
protected:
	bool	 m_IsMouseOver{};

	XMFLOAT4 m_ButtonAreas[2]{};

public:
	CButtonUI() = default;
	virtual ~CButtonUI() = default;

	void SetButtonArea(UINT Index, const XMFLOAT4& Area);
	const XMFLOAT4& GetButtonArea(UINT Index) const;
};

//=========================================================================================================================

class CMainButtonUI : public CButtonUI
{
public:
	CMainButtonUI() = default;
	virtual ~CMainButtonUI() = default;

	virtual void ProcessMouseMessage(UINT Message, const XMINT2& ScreenPosition, UINT RootFrameIndex);
};

//=========================================================================================================================

class CPanelButtonUI : public CButtonUI
{
public:
	CPanelButtonUI() = default;
	virtual ~CPanelButtonUI() = default;

	virtual void ProcessMouseMessage(UINT Message, const XMINT2& ScreenPosition, UINT RootFrameIndex);
};

//=========================================================================================================================

class CLoadingIconUI : public CQuadObject
{
public:
	CLoadingIconUI() = default;
	virtual ~CLoadingIconUI() = default;

	virtual void Reset();

	virtual void Animate(float ElapsedTime);
};

//=========================================================================================================================

class CGameOverUI : public CQuadObject
{
public:
	CGameOverUI() = default;
	virtual ~CGameOverUI() = default;

	virtual void Reset();

	virtual void Animate(float ElapsedTime);
};

//=========================================================================================================================

class CEndingCreditUI : public CQuadObject
{
public:
	CEndingCreditUI() = default;
	virtual ~CEndingCreditUI() = default;

	virtual void Reset();

	virtual void Animate(float ElapsedTime);
};
