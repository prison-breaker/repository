#pragma once
#include "Scene.h"

class CTitleScene : public CScene
{
	friend class CSceneManager;

private:
	CTitleScene();

	virtual void Enter();
	virtual void Exit();

public:
	virtual ~CTitleScene();

	virtual void Init(ID3D12Device* d3d12Device, ID3D12GraphicsCommandList* d3d12GraphicsCommandList);
};
