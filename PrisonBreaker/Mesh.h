#pragma once

class CMesh
{
protected:
	D3D12_PRIMITIVE_TOPOLOGY        m_D3D12PrimitiveTopology{ D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST };
							        
	UINT					        m_VertexCount{};
	vector<UINT>					m_IndexCounts{};
							        
	ComPtr<ID3D12Resource>	        m_D3D12PositionBuffer{};
	ComPtr<ID3D12Resource>	        m_D3D12PositionUploadBuffer{};
	D3D12_VERTEX_BUFFER_VIEW        m_D3D12PositionBufferView{};
							        
	ComPtr<ID3D12Resource>	        m_D3D12NormalBuffer{};
	ComPtr<ID3D12Resource>	        m_D3D12NormalUploadBuffer{};
	D3D12_VERTEX_BUFFER_VIEW        m_D3D12NormalBufferView{};
							        
	ComPtr<ID3D12Resource>	        m_D3D12TangentBuffer{};
	ComPtr<ID3D12Resource>	        m_D3D12TangentUploadBuffer{};
	D3D12_VERTEX_BUFFER_VIEW        m_D3D12TangentBufferView{};
							        
	ComPtr<ID3D12Resource>	        m_D3D12BiTangentBuffer{};
	ComPtr<ID3D12Resource>	        m_D3D12BiTangentUploadBuffer{};
	D3D12_VERTEX_BUFFER_VIEW        m_D3D12BiTangentBufferView{};

	ComPtr<ID3D12Resource>	        m_D3D12TexCoordBuffer{};
	ComPtr<ID3D12Resource>	        m_D3D12TexCoordUploadBuffer{};
	D3D12_VERTEX_BUFFER_VIEW        m_D3D12TexCoordBufferView{};
	
	ComPtr<ID3D12Resource>			m_D3D12BoundingBoxPositionBuffer{};
	ComPtr<ID3D12Resource>	        m_D3D12BoundingBoxPositionUploadBuffer{};
	D3D12_VERTEX_BUFFER_VIEW        m_D3D12BoundingBoxPositionBufferView{};

	vector<ComPtr<ID3D12Resource>>  m_D3D12IndexBuffers{};
	vector<ComPtr<ID3D12Resource>>  m_D3D12IndexUploadBuffers{};
	vector<D3D12_INDEX_BUFFER_VIEW>	m_D3D12IndexBufferViews{};
							 
	BoundingBox						m_BoundingBox{};

public:
	CMesh() = default;
	~CMesh() = default;

	void LoadMeshFromFile(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, tifstream& InFile);

	void ReleaseUploadBuffers();

	void SetBoundingBox(const BoundingBox& BoundingBox);
	const BoundingBox& GetBoundingBox() const;

	void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, UINT SubSetIndex);
	void RenderBoundingBox(ID3D12GraphicsCommandList* D3D12GraphicsCommandList);
};
