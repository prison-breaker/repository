#include "stdafx.h"
#include "Player.h"
#include "GameScene.h"

CPlayer::CPlayer(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	// ī�޶� ��ü�� �����Ѵ�.
	m_Camera = make_shared<CCamera>();
	m_Camera->CreateShaderVariables(D3D12Device, D3D12GraphicsCommandList);
	m_Camera->GeneratePerspectiveProjectionMatrix(90.0f, static_cast<float>(CLIENT_WIDTH) / static_cast<float>(CLIENT_HEIGHT), 1.0f, 200.0f);
	m_Camera->GenerateViewMatrix(XMFLOAT3(0.0f, 5.0f, -150.0f), XMFLOAT3(0.0f, 0.0f, 1.0f));
}

void CPlayer::Initialize()
{
	CGameObject::Initialize();

	// ���¸ӽ� ��ü�� �����Ѵ�.
	m_StateMachine = make_shared<CStateMachine<CPlayer>>(static_pointer_cast<CPlayer>(shared_from_this()));
	m_StateMachine->SetCurrentState(CPlayerIdleState::GetInstance());

	FindFrame(TEXT("gun_pr_1"))->SetActive(false);
}

void CPlayer::Reset(const XMFLOAT4X4& TransformMatrix)
{
	SwapWeapon(WEAPON_TYPE_PUNCH);
	ManagePistol(false);

	m_Health = 100;
	m_StateMachine->SetCurrentState(CPlayerIdleState::GetInstance());

	SetTransformMatrix(TransformMatrix);
	UpdateTransform(Matrix4x4::Identity());
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

void CPlayer::SetID(UINT ID)
{
	m_ID = ID;
}

UINT CPlayer::GetID() const
{
	return m_ID;
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

void CPlayer::ManagePistol(bool AcquirePistol)
{
	if (AcquirePistol)
	{
		m_PistolFrame = FindFrame(TEXT("gun_pr_1"));
	}
	else
	{
		m_PistolFrame = nullptr;
	}
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

	// ������ NewPosition�� NavMesh ���� ���ٸ�, ���� ��ġ�� �����Ѵ�.
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

bool CPlayer::IsCollidedByEventTrigger(const XMFLOAT3& NewPosition)
{
	vector<shared_ptr<CEventTrigger>>& EventTriggers{ static_pointer_cast<CGameScene>(CSceneManager::GetInstance()->GetCurrentScene())->GetEventTriggers() };

	for (const auto& EventTrigger : EventTriggers)
	{
		if (EventTrigger)
		{
			if (EventTrigger->IsInTriggerArea(GetPosition(), GetLook()))
			{
				return true;
			}
		}
	}

	// �ݺ����� ��� ���Ҵٸ�, �÷��̾�� Ʈ���� �����ȿ� ���� ���̹Ƿ� ��ȣ�ۿ� UI�� ���������� �ʵ��� �����.
	// �̶�, ��� Ʈ���Ŵ� ��ȣ�ۿ� UI(m_InteractionUI)�� �����Ͽ� ����ϹǷ� 0�� Ʈ���Ÿ� �̿��Ͽ� �Լ��� ȣ���Ͽ���.
	EventTriggers[0]->HideInteractionUI();

	return false;
}

void CPlayer::ProcessInput(float ElapsedTime, UINT InputMask)
{
	//if (m_StateMachine)
	//{
	//	m_StateMachine->ProcessInput(ElapsedTime, InputMask);
	//}

	//shared_ptr<CNavMesh> NavMesh{ static_pointer_cast<CGameScene>(CSceneManager::GetInstance()->GetCurrentScene())->GetNavMesh() };
	//XMFLOAT3 NewPosition{ Vector3::Add(GetPosition(), Vector3::ScalarProduct(m_Speed * ElapsedTime, m_MovingDirection, false)) };

	//if (!IsInNavMesh(NavMesh, NewPosition))
	//{
	//	// SlidingVector�� �̿��Ͽ� NewPosition�� ���� �����Ѵ�.
	//	ApplySlidingVectorToPosition(NavMesh, NewPosition);
	//}

	//// NewPosition���� �̵� ��, ���������� �浹, Ʈ���� �� ��ȣ�ۿ��� ó���ϰų�, ������ ���� ������ ���� �ʴ´ٸ� �����̵��� �����.
	//if (!IsCollidedByGuard(NewPosition) && !IsCollidedByEventTrigger(NewPosition, (InputMask & INPUT_MASK_F) ? true : false))
	//{
	//	SetPosition(NewPosition);
	//}
}
