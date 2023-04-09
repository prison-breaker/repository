#pragma once

class CPostProcessingShader;
class CUILayer;

struct CB_Core
{
	float m_totalTime;
	float m_elapsedTime;
};

class CCore : public CSingleton<CCore>
{
	friend class CSingleton;

private:
	// 윈도우 관련 멤버 변수
	HWND							  m_hWnd;		// 메인 윈도우 핸들
	char							  m_title[64];  // 윈도우 타이틀
	XMFLOAT2						  m_resolution; // 윈도우 해상도

	// DirectX12 관련 멤버 변수
	bool							  m_msaa4xEnable;
	UINT							  m_msaa4xQualityLevels;
									  
	ComPtr<IDXGIFactory4>			  m_dxgiIFactory;
	ComPtr<ID3D12Device>			  m_d3d12Device;
									  
	ComPtr<ID3D12CommandQueue>		  m_d3d12CommandQueue;
	ComPtr<ID3D12CommandAllocator>	  m_d3d12CommandAllocator;
	ComPtr<ID3D12GraphicsCommandList> m_d3d12GraphicsCommandList;
									 
	ComPtr<IDXGISwapChain3>			  m_dxgiSwapChain;
	static const UINT				  m_swapChainBufferCount = 2;
	UINT							  m_swapChainBufferIndex;
									  
	ComPtr<ID3D12Resource>			  m_d3d12RenderTargetBuffers[m_swapChainBufferCount];
	ComPtr<ID3D12DescriptorHeap>	  m_d3d12RtvDescriptorHeap;
	UINT							  m_rtvDescriptorIncrementSize;
									  
	ComPtr<ID3D12Resource>			  m_d3d12DepthStencilBuffer;
	ComPtr<ID3D12Resource>			  m_d3d12DepthBuffer; // for DepthWrite
	ComPtr<ID3D12DescriptorHeap>	  m_d3d12DsvDescriptorHeap;
	UINT							  m_dsvDescriptorIncrementSize;
									  
	ComPtr<ID3D12DescriptorHeap>	  m_d3d12CbvSrvUavDescriptorHeap;

	ComPtr<ID3D12Fence>				  m_d3d12Fence;
	UINT64							  m_fenceValues[m_swapChainBufferCount];
	HANDLE							  m_fenceEvent;

	ComPtr<ID3D12RootSignature>		  m_d3d12RootSignature;

	ComPtr<ID3D12Resource>			  m_d3d12Core;
	CB_Core*				          m_mappedCore;

	//shared_ptr<CTexture>			  m_RenderingResultTexture;
	//shared_ptr<CPostProcessingShader> m_PostProcessingShader;
	
	//shared_ptr<CUILayer>			  m_UILayer;

	// 네트워크 통신 관련 변수
	//SOCKET_INFO						  m_SocketInfo;

private:
	CCore();
	~CCore();

	void CreateDevice();
	void CreateCommandQueueAndList();
	void CreateSwapChain();
	void CreateRtvAndDsvDescriptorHeaps();
	void CreateCbvSrvUavDescriptorHeaps();
	void CreateRenderTargetViews();
	void CreateDepthStencilView();
	void CreateShaderResourceViews();
	void CreateRootSignature();

	void CreateShaderVariables();
	void UpdateShaderVariables();
	void ReleaseShaderVariables();

	void ChangeSwapChainState();

	void ResetCommandAllocatorAndList();
	void WaitForGpuComplete();
	void MoveToNextFrame();

	void PreRender();
	void Render();
	void PostRender();

	void PopulateCommandList();

	//shared_ptr<CPostProcessingShader> GetPostProcessingShader() const;

	//shared_ptr<CUILayer> GetUILayer() const;

	//void ConnectServer();
	//void DisconnectServer();

	//void ProcessPacket();

	//const SOCKET_INFO& GetSocketInfo() const;

public:
	HWND GetHwnd();
	const XMFLOAT2& GetResolution();

	ID3D12DescriptorHeap* GetRtvDescriptorHeap();
	ID3D12DescriptorHeap* GetDsvDescriptorHeap();
	ID3D12DescriptorHeap* GetCbvSrvUavDescriptorHeap();

	UINT GetRtvDescriptorIncrementSize();
	UINT GetDsvDescriptorIncrementSize();

	void Init(HWND hWnd, const XMFLOAT2& resolution);

	void AdvanceFrame();
};
