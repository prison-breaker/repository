#include "stdafx.h"
#include "Framework.h"
#include "GameScene.h"

CFramework::CFramework()
{
	m_Timer = make_unique<CTimer>();
	_tcscpy_s(m_Title, TEXT("PrisonBreaker ("));
}

void CFramework::UpdateWindowTitle()
{
	m_Timer->GetFrameRate(m_Title + 15, 48);
	SetWindowText(m_hWnd, m_Title);
}

CFramework* CFramework::GetInstance()
{
	static CFramework Instance{};

	return &Instance;
}

void CFramework::SetActive(bool IsActive)
{
	m_IsActive = IsActive;
}

void CFramework::OnCreate(HINSTANCE hInstance, HWND hWnd)
{
	m_hInstance = hInstance;
	m_hWnd = hWnd;

	CreateDevice();
	CreateCommandQueueAndList();
	CreateSwapChain();
	CreateRtvAndDsvDescriptorHeaps();
	CreateRenderTargetViews();
	CreateDepthStencilView();
	CreateShaderVariables();
	BuildObjects();

	//==========================================================================
	// ���ӽ��۽� Ŀ���� �߾ӿ� ��ġ�ϵ��� �Ͽ� ī�޶� ���ư��� �ʵ��� �Ѵ�.
	// ���߿� �� �ڵ�� �Űܾ� �Ѵ�.
	RECT Rect{};

	GetWindowRect(hWnd, &Rect);

	POINT OldCursorPos{ Rect.right / 2, Rect.bottom / 2 };

	SetCursorPos(OldCursorPos.x, OldCursorPos.y);
	ShowCursor(FALSE);
	//==========================================================================
}

void CFramework::OnDestroy()
{
	WaitForGpuComplete();
	ReleaseObjects();
	CloseHandle(m_FenceEvent);

	m_DXGISwapChain->SetFullscreenState(FALSE, nullptr);
}

void CFramework::BuildObjects()
{
	DX::ThrowIfFailed(m_D3D12GraphicsCommandList->Reset(m_D3D12CommandAllocator.Get(), nullptr));

	shared_ptr<CScene> GameScene{ make_shared<CGameScene>() };

	GameScene->OnCreate(m_D3D12Device.Get(), m_D3D12GraphicsCommandList.Get());
	CSceneManager::GetInstance()->RegisterScene(TEXT("GameScene"), GameScene);
	CSceneManager::GetInstance()->ChangeScene(TEXT("GameScene"));

	DX::ThrowIfFailed(m_D3D12GraphicsCommandList->Close());

	ComPtr<ID3D12CommandList> D3D12CommandLists[] = { m_D3D12GraphicsCommandList.Get() };

	m_D3D12CommandQueue->ExecuteCommandLists(_countof(D3D12CommandLists), D3D12CommandLists->GetAddressOf());

	WaitForGpuComplete();

	GameScene->ReleaseUploadBuffers();
	CTextureManager::GetInstance()->ReleaseUploadBuffers();

	m_Timer->Reset();
}

void CFramework::ReleaseObjects()
{

}

void CFramework::CreateDevice()
{
	UINT FlagCount{};

#ifdef DEBUG_MODE
	ComPtr<ID3D12Debug> D3D12DebugController{};

	DX::ThrowIfFailed(D3D12GetDebugInterface(__uuidof(ID3D12Debug), reinterpret_cast<void**>(D3D12DebugController.GetAddressOf())));

	if (D3D12DebugController)
	{
		D3D12DebugController->EnableDebugLayer();
	}

	FlagCount |= DXGI_CREATE_FACTORY_DEBUG;
#endif

	DX::ThrowIfFailed(CreateDXGIFactory2(FlagCount, __uuidof(IDXGIFactory4), reinterpret_cast<void**>(m_DXGIFactory.GetAddressOf())));

	ComPtr<IDXGIAdapter1> DXGIAdapter{};

	for (UINT i = 0; DXGI_ERROR_NOT_FOUND != m_DXGIFactory->EnumAdapters1(i, DXGIAdapter.GetAddressOf()); ++i)
	{
		DXGI_ADAPTER_DESC1 DXGIAdapterDesc{};
		HRESULT Result{};

		DXGIAdapter->GetDesc1(&DXGIAdapterDesc);

		if (DXGIAdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
		{
			continue;
		}

		Result = D3D12CreateDevice(DXGIAdapter.Get(), D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), reinterpret_cast<void**>(m_D3D12Device.GetAddressOf()));

		if (SUCCEEDED(Result))
		{
			break;
		}
		else
		{
			DX::ThrowIfFailed(Result);
		}
	}

	// ��� �ϵ���� ����� ���Ͽ� Ư�� ���� 12.0�� �����ϴ� �ϵ���� ����̽��� �����Ѵ�.
	if (!DXGIAdapter)
	{
		DX::ThrowIfFailed(m_DXGIFactory->EnumWarpAdapter(_uuidof(IDXGIAdapter1), reinterpret_cast<void**>(DXGIAdapter.GetAddressOf())));
		DX::ThrowIfFailed(D3D12CreateDevice(DXGIAdapter.Get(), D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), reinterpret_cast<void**>(m_D3D12Device.GetAddressOf())));
	}

	// Ư�� ���� 12.0�� �����ϴ� �ϵ���� ����̽��� ������ �� ������ WARP ����̽��� �����Ѵ�.
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS D3D12MsaaQualityLevels{};

	D3D12MsaaQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	D3D12MsaaQualityLevels.SampleCount = 4;

	// Msaa4x ���� ���ø�
	D3D12MsaaQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	D3D12MsaaQualityLevels.NumQualityLevels = 0;

	// ����̽��� �����ϴ� ���� ������ ǰ�� ������ Ȯ���Ѵ�.
	DX::ThrowIfFailed(m_D3D12Device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &D3D12MsaaQualityLevels, sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS)));

	// ���� ������ ǰ�� ������ 1���� ũ�� ���� ���ø��� Ȱ��ȭ�Ѵ�.
	m_Msaa4xQualityLevels = D3D12MsaaQualityLevels.NumQualityLevels;
	m_Msaa4xEnable = (m_Msaa4xQualityLevels > 1) ? true : false;

	// �潺�� �����ϰ� �潺 ���� 0���� �����Ѵ�.
	DX::ThrowIfFailed(m_D3D12Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), reinterpret_cast<void**>(m_D3D12Fence.GetAddressOf())));

	// �潺�� ����ȭ�� ���� �̺�Ʈ ��ü�� �����Ѵ�.(�̺�Ʈ ��ü�� �ʱⰪ�� FALSE�̴�.)
	// �̺�Ʈ�� ����Ǹ�(Signal) �̺�Ʈ�� ���� �ڵ������� FALSE�� �ǵ��� �����Ѵ�.
	m_FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
}

void CFramework::CreateCommandQueueAndList()
{
	D3D12_COMMAND_QUEUE_DESC D3D12CommandQueueDesc{};

	D3D12CommandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	D3D12CommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	DX::ThrowIfFailed(m_D3D12Device->CreateCommandQueue(&D3D12CommandQueueDesc, _uuidof(ID3D12CommandQueue), reinterpret_cast<void**>(m_D3D12CommandQueue.GetAddressOf())));
	DX::ThrowIfFailed(m_D3D12Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), reinterpret_cast<void**>(m_D3D12CommandAllocator.GetAddressOf())));
	DX::ThrowIfFailed(m_D3D12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_D3D12CommandAllocator.Get(), nullptr, __uuidof(ID3D12GraphicsCommandList), reinterpret_cast<void**>(m_D3D12GraphicsCommandList.GetAddressOf())));

	// ��� ����Ʈ�� �����Ǹ� ����(Open) �����̹Ƿ� ����(Closed) ���·� �����.
	DX::ThrowIfFailed(m_D3D12GraphicsCommandList->Close());
}

void CFramework::CreateSwapChain()
{
	DXGI_SWAP_CHAIN_DESC DXGISwapChainDesc{};

	DXGISwapChainDesc.BufferDesc.Width = CLIENT_WIDTH;
	DXGISwapChainDesc.BufferDesc.Height = CLIENT_HEIGHT;
	DXGISwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGISwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	DXGISwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	DXGISwapChainDesc.SampleDesc.Count = (m_Msaa4xEnable) ? 4 : 1;
	DXGISwapChainDesc.SampleDesc.Quality = (m_Msaa4xEnable) ? (m_Msaa4xQualityLevels - 1) : 0;
	DXGISwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	DXGISwapChainDesc.BufferCount = m_SwapChainBufferCount;
	DXGISwapChainDesc.OutputWindow = m_hWnd;
	DXGISwapChainDesc.Windowed = true;
	DXGISwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	DXGISwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	DX::ThrowIfFailed(m_DXGIFactory->CreateSwapChain(m_D3D12CommandQueue.Get(), &DXGISwapChainDesc, (IDXGISwapChain**)m_DXGISwapChain.GetAddressOf()));

	// ����ü���� ���� �ĸ���� �ε����� �����Ѵ�.
	m_SwapChainBufferIndex = m_DXGISwapChain->GetCurrentBackBufferIndex();

	// "Alt+Enter" Ű�� ������ ��Ȱ��ȭ�Ѵ�.
	DX::ThrowIfFailed(m_DXGIFactory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER));
}

void CFramework::CreateRtvAndDsvDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC D3D12DescriptorHeapDesc{};
	
	D3D12DescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	D3D12DescriptorHeapDesc.NumDescriptors = m_SwapChainBufferCount;
	D3D12DescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	D3D12DescriptorHeapDesc.NodeMask = 0;

	// ���� Ÿ�� ������ ��(�������� ������ ����ü�� ������ ����)�� �����Ѵ�.
	DX::ThrowIfFailed(m_D3D12Device->CreateDescriptorHeap(&D3D12DescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), reinterpret_cast<void**>(m_D3D12RtvDescriptorHeap.GetAddressOf())));

	// ���� Ÿ�� ������ ���� ������ ũ�⸦ �����Ѵ�.
	m_RtvDescriptorIncrementSize = m_D3D12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	// ����-���ٽ� ������ ��(�������� ������ 1)�� �����Ѵ�.
	D3D12DescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	D3D12DescriptorHeapDesc.NumDescriptors = 1;

	DX::ThrowIfFailed(m_D3D12Device->CreateDescriptorHeap(&D3D12DescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), reinterpret_cast<void**>(m_D3D12DsvDescriptorHeap.GetAddressOf())));

	// ����-���ٽ� ������ ���� ������ ũ�⸦ �����Ѵ�.
	m_DsvDescriptorIncrementSize = m_D3D12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}

void CFramework::CreateRenderTargetViews()
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE D3D12RtvCPUDescriptorHandle{ m_D3D12RtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart() };

	for (UINT i = 0; i < m_SwapChainBufferCount; ++i)
	{
		DX::ThrowIfFailed(m_DXGISwapChain->GetBuffer(i, __uuidof(ID3D12Resource), reinterpret_cast<void**>(m_D3D12RenderTargetBuffers[i].GetAddressOf())));
		m_D3D12Device->CreateRenderTargetView(m_D3D12RenderTargetBuffers[i].Get(), nullptr, D3D12RtvCPUDescriptorHandle);

		D3D12RtvCPUDescriptorHandle.ptr += m_RtvDescriptorIncrementSize;
	}
}

void CFramework::CreateDepthStencilView()
{
	CD3DX12_RESOURCE_DESC D3D12ResourceDesc{ D3D12_RESOURCE_DIMENSION_TEXTURE2D, 0, CLIENT_WIDTH, CLIENT_HEIGHT, 1, 1, DXGI_FORMAT_D24_UNORM_S8_UINT,
		(m_Msaa4xEnable) ? (UINT)4 : (UINT)1, (m_Msaa4xEnable) ? (m_Msaa4xQualityLevels - 1) : 0, D3D12_TEXTURE_LAYOUT_UNKNOWN, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL };
	CD3DX12_HEAP_PROPERTIES D3D312HeapProperties{ D3D12_HEAP_TYPE_DEFAULT, 1, 1 };
	CD3DX12_CLEAR_VALUE D3D12ClearValue{ DXGI_FORMAT_D24_UNORM_S8_UINT, 1.0f, 0 };

	// ����-���ٽ� ���۸� �����Ѵ�.
	DX::ThrowIfFailed(m_D3D12Device->CreateCommittedResource(&D3D312HeapProperties, D3D12_HEAP_FLAG_NONE, &D3D12ResourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&D3D12ClearValue, __uuidof(ID3D12Resource), reinterpret_cast<void**>(m_D3D12DepthStencilBuffer.GetAddressOf())));

	CD3DX12_CPU_DESCRIPTOR_HANDLE D3D12DsvCPUDescriptorHandle{ m_D3D12DsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart() };

	// ����-���ٽ� ���� �並 �����Ѵ�.
	m_D3D12Device->CreateDepthStencilView(m_D3D12DepthStencilBuffer.Get(), nullptr, D3D12DsvCPUDescriptorHandle);
}

void CFramework::CreateShaderVariables()
{
	UINT Bytes{ (sizeof(CB_FRAMEWORKINFO) + 255) & ~255 };

	m_D3D12FrameworkInfo = DX::CreateBufferResource(m_D3D12Device.Get(), m_D3D12GraphicsCommandList.Get(), nullptr, Bytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, nullptr);
	DX::ThrowIfFailed(m_D3D12FrameworkInfo->Map(0, nullptr, reinterpret_cast<void**>(&m_MappedFrameworkInfo)));
}

void CFramework::UpdateShaderVariables()
{
	m_MappedFrameworkInfo->m_TotalTime += m_Timer->GetElapsedTime();
	m_MappedFrameworkInfo->m_ElapsedTime = m_Timer->GetElapsedTime();

	m_D3D12GraphicsCommandList->SetGraphicsRootConstantBufferView(ROOT_PARAMETER_TYPE_FRAMEWORK_INFO, m_D3D12FrameworkInfo->GetGPUVirtualAddress());
}

void CFramework::ReleaseShaderVariables()
{
	m_D3D12FrameworkInfo->Unmap(0, nullptr);
}

void CFramework::ChangeSwapChainState()
{
	WaitForGpuComplete();

	BOOL FullScreenState{};

	m_DXGISwapChain->GetFullscreenState(&FullScreenState, nullptr);
	m_DXGISwapChain->SetFullscreenState(!FullScreenState, nullptr);

	DXGI_MODE_DESC DXGIModeDesc{};

	DXGIModeDesc.Width = CLIENT_WIDTH;
	DXGIModeDesc.Height = CLIENT_HEIGHT;
	DXGIModeDesc.RefreshRate.Numerator = 60;
	DXGIModeDesc.RefreshRate.Denominator = 1;
	DXGIModeDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGIModeDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	DXGIModeDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	DX::ThrowIfFailed(m_DXGISwapChain->ResizeTarget(&DXGIModeDesc));

	DXGI_SWAP_CHAIN_DESC DXGISwapChainDesc{};

	DX::ThrowIfFailed(m_DXGISwapChain->GetDesc(&DXGISwapChainDesc));
	DX::ThrowIfFailed(m_DXGISwapChain->ResizeBuffers(m_SwapChainBufferCount, CLIENT_WIDTH, CLIENT_HEIGHT, DXGISwapChainDesc.BufferDesc.Format, DXGISwapChainDesc.Flags));

	m_SwapChainBufferIndex = m_DXGISwapChain->GetCurrentBackBufferIndex();

	CreateRenderTargetViews();
}

void CFramework::ResetCommandAllocatorAndList()
{
	DX::ThrowIfFailed(m_D3D12CommandAllocator->Reset());
	DX::ThrowIfFailed(m_D3D12GraphicsCommandList->Reset(m_D3D12CommandAllocator.Get(), nullptr));
}

void CFramework::WaitForGpuComplete()
{
	const UINT64 FenceValue = ++m_FenceValues[m_SwapChainBufferIndex];

	// GPU�� �潺�� ���� �����ϴ� ����� ��� ť�� �߰��Ѵ�.
	DX::ThrowIfFailed(m_D3D12CommandQueue->Signal(m_D3D12Fence.Get(), FenceValue));

	// �潺�� ���� ���� ������ ������ ������ �潺�� ���� ���� ������ ���� �� ������ ��ٸ���.
	if (m_D3D12Fence->GetCompletedValue() < FenceValue)
	{
		DX::ThrowIfFailed(m_D3D12Fence->SetEventOnCompletion(FenceValue, m_FenceEvent));
		WaitForSingleObject(m_FenceEvent, INFINITE);
	}
}

void CFramework::MoveToNextFrame()
{
	// ������Ʈ�� �ϸ� ���� ���� Ÿ��(�ĸ����)�� ������ ������۷� �Ű����� ���� Ÿ�� �ε����� �ٲ� ���̴�.
	m_SwapChainBufferIndex = m_DXGISwapChain->GetCurrentBackBufferIndex();

	WaitForGpuComplete();
}

void CFramework::ProcessMouseMessage(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	CSceneManager::GetInstance()->ProcessMouseMessage(hWnd, Message, wParam, lParam);
}

void CFramework::ProcessKeyboardMessage(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch (Message)
	{
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		case VK_F12:
			ChangeSwapChainState();
			break;
		default:
			CSceneManager::GetInstance()->ProcessKeyboardMessage(hWnd, Message, wParam, lParam);
		}
	}
}

void CFramework::ProcessInput()
{
	if (m_IsActive)
	{
		CSceneManager::GetInstance()->ProcessInput(m_hWnd, m_Timer->GetElapsedTime());
	}
}

void CFramework::Animate()
{
	CSceneManager::GetInstance()->Animate(m_Timer->GetElapsedTime());
}

void CFramework::PreRender()
{
	CSceneManager::GetInstance()->PreRender(m_D3D12GraphicsCommandList.Get());
}

void CFramework::Render()
{
	CSceneManager::GetInstance()->Render(m_D3D12GraphicsCommandList.Get());
}

void CFramework::PostRender()
{
	CSceneManager::GetInstance()->PostRender(m_D3D12GraphicsCommandList.Get());
}

void CFramework::PopulateCommandList()
{
	ResetCommandAllocatorAndList();
	PreRender();
	UpdateShaderVariables();

	DX::ResourceTransition(m_D3D12GraphicsCommandList.Get(), m_D3D12RenderTargetBuffers[m_SwapChainBufferIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	CD3DX12_CPU_DESCRIPTOR_HANDLE D3D12RtvCPUDescriptorHandle{ m_D3D12RtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart() };
	CD3DX12_CPU_DESCRIPTOR_HANDLE D3D12DsvCPUDescriptorHandle{ m_D3D12DsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart() };

	D3D12RtvCPUDescriptorHandle = D3D12RtvCPUDescriptorHandle.Offset(m_SwapChainBufferIndex * m_RtvDescriptorIncrementSize);

	m_D3D12GraphicsCommandList->ClearRenderTargetView(D3D12RtvCPUDescriptorHandle, Colors::Black, 0, nullptr);
	m_D3D12GraphicsCommandList->ClearDepthStencilView(D3D12DsvCPUDescriptorHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
	m_D3D12GraphicsCommandList->OMSetRenderTargets(1, &D3D12RtvCPUDescriptorHandle, TRUE, &D3D12DsvCPUDescriptorHandle);

	Render();

	DX::ResourceTransition(m_D3D12GraphicsCommandList.Get(), m_D3D12RenderTargetBuffers[m_SwapChainBufferIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	DX::ThrowIfFailed(m_D3D12GraphicsCommandList->Close());

	ComPtr<ID3D12CommandList> D3D12CommandLists[] = { m_D3D12GraphicsCommandList.Get() };

	m_D3D12CommandQueue->ExecuteCommandLists(_countof(D3D12CommandLists), D3D12CommandLists->GetAddressOf());

	WaitForGpuComplete();
}

void CFramework::FrameAdvance()
{
	m_Timer->Tick(60.0f);

	ProcessInput();
	Animate();
	PopulateCommandList();

	DX::ThrowIfFailed(m_DXGISwapChain->Present(1, 0));

	MoveToNextFrame();
	UpdateWindowTitle();
}
