#include "stdafx.h"
#include "State_Player.h"
#include "GameScene.h"

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

void CPlayerIdleState::ProcessInput(const shared_ptr<CPlayer>& Entity, float ElapsedTime, UINT InputMask)
{
	if (InputMask & INPUT_MASK_LMB)
	{
		if (!Entity->IsEquippedPistol())
		{
			Entity->GetStateMachine()->ChangeState(CPlayerPunchingState::GetInstance());
		}
	}
	else if (InputMask & INPUT_MASK_RMB)
	{
		if (Entity->IsEquippedPistol())
		{
			Entity->SetAnimationClip(8);
			Entity->GetStateMachine()->ChangeState(CPlayerShootingState::GetInstance());
		}
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

void CPlayerWalkingState::ProcessInput(const shared_ptr<CPlayer>& Entity, float ElapsedTime, UINT InputMask)
{
	if (InputMask & INPUT_MASK_LMB)
	{
		if (!Entity->IsEquippedPistol())
		{
			Entity->GetStateMachine()->ChangeState(CPlayerPunchingState::GetInstance());
			return;
		}
	}
	else if (InputMask & INPUT_MASK_RMB)
	{
		if (Entity->IsEquippedPistol())
		{
			Entity->SetAnimationClip(8);
			Entity->GetStateMachine()->ChangeState(CPlayerShootingState::GetInstance());
			return;
		}
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

void CPlayerRunningState::ProcessInput(const shared_ptr<CPlayer>& Entity, float ElapsedTime, UINT InputMask)
{
	if (InputMask & INPUT_MASK_LMB)
	{
		if (!Entity->IsEquippedPistol())
		{
			Entity->GetStateMachine()->ChangeState(CPlayerPunchingState::GetInstance());
			return;
		}
	}
	else if (InputMask & INPUT_MASK_RMB)
	{
		if (Entity->IsEquippedPistol())
		{
			Entity->SetAnimationClip(8);
			Entity->GetStateMachine()->ChangeState(CPlayerShootingState::GetInstance());
			return;
		}
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

	auto GameObjects{ static_pointer_cast<CGameScene>(CSceneManager::GetInstance()->GetCurrentScene())->GetGameObjects() };

	for (const auto& GameObject : GameObjects[OBJECT_TYPE_NPC])
	{
		if (GameObject)
		{
			auto Guard{ static_pointer_cast<CGuard>(GameObject) };

			if (Guard->GetHealth() > 0)
			{
				XMFLOAT3 ToGuard{ Vector3::Subtract(Guard->GetPosition(), Entity->GetPosition()) };

				if ((Vector3::Length(ToGuard) < 3.0f) && (Vector3::Angle(Entity->GetLook(), Vector3::Normalize(ToGuard)) < 80.0f))
				{
					static_pointer_cast<CGuard>(Guard)->GetStateMachine()->ChangeState(CGuardHitState::GetInstance());
				}
			}
		}
	}
}

void CPlayerPunchingState::ProcessInput(const shared_ptr<CPlayer>& Entity, float ElapsedTime, UINT InputMask)
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

void CPlayerShootingState::ProcessInput(const shared_ptr<CPlayer>& Entity, float ElapsedTime, UINT InputMask)
{
	switch (InputMask)
	{
	case INPUT_MASK_LMB | INPUT_MASK_RMB:
	{
		auto GameScene{ static_pointer_cast<CGameScene>(CSceneManager::GetInstance()->GetCurrentScene()) };
		auto GameObjects{ GameScene->GetGameObjects() };
		auto BilboardObjects{ GameScene->GetBilboardObjects() };

		// 줌 애니메이션을 하는 상태이고, 보유한 총알이 1발 이상 존재해야 쏠 수 있다.
		if (Entity->GetAnimationController()->GetAnimationClip() == 8)
		{
			if (BilboardObjects[BILBOARD_OBJECT_TYPE_UI][7]->GetVertexCount() > 0)
			{
				shared_ptr<CGameObject> NearestIntersectedRootObject{};
				shared_ptr<CGameObject> NearestIntersectedObject{};

				float NearestHitDistance{ FLT_MAX };
				float HitDistance{};
				XMFLOAT3 RayOrigin{ Entity->GetCamera()->GetPosition() };
				XMFLOAT3 RayDirection{ Entity->GetCamera()->GetLook() };

				for (UINT i = OBJECT_TYPE_NPC; i <= OBJECT_TYPE_STRUCTURE; ++i)
				{
					for (const auto& GameObject : GameObjects[i])
					{
						if (GameObject)
						{
							if (GameObject->IsActive())
							{
								// 모델을 공유하기 때문에, 월드 변환 행렬을 객체마다 갱신시켜주어야 한다.
								shared_ptr<CAnimationController> AnimationController{ GameObject->GetAnimationController() };

								if (AnimationController)
								{
									AnimationController->UpdateShaderVariables();
								}

								shared_ptr<CGameObject> IntersectedObject{ GameObject->PickObjectByRayIntersection(RayOrigin, RayDirection, HitDistance, FLT_MAX) };

								//if (IntersectedObject)
								//{
								//	tcout << TEXT("광선을 맞은 객체명 : ") << IntersectedObject->GetName() << TEXT(" (거리 : ") << HitDistance << TEXT(")") << endl;
								//	tcout << TEXT("- 해당 객체의 위치 : ") << IntersectedObject->GetPosition().x << ", " << IntersectedObject->GetPosition().y << ", " << IntersectedObject->GetPosition().z << endl;
								//	tcout << TEXT("- 해당 객체 중심까지의 거리 : ") << Vector3::Length(Vector3::Subtract(IntersectedObject->GetPosition(), Entity->GetCamera()->GetPosition())) << endl;
								//}

								if (IntersectedObject && (HitDistance < NearestHitDistance))
								{
									NearestIntersectedRootObject = GameObject;
									NearestIntersectedObject = IntersectedObject;
									NearestHitDistance = HitDistance;
								}
							}
						}
					}
				}

				if (NearestIntersectedObject)
				{
					if (typeid(*NearestIntersectedRootObject) == typeid(CGuard))
					{
						shared_ptr<CGuard> Guard{ static_pointer_cast<CGuard>(NearestIntersectedRootObject) };

						if (!Guard->GetStateMachine()->IsInState(CGuardHitState::GetInstance()) &&
							!Guard->GetStateMachine()->IsInState(CGuardDyingState::GetInstance()))
						{
							Guard->GetStateMachine()->ChangeState(CGuardHitState::GetInstance());
						}
					}

					//tcout << TEXT("★ 가장 먼저 광선을 맞은 객체명 : ") << NearestIntersectedObject->GetName() << TEXT(" (거리 : ") << NearestHitDistance << TEXT(")") << endl << endl;
				}

				Entity->GetAnimationController()->SetAnimationClip(9);

				// 총알 UI의 총알 개수를 한 개 감소시킨다.
				BilboardObjects[BILBOARD_OBJECT_TYPE_UI][7]->SetVertexCount(BilboardObjects[BILBOARD_OBJECT_TYPE_UI][7]->GetVertexCount() - 1);

				CSoundManager::GetInstance()->Play(SOUND_TYPE_PISTOL_SHOT, 0.6f);
				break;
			}
			else
			{
				CSoundManager::GetInstance()->Play(SOUND_TYPE_PISTOL_EMPTY, 0.6f);
			}
		}
	}
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
	Entity->SetSpeed(0.0f);
	Entity->SetMovingDirection(XMFLOAT3(0.0f, 0.0f, 0.0f));
	Entity->SetAnimationClip(10);
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
