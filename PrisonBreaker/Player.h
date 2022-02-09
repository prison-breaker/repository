#pragma once
#include "GameObject.h"

class CPlayer : public CGameObject
{
private:
	float               m_Pitch{};
	float               m_Yaw{};
	float               m_Roll{};

	shared_ptr<CCamera> m_Camera{};

public:
	CPlayer() = default;
	virtual ~CPlayer() = default;

	void SetCamera(const shared_ptr<CCamera>& Camera);
	CCamera* GetCamera() const;

	void Rotate(float Pitch, float Yaw, float Roll, float ElapsedTime);
};
