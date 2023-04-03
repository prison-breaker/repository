#include "pch.h"
#include "State_Background.h"
#include "UIAnimationController.h"
#include "UIObjects.h"

CBackgroundUIFadeState* CBackgroundUIFadeState::GetInstance()
{
	static CBackgroundUIFadeState Instance{};

	return &Instance;
}

void CBackgroundUIFadeState::Enter(const shared_ptr<CBackgroundUI>& Entity)
{

}

void CBackgroundUIFadeState::ProcessInput(const shared_ptr<CBackgroundUI>& Entity, float ElapsedTime, UINT InputMask)
{

}

void CBackgroundUIFadeState::Update(const shared_ptr<CBackgroundUI>& Entity, float ElapsedTime)
{
	Entity->GetAnimationController()->UpdateAnimationClip(ANIMATION_TYPE_LOOP);
}

void CBackgroundUIFadeState::Exit(const shared_ptr<CBackgroundUI>& Entity)
{

}
