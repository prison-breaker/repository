#include "stdafx.h"
#include "Guard.h"

void CGuard::Initialize()
{
	CGameObject::Initialize();

	// ���¸ӽ� ��ü�� �����Ѵ�.
	m_StateMachine = make_shared<CStateMachine<CGuard>>(static_pointer_cast<CGuard>(shared_from_this()));
	m_StateMachine->SetCurrentState(CGuardPatrolState::GetInstance());
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

void CGuard::SetMovingDirection(const XMFLOAT3& MovingDirection)
{
	m_MovingDirection = MovingDirection;
}

const XMFLOAT3& CGuard::GetMovingDirection() const
{
	return m_MovingDirection;
}

CStateMachine<CGuard>* CGuard::GetStateMachine() const
{
	return m_StateMachine.get();
}

void CGuard::SetTargetPosition(const XMFLOAT3& TargetPosition)
{
	m_TargetPosition = TargetPosition;
}

const XMFLOAT3& CGuard::GetTargetPosition() const
{
	return m_TargetPosition;
}

bool CGuard::IsFoundPlayer(const XMFLOAT3& Position)
{
	XMFLOAT3 ToPlayer{ Vector3::Subtract(Position, GetPosition()) };

	if (Vector3::Length(ToPlayer) < 20.0f)
	{
		float BetweenDegree{ Vector3::Angle(Vector3::Normalize(GetLook()), Vector3::Normalize(ToPlayer)) };

		if (BetweenDegree < 40.0f)
		{
			return true;
		}
	}

	return false;
}

void CGuard::FindNavPath(const shared_ptr<CNavMesh>& NavMesh, const vector<vector<shared_ptr<CGameObject>>>& GameObjects)
{
	priority_queue<shared_ptr<CNavNode>, vector<shared_ptr<CNavNode>>, compare> NavNodeQueue{};

	UINT StartNavNodeIndex{ NavMesh->GetNodeIndex(GetPosition()) };
	UINT TargetNavNodeIndex{ NavMesh->GetNodeIndex(GameObjects[OBJECT_TYPE_PLAYER].back()->GetPosition()) };

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
			//tcout << TEXT("�������� ã�� ���߽��ϴ�.") << endl;
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
			//tcout << TEXT("�������� ã�ҽ��ϴ�.") << endl;
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
	m_NavPath.push_back(GameObjects[OBJECT_TYPE_PLAYER].back()->GetPosition());

	while (true)
	{
		if (!CurrentNavNode->GetParent())
		{
			m_NavPath.push_back(GetPosition());
			break;
		}

		UINT NeighborSideIndex{ CurrentNavNode->CalculateNeighborSideIndex(CurrentNavNode->GetParent()) };

		m_NavPath.push_back(CurrentNavNode->GetTriangle().m_CenterSides[NeighborSideIndex]);
		CurrentNavNode = CurrentNavNode->GetParent();
	}

	// ��θ� ������ٸ�, ��带 ��ȸ�ϸ� ������ �� �� ����ȭ�� ��θ� ���Ѵ�.
	FindRayCastingNavPath(GameObjects);

	m_MovingDirection = Vector3::Normalize(Vector3::Subtract(m_NavPath.back(), GetPosition()));

	const XMFLOAT3 WorldUp{ 0.0f, 1.0f, 0.0f };

	SetLook(m_MovingDirection);
	SetRight(Vector3::CrossProduct(WorldUp, GetLook(), false));
	SetUp(Vector3::CrossProduct(GetLook(), GetRight(), false));
}

void CGuard::FindRayCastingNavPath(const vector<vector<shared_ptr<CGameObject>>>& GameObjects)
{
	vector<XMFLOAT3> RayCastingNavPath{};

	shared_ptr<CGameObject> NearestIntersectedObject{};
	UINT NavPathSize{ static_cast<UINT>(m_NavPath.size()) };

	RayCastingNavPath.push_back(m_NavPath.front());

	for (UINT SearchIndex = 0; SearchIndex < NavPathSize - 1; ++SearchIndex)
	{
		XMFLOAT3 SearchPosition{ m_NavPath[SearchIndex] };

		for (UINT CheckIndex = NavPathSize - 1; CheckIndex > SearchIndex; --CheckIndex)
		{
			XMFLOAT3 CheckPosition{ m_NavPath[CheckIndex] };

			// �ٿ�� �ڽ��� �浹�� �Ͼ�� ���� �ּ� �����̴�.
			SearchPosition.y = CheckPosition.y = 3.0f;

			// Normalize ���ϴϱ� ������? �ݴ� ������ �� �ȵǴ���?
			XMFLOAT3 LookDirection{ Vector3::Normalize(Vector3::Subtract(SearchPosition, CheckPosition)) }; 
			float NearestHitDistance{ FLT_MAX };
			float HitDistance{};

			for (const auto& GameObject : GameObjects[OBJECT_TYPE_STRUCTURE])
			{
				if (GameObject)
				{
					shared_ptr<CGameObject> IntersectedObject{ GameObject->PickObjectByRayIntersection(CheckPosition, LookDirection, HitDistance) };

					if (IntersectedObject && (HitDistance < NearestHitDistance))
					{
						NearestIntersectedObject = IntersectedObject;
						NearestHitDistance = HitDistance;
					}
				}
			}

			// �ٿ���ڽ� ���� �־� ������ ���� ��� ���� ��θ� ����ϵ��� �����Ѵ�.
			if (NearestHitDistance < 0)
			{
				return;
			}

			float Distance{ Math::Distance(CheckPosition, SearchPosition) };

			if (Distance <= NearestHitDistance)
			{
				RayCastingNavPath.push_back(m_NavPath[CheckIndex]);
				SearchIndex = CheckIndex - 1;
				break;
			}
		}
	}

	m_NavPath.clear();
	m_NavPath = RayCastingNavPath;
}

void CGuard::FindPatrolNavPath(const shared_ptr<CNavMesh>& NavMesh)
{
	priority_queue<shared_ptr<CNavNode>, vector<shared_ptr<CNavNode>>, compare> NavNodeQueue{};

	UINT StartNavNodeIndex{ NavMesh->GetNodeIndex(GetPosition()) };
	UINT TargetNavNodeIndex{ NavMesh->GetNodeIndex(GetTargetPosition()) };

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
			//tcout << TEXT("�������� ã�� ���߽��ϴ�.") << endl;
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
			//tcout << TEXT("�������� ã�ҽ��ϴ�.") << endl;
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

	m_PatrolNavPath.clear();
	m_PatrolNavPath.push_back(GetTargetPosition());

	while (true)
	{
		if (!CurrentNavNode->GetParent())
		{
			m_PatrolNavPath.push_back(GetPosition());
			break;
		}

		UINT NeighborSideIndex{ CurrentNavNode->CalculateNeighborSideIndex(CurrentNavNode->GetParent()) };

		m_PatrolNavPath.push_back(CurrentNavNode->GetTriangle().m_CenterSides[NeighborSideIndex]);
		CurrentNavNode = CurrentNavNode->GetParent();
	}

	reverse(m_PatrolNavPath.begin(), m_PatrolNavPath.end());

	m_MovingDirection = Vector3::Normalize(Vector3::Subtract(m_PatrolNavPath.front(), GetPosition()));

	const XMFLOAT3 WorldUp{ 0.0f, 1.0f, 0.0f };

	SetLook(m_MovingDirection);
	SetRight(Vector3::CrossProduct(WorldUp, GetLook(), false));
	SetUp(Vector3::CrossProduct(GetLook(), GetRight(), false));
}

void CGuard::MoveToNavPath(float ElapsedTime)
{
	if (!m_NavPath.empty())
	{
		if (Math::Distance(m_NavPath.back(), GetPosition()) < 1.0f)
		{
			m_NavPath.pop_back();

			if (!m_NavPath.empty())
			{
				m_MovingDirection = Vector3::Normalize(Vector3::Subtract(m_NavPath.back(), GetPosition()));

				const XMFLOAT3 WorldUp{ 0.0f, 1.0f, 0.0f };

				SetLook(m_MovingDirection);
				SetRight(Vector3::CrossProduct(WorldUp, GetLook(), false));
				SetUp(Vector3::CrossProduct(GetLook(), GetRight(), false));
			}
		}
		else
		{
			CGameObject::Move(m_MovingDirection, m_Speed * ElapsedTime);
		}
	}
}

void CGuard::Patrol(float ElapsedTime)
{
	if (Math::Distance(m_PatrolNavPath[m_PatrolIndex], GetPosition()) < 1.0f)
	{
		m_PatrolIndex += 1;

		if (m_PatrolIndex == m_PatrolNavPath.size())
		{
			m_PatrolIndex = 0;

			reverse(m_PatrolNavPath.begin(), m_PatrolNavPath.end());
		}

		m_MovingDirection = Vector3::Normalize(Vector3::Subtract(m_PatrolNavPath[m_PatrolIndex], GetPosition()));

		const XMFLOAT3 WorldUp{ 0.0f, 1.0f, 0.0f };

		SetLook(m_MovingDirection);
		SetRight(Vector3::CrossProduct(WorldUp, GetLook(), false));
		SetUp(Vector3::CrossProduct(GetLook(), GetRight(), false));
	}
	else
	{
		CGameObject::Move(m_MovingDirection, m_Speed * ElapsedTime);
	}
}
