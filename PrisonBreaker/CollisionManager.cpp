#include "pch.h"
#include "CollisionManager.h"

#include "SceneManager.h"

#include "Scene.h"

#include "Object.h"

#include "Transform.h"
#include "Collider.h"

CCollisionManager::CCollisionManager() :
	m_layerMask{},
	m_collisionStates()
{
}

CCollisionManager::~CCollisionManager()
{
}

void CCollisionManager::SetCollisionGroup(GROUP_TYPE group1, GROUP_TYPE group2)
{
	// 더 작은 그룹 타입을 행으로, 큰 그룹 타입을 열로 사용한다.
	UINT row = (UINT)group1;
	UINT col = (UINT)group2;

	if (row > col)
	{
		swap(row, col);
	}

	col = 1 << col;

	// 이미 체크되어 있는 레이어였다면, 체크를 해제한다.
	if (m_layerMask[row] & col)
	{
		m_layerMask[row] &= ~col;
	}
	else
	{
		m_layerMask[row] |= col;
	}
}

void CCollisionManager::UpdateCollisionGroup(GROUP_TYPE group1, GROUP_TYPE group2)
{
	CScene* currentScene = CSceneManager::GetInstance()->GetCurrentScene();
	const vector<CObject*>& group1Objects = currentScene->GetGroupObject(group1);
	const vector<CObject*>& group2Objects = currentScene->GetGroupObject(group2);

	for (const auto& object1 : group1Objects)
	{
		for (const auto& object2 : group2Objects)
		{
			if (object1 == object2)
			{
				continue;
			}

			// 두 객체의 충돌 상태
			CollisionID collisionID = {};

			collisionID.m_lowPart = object1->GetInstanceID();
			collisionID.m_highPart = object2->GetInstanceID();

			// 현재 프레임에 두 객체가 충돌한 경우
			if (IsCollided(object1, object2))
			{
				// 이전 프레임에도 충돌한 경우
				if (m_collisionStates[collisionID.m_quadPart])
				{
					// 두 객체 중 한 객체라도 비활성화 되었거나, 삭제 예정인 객체인 경우 두 객체의 충돌을 해제시킨다.
					if (!object1->IsActive() || object1->IsDeleted() || !object2->IsActive() || object2->IsDeleted())
					{
						object1->OnCollisionExit(object2);
						object2->OnCollisionExit(object1);

						m_collisionStates[collisionID.m_quadPart] = false;
					}
					else
					{
						object1->OnCollision(object2);
						object2->OnCollision(object1);
					}
				}
				else
				{
					// 두 객체 중 한 객체라도 비활성화 되었거나, 삭제 예정인 객체인 경우 두 객체의 충돌은 무효화된다.
					if (object1->IsActive() && !object1->IsDeleted() && object2->IsActive() && !object2->IsDeleted())
					{
						object1->OnCollisionEnter(object2);
						object2->OnCollisionEnter(object1);

						m_collisionStates[collisionID.m_quadPart] = true;
					}
				}
			}
			else
			{
				// 이전 프레임에 충돌한 경우
				if (m_collisionStates[collisionID.m_quadPart])
				{
					object1->OnCollisionExit(object2);
					object2->OnCollisionExit(object1);

					m_collisionStates[collisionID.m_quadPart] = false;
				}
			}
		}
	}
}

void CCollisionManager::ResetCollisionGroup()
{
	memset(m_layerMask, 0, sizeof(UINT) * (int)GROUP_TYPE::COUNT);
}

bool CCollisionManager::IsCollided(CObject* object1, CObject* object2)
{
	return Math::Distance(object1->GetComponent<CTransform>()->GetPosition(), object2->GetComponent<CTransform>()->GetPosition()) <= 2.0f;
}

void CCollisionManager::Update()
{
	for (UINT row = 0; row < (UINT)GROUP_TYPE::COUNT; ++row)
	{
		for (UINT col = row; col < (UINT)GROUP_TYPE::COUNT; ++col)
		{
			// 레이어 마스크가 체크 되어있는 그룹 간의 충돌을 검사한다.
			if (m_layerMask[row] & (1 << col))
			{
				UpdateCollisionGroup((GROUP_TYPE)row, (GROUP_TYPE)col);
			}
		}
	}
}
