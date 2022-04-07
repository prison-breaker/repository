#pragma once
#include "State.h"

class CGuard;

class CGuardIdleState : public CState<CGuard>
{
private:
	CGuardIdleState() = default;
	virtual ~CGuardIdleState() = default;

public:
	static CGuardIdleState* GetInstance();

	virtual void Enter(const shared_ptr<CGuard>& Entity);
	virtual void ProcessInput(const shared_ptr<CGuard>& Entity, UINT InputMask, float ElapsedTime);
	virtual void Update(const shared_ptr<CGuard>& Entity, float ElapsedTime);
	virtual void Exit(const shared_ptr<CGuard>& Entity);
};

//=========================================================================================================================

class CGuardPatrolState : public CState<CGuard>
{
private:
	CGuardPatrolState() = default;
	virtual ~CGuardPatrolState() = default;

public:
	static CGuardPatrolState* GetInstance();

	virtual void Enter(const shared_ptr<CGuard>& Entity);
	virtual void ProcessInput(const shared_ptr<CGuard>& Entity, UINT InputMask, float ElapsedTime);
	virtual void Update(const shared_ptr<CGuard>& Entity, float ElapsedTime);
	virtual void Exit(const shared_ptr<CGuard>& Entity);
};

//=========================================================================================================================

class CGuardChaseState : public CState<CGuard>
{
private:
	CGuardChaseState() = default;
	virtual ~CGuardChaseState() = default;

public:
	static CGuardChaseState* GetInstance();

	virtual void Enter(const shared_ptr<CGuard>& Entity);
	virtual void ProcessInput(const shared_ptr<CGuard>& Entity, UINT InputMask, float ElapsedTime);
	virtual void Update(const shared_ptr<CGuard>& Entity, float ElapsedTime);
	virtual void Exit(const shared_ptr<CGuard>& Entity);
};

//=========================================================================================================================

class CGuardShootingState : public CState<CGuard>
{
private:
	CGuardShootingState() = default;
	virtual ~CGuardShootingState() = default;

public:
	static CGuardShootingState* GetInstance();

	virtual void Enter(const shared_ptr<CGuard>& Entity);
	virtual void ProcessInput(const shared_ptr<CGuard>& Entity, UINT InputMask, float ElapsedTime);
	virtual void Update(const shared_ptr<CGuard>& Entity, float ElapsedTime);
	virtual void Exit(const shared_ptr<CGuard>& Entity);
};

//=========================================================================================================================

class CGuardDyingState : public CState<CGuard>
{
private:
	CGuardDyingState() = default;
	virtual ~CGuardDyingState() = default;

public:
	static CGuardDyingState* GetInstance();

	virtual void Enter(const shared_ptr<CGuard>& Entity);
	virtual void ProcessInput(const shared_ptr<CGuard>& Entity, UINT InputMask, float ElapsedTime);
	virtual void Update(const shared_ptr<CGuard>& Entity, float ElapsedTime);
	virtual void Exit(const shared_ptr<CGuard>& Entity);
};
