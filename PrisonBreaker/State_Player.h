#pragma once
#include "State.h"

class CPlayer;

class CPlayerIdleState : public CState<CPlayer>
{
private:
	CPlayerIdleState() = default;
	virtual ~CPlayerIdleState() = default;

public:
	static CPlayerIdleState* GetInstance();

	virtual void Enter(const shared_ptr<CPlayer>& Entity);
	virtual void ProcessInput(const shared_ptr<CPlayer>& Entity, UINT InputMask);
	virtual void Update(const shared_ptr<CPlayer>& Entity);
	virtual void Exit(const shared_ptr<CPlayer>& Entity);
};

//=========================================================================================================================

class CPlayerWalkState : public CState<CPlayer>
{
private:
	CPlayerWalkState() = default;
	virtual ~CPlayerWalkState() = default;

public:
	static CPlayerWalkState* GetInstance();

	virtual void Enter(const shared_ptr<CPlayer>& Entity);
	virtual void ProcessInput(const shared_ptr<CPlayer>& Entity, UINT InputMask);
	virtual void Update(const shared_ptr<CPlayer>& Entity);
	virtual void Exit(const shared_ptr<CPlayer>& Entity);
};

//=========================================================================================================================

class CPlayerRunState : public CState<CPlayer>
{
private:
	CPlayerRunState() = default;
	virtual ~CPlayerRunState() = default;

public:
	static CPlayerRunState* GetInstance();

	virtual void Enter(const shared_ptr<CPlayer>& Entity);
	virtual void ProcessInput(const shared_ptr<CPlayer>& Entity, UINT InputMask);
	virtual void Update(const shared_ptr<CPlayer>& Entity);
	virtual void Exit(const shared_ptr<CPlayer>& Entity);
};

//=========================================================================================================================

class CPlayerAttackState : public CState<CPlayer>
{
private:
	CPlayerAttackState() = default;
	virtual ~CPlayerAttackState() = default;

public:
	static CPlayerAttackState* GetInstance();

	virtual void Enter(const shared_ptr<CPlayer>& Entity);
	virtual void ProcessInput(const shared_ptr<CPlayer>& Entity, UINT InputMask);
	virtual void Update(const shared_ptr<CPlayer>& Entity);
	virtual void Exit(const shared_ptr<CPlayer>& Entity);
};

//=========================================================================================================================

class CPlayerDieState : public CState<CPlayer>
{
private:
	CPlayerDieState() = default;
	virtual ~CPlayerDieState() = default;

public:
	static CPlayerDieState* GetInstance();

	virtual void Enter(const shared_ptr<CPlayer>& Entity);
	virtual void ProcessInput(const shared_ptr<CPlayer>& Entity, UINT InputMask);
	virtual void Update(const shared_ptr<CPlayer>& Entity);
	virtual void Exit(const shared_ptr<CPlayer>& Entity);
};
