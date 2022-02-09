#pragma once
#include "BilboardMesh.h"
#include "Camera.h"

class CBilboardObject
{
protected:
	shared_ptr<CTexture>	 m_Texture{};

	ComPtr<ID3D12Resource>	 m_D3D12PositionBuffer{};
	ComPtr<ID3D12Resource>	 m_D3D12PositionUploadBuffer{};
	D3D12_VERTEX_BUFFER_VIEW m_D3D12PositionBufferView{};

public:
	CBilboardObject() = default;
	virtual ~CBilboardObject() = default;

	virtual void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera);

	void ReleaseUploadBuffers();

	void SetTexture(const shared_ptr<CTexture>& Texture);
};
