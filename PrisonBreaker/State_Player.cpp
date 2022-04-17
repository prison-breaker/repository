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
	Entity->SetSpeed(0.0f);
	Entity->SetMovingDirection(XMFLOAT3(0.0f, 0.0f, 0.0f));
	Entity->SetAnimationClip(0);
}

void CPlayerIdleState::ProcessInput(const shared_ptr<CPlayer>& Entity, UINT InputMask, float ElapsedTime)
{
	if (InputMask & INPUT_MASK_LMB)
	{
		Entity->GetStateMachine()->ChangeState(CPlayerPunchingState::GetInstance());
	}
	else if (InputMask & INPUT_MASK_RMB)
	{
		Entity->SetAnimationClip(8);
		Entity->GetStateMachine()->ChangeState(CPlayerShootingState::GetInstance());
	}
	else if (InputMask & INPUT_MASK_W || InputMask & INPUT_MASK_S || InputMask & INPUT_MASK_A || InputMask & INPUT_MASK_D)
	{
		(InputMask & INPUT_MASK_SHIFT) ? Entity->GetStateMachine()->ChangeState(CPlayerRunningState::GetInstance()) : Entity->GetStateMachine()->ChangeState(CPlayerWalkingState::GetInstance());
	}
}

void CPlayerIdleState::Update(const shared_ptr<CPlayer>& Entity, float ElapsedTime)
{
	Entity->GetAnimationController()->UpdateAnimationClip(ANIMATION_TYPE_LOOP);
}

void CPlayerIdleState::Exit(const shared_ptr<CPlayer>& Entity)
{

}

//=========================================================================================================================

CPlayerWalkingState* CPlayerWalkingState::GetInstance()
{
	static CPlayerWalkingState Instance{};

	return &Instance;
}

void CPlayerWalkingState::Enter(const shared_ptr<CPlayer>& Entity)
{

}

void CPlayerWalkingState::ProcessInput(const shared_ptr<CPlayer>& Entity, UINT InputMask, float ElapsedTime)
{
	if (InputMask & INPUT_MASK_LMB)
	{
		Entity->GetStateMachine()->ChangeState(CPlayerPunchingState::GetInstance());
		return;
	}
	else if (InputMask & INPUT_MASK_RMB)
	{
		Entity->SetAnimationClip(8);
		Entity->GetStateMachine()->ChangeState(CPlayerShootingState::GetInstance());
		return;
	}
	else if (InputMask & INPUT_MASK_SHIFT)
	{
		if (InputMask & INPUT_MASK_W || InputMask & INPUT_MASK_S || InputMask & INPUT_MASK_A || InputMask & INPUT_MASK_D)
		{
			Entity->GetStateMachine()->ChangeState(CPlayerRunningState::GetInstance());
			return;
		}
	}

	switch (InputMask)
	{
	case INPUT_MASK_W:
		Entity->SetSpeed(3.15f);
		Entity->SetMovingDirection(Entity->GetLook());
		Entity->SetAnimationClip(1);
		break;
	case INPUT_MASK_W | INPUT_MASK_A:
		Entity->SetSpeed(3.15f);
		Entity->SetMovingDirection(Vector3::Normalize(Vector3::Add(Entity->GetLook(), Vector3::Inverse(Entity->GetRight()))));
		Entity->SetAnimationClip(1);
		break;
	case INPUT_MASK_W | INPUT_MASK_D:
		Entity->SetSpeed(3.15f);
		Entity->SetMovingDirection(Vector3::Normalize(Vector3::Add(Entity->GetLook(), Entity->GetRight())));
		Entity->SetAnimationClip(1);
		break;
	case INPUT_MASK_S:
		Entity->SetSpeed(-3.15f);
		Entity->SetMovingDirection(Entity->GetLook());
		Entity->SetAnimationClip(1);
		break;
	case INPUT_MASK_S | INPUT_MASK_A:
		Entity->SetSpeed(-3.15f);
		Entity->SetMovingDirection(Vector3::Normalize(Vector3::Add(Entity->GetLook(), Entity->GetRight())));
		Entity->SetAnimationClip(1);
		break;
	case INPUT_MASK_S | INPUT_MASK_D:
		Entity->SetSpeed(-3.15f);
		Entity->SetMovingDirection(Vector3::Normalize(Vector3::Add(Entity->GetLook(), Vector3::Inverse(Entity->GetRight()))));
		Entity->SetAnimationClip(1);
		break;
	case INPUT_MASK_A:
		Entity->SetSpeed(-3.15f);
		Entity->SetMovingDirection(Entity->GetRight());
		Entity->SetAnimationClip(2);
		break;
	case INPUT_MASK_D:
		Entity->SetSpeed(3.15f);
		Entity->SetMovingDirection(Entity->GetRight());
		Entity->SetAnimationClip(3);
		break;
	default:
		Entity->GetStateMachine()->ChangeState(CPlayerIdleState::GetInstance());
	}
}

void CPlayerWalkingState::Update(const shared_ptr<CPlayer>& Entity, float ElapsedTime)
{
	Entity->GetAnimationController()->UpdateAnimationClip(ANIMATION_TYPE_LOOP);
}

void CPlayerWalkingState::Exit(const shared_ptr<CPlayer>& Entity)
{

}

//=========================================================================================================================

CPlayerRunningState* CPlayerRunningState::GetInstance()
{
	static CPlayerRunningState Instance{};

	return &Instance;
}

void CPlayerRunningState::Enter(const shared_ptr<CPlayer>& Entity)
{

}

void CPlayerRunningState::ProcessInput(const shared_ptr<CPlayer>& Entity, UINT InputMask, float ElapsedTime)
{
	if (InputMask & INPUT_MASK_LMB)
	{
		Entity->GetStateMachine()->ChangeState(CPlayerPunchingState::GetInstance());
		return;
	}
	else if (InputMask & INPUT_MASK_RMB)
	{
		Entity->SetAnimationClip(8);
		Entity->GetStateMachine()->ChangeState(CPlayerShootingState::GetInstance());
		return;
	}
	else if (!(InputMask & INPUT_MASK_SHIFT))
	{
		if (InputMask & INPUT_MASK_W || InputMask & INPUT_MASK_S || InputMask & INPUT_MASK_A || InputMask & INPUT_MASK_D)
		{
			Entity->GetStateMachine()->ChangeState(CPlayerWalkingState::GetInstance());
			return;
		}
	}

	switch (InputMask)
	{
	case INPUT_MASK_W | INPUT_MASK_SHIFT:
		Entity->SetSpeed(12.6f);
		Entity->SetMovingDirection(Entity->GetLook());
		Entity->SetAnimationClip(4);
		break;
	case INPUT_MASK_W | INPUT_MASK_A | INPUT_MASK_SHIFT:
		Entity->SetSpeed(12.6f);
		Entity->SetMovingDirection(Vector3::Normalize(Vector3::Add(Entity->GetLook(), Vector3::Inverse(Entity->GetRight()))));
		Entity->SetAnimationClip(4);
		break;
	case INPUT_MASK_W | INPUT_MASK_D | INPUT_MASK_SHIFT:
		Entity->SetSpeed(12.6f);
		Entity->SetMovingDirection(Vector3::Normalize(Vector3::Add(Entity->GetLook(), Entity->GetRight())));
		Entity->SetAnimationClip(4);
		break;
	case INPUT_MASK_S | INPUT_MASK_SHIFT:
		Entity->SetSpeed(-3.15f);
		Entity->SetMovingDirection(Entity->GetLook());
		Entity->SetAnimationClip(1);
		break;
	case INPUT_MASK_S | INPUT_MASK_A | INPUT_MASK_SHIFT:
		Entity->SetSpeed(-3.15f);
		Entity->SetMovingDirection(Vector3::Normalize(Vector3::Add(Entity->GetLook(), Entity->GetRight())));
		Entity->SetAnimationClip(1);
		break;
	case INPUT_MASK_S | INPUT_MASK_D | INPUT_MASK_SHIFT:
		Entity->SetSpeed(-3.15f);
		Entity->SetMovingDirection(Vector3::Normalize(Vector3::Add(Entity->GetLook(), Vector3::Inverse(Entity->GetRight()))));
		Entity->SetAnimationClip(1);
		break;
	case INPUT_MASK_A | INPUT_MASK_SHIFT:
		Entity->SetSpeed(-12.6f);
		Entity->SetMovingDirection(Entity->GetRight());
		Entity->SetAnimationClip(5);
		break;
	case INPUT_MASK_D | INPUT_MASK_SHIFT:
		Entity->SetSpeed(12.6f);
		Entity->SetMovingDirection(Entity->GetRight());
		Entity->SetAnimationClip(6);
		break;
	default:
		Entity->GetStateMachine()->ChangeState(CPlayerIdleState::GetInstance());
		break;
	}

	Entity->SetSpeed(Entity->GetSpeed());
}

void CPlayerRunningState::Update(const shared_ptr<CPlayer>& Entity, float ElapsedTime)
{
	Entity->GetAnimationController()->UpdateAnimationClip(ANIMATION_TYPE_LOOP);
}

void CPlayerRunningState::Exit(const shared_ptr<CPlayer>& Entity)
{

}

//=========================================================================================================================

CPlayerPunchingState* CPlayerPunchingState::GetInstance()
{
	static CPlayerPunchingState Instance{};

	return &Instance;
}

void CPlayerPunchingState::Enter(const shared_ptr<CPlayer>& Entity)
{
	Entity->SetSpeed(0.0f);
	Entity->SetMovingDirection(XMFLOAT3(0.0f, 0.0f, 0.0f));
	Entity->SetAnimationClip(7);
}

void CPlayerPunchingState::ProcessInput(const shared_ptr<CPlayer>& Entity, UINT InputMask, float ElapsedTime)
{

}

void CPlayerPunchingState::Update(const shared_ptr<CPlayer>& Entity, float ElapsedTime)
{
	if (Entity->GetAnimationController()->UpdateAnimationClip(ANIMATION_TYPE_ONCE))
	{
		Entity->GetStateMachine()->ChangeState(CPlayerIdleState::GetInstance());
	}
}

void CPlayerPunchingState::Exit(const shared_ptr<CPlayer>& Entity)
{

}

//=========================================================================================================================

CPlayerShootingState* CPlayerShootingState::GetInstance()
{
	static CPlayerShootingState Instance{};

	return &Instance;
}

void CPlayerShootingState::Enter(const shared_ptr<CPlayer>& Entity)
{
	Entity->SetSpeed(0.0f);
	Entity->SetMovingDirection(XMFLOAT3(0.0f, 0.0f, 0.0f));
	Entity->GetCamera()->SetZoomIn(true);
}

void CPlayerShootingState::ProcessInput(const shared_ptr<CPlayer>& Entity, UINT InputMask, float ElapsedTime)
{
	switch (InputMask)
	{
	case INPUT_MASK_LMB | INPUT_MASK_RMB:
		Entity->GetAnimationController()->SetAnimationClip(9);
		break;
	}

	if (InputMask & INPUT_MASK_RMB)
	{
		Entity->GetCamera()->IncreaseZoomFactor(ElapsedTime);
	}
	else
	{
		Entity->GetStateMachine()->ChangeState(CPlayerIdleState::GetInstance());
	}
}

void CPlayerShootingState::Update(const shared_ptr<CPlayer>& Entity, float ElapsedTime)
{
	switch (Entity->GetAnimationController()->GetAnimationClip())
	{
	case 8:
		Entity->GetAnimationController()->UpdateAnimationClip(ANIMATION_TYPE_LOOP);
		break;
	case 9:
		if (Entity->GetAnimationController()->UpdateAnimationClip(ANIMATION_TYPE_ONCE))
		{ 
			Entity->GetAnimationController()->SetAnimationClip(8);
		}
		break;
	}
}

void CPlayerShootingState::Exit(const shared_ptr<CPlayer>& Entity)
{
	Entity->GetCamera()->SetZoomIn(false);
}

//=========================================================================================================================

CPlayerDyingState* CPlayerDyingState::GetInstance()
{
	static CPlayerDyingState Instance{};

	return &Instance;
}

void CPlayerDyingState::Enter(const shared_ptr<CPlayer>& Entity)
{

}

void CPlayerDyingState::ProcessInput(const shared_ptr<CPlayer>& Entity, UINT InputMask, float ElapsedTime)
{

}

void CPlayerDyingState::Update(const shared_ptr<CPlayer>& Entity, float ElapsedTime)
{

}

void CPlayerDyingState::Exit(const shared_ptr<CPlayer>& Entity)
{

}
