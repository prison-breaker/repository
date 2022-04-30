#include "stdafx.h"
#include "State_Guard.h"
#include "GameScene.h"

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

void CGuardIdleState::ProcessInput(const shared_ptr<CGuard>& Entity, float ElapsedTime, UINT InputMask)
{

}

void CGuardIdleState::Update(const shared_ptr<CGuard>& Entity, float ElapsedTime)
{
	auto GameScene{ static_pointer_cast<CGameScene>(CSceneManager::GetInstance()->GetCurrentScene()) };
	auto GameObjects{ GameScene->GetGameObjects() };

	shared_ptr<CGameObject> NearestPlayer{ Entity->IsFoundPlayer(GameObjects) };

	if (NearestPlayer)
	{
		auto NavMesh{ GameScene->GetNavMesh() };

		Entity->FindNavPath(NavMesh, NearestPlayer->GetPosition(), GameObjects);
		Entity->SetTarget(NearestPlayer);
		Entity->GetStateMachine()->ChangeState(CGuardChaseState::GetInstance());
	}
	else
	{
		// �ִϸ��̼��� ������ Patrol ���·� �����Ѵ�.
		if (Entity->GetAnimationController()->UpdateAnimationClip(ANIMATION_TYPE_ONCE))
		{
			Entity->GetStateMachine()->ChangeState(CGuardPatrolState::GetInstance());
		}
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
	XMFLOAT3 Direction{ Vector3::Normalize(Vector3::Subtract(Entity->GetPatrolNavPath()[Entity->GetPatrolIndex()], Entity->GetPosition())) };

	Entity->UpdateLocalCoord(Direction);
	Entity->SetElapsedTime(0.0f);
	Entity->SetRecentTransition(false);
	Entity->SetAnimationClip(1);
	Entity->SetSpeed(5.0f);
}

void CGuardPatrolState::ProcessInput(const shared_ptr<CGuard>& Entity, float ElapsedTime, UINT InputMask)
{

}

void CGuardPatrolState::Update(const shared_ptr<CGuard>& Entity, float ElapsedTime)
{
	auto GameScene{ static_pointer_cast<CGameScene>(CSceneManager::GetInstance()->GetCurrentScene()) };
	auto GameObjects{ GameScene->GetGameObjects() };

	shared_ptr<CGameObject> NearestPlayer{ Entity->IsFoundPlayer(GameObjects) };

	if (NearestPlayer)
	{
		auto NavMesh{ GameScene->GetNavMesh() };

		Entity->FindNavPath(NavMesh, NearestPlayer->GetPosition(), GameObjects);
		Entity->SetTarget(NearestPlayer);
		Entity->GetStateMachine()->ChangeState(CGuardChaseState::GetInstance());
	}
	else
	{
		// ToIdleEntryTime�� �ȴٸ� IdleState�� �����Ѵ�.
		Entity->SetElapsedTime(Entity->GetElapsedTime() + ElapsedTime);

		if (Entity->GetElapsedTime() > Entity->GetToIdleEntryTime())
		{
			Entity->GetStateMachine()->ChangeState(CGuardIdleState::GetInstance());
		}
		else
		{
			Entity->GetAnimationController()->UpdateAnimationClip(ANIMATION_TYPE_LOOP);
			Entity->Patrol(ElapsedTime);
		}
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

void CGuardChaseState::ProcessInput(const shared_ptr<CGuard>& Entity, float ElapsedTime, UINT InputMask)
{

}

void CGuardChaseState::Update(const shared_ptr<CGuard>& Entity, float ElapsedTime)
{
	auto GameScene{ static_pointer_cast<CGameScene>(CSceneManager::GetInstance()->GetCurrentScene()) };
	auto GameObjects{ GameScene->GetGameObjects() };
	auto NavMesh{ GameScene->GetNavMesh() };

	shared_ptr<CGameObject> NearestPlayer{ Entity->IsFoundPlayer(GameObjects) };
	XMFLOAT3 Direction{};

	if (NearestPlayer)
	{
		Entity->SetTarget(NearestPlayer);
	}

	shared_ptr<CGameObject> Target{ Entity->GetTarget() };

	if (!NearestPlayer && Entity->GetRecentTransition())
	{
		// �÷��̾ �þ߰����� ������ �ʴ´ٸ� ReturnState�� �����Ͽ� ���� �����ϴ� ��ġ�� ���ư���.
		Entity->FindNavPath(NavMesh, Entity->GetPatrolNavPath()[Entity->GetPatrolIndex()], GameObjects);
		Entity->GetStateMachine()->ChangeState(CGuardReturnState::GetInstance());
		return;
	}
	else if (Target)
	{
		if (Math::Distance(Target->GetPosition(), Entity->GetPosition()) < 10.0f)
		{ 
			// �÷��̾�� �����Ÿ� ���ϰ� �Ǹ� RayCasting�� �Ͽ� ���� �ľ��� �� ���� ���.
			Direction = Vector3::Normalize(Vector3::Subtract(Target->GetPosition(), Entity->GetPosition()));

			float NearestHitDistance{ FLT_MAX };
			float HitDistance{};
			bool IsHit{};

			for (const auto& GameObject : GameObjects[OBJECT_TYPE_STRUCTURE])
			{
				if (GameObject)
				{
					XMFLOAT3 RayOrigin{ Entity->GetPosition() };
					RayOrigin.y = 3.0f;

					shared_ptr<CGameObject> IntersectedObject{ GameObject->PickObjectByRayIntersection(RayOrigin, Direction, HitDistance, 10.0f) };

					if (IntersectedObject && HitDistance < 10.0f)
					{
						IsHit = true;
						break;
					}
				}
			}

			if (!IsHit)
			{
				Entity->UpdateLocalCoord(Direction);
				Entity->GetStateMachine()->ChangeState(CGuardShootingState::GetInstance());
				return;
			}
		}

		if (Entity->GetElapsedTime() > Entity->GetUpdateTargetTime() || Entity->GetNavPath().empty())
		{
			// 3�ʿ� �ѹ��� Ȥ�� NavPath�� ����� ��� NavPath�� �����Ѵ�.
			Entity->FindNavPath(NavMesh, Target->GetPosition(), GameObjects);

			Direction = Vector3::Normalize(Vector3::Subtract(Entity->GetNavPath().back(), Entity->GetPosition()));

			Entity->UpdateLocalCoord(Direction);
			Entity->SetElapsedTime(0.0f);
		}
	}

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

void CGuardReturnState::ProcessInput(const shared_ptr<CGuard>& Entity, float ElapsedTime, UINT InputMask)
{

}

void CGuardReturnState::Update(const shared_ptr<CGuard>& Entity, float ElapsedTime)
{
	auto GameScene{ static_pointer_cast<CGameScene>(CSceneManager::GetInstance()->GetCurrentScene()) };
	auto GameObjects{ GameScene->GetGameObjects() };

	shared_ptr<CGameObject> NearestPlayer{ Entity->IsFoundPlayer(GameObjects) };

	if (NearestPlayer)
	{
		auto NavMesh{ GameScene->GetNavMesh() };

		Entity->FindNavPath(NavMesh, NearestPlayer->GetPosition(), GameObjects);
		Entity->SetTarget(NearestPlayer);
		Entity->GetStateMachine()->ChangeState(CGuardChaseState::GetInstance());
	}
	else
	{
		Entity->MoveToNavPath(ElapsedTime);

		// ���� ���������� ���ư��ٸ� PatrolState�� �����Ѵ�.
		if (Entity->GetNavPath().empty())
		{
			Entity->GetStateMachine()->ChangeState(CGuardPatrolState::GetInstance());
		}
		else
		{
			Entity->GetAnimationController()->UpdateAnimationClip(ANIMATION_TYPE_LOOP);
		}
	}
}

void CGuardReturnState::Exit(const shared_ptr<CGuard>& Entity)
{

}

// ========================================================================================================================

CGuardAssembleState* CGuardAssembleState::GetInstance()
{
	static CGuardAssembleState Instance{};

	return &Instance;
}

void CGuardAssembleState::Enter(const shared_ptr<CGuard>& Entity)
{
	XMFLOAT3 Direction{ Vector3::Normalize(Vector3::Subtract(Entity->GetNavPath().back(), Entity->GetPosition())) };

	Entity->UpdateLocalCoord(Direction);
	Entity->SetElapsedTime(0.0f);
	Entity->SetRecentTransition(false);
	Entity->SetAnimationClip(2);
	Entity->SetSpeed(13.0f);
}

void CGuardAssembleState::ProcessInput(const shared_ptr<CGuard>& Entity, float ElapsedTime, UINT InputMask)
{

}

void CGuardAssembleState::Update(const shared_ptr<CGuard>& Entity, float ElapsedTime)
{
	auto GameScene{ static_pointer_cast<CGameScene>(CSceneManager::GetInstance()->GetCurrentScene()) };
	auto GameObjects{ GameScene->GetGameObjects() };
	auto NavMesh{ GameScene->GetNavMesh() };

	shared_ptr<CGameObject> NearestPlayer{ Entity->IsFoundPlayer(GameObjects) };

	if (NearestPlayer)
	{
		Entity->FindNavPath(NavMesh, NearestPlayer->GetPosition(), GameObjects);
		Entity->SetTarget(NearestPlayer);
		Entity->GetStateMachine()->ChangeState(CGuardChaseState::GetInstance());
	}
	else
	{
		Entity->MoveToNavPath(ElapsedTime);

		if (Entity->GetNavPath().empty())
		{
			Entity->FindNavPath(NavMesh, Entity->GetPatrolNavPath()[Entity->GetPatrolIndex()], GameObjects);
			Entity->GetStateMachine()->ChangeState(CGuardReturnState::GetInstance());
		}
		else
		{
			Entity->GetAnimationController()->UpdateAnimationClip(ANIMATION_TYPE_LOOP);
		}
	}
}

void CGuardAssembleState::Exit(const shared_ptr<CGuard>& Entity)
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
	Entity->SetSpeed(0.0f);
}

void CGuardShootingState::ProcessInput(const shared_ptr<CGuard>& Entity, float ElapsedTime, UINT InputMask)
{

}

void CGuardShootingState::Update(const shared_ptr<CGuard>& Entity, float ElapsedTime)
{	
	// ���� ��� �ٽ� ChaseState�� �����Ѵ�.
	if (Entity->GetAnimationController()->UpdateAnimationClip(ANIMATION_TYPE_ONCE))
	{
		shared_ptr<CGameObject> Target{ Entity->GetTarget() };

		if (Target->IsActive())
		{ 
			if (Math::Distance(Target->GetPosition(), Entity->GetPosition()) < 10.0f)
			{
				auto GameScene{ static_pointer_cast<CGameScene>(CSceneManager::GetInstance()->GetCurrentScene()) };
				auto GameObjects{ GameScene->GetGameObjects() };

				// �÷��̾�� �����Ÿ� ���ϰ� �Ǹ� RayCasting�� �Ͽ� ���� �ľ��� �� ���� ���.
				XMFLOAT3 Direction{ Vector3::Normalize(Vector3::Subtract(Target->GetPosition(), Entity->GetPosition())) };

				float NearestHitDistance{ FLT_MAX };
				float HitDistance{};
				bool IsHit{};

				for (const auto& GameObject : GameObjects[OBJECT_TYPE_STRUCTURE])
				{
					if (GameObject)
					{
						XMFLOAT3 RayOrigin{ Entity->GetPosition() };
						RayOrigin.y = 3.0f;

						shared_ptr<CGameObject> IntersectedObject{ GameObject->PickObjectByRayIntersection(RayOrigin, Direction, HitDistance, 10.0f) };

						if (IntersectedObject && HitDistance < 10.0f)
						{
							IsHit = true;
							break;
						}
					}
				}

				if (!IsHit)
				{
					// RayCasting ���� �� �¾Ҵٸ� ������ �ٽ� ������ �� ���� �ִϸ��̼��� �ϵ��� ��Ʈ�ѷ��� �ε����� 0���� �����.
					Entity->UpdateLocalCoord(Direction);
					Entity->GetAnimationController()->SetKeyFrameIndex(0);

					auto BilboardObjects{ GameScene->GetBilboardObjects() };

					// �ǰ� UI �ִϸ��̼��� �����Ű��, UI ü���� 1���ҽ�Ų��.
					static_pointer_cast<CHitUI>(BilboardObjects[BILBOARD_OBJECT_TYPE_UI][10])->GetStateMachine()->SetCurrentState(CHitUIActivationState::GetInstance());
					BilboardObjects[BILBOARD_OBJECT_TYPE_UI][3]->SetVertexCount(BilboardObjects[BILBOARD_OBJECT_TYPE_UI][3]->GetVertexCount() - 1);

					auto Player{ static_pointer_cast<CPlayer>(Target) };

					Player->SetHealth(Player->GetHealth() - 10);

					if (Player->GetHealth() <= 0)
					{
						auto NavMesh{ GameScene->GetNavMesh() };

						Player->GetStateMachine()->ChangeState(CPlayerDyingState::GetInstance());

						Entity->FindNavPath(NavMesh, Entity->GetPatrolNavPath()[Entity->GetPatrolIndex()], GameObjects);
						Entity->GetStateMachine()->ChangeState(CGuardReturnState::GetInstance());
						return;
					}
					else
					{
						return;
					}
				}
			}
		}
		
		Entity->GetStateMachine()->ChangeState(CGuardChaseState::GetInstance());
	}
}

void CGuardShootingState::Exit(const shared_ptr<CGuard>& Entity)
{

}

//=========================================================================================================================

CGuardHitState* CGuardHitState::GetInstance()
{
	static CGuardHitState Instance{};

	return &Instance;
}

void CGuardHitState::Enter(const shared_ptr<CGuard>& Entity)
{
	Entity->SetElapsedTime(0.0f);
	Entity->SetRecentTransition(false);
	Entity->SetAnimationClip(4);
	Entity->SetSpeed(0.0f);
	Entity->SetHealth(Entity->GetHealth() - 35);
}

void CGuardHitState::ProcessInput(const shared_ptr<CGuard>& Entity, float ElapsedTime, UINT InputMask)
{

}

void CGuardHitState::Update(const shared_ptr<CGuard>& Entity, float ElapsedTime)
{
	if (Entity->GetHealth() <= 0)
	{
		Entity->GetStateMachine()->ChangeState(CGuardDyingState::GetInstance());
		return;
	}

	if (Entity->GetAnimationController()->UpdateAnimationClip(ANIMATION_TYPE_ONCE))
	{
		Entity->GetStateMachine()->ChangeState(Entity->GetStateMachine()->GetPreviousState());
	}
}

void CGuardHitState::Exit(const shared_ptr<CGuard>& Entity)
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
	Entity->SetAnimationClip(5);
	Entity->SetSpeed(0.0f);

	//auto EventTriggers{ static_pointer_cast<CGameScene>(CSceneManager::GetInstance()->GetCurrentScene())->GetEventTriggers() };

	//for (const auto& EventTrigger : EventTriggers)
	//{
	//	if (EventTrigger)
	//	{
	//		// Entity�� �̺�Ʈ ��ü�� ���� �ִ� Ʈ���Ÿ� ã�� �������� ����� ��ġ�� Ʈ���Ű� �����ǵ��� ��ġ�� �������ش�.
	//		if (EventTrigger->GetEventObject(0) == Entity)
	//		{
	//			EventTrigger->CalculateTriggerAreaByPoint(Entity->GetPosition(), 6.0f, 6.0f);
	//			break;
	//		}
	//	}
	//}
}

void CGuardDyingState::ProcessInput(const shared_ptr<CGuard>& Entity, float ElapsedTime, UINT InputMask)
{

}

void CGuardDyingState::Update(const shared_ptr<CGuard>& Entity, float ElapsedTime)
{
	// �������� ����ϸ� ������Ʈ�� ��Ȱ��ȭ�Ѵ�.
	if (Entity->GetAnimationController()->UpdateAnimationClip(ANIMATION_TYPE_ONCE))
	{
		Entity->SetElapsedTime(Entity->GetElapsedTime() + ElapsedTime);
		
		if (Entity->GetElapsedTime() > 10.0f)
		{
			Entity->SetActive(false);
		}
	}
}

void CGuardDyingState::Exit(const shared_ptr<CGuard>& Entity)
{

}
