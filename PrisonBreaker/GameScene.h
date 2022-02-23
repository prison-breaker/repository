#pragma once
#include "Scene.h"
#include "SkyBoxShader.h"
#include "ShadowMapShader.h"
#include "DebugShader.h"

struct LIGHT
{
	bool								m_IsActive{};

	XMFLOAT3							m_Position{};
	XMFLOAT3							m_Direction{};

	int									m_Type{};

	XMFLOAT4							m_Color{};

	XMFLOAT3							m_Attenuation;
	float 								m_Falloff{};
	float								m_Range;
	float 								m_Theta{};
	float								m_Phi{};

	float								PADDING{};

	XMFLOAT4X4							m_ToTexCoordMatrix{};
};

struct CB_LIGHT
{
	LIGHT								m_Lights[MAX_LIGHTS]{};
};

class CGameScene : public CScene
{
private:
	vector<LIGHT>						m_Lights{};
	ComPtr<ID3D12Resource>				m_D3D12Lights{};
	CB_LIGHT*							m_MappedLights{};

	shared_ptr<CPlayer>				    m_Player{};
	vector<shared_ptr<CGameObject>>     m_Guards{};
	shared_ptr<CGameObject>				m_Ground{};
	vector<shared_ptr<CGameObject>>		m_Structures{};

	shared_ptr<CDepthWriteShader>		m_DepthWriteShader{};
	vector<shared_ptr<CGraphicsShader>> m_Shaders{};
	shared_ptr<CDebugShader>			m_DebugShader{};

public:
	CGameScene() = default;
	virtual ~CGameScene() = default;

	virtual void OnCreate(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList);
	virtual void OnDestroy();

	virtual void BuildObjects(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList);
	virtual void ReleaseObjects();

	virtual void CreateRootSignature(ID3D12Device* D3D12Device);

	virtual void CreateShaderVariables(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* D3D12GraphicsCommandList);
	virtual void ReleaseShaderVariables();

	virtual void ReleaseUploadBuffers();

	virtual void ProcessMouseMessage(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
	virtual void ProcessKeyboardMessage(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
	virtual void ProcessInput(HWND hWnd, float ElapsedTime);

	virtual void Animate(float ElapsedTime);
	virtual void PreRender(ID3D12GraphicsCommandList* D3D12GraphicsCommandList);
	virtual void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList) const;

	void LoadSceneFromFile(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, const tstring& FileName);
	void BuildLights();
};
