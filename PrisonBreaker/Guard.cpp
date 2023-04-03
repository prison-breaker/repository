#include "pch.h"
#include "Guard.h"

#include "TimeManager.h"
#include "AssetManager.h"
#include "SceneManager.h"

#include "Scene.h"

#include "RigidBody.h"
#include "StateMachine.h"

#include "NavMesh.h"
#include "NavNode.h"

#include "GuardStates.h"

CGuard::CGuard() :
	m_idleEntryTime(Random::Range(7.0f, 10.0f)),
	m_targetUpdateTime(3.0f),
	m_elapsedTime(),
	m_movePath(),
	m_patrolPath(),
	m_patrolIndex(),
	m_target()
{
}

CGuard::~CGuard()
{
}

float CGuard::GetIdleEntryTime()
{
	return m_idleEntryTime;
}

float CGuard::GetTargetUpdateTime()
{
	return m_targetUpdateTime;
}

void CGuard::SetElapsedTime(float elapsedTime)
{
	m_elapsedTime = elapsedTime;
}

float CGuard::GetElapsedTime()
{
	return m_elapsedTime;
}

bool CGuard::IsFinishedMovePath()
{
	return m_movePath.empty();
}

const XMFLOAT3& CGuard::GetNextPatrolPosition()
{
	return m_patrolPath[m_patrolIndex];
}

void CGuard::SetTarget(CCharacter* target)
{
	m_target = target;
}

CCharacter* CGuard::GetTarget()
{
	return m_target;
}

void CGuard::Init()
{
	GetStateMachine()->SetCurrentState(CGuardIdleState::GetInstance());
}

CCharacter* CGuard::FindTarget(float maxDist, float fov)
{
	CCharacter* target = nullptr;
	float nearestDist = FLT_MAX;
	const vector<CObject*>& players = CSceneManager::GetInstance()->GetCurrentScene()->GetGroupObject(GROUP_TYPE::PLAYER);
	const vector<CObject*>& structures = CSceneManager::GetInstance()->GetCurrentScene()->GetGroupObject(GROUP_TYPE::STRUCTURE);

	for (const auto& object : players)
	{
		CCharacter* player = (CCharacter*)object;

		if (player->GetHealth() > 0)
		{
			XMFLOAT3 toPlayer = Vector3::Subtract(player->GetPosition(), GetPosition());
			float dist = Vector3::Length(toPlayer);

			if (dist <= maxDist)
			{
				// �÷��̾���� ���̰��� ���Ѵ�.
				toPlayer = Vector3::Normalize(toPlayer);

				float angle = Vector3::Angle(GetForward(), toPlayer);

				// ���̰��� fov�� ���϶��, toPlayer �������� ������ ��� ���� ���θ� �ľ��Ͽ� Ÿ���� �����Ѵ�.
				if (angle <= fov)
				{
					float nearestHitDist = FLT_MAX;
					bool isHit = false;
					XMFLOAT3 rayOrigin = GetPosition();

					rayOrigin.y = 5.0f;

					for (const auto& structure : structures)
					{
						float hitDist = 0.0f, maxDist = 30.0f;
						CObject* intersectedObject = structure->CheckRayIntersection(rayOrigin, toPlayer, hitDist, maxDist);

						if ((intersectedObject != nullptr) && (hitDist <= maxDist))
						{
							isHit = true;
							break;
						}
					}

					if ((!isHit) && (dist < nearestDist))
					{
						target = player;
						nearestDist = dist;
					}
				}
			}
		}
	}

	return target;
}

void CGuard::CreateMovePath(const XMFLOAT3& targetPosition)
{
	m_movePath.clear();

	CreatePath(m_movePath, targetPosition);
	OptimizePath(m_movePath);
}

void CGuard::CreatePatrolPath(const XMFLOAT3& targetPosition)
{
	CreatePath(m_patrolPath, targetPosition);

	// patrolPath�� ��쿡��, �������� ����̹Ƿ� index�� ����Ͽ� �����ϸ� ������ �ε����� �������� ��� reverse �Լ��� ���� ���͸� ������ �ǵ��ư����� �Ѵ�.
	reverse(m_patrolPath.begin(), m_patrolPath.end());
}

void CGuard::CreatePath(vector<XMFLOAT3>& path, const XMFLOAT3& targetPosition)
{
	CNavMesh* navMesh = (CNavMesh*)CAssetManager::GetInstance()->GetMesh("NavMesh");
	const vector<CNavNode*>& navNodes = navMesh->GetNavNodes();

	for (int i = 0; i < navNodes.size(); ++i)
	{
		navNodes[i]->Reset();
	}

	CNavNode* startNode = navNodes[navMesh->GetNodeIndex(GetPosition())];
	CNavNode* targetNode = navNodes[navMesh->GetNodeIndex(targetPosition)];
	CNavNode* currentNode = nullptr;

	priority_queue<CNavNode*, vector<CNavNode*>, compare> priorityQueue;

	startNode->CalculateF(targetNode, startNode);
	priorityQueue.push(startNode);

	while (true)
	{
		if (priorityQueue.empty())
		{
			break;
		}

		currentNode = priorityQueue.top();
		priorityQueue.pop();

		if (currentNode->IsVisited())
		{
			continue;
		}

		currentNode->SetVisited(true);

		if (Vector3::IsEqual(currentNode->GetTriangle().m_centroid, targetNode->GetTriangle().m_centroid))
		{
			break;
		}

		for (const auto& nearNode : currentNode->GetNearNodes())
		{
			if (!nearNode->IsVisited())
			{
				nearNode->SetParent(currentNode);
				nearNode->CalculateF(targetNode, currentNode);
				priorityQueue.push(nearNode);
			}
		}
	}

	// ��ǥ ������ ������ ��� ������ ������ �������� ���Ϳ� �����Ѵ�.
	path.push_back(targetPosition);

	while (true)
	{
		CNavNode* parentNode = currentNode->GetParent();

		if (parentNode == nullptr)
		{
			break;
		}

		// �θ� ���� ������ ���� �߿��� �� ������ �����ϴ� ���� ��ġ�� ã�� Path�� �߰��Ѵ�.
		int nearSideIndex = currentNode->CalculateNearSideIndex(currentNode->GetParent());

		path.push_back(currentNode->GetTriangle().m_centerSides[nearSideIndex]);
		currentNode = parentNode;
	}

	path.push_back(GetPosition());
}

void CGuard::OptimizePath(vector<XMFLOAT3>& path)
{
	vector<XMFLOAT3> optimizedPath;

	// targetPosition�� ����ȭ ���Ϳ� �߰��Ѵ�.
	optimizedPath.push_back(path.front());

	// i�� ������ ��� ��ġ�� �ε��� ������, ���� ��ġ���� ��ȸ�Ѵ�.
	for (int i = 0; i < path.size() - 1; ++i)
	{
		XMFLOAT3 rayOrigin = path[i];

		// j�� ������ ��� ����� �Ǵ� ��ġ�� �ε��� ������, ��ǥ ��ġ�� ������ ��ġ���� ��ȸ�Ѵ�.
		for (int j = (int)path.size() - 1; j > i; --j)
		{
			XMFLOAT3 rayTarget = path[j];

			// ������ �ٿ�� �ڽ��� �浹�� �� �ִ� �ּ� �����̴�.
			rayOrigin.y = rayTarget.y = 2.6f;

			XMFLOAT3 rayDirection = Vector3::Subtract(rayTarget, rayOrigin);
			float dist = Vector3::Length(rayDirection);
			bool isHit = false;

			const vector<CObject*>& structures = CSceneManager::GetInstance()->GetCurrentScene()->GetGroupObject(GROUP_TYPE::STRUCTURE);

			for (const auto& structure : structures)
			{
				float hitDist = 0.0f;
				CObject* intersectedObject = structure->CheckRayIntersection(rayOrigin, Vector3::Normalize(rayDirection), hitDist, dist);

				if ((intersectedObject != nullptr) && (hitDist <= dist))
				{
					isHit = true;
					break;
				}
			}

			if (!isHit)
			{
				optimizedPath.push_back(path[j]);
				i = j - 1;
				break;
			}
		}
	}

	path = move(optimizedPath);
}

void CGuard::FollowMovePath(float force)
{
	XMFLOAT3 position = GetPosition();
	XMFLOAT3 forward = GetForward();

	XMFLOAT3 toNextNode = Vector3::Subtract(m_movePath.back(), position);
	float restDist = Vector3::Length(toNextNode);

	// ���� ��忡 ������ ���
	if (restDist <= 1.0f)
	{
		m_movePath.pop_back();

		if (m_movePath.empty())
		{
			return;
		}

		toNextNode = Vector3::Normalize(Vector3::Subtract(m_movePath.back(), position));

		float angle = Vector3::Angle(forward, toNextNode);

		// �� ���͸� �����Ͽ� ȸ������ ���� �ּ� ȸ�� �������� ȸ����Ų��.
		float axis = Vector3::CrossProduct(forward, toNextNode, false).y;

		if (axis < 0.0f)
		{
			angle = -angle;
		}

		Rotate(GetUp(), angle);
	}
	else
	{
		GetRigidBody()->AddForce(Vector3::ScalarProduct(force, forward, false));
	}
}

void CGuard::FollowPatrolPath(float force)
{
	XMFLOAT3 position = GetPosition();
	XMFLOAT3 forward = GetForward();

	XMFLOAT3 toNextNode = Vector3::Subtract(m_patrolPath[m_patrolIndex], position);
	float restDist = Vector3::Length(toNextNode);

	// ���� ��忡 ������ ���
	if (restDist <= 1.0f)
	{
		++m_patrolIndex;

		// ������ ��忡 �����ߴٸ�, ��θ� ������ ���ư���.
		if (m_patrolIndex >= m_patrolPath.size())
		{
			reverse(m_patrolPath.begin(), m_patrolPath.end());
			m_patrolIndex = 0;
		}

		toNextNode = Vector3::Normalize(Vector3::Subtract(m_patrolPath[m_patrolIndex], position));

		float angle = Vector3::Angle(forward, toNextNode);

		// �� ���͸� �����Ͽ� ȸ������ ���� �ּ� ȸ�� �������� ȸ����Ų��.
		float axis = Vector3::CrossProduct(forward, toNextNode, false).y;

		if (axis < 0.0f)
		{
			angle = -angle;
		}

		Rotate(GetUp(), angle);
	}
	else
	{
		GetRigidBody()->AddForce(Vector3::ScalarProduct(force, forward, false));
	}
}
