#pragma once
#include "BilboardMesh.h"

class CCamera;
class CMaterial;

class CBilboardObject
{
protected:
	bool				          m_IsActive{};

	vector<shared_ptr<CMaterial>> m_Materials{};

	UINT						  m_VertexCount{};

	ComPtr<ID3D12Resource>	      m_D3D12VertexBuffer{};
	ComPtr<ID3D12Resource>	      m_D3D12VertexUploadBuffer{};
	D3D12_VERTEX_BUFFER_VIEW      m_D3D12VertexBufferView{};

	CBilboardMesh*				  m_MappedImageInfo{};

public:
	CBilboardObject() = default;
	virtual ~CBilboardObject() = default;

	static shared_ptr<CBilboardObject> LoadObjectInfoFromFile(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, tifstream& InFile);

	virtual void Animate(float ElapsedTime);
	virtual void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera, RENDER_TYPE RenderType);

	virtual void ReleaseUploadBuffers();

	bool IsActive() const;
	void SetActive(bool IsActive);

	void SetMaterial(const shared_ptr<CMaterial>& Material);
};
