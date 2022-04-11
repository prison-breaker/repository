#include "stdafx.h"
#include "Guard.h"
#include "State_Guard.h"

void CGuard::Initialize()
{
	CGameObject::Initialize();

	// 상태머신 객체를 생성한다.
	m_StateMachine = make_shared<CStateMachine<CGuard>>(static_pointer_cast<CGuard>(shared_from_this()));
	
	switch (rand() % 5)
	{
	case 0:
		m_StateMachine->SetCurrentState(CGuardIdleState::GetInstance());
		break;
	case 1:
		m_StateMachine->SetCurrentState(CGuardPatrolState::GetInstance());
		break;
	case 2:
		m_StateMachine->SetCurrentState(CGuardChaseState::GetInstance());
		break;
	case 3:
		m_StateMachine->SetCurrentState(CGuardShootingState::GetInstance());
		break;
	case 4:
		m_StateMachine->SetCurrentState(CGuardDyingState::GetInstance());
		break;
	}
}

void CGuard::Animate(float ElapsedTime)
{
	if (IsActive())
	{
		if (m_StateMachine)
		{
			m_StateMachine->Update(ElapsedTime);
		}
	}
}

void CGuard::SetHealth(UINT Health)
{
	m_Health = Health;
}

UINT CGuard::GetHealth() const
{
	return m_Health;
}

void CGuard::SetSpeed(float Speed)
{
	m_Speed = Speed;
}

float CGuard::GetSpeed() const
{
	return m_Speed;
}

CCamera* CGuard::GetCamera() const
{
	return m_Camera.get();
}

CStateMachine<CGuard>* CGuard::GetStateMachine() const
{
	return m_StateMachine.get();
}

void CGuard::FindPath(const shared_ptr<CNavMesh>& NavMesh, const XMFLOAT3& TargetPosition)
{
	priority_queue<shared_ptr<CNavNode>, vector<shared_ptr<CNavNode>>, compare> NavNodeQueue{};

	UINT StartNavNodeIndex{ NavMesh->GetNodeIndex(GetPosition()) };
	UINT TargetNavNodeIndex{ NavMesh->GetNodeIndex(TargetPosition) };

	shared_ptr<CNavNode> StartNavNode{ NavMesh->GetNavNodes()[StartNavNodeIndex] };
	shared_ptr<CNavNode> TargetNavNode{ NavMesh->GetNavNodes()[TargetNavNodeIndex] };
	
	for (const auto& NavNode : NavMesh->GetNavNodes())
	{
		NavNode->Reset();
	}

	StartNavNode->CalculateF(StartNavNode, TargetNavNode);
	NavNodeQueue.push(StartNavNode);

	shared_ptr<CNavNode> CurrentNavNode{};
	UINT CurrentIndex{};

	while (true)
	{
		if (NavNodeQueue.empty())
		{
			//tcout << TEXT("목적지를 찾지 못했습니다.") << endl;
			break;
		}

		CurrentNavNode = NavNodeQueue.top();
		NavNodeQueue.pop();

		if (CurrentNavNode->IsVisited())
		{
			continue;
		}

		CurrentNavNode->SetVisit(true);

		if (Vector3::IsEqual(CurrentNavNode->GetTriangle().m_Centroid, TargetNavNode->GetTriangle().m_Centroid))
		{
			//tcout << TEXT("목적지를 찾았습니다.") << endl;
			break;
		}

		for (const auto& NeighborNavNode : CurrentNavNode->GetNeighborNavNodes())
		{
			if (!NeighborNavNode->IsVisited())
			{
				NeighborNavNode->SetParent(CurrentNavNode);
				NeighborNavNode->CalculateF(CurrentNavNode, TargetNavNode);
				NavNodeQueue.push(NeighborNavNode);
			}
		}
	}

	m_NavPath.clear();
	m_NavPath.push_back(TargetPosition);

	while (true)
	{
		if (CurrentNavNode->GetParent() == NULL)
		{
			break;
		}

		UINT NeighborSideIndex{ CurrentNavNode->CalculateNeighborSideIndex(CurrentNavNode->GetParent()) };

		m_NavPath.push_back(CurrentNavNode->GetTriangle().m_CenterSides[NeighborSideIndex]);		
		CurrentNavNode = CurrentNavNode->GetParent(); 
	}
}

void CGuard::MoveToNavPath(float ElapsedTime)
{
	if (!m_NavPath.empty())
	{
		XMFLOAT3 NextVertex = m_NavPath.back();
		XMFLOAT3 Direction{ Vector3::Normalize(Vector3::Subtract(NextVertex, GetPosition())) };

		const XMFLOAT3 WorldUp{ 0.0f, 1.0f, 0.0f };

		SetLook(Direction);
		SetRight(Vector3::CrossProduct(WorldUp, GetLook(), false));
		SetUp(Vector3::CrossProduct(GetLook(), GetRight(), false));

		if (Math::Distance(NextVertex, GetPosition()) < 1.0f)
		{
			m_NavPath.pop_back();
		}

		CGameObject::Move(Direction, 20.0f * ElapsedTime);
	}
}
