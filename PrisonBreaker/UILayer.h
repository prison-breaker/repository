#pragma once

struct TEXT_BLOCK
{
    string                    m_Text{};
    int                       m_TextPos{};

    D2D1_RECT_F               m_D2D1Rect{};
    ComPtr<IDWriteTextFormat> m_WriteFormat{};
};

class CUILayer
{
private:
    bool                           m_isActive{};

    float                          m_Width{};
    float                          m_Height{};
                                   
    ComPtr<ID3D11DeviceContext>    m_D3D11DeviceContext{};
    ComPtr<ID3D11On12Device>       m_D3D11On12Device{};

    ComPtr<ID2D1Factory3>          m_D2D1Factory{};
    ComPtr<ID2D1Device2>           m_D2D1Device{};
    ComPtr<ID2D1DeviceContext2>    m_D2D1DeviceContext{};
    ComPtr<ID2D1SolidColorBrush>   m_D2D1TextBrush{};

    ComPtr<IDWriteFactory>         m_DWriteFactory{};
    ComPtr<IDWriteTextFormat>      m_DWTextFormat{};

    vector<ComPtr<ID3D11Resource>> m_D3D11WrappedRenderTargets{};
    vector<ComPtr<ID2D1Bitmap1>>   m_D2D1RenderTargets{};

    TEXT_BLOCK                     m_TextBlock{};

public:
    CUILayer(ID3D12Device* d3d12Device, ID3D12CommandQueue* D3D12CommandQueue, UINT RenderTargetCount);
    ~CUILayer() = default;

    void SetActive(bool IsActive);
    bool IsActive() const;

    void UpdateText(WPARAM wParam);
    const string& GetText() const;

    void Render(UINT RenderTargetIndex);
    void ReleaseResources();
    void Resize(ID3D12Resource** D3D12RenderTargets, UINT Width, UINT Height);
};
