#pragma once
#include "Scene.h"
#include "BilboardObject.h"
#include "UIShader.h"

class CTitleScene : public CScene
{
private:
	// TilteScene에는 별도의 카메라가 없기 때문에 뷰포트와 시져렉트 영역을 멤버변수로 갖는다.
	D3D12_VIEWPORT						m_ViewPort{ 0, 0, CLIENT_WIDTH, CLIENT_HEIGHT, 0.0f, 1.0f };
	D3D12_RECT							m_ScissorRect{ 0, 0, CLIENT_WIDTH, CLIENT_HEIGHT };

	vector<shared_ptr<CBilboardObject>> m_BilboardObjects{};

	bool								m_ButtonOver{};
	XMFLOAT4							m_ButtonArea[3]{};

public:
	CTitleScene() = default;
	virtual ~CTitleScene() = default;

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

	// Server Function
	virtual void ProcessPacket();

	void RSSetViewportsAndScissorRects(ID3D12GraphicsCommandList* D3D12GraphicsCommandList);
};
