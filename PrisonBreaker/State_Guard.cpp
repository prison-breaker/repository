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

void CGuardIdleState::ProcessInput(const shared_ptr<CGuard>& Entity, const vector<vector<shared_ptr<CGameObject>>>& GameObjects, const shared_ptr<CNavMesh>& NavMesh, float ElapsedTime, UINT InputMask)
{

}

void CGuardIdleState::Update(const shared_ptr<CGuard>& Entity, const vector<vector<shared_ptr<CGameObject>>>& GameObjects, const shared_ptr<CNavMesh>& NavMesh, float ElapsedTime)
{
	shared_ptr<CGameObject> NearestPlayer{ Entity->IsFoundPlayer(GameObjects) };

	if (NearestPlayer)
	{
		Entity->FindNavPath(NavMesh, NearestPlayer->GetPosition(), GameObjects);
		Entity->SetTarget(NearestPlayer);
		Entity->GetStateMachine()->ChangeState(CGuardChaseState::GetInstance());
	}
	else
	{
		// 애니메이션이 끝나면 Patrol 상태로 변경한다.
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
	XMFLOAT3 Direction{ Vector3::Normalize(Vector3::Subtract(Entity->GetPatrolNavPath().back(), Entity->GetPosition())) };

	Entity->UpdateLocalCoord(Direction);
	Entity->SetElapsedTime(0.0f);
	Entity->SetRecentTransition(false);
	Entity->SetAnimationClip(1);
	Entity->SetSpeed(5.0f);
}

void CGuardPatrolState::ProcessInput(const shared_ptr<CGuard>& Entity, const vector<vector<shared_ptr<CGameObject>>>& GameObjects, const shared_ptr<CNavMesh>& NavMesh, float ElapsedTime, UINT InputMask)
{

}

void CGuardPatrolState::Update(const shared_ptr<CGuard>& Entity, const vector<vector<shared_ptr<CGameObject>>>& GameObjects, const shared_ptr<CNavMesh>& NavMesh, float ElapsedTime)
{
	shared_ptr<CGameObject> NearestPlayer{ Entity->IsFoundPlayer(GameObjects) };

	if (NearestPlayer)
	{
		Entity->FindNavPath(NavMesh, NearestPlayer->GetPosition(), GameObjects);
		Entity->SetTarget(NearestPlayer);
		Entity->GetStateMachine()->ChangeState(CGuardChaseState::GetInstance());
	}
	else
	{
		// ToIdleEntryTime이 된다면 IdleState로 전이한다.
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

void CGuardChaseState::ProcessInput(const shared_ptr<CGuard>& Entity, const vector<vector<shared_ptr<CGameObject>>>& GameObjects, const shared_ptr<CNavMesh>& NavMesh, float ElapsedTime, UINT InputMask)
{

}

void CGuardChaseState::Update(const shared_ptr<CGuard>& Entity, const vector<vector<shared_ptr<CGameObject>>>& GameObjects, const shared_ptr<CNavMesh>& NavMesh, float ElapsedTime)
{
	shared_ptr<CGameObject> NearestPlayer{ Entity->IsFoundPlayer(GameObjects) };
	XMFLOAT3 Direction{};

	if (NearestPlayer)
	{
		Entity->SetTarget(NearestPlayer);
	}

	shared_ptr<CGameObject> Target{ Entity->GetTarget() };

	if (!NearestPlayer && Entity->GetRecentTransition())
	{
		// 플레이어가 시야각에서 보이지 않는다면 ReturnState로 전이하여 원래 순찰하는 위치로 돌아간다.
		Entity->FindNavPath(NavMesh, Entity->GetPatrolNavPath()[Entity->GetPatrolIndex()], GameObjects);
		Entity->GetStateMachine()->ChangeState(CGuardReturnState::GetInstance());
		return;
	}
	else if (Target)
	{
		if (Math::Distance(Target->GetPosition(), Entity->GetPosition()) < 10.0f)
		{ 
			// 플레이어와 일정거리 이하가 되면 RayCasting을 하여 차폐를 파악한 후 총을 쏜다.
			Direction = Vector3::Normalize(Vector3::Subtract(Target->GetPosition(), Entity->GetPosition()));

			float NearestHitDistance{ FLT_MAX };
			float HitDistance{};
			bool IsHit{};

			for (const auto& GameObject : GameObjects[OBJECT_TYPE_STRUCTURE])
			{
				if (GameObject)
				{
					shared_ptr<CGameObject> IntersectedObject{ GameObject->PickObjectByRayIntersection(Entity->GetPosition(), Direction, HitDistance, 10.0f) };

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
			// 3초에 한번씩 혹은 NavPath가 비었을 경우 NavPath를 갱신한다.
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

void CGuardReturnState::ProcessInput(const shared_ptr<CGuard>& Entity, const vector<vector<shared_ptr<CGameObject>>>& GameObjects, const shared_ptr<CNavMesh>& NavMesh, float ElapsedTime, UINT InputMask)
{

}

void CGuardReturnState::Update(const shared_ptr<CGuard>& Entity, const vector<vector<shared_ptr<CGameObject>>>& GameObjects, const shared_ptr<CNavMesh>& NavMesh, float ElapsedTime)
{
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

		// 원래 포지션으로 돌아갔다면 PatrolState로 전이한다.
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

void CGuardAssembleState::ProcessInput(const shared_ptr<CGuard>& Entity, const vector<vector<shared_ptr<CGameObject>>>& GameObjects, const shared_ptr<CNavMesh>& NavMesh, float ElapsedTime, UINT InputMask)
{

}

void CGuardAssembleState::Update(const shared_ptr<CGuard>& Entity, const vector<vector<shared_ptr<CGameObject>>>& GameObjects, const shared_ptr<CNavMesh>& NavMesh, float ElapsedTime)
{
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

void CGuardShootingState::ProcessInput(const shared_ptr<CGuard>& Entity, const vector<vector<shared_ptr<CGameObject>>>& GameObjects, const shared_ptr<CNavMesh>& NavMesh, float ElapsedTime, UINT InputMask)
{

}

void CGuardShootingState::Update(const shared_ptr<CGuard>& Entity, const vector<vector<shared_ptr<CGameObject>>>& GameObjects, const shared_ptr<CNavMesh>& NavMesh, float ElapsedTime)
{	
	// 총을 쏘면 다시 ChaseState로 전이한다.
	if (Entity->GetAnimationController()->UpdateAnimationClip(ANIMATION_TYPE_ONCE))
	{
		shared_ptr<CGameObject> Target{ Entity->GetTarget() };

		if (Math::Distance(Target->GetPosition(), Entity->GetPosition()) < 10.0f)
		{
			// 플레이어와 일정거리 이하가 되면 RayCasting을 하여 차폐를 파악한 후 총을 쏜다.
			XMFLOAT3 Direction{ Vector3::Normalize(Vector3::Subtract(Target->GetPosition(), Entity->GetPosition())) };

			float NearestHitDistance{ FLT_MAX };
			float HitDistance{};
			bool IsHit{};

			for (const auto& GameObject : GameObjects[OBJECT_TYPE_STRUCTURE])
			{
				if (GameObject)
				{
					shared_ptr<CGameObject> IntersectedObject{ GameObject->PickObjectByRayIntersection(Entity->GetPosition(), Direction, HitDistance, 10.0f) };

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
				Entity->GetAnimationController()->SetKeyFrameIndex(0);
				return;
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

void CGuardHitState::ProcessInput(const shared_ptr<CGuard>& Entity, const vector<vector<shared_ptr<CGameObject>>>& GameObjects, const shared_ptr<CNavMesh>& NavMesh, float ElapsedTime, UINT InputMask)
{

}

void CGuardHitState::Update(const shared_ptr<CGuard>& Entity, const vector<vector<shared_ptr<CGameObject>>>& GameObjects, const shared_ptr<CNavMesh>& NavMesh, float ElapsedTime)
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
}

void CGuardDyingState::ProcessInput(const shared_ptr<CGuard>& Entity, const vector<vector<shared_ptr<CGameObject>>>& GameObjects, const shared_ptr<CNavMesh>& NavMesh, float ElapsedTime, UINT InputMask)
{

}

void CGuardDyingState::Update(const shared_ptr<CGuard>& Entity, const vector<vector<shared_ptr<CGameObject>>>& GameObjects, const shared_ptr<CNavMesh>& NavMesh, float ElapsedTime)
{
	// 교도관이 사망하면 오브젝트를 비활성화한다.
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
