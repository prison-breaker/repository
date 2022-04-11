#pragma once

class CGameObject;

class CMesh
{
protected:
	tstring						    m_Name{};

	D3D12_PRIMITIVE_TOPOLOGY        m_D3D12PrimitiveTopology{ D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST };
							        
	vector<XMFLOAT3>				m_Positions{};
	vector<vector<UINT>>			m_Indices{};
							        
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
	virtual ~CMesh() = default;

	virtual void CreateShaderVariables(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* D3D12GraphicsCommandList);
	virtual void ReleaseShaderVariables();

	virtual void ReleaseUploadBuffers();

	virtual void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, UINT SubSetIndex);

	void LoadMeshInfoFromFile(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, tifstream& InFile);

	void SetName(const tstring& Name);
	const tstring& GetName() const;

	void SetBoundingBox(const BoundingBox& BoundingBox);
	const BoundingBox& GetBoundingBox() const;

	void RenderBoundingBox(ID3D12GraphicsCommandList* D3D12GraphicsCommandList);

	bool CheckRayIntersection(const XMFLOAT3& RayOrigin, const XMFLOAT3& RayDirection, float& Distance);
};

//=========================================================================================================================

class CSkinnedMesh : public CMesh
{
private:
	UINT							m_BoneCount{};

	ComPtr<ID3D12Resource>	        m_D3D12BoneIndexBuffer{};
	ComPtr<ID3D12Resource>	        m_D3D12BoneIndexUploadBuffer{};
	D3D12_VERTEX_BUFFER_VIEW        m_D3D12BoneIndexBufferView{};

	ComPtr<ID3D12Resource>	        m_D3D12BoneWeightBuffer{};
	ComPtr<ID3D12Resource>	        m_D3D12BoneWeightUploadBuffer{};
	D3D12_VERTEX_BUFFER_VIEW        m_D3D12BoneWeightBufferView{};

	vector<XMFLOAT4X4>				m_BoneOffsetMatrixes{};
	ComPtr<ID3D12Resource>			m_D3D12BoneOffsetMatrixes{};
	XMFLOAT4X4*						m_MappedBoneOffsetMatrixes{};

	vector<shared_ptr<CGameObject>> m_BoneFrameCaches{};
	ComPtr<ID3D12Resource>			m_D3D12BoneTransformMatrixes{};
	XMFLOAT4X4*						m_MappedBoneTransformMatrixes{};

public:
	CSkinnedMesh() = default;
	CSkinnedMesh(const CSkinnedMesh& Rhs);
	virtual ~CSkinnedMesh() = default;

	virtual void CreateShaderVariables(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* D3D12GraphicsCommandList);
	virtual void ReleaseShaderVariables();

	virtual void ReleaseUploadBuffers();

	virtual void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, UINT SubSetIndex);

	void LoadSkinInfoFromFile(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, tifstream& InFile);
	
	void SetBoneFrameCaches(const vector <shared_ptr<CGameObject>>& BoneFrames);
	void SetBoneTransformInfo(const ComPtr<ID3D12Resource>& D3D12BoneTransformMatrixes, XMFLOAT4X4* MappedBoneTransformMatrixes);
};
