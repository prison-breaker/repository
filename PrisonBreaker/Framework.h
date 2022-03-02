#pragma once
#include "Timer.h"

struct CB_FRAMEWORKINFO
{
	float							  m_TotalTime{};
	float							  m_ElapsedTime{};
};

class CFramework
{
private:
	TCHAR							  m_Title[MAX_TITLE_LENGTH]{};
			
	bool							  m_IsActive{};
									  
	HINSTANCE						  m_hInstance{};
	HWND							  m_hWnd{};
	
	unique_ptr<CTimer>				  m_Timer{};

	bool							  m_Msaa4xEnable{};
	UINT							  m_Msaa4xQualityLevels{};
									  
	ComPtr<IDXGIFactory4>			  m_DXGIFactory{};
	ComPtr<ID3D12Device>			  m_D3D12Device{};
									  
	ComPtr<ID3D12CommandQueue>		  m_D3D12CommandQueue{};
	ComPtr<ID3D12CommandAllocator>	  m_D3D12CommandAllocator{};
	ComPtr<ID3D12GraphicsCommandList> m_D3D12GraphicsCommandList{};
									 
	ComPtr<IDXGISwapChain3>			  m_DXGISwapChain{};
	static const UINT				  m_SwapChainBufferCount{ 2 };
	UINT							  m_SwapChainBufferIndex{};
									  
	ComPtr<ID3D12Resource>			  m_D3D12RenderTargetBuffers[m_SwapChainBufferCount]{};
	ComPtr<ID3D12DescriptorHeap>	  m_D3D12RtvDescriptorHeap{};
	UINT							  m_RtvDescriptorIncrementSize{};
									  
	ComPtr<ID3D12Resource>			  m_D3D12DepthStencilBuffer{};
	ComPtr<ID3D12DescriptorHeap>	  m_D3D12DsvDescriptorHeap{};
	UINT							  m_DsvDescriptorIncrementSize{};
									  
	ComPtr<ID3D12Fence>				  m_D3D12Fence{};
	UINT64							  m_FenceValues[m_SwapChainBufferCount]{};
	HANDLE							  m_FenceEvent{};

	ComPtr<ID3D12Resource>			  m_D3D12FrameworkInfo{};
	CB_FRAMEWORKINFO*				  m_MappedFrameworkInfo{};
				
public:
	CFramework();
	~CFramework() = default;

	static CFramework* GetInstance();

	void UpdateWindowTitle();
	void SetActive(bool IsActive);

	void OnCreate(HINSTANCE hInstance, HWND hWnd);
	void OnDestroy();

	void BuildObjects();
	void ReleaseObjects();

	void CreateDevice();
	void CreateCommandQueueAndList();
	void CreateSwapChain();
	void CreateRtvAndDsvDescriptorHeaps();
	void CreateRenderTargetViews();
	void CreateDepthStencilView();

	void CreateShaderVariables();
	void UpdateShaderVariables();
	void ReleaseShaderVariables();

	void ChangeSwapChainState();

	void ResetCommandAllocatorAndList();
	void WaitForGpuComplete();
	void MoveToNextFrame();

	void ProcessMouseMessage(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
	void ProcessKeyboardMessage(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
	void ProcessInput();

	void Animate();
	void PreRender();
	void Render();

	void PopulateCommandList();
	void FrameAdvance();
};
