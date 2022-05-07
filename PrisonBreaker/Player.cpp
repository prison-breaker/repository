#include "stdafx.h"
#include "Player.h"
#include "GameScene.h"

CPlayer::CPlayer(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	// 카메라 객체를 생성한다.
	m_Camera = make_shared<CCamera>();
	m_Camera->CreateShaderVariables(D3D12Device, D3D12GraphicsCommandList);
	m_Camera->GeneratePerspectiveProjectionMatrix(90.0f, static_cast<float>(CLIENT_WIDTH) / static_cast<float>(CLIENT_HEIGHT), 1.0f, 200.0f);
	m_Camera->GenerateViewMatrix(XMFLOAT3(0.0f, 5.0f, -150.0f), XMFLOAT3(0.0f, 0.0f, 1.0f));
}

void CPlayer::Initialize()
{
	CGameObject::Initialize();

	// 상태머신 객체를 생성한다.
	m_StateMachine = make_shared<CStateMachine<CPlayer>>(static_pointer_cast<CPlayer>(shared_from_this()));
	m_StateMachine->SetCurrentState(CPlayerIdleState::GetInstance());

	FindFrame(TEXT("gun_pr_1"))->SetActive(false);
}

void CPlayer::Animate(float ElapsedTime)
{
	if (IsActive())
	{
		if (m_StateMachine)
		{
			m_StateMachine->Update(ElapsedTime);
		}
	}
}

void CPlayer::SetHealth(UINT Health)
{
	// UINT UnderFlow
	if (Health > 100)
	{
		m_Health = 0;
	}
	else
	{
		m_Health = Health;
	}
}

UINT CPlayer::GetHealth() const
{
	return m_Health;
}

void CPlayer::SetSpeed(float Speed)
{
	m_Speed = Speed;
}

float CPlayer::GetSpeed() const
{
	return m_Speed;
}

void CPlayer::SetMovingDirection(const XMFLOAT3& MovingDirection)
{
	m_MovingDirection = MovingDirection;
}

const XMFLOAT3& CPlayer::GetMovingDirection() const
{
	return m_MovingDirection;
}

shared_ptr<CCamera> CPlayer::GetCamera() const
{
	return m_Camera;
}

shared_ptr<CStateMachine<CPlayer>> CPlayer::GetStateMachine() const
{
	return m_StateMachine;
}

void CPlayer::AcquirePistol()
{
	m_PistolFrame = FindFrame(TEXT("gun_pr_1"));
}

bool CPlayer::HasPistol() const
{
	return (m_PistolFrame) ? true : false;
}

bool CPlayer::IsEquippedPistol() const
{
	if (m_PistolFrame)
	{
		return m_PistolFrame->IsActive();
	}

	return false;
}

bool CPlayer::SwapWeapon(WEAPON_TYPE WeaponType)
{
	bool IsSwapped{};

	if (m_PistolFrame)
	{
		switch (WeaponType)
		{
		case WEAPON_TYPE_PUNCH:
			m_PistolFrame->SetActive(false);
			IsSwapped = true;
			break;
		case WEAPON_TYPE_PISTOL:
			m_PistolFrame->SetActive(true);
			IsSwapped = true;
			break;
		}
	}

	return IsSwapped;
}

void CPlayer::ApplySlidingVectorToPosition(const shared_ptr<CNavMesh>& NavMesh, XMFLOAT3& NewPosition)
{
	XMFLOAT3 Shift{ Vector3::Subtract(NewPosition, GetPosition()) };

	shared_ptr<CNavNode> NavNode{ NavMesh->GetNavNodes()[NavMesh->GetNodeIndex(GetPosition())] };

	XMFLOAT3 SlidingVector{};
	XMFLOAT3 Vertices[3]{ NavNode->GetTriangle().m_Vertices[0], NavNode->GetTriangle().m_Vertices[1], NavNode->GetTriangle().m_Vertices[2] };
	XMFLOAT3 Edge{};
	XMFLOAT3 ContactNormal{};

	if (Math::LineIntersection(Vertices[0], Vertices[1], NewPosition, GetPosition()))
	{
		Edge = Vector3::Subtract(Vertices[0], Vertices[1]);
		ContactNormal = Vector3::Normalize(Vector3::TransformNormal(Edge, Matrix4x4::RotationYawPitchRoll(0.0f, 90.0f, 0.0f)));

		SlidingVector = Vector3::Subtract(Shift, Vector3::ScalarProduct(Vector3::DotProduct(Shift, ContactNormal), ContactNormal, false));
		NewPosition = Vector3::Add(GetPosition(), SlidingVector);
	}
	else if (Math::LineIntersection(Vertices[1], Vertices[2], NewPosition, GetPosition()))
	{
		Edge = Vector3::Subtract(Vertices[1], Vertices[2]);
		ContactNormal = Vector3::Normalize(Vector3::TransformNormal(Edge, Matrix4x4::RotationYawPitchRoll(0.0f, 90.0f, 0.0f)));

		SlidingVector = Vector3::Subtract(Shift, Vector3::ScalarProduct(Vector3::DotProduct(Shift, ContactNormal), ContactNormal, false));
		NewPosition = Vector3::Add(GetPosition(), SlidingVector);
	}
	else if (Math::LineIntersection(Vertices[2], Vertices[0], NewPosition, GetPosition()))
	{
		Edge = Vector3::Subtract(Vertices[2], Vertices[0]);
		ContactNormal = Vector3::Normalize(Vector3::TransformNormal(Edge, Matrix4x4::RotationYawPitchRoll(0.0f, 90.0f, 0.0f)));

		SlidingVector = Vector3::Subtract(Shift, Vector3::ScalarProduct(Vector3::DotProduct(Shift, ContactNormal), ContactNormal, false));
		NewPosition = Vector3::Add(GetPosition(), SlidingVector);
	}

	// 보정된 NewPosition도 NavMesh 위에 없다면, 기존 위치로 설정한다.
	if (!IsInNavMesh(NavMesh, NewPosition))
	{
		NewPosition = GetPosition();
		//tcout << TEXT("Not in NavMesh!") << endl;
	}
}

void CPlayer::Rotate(float Pitch, float Yaw, float Roll, float ElapsedTime, float NearestHitDistance)
{
	if (!Math::IsZero(Pitch))
	{
		m_Rotation.x += Pitch;

		if (m_Rotation.x > 15.0f)
		{
			Pitch -= (Pitch - 15.0f);
			m_Rotation.x = 15.0f;

		}

		if (m_Rotation.x < -15.0f)
		{
			Pitch -= (Pitch + 15.0f);
			m_Rotation.x = -15.0f;
		}
	}

	if (!Math::IsZero(Yaw))
	{
		m_Rotation.y += Yaw;

		if (m_Rotation.y > 360.0f)
		{
			m_Rotation.y -= 360.0f;
		}

		if (m_Rotation.y < 0.0f)
		{
			m_Rotation.y += 360.0f;
		}
	}

	if (!Math::IsZero(Yaw))
	{
		XMFLOAT4X4 RotationMatrix{ Matrix4x4::RotationAxis(GetUp(), Yaw) };

		SetRight(Vector3::TransformNormal(GetRight(), RotationMatrix));
		SetLook(Vector3::TransformNormal(GetLook(), RotationMatrix));
	}

	CGameObject::UpdateLocalCoord(Vector3::Normalize(GetLook()));

	XMFLOAT4X4 RotationMatrix{ Matrix4x4::Multiply(Matrix4x4::RotationYawPitchRoll(m_Rotation.x, 0.0f, 0.0f), GetWorldMatrix()) };
	
	m_Camera->Rotate(RotationMatrix, ElapsedTime, NearestHitDistance);
}

bool CPlayer::IsCollidedByGuard(const XMFLOAT3& NewPosition)
{
	vector<vector<shared_ptr<CGameObject>>>& GameObjects{ static_pointer_cast<CGameScene>(CSceneManager::GetInstance()->GetCurrentScene())->GetGameObjects() };

	for (const auto& GameObject : GameObjects[OBJECT_TYPE_NPC])
	{
		if (GameObject)
		{
			shared_ptr<CGuard> Guard{ static_pointer_cast<CGuard>(GameObject) };

			if (Guard->GetHealth() > 0)
			{
				if (Math::Distance(Guard->GetPosition(), NewPosition) <= 2.0f)
				{
					return true;
				}
			}
		}
	}

	return false;
}

bool CPlayer::IsCollidedByEventTrigger(const XMFLOAT3& NewPosition, bool IsInteracted)
{
	vector<shared_ptr<CEventTrigger>>& EventTriggers{ static_pointer_cast<CGameScene>(CSceneManager::GetInstance()->GetCurrentScene())->GetEventTriggers() };

	for (auto iter = EventTriggers.begin(); iter != EventTriggers.end(); ++iter)
	{
		shared_ptr<CEventTrigger> EventTrigger = *iter;

		if (EventTrigger)
		{
			if (EventTrigger->IsInTriggerArea(GetPosition(), GetLook()))
			{
				if (IsInteracted)
				{
					EventTrigger->InteractEventTrigger();
				}

				if (EventTrigger->CanPassTriggerArea(GetPosition(), NewPosition))
				{
					return false;
				}
				else
				{
					return true;
				}
			}
		}
	}

	// 반복문을 모두 돌았다면, 플레이어는 트리거 영역안에 없는 것이므로 상호작용 UI를 렌더링되지 않도록 만든다.
	// 이때, 모든 트리거는 상호작용 UI(m_InteractionUI)를 공유하여 사용하므로 0번 트리거를 이용하여 함수를 호출하였다.
	EventTriggers[0]->HideInteractionUI();

	return false;
}

void CPlayer::ProcessInput(float ElapsedTime, UINT InputMask)
{
	if (m_StateMachine)
	{
		m_StateMachine->ProcessInput(ElapsedTime, InputMask);
	}

	shared_ptr<CNavMesh> NavMesh{ static_pointer_cast<CGameScene>(CSceneManager::GetInstance()->GetCurrentScene())->GetNavMesh() };
	XMFLOAT3 NewPosition{ Vector3::Add(GetPosition(), Vector3::ScalarProduct(m_Speed * ElapsedTime, m_MovingDirection, false)) };

	if (!IsInNavMesh(NavMesh, NewPosition))
	{
		// SlidingVector를 이용하여 NewPosition의 값을 보정한다.
		ApplySlidingVectorToPosition(NavMesh, NewPosition);
	}

	// NewPosition으로 이동 시, 교도관과의 충돌, 트리거 내 상호작용을 처리하거나, 움직임 제어 영향을 받지 않는다면 움직이도록 만든다.
	if (!IsCollidedByGuard(NewPosition) && !IsCollidedByEventTrigger(NewPosition, (InputMask & INPUT_MASK_F) ? true : false))
	{
		SetPosition(NewPosition);
	}
}
