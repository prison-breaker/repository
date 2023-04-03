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
				// 플레이어와의 사이각을 구한다.
				toPlayer = Vector3::Normalize(toPlayer);

				float angle = Vector3::Angle(GetForward(), toPlayer);

				// 사이각이 fov도 이하라면, toPlayer 방향으로 광선을 쏘아 차폐 여부를 파악하여 타겟을 설정한다.
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

	// patrolPath의 경우에는, 영구적인 경로이므로 index를 사용하여 접근하며 마지막 인덱스에 도달했을 경우 reverse 함수를 통해 벡터를 뒤집어 되돌아가도록 한다.
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

	// 목표 지점의 노드부터 출발 지점의 노드까지 역순으로 벡터에 대입한다.
	path.push_back(targetPosition);

	while (true)
	{
		CNavNode* parentNode = currentNode->GetParent();

		if (parentNode == nullptr)
		{
			break;
		}

		// 부모 노드와 인접한 변들 중에서 두 정점을 공유하는 변의 위치를 찾아 Path에 추가한다.
		int nearSideIndex = currentNode->CalculateNearSideIndex(currentNode->GetParent());

		path.push_back(currentNode->GetTriangle().m_centerSides[nearSideIndex]);
		currentNode = parentNode;
	}

	path.push_back(GetPosition());
}

void CGuard::OptimizePath(vector<XMFLOAT3>& path)
{
	vector<XMFLOAT3> optimizedPath;

	// targetPosition을 최적화 벡터에 추가한다.
	optimizedPath.push_back(path.front());

	// i는 광선을 쏘는 위치의 인덱스 값으로, 현재 위치부터 순회한다.
	for (int i = 0; i < path.size() - 1; ++i)
	{
		XMFLOAT3 rayOrigin = path[i];

		// j는 광선을 쏘는 대상이 되는 위치의 인덱스 값으로, 목표 위치에 근접한 위치부터 순회한다.
		for (int j = (int)path.size() - 1; j > i; --j)
		{
			XMFLOAT3 rayTarget = path[j];

			// 광선과 바운딩 박스가 충돌할 수 있는 최소 높이이다.
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

	// 다음 노드에 도착한 경우
	if (restDist <= 1.0f)
	{
		m_movePath.pop_back();

		if (m_movePath.empty())
		{
			return;
		}

		toNextNode = Vector3::Normalize(Vector3::Subtract(m_movePath.back(), position));

		float angle = Vector3::Angle(forward, toNextNode);

		// 두 벡터를 외적하여 회전축을 구해 최소 회전 방향으로 회전시킨다.
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

	// 다음 노드에 도착한 경우
	if (restDist <= 1.0f)
	{
		++m_patrolIndex;

		// 마지막 노드에 도달했다면, 경로를 뒤집어 돌아간다.
		if (m_patrolIndex >= m_patrolPath.size())
		{
			reverse(m_patrolPath.begin(), m_patrolPath.end());
			m_patrolIndex = 0;
		}

		toNextNode = Vector3::Normalize(Vector3::Subtract(m_patrolPath[m_patrolIndex], position));

		float angle = Vector3::Angle(forward, toNextNode);

		// 두 벡터를 외적하여 회전축을 구해 최소 회전 방향으로 회전시킨다.
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
