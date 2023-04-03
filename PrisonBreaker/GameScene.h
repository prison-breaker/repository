#pragma once
#include "Scene.h"

struct Fog
{
	XMFLOAT4 m_color;
	float	 m_density;
};

struct CB_Fog
{
	Fog m_fog;
};

struct Light
{
	bool	   m_isActive;
			   
	XMFLOAT3   m_position;
	XMFLOAT3   m_direction;
			   
	int		   m_type;
			   
	XMFLOAT4   m_color;
			   
	XMFLOAT3   m_attenuation;
	float 	   m_fallOff;
	float	   m_range;
	float 	   m_theta;
	float	   m_phi;
			   
	bool	   m_shadowMapping;

	XMFLOAT4X4 m_toTexCoord;
};

struct CB_Light
{
	Light m_lights[MAX_LIGHTS];
};

class CGameScene : public CScene
{
	friend class CSceneManager;

private:
	//INIT_GAME_DATA		   m_InitGameData{};

	vector<Light>		   m_lights;
	ComPtr<ID3D12Resource> m_d3d12Lights;
	CB_Light*			   m_mappedLights;
	CObject*			   m_towerLight;
	float				   m_towerLightAngle;

	ComPtr<ID3D12Resource> m_d3d12Fog;
	CB_Fog*                m_mappedFog;

private:
	CGameScene();

	virtual void Load(ID3D12Device* d3d12Device, ID3D12GraphicsCommandList* d3d12GraphicsCommandList, const string& fileName);

	virtual void Enter();
	virtual void Exit();

	virtual void CreateShaderVariables(ID3D12Device* d3d12Device, ID3D12GraphicsCommandList* d3d12GraphicsCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* d3d12GraphicsCommandList);
	virtual void ReleaseShaderVariables();

	void UpdateLightTower();

public:
	virtual ~CGameScene();

	const vector<Light>& GetLights();

	virtual void Init(ID3D12Device* d3d12Device, ID3D12GraphicsCommandList* d3d12GraphicsCommandList);

	virtual void Update();

	virtual void PreRender(ID3D12GraphicsCommandList* d3d12GraphicsCommandList);
	virtual void Render(ID3D12GraphicsCommandList* d3d12GraphicsCommandList);
};
