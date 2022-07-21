#include "stdafx.h"
#include "State_Player.h"
#include "GameScene.h"
#include "Framework.h"

CPlayerIdleState* CPlayerIdleState::GetInstance()
{
	static CPlayerIdleState Instance{};

	return &Instance;
}

void CPlayerIdleState::Enter(const shared_ptr<CPlayer>& Entity)
{
	Entity->SetSpeed(0.0f);
	Entity->SetMovingDirection(XMFLOAT3(0.0f, 0.0f, 0.0f));
}

void CPlayerIdleState::ProcessInput(const shared_ptr<CPlayer>& Entity, float ElapsedTime, UINT InputMask)
{

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
	Entity->SetSpeed(3.15f);
}

void CPlayerWalkingState::ProcessInput(const shared_ptr<CPlayer>& Entity, float ElapsedTime, UINT InputMask)
{
	
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

void CPlayerRunningState::ProcessInput(const shared_ptr<CPlayer>& Entity, float ElapsedTime, UINT InputMask)
{

}

void CPlayerRunningState::Update(const shared_ptr<CPlayer>& Entity, float ElapsedTime)
{
	if (CFramework::GetInstance()->GetSocketInfo().m_ID == Entity->GetID()) 
	{
		Entity->GetAnimationController()->UpdateAnimationClip(ANIMATION_TYPE_LOOP, SOUND_TYPE_BREATH, 0.8f, 0.96f, ElapsedTime);
	}
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
}

void CPlayerPunchingState::ProcessInput(const shared_ptr<CPlayer>& Entity, float ElapsedTime, UINT InputMask)
{

}

void CPlayerPunchingState::Update(const shared_ptr<CPlayer>& Entity, float ElapsedTime)
{
	Entity->GetAnimationController()->UpdateAnimationClip(ANIMATION_TYPE_ONCE);
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

void CPlayerShootingState::ProcessInput(const shared_ptr<CPlayer>& Entity, float ElapsedTime, UINT InputMask)
{

}

void CPlayerShootingState::Update(const shared_ptr<CPlayer>& Entity, float ElapsedTime)
{
	switch (Entity->GetAnimationController()->GetAnimationClipType())
	{
	case ANIMATION_CLIP_TYPE_PLAYER_PISTOL_IDLE:
		Entity->GetAnimationController()->UpdateAnimationClip(ANIMATION_TYPE_LOOP);
		Entity->GetCamera()->IncreaseZoomFactor(ElapsedTime);
		break;
	case ANIMATION_CLIP_TYPE_PLAYER_SHOOT:
		Entity->GetAnimationController()->UpdateAnimationClip(ANIMATION_TYPE_ONCE);
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
	Entity->SetSpeed(0.0f);
	Entity->SetMovingDirection(XMFLOAT3(0.0f, 0.0f, 0.0f));
	Entity->SetHealth(0);
}

void CPlayerDyingState::ProcessInput(const shared_ptr<CPlayer>& Entity, float ElapsedTime, UINT InputMask)
{

}

void CPlayerDyingState::Update(const shared_ptr<CPlayer>& Entity, float ElapsedTime)
{
	Entity->GetAnimationController()->UpdateAnimationClip(ANIMATION_TYPE_ONCE);
}

void CPlayerDyingState::Exit(const shared_ptr<CPlayer>& Entity)
{

}
