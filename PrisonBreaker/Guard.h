#pragma once
#include "GameObject.h"

class CGuard : public CGameObject
{
private:
	UINT							  m_Health{};

	shared_ptr<CCamera>               m_Camera{};

	shared_ptr<CStateMachine<CGuard>> m_StateMachine{};
	shared_ptr<CAnimationController>  m_AnimationController{};

public:
	CGuard() = default;
	virtual ~CGuard() = default;

	virtual void Initialize();

	virtual void Animate(float ElapsedTime);

	virtual void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera, RENDER_TYPE RenderType);

	void SetHealth(UINT Health);
	UINT GetHealth() const;

	CCamera* GetCamera() const;

	CStateMachine<CGuard>* GetStateMachine() const;

	void SetAnimationController(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, const shared_ptr<LOADED_MODEL_INFO>& ModelInfo);
	CAnimationController* GetAnimationController() const;

	void SetAnimationClip(UINT ClipNum);
	UINT GetAnimationClip() const;
};
