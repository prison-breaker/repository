#pragma once
#include "GameObject.h"

class CPlayer : public CGameObject
{
private:
	UINT							   m_Health{};
	
	XMFLOAT3						   m_MovingDirection{};
	XMFLOAT3						   m_Rotation{};

	shared_ptr<CGameObject>            m_PistolFrame{};

	shared_ptr<CCamera>                m_Camera{};

	shared_ptr<CStateMachine<CPlayer>> m_StateMachine{};
	shared_ptr<CAnimationController>   m_AnimationController{};

public:
	CPlayer(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList);
	virtual ~CPlayer() = default;

	virtual void Initialize();

	virtual void Animate(float ElapsedTime);

	virtual void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera, RENDER_TYPE RenderType);

	void SetHealth(UINT Health);
	UINT GetHealth() const;

	void SetMovingDirection(const XMFLOAT3& MovingDirection);
	const XMFLOAT3& GetMovingDirection() const;

	CCamera* GetCamera() const;

	CStateMachine<CPlayer>* GetStateMachine() const;

	void SetAnimationController(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, const shared_ptr<LOADED_MODEL_INFO>& ModelInfo);
	CAnimationController* GetAnimationController() const;

	void SetAnimationClip(UINT ClipNum);
	UINT GetAnimationClip() const;

	void AcquirePistol();
	bool SwapWeapon(WEAPON_TYPE WeaponType);

	void Rotate(float Pitch, float Yaw, float Roll, float ElapsedTime);

	void ProcessInput(UINT InputMask, float ElapsedTime, const shared_ptr<CNavMesh>& NavMesh);
};
