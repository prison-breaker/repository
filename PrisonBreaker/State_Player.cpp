#include "stdafx.h"
#include "State_Player.h"
#include "Player.h"

CPlayerIdleState* CPlayerIdleState::GetInstance()
{
	static CPlayerIdleState Instance{};

	return &Instance;
}

void CPlayerIdleState::Enter(const shared_ptr<CPlayer>& Entity)
{

}

void CPlayerIdleState::ProcessInput(const shared_ptr<CPlayer>& Entity, UINT InputMask)
{

}

void CPlayerIdleState::Update(const shared_ptr<CPlayer>& Entity)
{
	Entity->GetAnimationController()->UpdateAnimationClip(ANIMATION_TYPE_LOOP);
}

void CPlayerIdleState::Exit(const shared_ptr<CPlayer>& Entity)
{

}

//=========================================================================================================================

CPlayerWalkState* CPlayerWalkState::GetInstance()
{
	static CPlayerWalkState Instance{};

	return &Instance;
}

void CPlayerWalkState::Enter(const shared_ptr<CPlayer>& Entity)
{

}

void CPlayerWalkState::ProcessInput(const shared_ptr<CPlayer>& Entity, UINT InputMask)
{

}

void CPlayerWalkState::Update(const shared_ptr<CPlayer>& Entity)
{
	Entity->GetAnimationController()->UpdateAnimationClip(ANIMATION_TYPE_LOOP);
}

void CPlayerWalkState::Exit(const shared_ptr<CPlayer>& Entity)
{

}

//=========================================================================================================================

CPlayerRunState* CPlayerRunState::GetInstance()
{
	static CPlayerRunState Instance{};

	return &Instance;
}

void CPlayerRunState::Enter(const shared_ptr<CPlayer>& Entity)
{

}

void CPlayerRunState::ProcessInput(const shared_ptr<CPlayer>& Entity, UINT InputMask)
{

}

void CPlayerRunState::Update(const shared_ptr<CPlayer>& Entity)
{

}

void CPlayerRunState::Exit(const shared_ptr<CPlayer>& Entity)
{

}

//=========================================================================================================================

CPlayerAttackState* CPlayerAttackState::GetInstance()
{
	static CPlayerAttackState Instance{};

	return &Instance;
}

void CPlayerAttackState::Enter(const shared_ptr<CPlayer>& Entity)
{

}
void CPlayerAttackState::ProcessInput(const shared_ptr<CPlayer>& Entity, UINT InputMask)
{

}

void CPlayerAttackState::Update(const shared_ptr<CPlayer>& Entity)
{

}

void CPlayerAttackState::Exit(const shared_ptr<CPlayer>& Entity)
{

}

//=========================================================================================================================

CPlayerDieState* CPlayerDieState::GetInstance()
{
	static CPlayerDieState Instance{};

	return &Instance;
}

void CPlayerDieState::Enter(const shared_ptr<CPlayer>& Entity)
{

}

void CPlayerDieState::ProcessInput(const shared_ptr<CPlayer>& Entity, UINT InputMask)
{

}

void CPlayerDieState::Update(const shared_ptr<CPlayer>& Entity)
{

}

void CPlayerDieState::Exit(const shared_ptr<CPlayer>& Entity)
{

}
