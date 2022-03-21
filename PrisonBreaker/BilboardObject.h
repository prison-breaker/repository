#pragma once
#include "BilboardMesh.h"

class CCamera;
class CMaterial;

class CBilboardObject
{
protected:
	bool				          m_IsActive{};

	XMFLOAT3					  m_Position{};
	float						  m_IndexTime{};

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

	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* D3D12GraphicsCommandList);

	virtual void Animate(float ElapsedTime);
	virtual void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera, RENDER_TYPE RenderType);

	virtual void ReleaseUploadBuffers();

	bool IsActive() const;
	void SetActive(bool IsActive);

	void SetPosition(const XMFLOAT3& Position);
	const XMFLOAT3& GetPosition() const;

	void SetIndexTime(float IndexTime);
	float GetIndexTime() const;

	void SetMaterial(const shared_ptr<CMaterial>& Material);
};
