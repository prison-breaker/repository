#pragma once
#include "Asset.h"

class CMesh : public CAsset
{
	friend class CAssetManager;

protected:
	D3D12_PRIMITIVE_TOPOLOGY        m_d3d12PrimitiveTopology;
							        
	vector<XMFLOAT3>				m_positions;
	vector<vector<UINT>>			m_indices;
							        
	ComPtr<ID3D12Resource>	        m_d3d12PositionBuffer;
	ComPtr<ID3D12Resource>	        m_d3d12PositionUploadBuffer;
	D3D12_VERTEX_BUFFER_VIEW        m_d3d12PositionBufferView;
							        
	ComPtr<ID3D12Resource>	        m_d3d12NormalBuffer;
	ComPtr<ID3D12Resource>	        m_d3d12NormalUploadBuffer;
	D3D12_VERTEX_BUFFER_VIEW        m_d3d12NormalBufferView;
							        
	ComPtr<ID3D12Resource>	        m_d3d12TangentBuffer;
	ComPtr<ID3D12Resource>	        m_d3d12TangentUploadBuffer;
	D3D12_VERTEX_BUFFER_VIEW        m_d3d12TangentBufferView;
							        
	ComPtr<ID3D12Resource>	        m_d3d12BiTangentBuffer;
	ComPtr<ID3D12Resource>	        m_d3d12BiTangentUploadBuffer;
	D3D12_VERTEX_BUFFER_VIEW        m_d3d12BiTangentBufferView;

	ComPtr<ID3D12Resource>	        m_d3d12TexCoordBuffer;
	ComPtr<ID3D12Resource>	        m_d3d12TexCoordUploadBuffer;
	D3D12_VERTEX_BUFFER_VIEW        m_d3d12TexCoordBufferView;
	
	ComPtr<ID3D12Resource>			m_d3d12BoundingBoxPositionBuffer;
	ComPtr<ID3D12Resource>	        m_d3d12BoundingBoxPositionUploadBuffer;
	D3D12_VERTEX_BUFFER_VIEW        m_d3d12BoundingBoxPositionBufferView;

	vector<ComPtr<ID3D12Resource>>  m_d3d12IndexBuffers;
	vector<ComPtr<ID3D12Resource>>  m_d3d12IndexUploadBuffers;
	vector<D3D12_INDEX_BUFFER_VIEW>	m_d3d12IndexBufferViews;
							 
	BoundingBox						m_boundingBox;

protected:
	CMesh();
	CMesh(const CMesh& rhs);

public:
	virtual ~CMesh();

	const BoundingBox& GetBoundingBox();

	void Load(ID3D12Device* d3d12Device, ID3D12GraphicsCommandList* d3d12GraphicsCommandList, ifstream& in);

	virtual void CreateShaderVariables(ID3D12Device* d3d12Device, ID3D12GraphicsCommandList* d3d12GraphicsCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* d3d12GraphicsCommandList);
	virtual void ReleaseShaderVariables();

	virtual void ReleaseUploadBuffers();

	bool CheckRayIntersection(const XMFLOAT3& rayOrigin, const XMFLOAT3& rayDirection, const XMMATRIX& worldMatrix, float& hitDistance);

	virtual void Render(ID3D12GraphicsCommandList* d3d12GraphicsCommandList, int subSetIndex);
	void RenderBoundingBox(ID3D12GraphicsCommandList* d3d12GraphicsCommandList);
};
