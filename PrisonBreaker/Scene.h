#pragma once

class CScene
{
public:
	CScene() = default;
	virtual ~CScene() = default;

	virtual void OnCreate(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, ID3D12RootSignature* D3D12RootSignature) = 0;
	virtual void OnDestroy() = 0;

	virtual void BuildObjects(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, ID3D12RootSignature* D3D12RootSignature) = 0;
	virtual void ReleaseObjects() = 0;

	virtual void LoadSceneInfoFromFile(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, const tstring& FileName) = 0;
	virtual void LoadUIInfoFromFile(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, const tstring& FileName) = 0;

	virtual void CreateShaderVariables(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList) = 0;
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* D3D12GraphicsCommandList) = 0;
	virtual void ReleaseShaderVariables() = 0;

	virtual void ReleaseUploadBuffers() = 0;

	virtual void ProcessMouseMessage(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam) = 0;
	virtual void ProcessKeyboardMessage(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam) = 0;
	virtual void ProcessInput(HWND hWnd, float ElapsedTime) = 0;

	virtual void Animate(float ElapsedTime) = 0;

	virtual void PreRender(ID3D12GraphicsCommandList* D3D12GraphicsCommandList) = 0;
	virtual void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList) = 0;
	virtual void PostRender(ID3D12GraphicsCommandList* D3D12GraphicsCommandList) = 0;

	virtual void ApplyPacketData(const SERVER_TO_CLIENT_DATA& PacketData) = 0;
};
