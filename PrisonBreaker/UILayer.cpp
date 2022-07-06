#include "stdafx.h"
#include "UILayer.h"

using namespace std;

CUILayer::CUILayer(ID3D12Device* D3D12Device, ID3D12CommandQueue* D3D12CommandQueue, UINT RenderTargetCount)
{
    m_D3D11WrappedRenderTargets.resize(RenderTargetCount);
    m_D2D1RenderTargets.resize(RenderTargetCount);

    UINT D3D11DeviceFlags{ D3D11_CREATE_DEVICE_BGRA_SUPPORT };
    D2D1_FACTORY_OPTIONS D3D1FactoryOptions{};

#ifdef _DEBUG
    D3D1FactoryOptions.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
    D3D11DeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    ComPtr<ID3D11Device> D3D11Device{};
    ComPtr<ID3D12CommandQueue> D3D12CommandQueues[]{ D3D12CommandQueue };

    DX::ThrowIfFailed(D3D11On12CreateDevice(D3D12Device, D3D11DeviceFlags, nullptr, 0, reinterpret_cast<IUnknown**>(D3D12CommandQueues->GetAddressOf()), _countof(D3D12CommandQueues), 0, reinterpret_cast<ID3D11Device**>(D3D11Device.GetAddressOf()), reinterpret_cast<ID3D11DeviceContext**>(m_D3D11DeviceContext.GetAddressOf()), nullptr));
    DX::ThrowIfFailed(D3D11Device->QueryInterface(__uuidof(ID3D11On12Device), reinterpret_cast<void**>(m_D3D11On12Device.GetAddressOf())));

#ifdef _DEBUG
    ComPtr<ID3D12InfoQueue> D3D12InfoQueue{};

    if (SUCCEEDED(D3D12Device->QueryInterface(IID_PPV_ARGS(D3D12InfoQueue.GetAddressOf()))))
    {
        D3D12_MESSAGE_SEVERITY D3D12MsgSeverities[]{  D3D12_MESSAGE_SEVERITY_INFO };
        D3D12_MESSAGE_ID D3D12MsgIDs[]{ D3D12_MESSAGE_ID_INVALID_DESCRIPTOR_HANDLE };
        D3D12_INFO_QUEUE_FILTER D3D12InfoQueueFilter{};

        D3D12InfoQueueFilter.DenyList.NumSeverities = _countof(D3D12MsgSeverities);
        D3D12InfoQueueFilter.DenyList.pSeverityList = D3D12MsgSeverities;
        D3D12InfoQueueFilter.DenyList.NumIDs = _countof(D3D12MsgIDs);
        D3D12InfoQueueFilter.DenyList.pIDList = D3D12MsgIDs;

        DX::ThrowIfFailed(D3D12InfoQueue->PushStorageFilter(&D3D12InfoQueueFilter));
    }
#endif

    ComPtr<IDXGIDevice> IDxgiDevice{};

    DX::ThrowIfFailed(m_D3D11On12Device->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(IDxgiDevice.GetAddressOf())));
    DX::ThrowIfFailed(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory3), &D3D1FactoryOptions, reinterpret_cast<void**>(m_D2D1Factory.GetAddressOf())));
    DX::ThrowIfFailed(m_D2D1Factory->CreateDevice(IDxgiDevice.Get(), reinterpret_cast<ID2D1Device2**>(m_D2D1Device.GetAddressOf())));
    DX::ThrowIfFailed(m_D2D1Device->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, reinterpret_cast<ID2D1DeviceContext2**>(m_D2D1DeviceContext.GetAddressOf())));
    DX::ThrowIfFailed(m_D2D1DeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), reinterpret_cast<ID2D1SolidColorBrush**>(m_D2D1TextBrush.GetAddressOf())));
    DX::ThrowIfFailed(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(m_DWriteFactory.GetAddressOf())));

    m_D2D1DeviceContext->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
}

void CUILayer::SetActive(bool IsActive)
{
    m_IsActive = IsActive;
}

bool CUILayer::IsActive() const
{
    return m_IsActive;
}

void CUILayer::UpdateText(WPARAM wParam)
{
    if (m_IsActive)
    {
        if (wParam == VK_BACK && m_TextBlock.m_TextPos > 0)
        {
            m_TextBlock.m_Text[--m_TextBlock.m_TextPos] = NULL;
        }
        else if (m_TextBlock.m_TextPos <= m_TextBlock.m_Text.length())
        {
            m_TextBlock.m_Text[m_TextBlock.m_TextPos++] = static_cast<WCHAR>(wParam);
        }
    }
}

tstring CUILayer::GetText() const
{
    return tstring{ m_TextBlock.m_Text.begin(), m_TextBlock.m_Text.end() };
}

void CUILayer::Render(UINT RenderTargetIndex)
{
    ComPtr<ID3D11Resource> D3D11Resources[]{ m_D3D11WrappedRenderTargets[RenderTargetIndex].Get() };

    m_D2D1DeviceContext->SetTarget(m_D2D1RenderTargets[RenderTargetIndex].Get());
    m_D3D11On12Device->AcquireWrappedResources(D3D11Resources->GetAddressOf(), _countof(D3D11Resources));

    if (m_IsActive)
    {
        m_D2D1DeviceContext->BeginDraw();
        m_D2D1DeviceContext->DrawText(m_TextBlock.m_Text.c_str(), static_cast<UINT>(m_TextBlock.m_Text.length()), m_TextBlock.m_WriteFormat.Get(), m_TextBlock.m_D2D1Rect, m_D2D1TextBrush.Get());
        DX::ThrowIfFailed(m_D2D1DeviceContext->EndDraw());
    }

    m_D3D11On12Device->ReleaseWrappedResources(D3D11Resources->GetAddressOf(), _countof(D3D11Resources));
    m_D3D11DeviceContext->Flush();
}

void CUILayer::ReleaseResources()
{
    const UINT RenderTargetCount{ static_cast<UINT>(m_D2D1RenderTargets.size()) };

    for (UINT i = 0; i < RenderTargetCount; ++i)
    {
        ComPtr<ID3D11Resource> D3D11Resources[] = { m_D3D11WrappedRenderTargets[i].Get() };

        m_D3D11On12Device->ReleaseWrappedResources(D3D11Resources->GetAddressOf(), _countof(D3D11Resources));
    }

    m_D2D1DeviceContext->SetTarget(nullptr);
    m_D3D11DeviceContext->Flush();
}

void CUILayer::Resize(ID3D12Resource** D3D12RenderTargets, UINT Width, UINT Height)
{
    m_Width = static_cast<float>(Width);
    m_Height = static_cast<float>(Height);

    D2D1_BITMAP_PROPERTIES1 D2D1BitmapProperties{ D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW, D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED)) };
    const UINT RenderTargetCount{ static_cast<UINT>(m_D2D1RenderTargets.size()) };

    for (UINT i = 0; i < RenderTargetCount; ++i)
    {
        D3D11_RESOURCE_FLAGS D3D11Flags = { D3D11_BIND_RENDER_TARGET };

        m_D3D11On12Device->CreateWrappedResource(D3D12RenderTargets[i], &D3D11Flags, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT, IID_PPV_ARGS(m_D3D11WrappedRenderTargets[i].GetAddressOf()));
        
        ComPtr<IDXGISurface> DxgiSurface{};

        m_D3D11WrappedRenderTargets[i]->QueryInterface(__uuidof(IDXGISurface), reinterpret_cast<void **>(DxgiSurface.GetAddressOf()));
        m_D2D1DeviceContext->CreateBitmapFromDxgiSurface(DxgiSurface.Get(), D2D1BitmapProperties, m_D2D1RenderTargets[i].GetAddressOf());
    }

    if (m_D2D1DeviceContext)
    {
        m_D2D1DeviceContext.ReleaseAndGetAddressOf();
    }

    DX::ThrowIfFailed(m_D2D1Device->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, reinterpret_cast<ID2D1DeviceContext2**>(m_D2D1DeviceContext.GetAddressOf())));
    m_D2D1DeviceContext->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
    
    if (m_D2D1TextBrush)
    {
        m_D2D1TextBrush.ReleaseAndGetAddressOf();
    }
    
    DX::ThrowIfFailed(m_D2D1DeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), reinterpret_cast<ID2D1SolidColorBrush**>(m_D2D1TextBrush.GetAddressOf())));

    const float FontSize = m_Height / 30.0f;

    DX::ThrowIfFailed(m_DWriteFactory->CreateTextFormat(L"capture it", nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, FontSize, L"en-us", &m_DWTextFormat));
    DX::ThrowIfFailed(m_DWTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER));
    DX::ThrowIfFailed(m_DWTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER));

    m_TextBlock = { L"", 0, D2D1::RectF(0.0f, 0.0f, m_Width, m_Height), m_DWTextFormat.Get() };
    m_TextBlock.m_Text.resize(20);
}
