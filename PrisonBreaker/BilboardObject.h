#pragma once
#include "BilboardMesh.h"
#include "Camera.h"

class CMaterial;

class CBilboardObject
{
protected:
	vector<shared_ptr<CMaterial>> m_Materials{};

	ComPtr<ID3D12Resource>	      m_D3D12PositionBuffer{};
	ComPtr<ID3D12Resource>	      m_D3D12PositionUploadBuffer{};
	D3D12_VERTEX_BUFFER_VIEW      m_D3D12PositionBufferView{};

public:
	CBilboardObject() = default;
	virtual ~CBilboardObject() = default;

	virtual void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera);

	void ReleaseUploadBuffers();

	void SetMaterial(const shared_ptr<CMaterial>& Material);
};
