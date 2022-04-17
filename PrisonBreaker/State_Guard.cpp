#include "stdafx.h"
#include "State_Guard.h"
#include "Guard.h"

CGuardIdleState* CGuardIdleState::GetInstance()
{
	static CGuardIdleState Instance{};

	return &Instance;
}

void CGuardIdleState::Enter(const shared_ptr<CGuard>& Entity)
{
	Entity->SetElapsedTime(0.0f);
	Entity->SetAnimationClip(0);
}

void CGuardIdleState::ProcessInput(const shared_ptr<CGuard>& Entity, UINT InputMask, float ElapsedTime)
{

}

void CGuardIdleState::Update(const shared_ptr<CGuard>& Entity, float ElapsedTime)
{
	// 애니메이션이 끝나면 Patrol 상태로 변경한다.
	if (Entity->GetAnimationController()->UpdateAnimationClip(ANIMATION_TYPE_ONCE))
	{
		Entity->GetStateMachine()->ChangeState(CGuardPatrolState::GetInstance());
	}
}

void CGuardIdleState::Exit(const shared_ptr<CGuard>& Entity)
{

}

//=========================================================================================================================

CGuardPatrolState* CGuardPatrolState::GetInstance()
{
	static CGuardPatrolState Instance{};

	return &Instance;
}

void CGuardPatrolState::Enter(const shared_ptr<CGuard>& Entity)
{
	XMFLOAT3 Direction{ Vector3::Normalize(Vector3::Subtract(Entity->GetPatrolNavPath().back(), Entity->GetPosition())) };

	Entity->UpdateLocalCoord(Direction);
	Entity->SetElapsedTime(0.0f);
	Entity->SetRecentTransition(false);
	Entity->SetAnimationClip(1);
	Entity->SetSpeed(5.0f);
}

void CGuardPatrolState::ProcessInput(const shared_ptr<CGuard>& Entity, UINT InputMask, float ElapsedTime)
{

}

void CGuardPatrolState::Update(const shared_ptr<CGuard>& Entity, float ElapsedTime)
{
	// ToIdleEntryTime이 된다면 IdleState로 전이한다.
	Entity->SetElapsedTime(Entity->GetElapsedTime() + ElapsedTime);

	if (Entity->GetElapsedTime() >= Entity->GetToIdleEntryTime())
	{
		Entity->GetStateMachine()->ChangeState(CGuardIdleState::GetInstance());
	}
	else
	{
		Entity->GetAnimationController()->UpdateAnimationClip(ANIMATION_TYPE_LOOP);
		Entity->Patrol(ElapsedTime);
	}
}

void CGuardPatrolState::Exit(const shared_ptr<CGuard>& Entity)
{

}

//=========================================================================================================================

CGuardChaseState* CGuardChaseState::GetInstance()
{
	static CGuardChaseState Instance{};

	return &Instance;
}

void CGuardChaseState::Enter(const shared_ptr<CGuard>& Entity)
{
	XMFLOAT3 Direction{ Vector3::Normalize(Vector3::Subtract(Entity->GetNavPath().back(), Entity->GetPosition())) };

	Entity->UpdateLocalCoord(Direction);
	Entity->SetElapsedTime(0.0f);
	Entity->SetRecentTransition(false);
	Entity->SetAnimationClip(2);
	Entity->SetSpeed(13.0f);
}

void CGuardChaseState::ProcessInput(const shared_ptr<CGuard>& Entity, UINT InputMask, float ElapsedTime)
{

}

void CGuardChaseState::Update(const shared_ptr<CGuard>& Entity, float ElapsedTime)
{
	Entity->SetElapsedTime(Entity->GetElapsedTime() + ElapsedTime);
	Entity->GetAnimationController()->UpdateAnimationClip(ANIMATION_TYPE_LOOP);
	Entity->MoveToNavPath(ElapsedTime);
}

void CGuardChaseState::Exit(const shared_ptr<CGuard>& Entity)
{

}

//=========================================================================================================================

CGuardReturnState* CGuardReturnState::GetInstance()
{
	static CGuardReturnState Instance{};

	return &Instance;
}

void CGuardReturnState::Enter(const shared_ptr<CGuard>& Entity)
{
	XMFLOAT3 Direction{ Vector3::Normalize(Vector3::Subtract(Entity->GetNavPath().back(), Entity->GetPosition())) };

	Entity->UpdateLocalCoord(Direction);
	Entity->SetElapsedTime(0.0f);
	Entity->SetRecentTransition(false);
	Entity->SetAnimationClip(1);
	Entity->SetSpeed(5.0f);
}

void CGuardReturnState::ProcessInput(const shared_ptr<CGuard>& Entity, UINT InputMask, float ElapsedTime)
{

}

void CGuardReturnState::Update(const shared_ptr<CGuard>& Entity, float ElapsedTime)
{
	Entity->SetElapsedTime(Entity->GetElapsedTime() + ElapsedTime);
	Entity->MoveToNavPath(ElapsedTime);

	// 원래 포지션으로 돌아갔다면 PatroState로 전이한다.
	if (Entity->GetNavPath().empty())
	{
		Entity->GetStateMachine()->ChangeState(CGuardPatrolState::GetInstance());
	}
	else
	{
		Entity->GetAnimationController()->UpdateAnimationClip(ANIMATION_TYPE_LOOP);
	}
}

void CGuardReturnState::Exit(const shared_ptr<CGuard>& Entity)
{

}

//=========================================================================================================================

CGuardShootingState* CGuardShootingState::GetInstance()
{
	static CGuardShootingState Instance{};

	return &Instance;
}

void CGuardShootingState::Enter(const shared_ptr<CGuard>& Entity)
{
	Entity->SetElapsedTime(0.0f);
	Entity->SetRecentTransition(false);
	Entity->SetAnimationClip(3);
}

void CGuardShootingState::ProcessInput(const shared_ptr<CGuard>& Entity, UINT InputMask, float ElapsedTime)
{

}

void CGuardShootingState::Update(const shared_ptr<CGuard>& Entity, float ElapsedTime)
{
	// 총을 쏘면 다시 ChaseState로 전이한다.
	if (Entity->GetAnimationController()->UpdateAnimationClip(ANIMATION_TYPE_ONCE))
	{
		Entity->GetStateMachine()->ChangeState(CGuardChaseState::GetInstance());
	}
}

void CGuardShootingState::Exit(const shared_ptr<CGuard>& Entity)
{

}

//=========================================================================================================================

CGuardDyingState* CGuardDyingState::GetInstance()
{
	static CGuardDyingState Instance{};

	return &Instance;
}

void CGuardDyingState::Enter(const shared_ptr<CGuard>& Entity)
{
	Entity->SetElapsedTime(0.0f);
	Entity->SetRecentTransition(false);
	Entity->SetAnimationClip(4);
}

void CGuardDyingState::ProcessInput(const shared_ptr<CGuard>& Entity, UINT InputMask, float ElapsedTime)
{

}

void CGuardDyingState::Update(const shared_ptr<CGuard>& Entity, float ElapsedTime)
{
	// 교도관이 사망하면 오브젝트를 비활성화한다.
	if (Entity->GetAnimationController()->UpdateAnimationClip(ANIMATION_TYPE_ONCE))
	{
		Entity->SetActive(false);
	}
}

void CGuardDyingState::Exit(const shared_ptr<CGuard>& Entity)
{

}
