#pragma once
#include "State.h"

class CGuardIdleState : public CState, public CSingleton<CGuardIdleState>
{
	friend class CSingleton;

private:
	CGuardIdleState();
	virtual ~CGuardIdleState();

public:
	virtual void Enter(CObject* object);
	virtual void Exit(CObject* object);

	virtual void Update(CObject* object);
};

//=========================================================================================================================

class CGuardPatrolState : public CState, public CSingleton<CGuardPatrolState>
{
	friend class CSingleton;

private:
	CGuardPatrolState();
	virtual ~CGuardPatrolState();

public:
	virtual void Enter(CObject* object);
	virtual void Exit(CObject* object);

	virtual void Update(CObject* object);
};

//=========================================================================================================================

class CGuardChaseState : public CState, public CSingleton<CGuardChaseState>
{
	friend class CSingleton;

private:
	CGuardChaseState();
	virtual ~CGuardChaseState();

public:
	virtual void Enter(CObject* object);
	virtual void Exit(CObject* object);

	virtual void Update(CObject* object);
};

//=========================================================================================================================

class CGuardReturnState : public CState, public CSingleton<CGuardReturnState>
{
	friend class CSingleton;

private:
	CGuardReturnState();
	virtual ~CGuardReturnState();

public:
	virtual void Enter(CObject* object);
	virtual void Exit(CObject* object);

	virtual void Update(CObject* object);
};

//=========================================================================================================================

class CGuardAssembleState : public CState, public CSingleton<CGuardAssembleState>
{
	friend class CSingleton;

private:
	CGuardAssembleState();
	virtual ~CGuardAssembleState();

public:
	virtual void Enter(CObject* object);
	virtual void Exit(CObject* object);

	virtual void Update(CObject* object);
};

//=========================================================================================================================

class CGuardShootState : public CState, public CSingleton<CGuardShootState>
{
	friend class CSingleton;

private:
	CGuardShootState();
	virtual ~CGuardShootState();

public:
	virtual void Enter(CObject* object);
	virtual void Exit(CObject* object);

	virtual void Update(CObject* object);
};

//=========================================================================================================================

class CGuardHitState : public CState, public CSingleton<CGuardHitState>
{
	friend class CSingleton;

private:
	CGuardHitState();
	virtual ~CGuardHitState();

public:
	virtual void Enter(CObject* object);
	virtual void Exit(CObject* object);

	virtual void Update(CObject* object);
};

//=========================================================================================================================

class CGuardDieState : public CState, public CSingleton<CGuardDieState>
{
	friend class CSingleton;

private:
	CGuardDieState();
	virtual ~CGuardDieState();

public:
	virtual void Enter(CObject* object);
	virtual void Exit(CObject* object);

	virtual void Update(CObject* object);
};
