#pragma once
#include "Scene.h"
#include "UIs.h"
#include "EventTriggers.h"
#include "ShadowMapShader.h"
#include "SkyBoxShader.h"
#include "UIShader.h"
#include "DebugShader.h"

struct FOG
{
	XMFLOAT4 m_Color{};

	float	 m_Density{};
};

struct CB_FOG
{
	FOG m_Fog{};
};

struct LIGHT
{
	bool	   m_IsActive{};
			   
	XMFLOAT3   m_Position{};
	XMFLOAT3   m_Direction{};
			   
	UINT	   m_Type{};
			   
	XMFLOAT4   m_Color{};
			   
	XMFLOAT3   m_Attenuation{};
	float 	   m_Falloff{};
	float	   m_Range{};
	float 	   m_Theta{};
	float	   m_Phi{};
			   
	bool	   m_ShadowMapping{};

	XMFLOAT4X4 m_ToTexCoordMatrix{};
};

struct CB_LIGHT
{
	LIGHT m_Lights[MAX_LIGHTS]{};
};

class CGameScene : public CScene
{
private:
	vector<vector<shared_ptr<CGameObject>>>     m_GameObjects{};
	vector<vector<shared_ptr<CBilboardObject>>> m_BilboardObjects{};

	vector<shared_ptr<CEventTrigger>>			m_EventTriggers{};

	shared_ptr<CNavMesh>					    m_NavMesh{};

	vector<LIGHT>						        m_Lights{};
	ComPtr<ID3D12Resource>				        m_D3D12Lights{};
	CB_LIGHT*							        m_MappedLights{};

	ComPtr<ID3D12Resource>						m_D3D12Fog{};
	CB_FOG*										m_MappedFog{};

	bool										m_InvincibleMode{};
	bool										m_RenderBoundingBox{};

	float                                       m_SpotLightAngle{ XMConvertToRadians(90.0f) };

public:
	CGameScene() = default;
	virtual ~CGameScene() = default;

	virtual void OnCreate(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, ID3D12RootSignature* D3D12RootSignature);
	virtual void OnDestroy();

	virtual void BuildObjects(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, ID3D12RootSignature* D3D12RootSignature);
	virtual void ReleaseObjects();

	virtual void LoadSceneInfoFromFile(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, const tstring& FileName);
	virtual void LoadUIInfoFromFile(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, const tstring& FileName);

	virtual void CreateShaderVariables(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* D3D12GraphicsCommandList);
	virtual void ReleaseShaderVariables();

	virtual void ReleaseUploadBuffers();

	virtual void ProcessMouseMessage(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
	virtual void ProcessKeyboardMessage(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
	virtual void ProcessInput(HWND hWnd, float ElapsedTime);

	virtual void Animate(float ElapsedTime);

	virtual void PreRender(ID3D12GraphicsCommandList* D3D12GraphicsCommandList);
	virtual void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList);
	virtual void PostRender(ID3D12GraphicsCommandList* D3D12GraphicsCommandList);

	vector<vector<shared_ptr<CGameObject>>>& GetGameObjects();
	vector<vector<shared_ptr<CBilboardObject>>>& GetBilboardObjects();

	vector<shared_ptr<CEventTrigger>>& GetEventTriggers();

	vector<LIGHT>& GetLights();

	shared_ptr<CNavMesh>& GetNavMesh();

	void LoadMeshCachesFromFile(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, const tstring& FileName, unordered_map<tstring, shared_ptr<CMesh>>& MeshCaches);
	void LoadMaterialCachesFromFile(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, const tstring& FileName, unordered_map<tstring, shared_ptr<CMaterial>>& MaterialCaches);
	void LoadEventTriggerFromFile(const tstring& FileName);

	void BuildLights();
	void BuildFog();

	void UpdatePerspective(HWND hWnd, float ElapsedTime, const shared_ptr<CPlayer>& Player);

	void InteractSpotLight(float ElapsedTime);

	bool IsInvincibleMode() const;
};
