#pragma once
#include "BilboardObject.h"
#include "StateMachine.h"
#include "State_Background.h"
#include "State_MissionUI.h"
#include "State_KeyUI.h"
#include "State_HitUI.h"

class CBackgroundUI : public CBilboardObject
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

class CMissionUI : public CBilboardObject
{
private:
	shared_ptr<CStateMachine<CMissionUI>> m_StateMachine{};

public:
	CMissionUI() = default;
	virtual ~CMissionUI() = default;

	virtual void Initialize();

	virtual void Animate(float ElapsedTime);

	shared_ptr<CStateMachine<CMissionUI>> GetStateMachine() const;
};

//=========================================================================================================================

class CKeyUI : public CBilboardObject
{
private:
	shared_ptr<CStateMachine<CKeyUI>> m_StateMachine{};

public:
	CKeyUI() = default;
	virtual ~CKeyUI() = default;

	virtual void Initialize();

	virtual void Animate(float ElapsedTime);

	shared_ptr<CStateMachine<CKeyUI>> GetStateMachine() const;
};

//=========================================================================================================================

class CHitUI : public CBilboardObject
{
private:
	shared_ptr<CStateMachine<CHitUI>> m_StateMachine{};

public:
	CHitUI() = default;
	virtual ~CHitUI() = default;

	virtual void Initialize();

	virtual void Animate(float ElapsedTime);

	shared_ptr<CStateMachine<CHitUI>> GetStateMachine() const;
};