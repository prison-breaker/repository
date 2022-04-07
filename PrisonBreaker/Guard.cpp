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
		m_StateMachine->Update(ElapsedTime);
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

void CGuard::RenderBoundingBox(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera)
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
				m_Mesh->RenderBoundingBox(D3D12GraphicsCommandList);
			}
		}

		for (const auto& ChildObject : m_ChildObjects)
		{
			if (ChildObject)
			{
				ChildObject->RenderBoundingBox(D3D12GraphicsCommandList, Camera);
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
