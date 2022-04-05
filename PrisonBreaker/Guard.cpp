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
	if (m_StateMachine)
	{
		m_StateMachine->Update();
	}
}

void CGuard::Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera, RENDER_TYPE RenderType)
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

void CGuard::SetHealth(UINT Health)
{
	m_Health = Health;
}

UINT CGuard::GetHealth() const
{
	return m_Health;
}

CCamera* CGuard::GetCamera() const
{
	return m_Camera.get();
}

CStateMachine<CGuard>* CGuard::GetStateMachine() const
{
	return m_StateMachine.get();
}

void CGuard::SetAnimationController(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, const shared_ptr<LOADED_MODEL_INFO>& ModelInfo)
{
	if (ModelInfo)
	{
		m_AnimationController = make_shared<CAnimationController>(D3D12Device, D3D12GraphicsCommandList, ModelInfo, shared_from_this());
	}
}

CAnimationController* CGuard::GetAnimationController() const
{
	return m_AnimationController.get();
}

void CGuard::SetAnimationClip(UINT ClipNum)
{
	if (m_AnimationController)
	{
		m_AnimationController->SetAnimationClip(ClipNum);
	}
}

UINT CGuard::GetAnimationClip() const
{
	if (m_AnimationController)
	{
		return m_AnimationController->GetAnimationClip();
	}

	return 0;
}
