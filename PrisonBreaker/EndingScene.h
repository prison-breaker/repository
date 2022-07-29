#pragma once
#include "Scene.h"

class CGameObject;
class CQuadObject;

class CEndingScene : public CScene
{
private:
	vector<vector<shared_ptr<CGameObject>>>& m_GameObjects;
	vector<shared_ptr<CQuadObject>>		     m_QuadObjects{};

public:
	CEndingScene(vector<vector<shared_ptr<CGameObject>>>& GameObjects, shared_ptr<CQuadObject>& SkyBox);
	virtual ~CEndingScene() = default;

	virtual void Initialize();

	virtual void OnCreate(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, ID3D12RootSignature* D3D12RootSignature);
	virtual void OnDestroy();

	virtual void BuildObjects(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, ID3D12RootSignature* D3D12RootSignature);
	virtual void ReleaseObjects();

	virtual void Enter(MSG_TYPE MsgType);
	virtual void Exit();

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

	virtual void ProcessPacket();
};
