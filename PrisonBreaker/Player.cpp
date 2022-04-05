#include "stdafx.h"
#include "Player.h"
#include "State_Player.h"

CPlayer::CPlayer(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	// 카메라 객체를 생성한다.
	m_Camera = make_shared<CCamera>();
	m_Camera->CreateShaderVariables(D3D12Device, D3D12GraphicsCommandList);
	m_Camera->GeneratePerspectiveProjectionMatrix(60.0f, static_cast<float>(CLIENT_WIDTH) / static_cast<float>(CLIENT_HEIGHT), 1.0f, 200.0f);
	m_Camera->GenerateViewMatrix(XMFLOAT3(0.0f, 5.0f, -150.0f), XMFLOAT3(0.0f, 0.0f, 1.0f));
}

void CPlayer::Initialize()
{
	CGameObject::Initialize();

	// 상태머신 객체를 생성한다.
	m_StateMachine = make_shared<CStateMachine<CPlayer>>(static_pointer_cast<CPlayer>(shared_from_this()));
	m_StateMachine->SetCurrentState(CPlayerIdleState::GetInstance());
}

void CPlayer::Animate(float ElapsedTime)
{
	if (IsActive())
	{
		if (m_StateMachine)
		{
			m_StateMachine->Update();
		}
	}
}

void CPlayer::Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera, RENDER_TYPE RenderType)
{
	if (IsActive())
	{
		if (m_AnimationController)
		{
			m_AnimationController->UpdateShaderVariables(D3D12GraphicsCommandList);
		}

		if (IsVisible(Camera))
		{
			if (m_Mesh)
			{
				UpdateShaderVariables(D3D12GraphicsCommandList);

				UINT MaterialCount{ static_cast<UINT>(m_Materials.size()) };

				for (UINT i = 0; i < MaterialCount; ++i)
				{
					if (m_Materials[i])
					{
						m_Materials[i]->SetPipelineState(D3D12GraphicsCommandList, RenderType);
						m_Materials[i]->UpdateShaderVariables(D3D12GraphicsCommandList);
					}

					m_Mesh->Render(D3D12GraphicsCommandList, i);
				}
			}
		}

		for (const auto& ChildObject : m_ChildObjects)
		{
			if (ChildObject)
			{
				ChildObject->Render(D3D12GraphicsCommandList, Camera, RenderType);
			}
		}
	}
}

void CPlayer::SetHealth(UINT Health)
{
	m_Health = Health;
}

UINT CPlayer::GetHealth() const
{
	return m_Health;
}

void CPlayer::SetMovingDirection(const XMFLOAT3& MovingDirection)
{
	m_MovingDirection = MovingDirection;
}

const XMFLOAT3& CPlayer::GetMovingDirection() const
{
	return m_MovingDirection;
}

CCamera* CPlayer::GetCamera() const
{
	return m_Camera.get();
}

CStateMachine<CPlayer>* CPlayer::GetStateMachine() const
{
	return m_StateMachine.get();
}

void CPlayer::SetAnimationController(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, const shared_ptr<LOADED_MODEL_INFO>& ModelInfo)
{
	if (ModelInfo)
	{
		m_AnimationController = make_shared<CAnimationController>(D3D12Device, D3D12GraphicsCommandList, ModelInfo, shared_from_this());
	}
}

CAnimationController* CPlayer::GetAnimationController() const
{
	return m_AnimationController.get();
}

void CPlayer::SetAnimationClip(UINT ClipNum)
{
	if (m_AnimationController)
	{
		m_AnimationController->SetAnimationClip(ClipNum);
	}
}

UINT CPlayer::GetAnimationClip() const
{
	if (m_AnimationController)
	{
		return m_AnimationController->GetAnimationClip();
	}

	return 0;
}

void CPlayer::AcquirePistol()
{
	m_PistolFrame = FindFrame(TEXT("gun_pr_1"));
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

void CPlayer::Rotate(float Pitch, float Yaw, float Roll, float ElapsedTime)
{
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

	m_Camera->Rotate(GetWorldMatrix(), ElapsedTime);

	if (!Math::IsZero(Yaw))
	{
		XMFLOAT4X4 RotationMatrix{ Matrix4x4::RotationAxis(GetUp(), Yaw) };

		SetRight(Vector3::TransformNormal(GetRight(), RotationMatrix));
		SetLook(Vector3::TransformNormal(GetLook(), RotationMatrix));
	}

	const XMFLOAT3 WorldUp{ 0.0f, 1.0f, 0.0f };

	SetLook(Vector3::Normalize(GetLook()));
	SetRight(Vector3::CrossProduct(WorldUp, GetLook(), false));
	SetUp(Vector3::CrossProduct(GetLook(), GetRight(), false));
}

void CPlayer::ProcessInput(UINT InputMask, float ElapsedTime)
{
	if (m_StateMachine)
	{
		m_StateMachine->ProcessInput(InputMask, ElapsedTime);
	}

	CGameObject::Move(m_MovingDirection, m_Speed * ElapsedTime);
}
