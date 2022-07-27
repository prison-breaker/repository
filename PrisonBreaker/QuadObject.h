#pragma once

class CMaterial;
class CUIAnimationController;
class CCamera;

struct QUAD_INFO
{
	XMFLOAT3 m_Position{};
	XMFLOAT2 m_Size{};

	float	 m_AlphaColor{ 1.0f };

	XMUINT2  m_CellCount{};
	float	 m_CellIndex{};
};

class CQuadObject : public enable_shared_from_this<CQuadObject>
{
protected:
	bool				               m_IsActive{};
								       							   
	UINT						       m_MaxVertexCount{};
	UINT						       m_VertexCount{};
								       
	ComPtr<ID3D12Resource>	           m_D3D12VertexBuffer{};
	ComPtr<ID3D12Resource>	           m_D3D12VertexUploadBuffer{};
	D3D12_VERTEX_BUFFER_VIEW           m_D3D12VertexBufferView{};
								        
	QUAD_INFO*				           m_MappedQuadInfo{};
		
	vector<shared_ptr<CMaterial>>      m_Materials{};

	shared_ptr<CUIAnimationController> m_AnimationController{};

	vector<shared_ptr<CQuadObject>>    m_ChildObjects{};

public:
	CQuadObject() = default;
	virtual ~CQuadObject() = default;

	static shared_ptr<CQuadObject> LoadObjectInfoFromFile(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, tifstream& InFile, unordered_map<tstring, shared_ptr<CMaterial>>& MaterialCaches);
	static void LoadAnimationInfoFromFile(tifstream& InFile, const shared_ptr<CQuadObject>& Model);

	virtual void Initialize();
	virtual void Reset();

	virtual void ProcessMouseMessage(UINT Message, const XMINT2& ScreenPosition, UINT RootFrameIndex);

	virtual void Animate(float ElapsedTime);

	virtual void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera, RENDER_TYPE RenderType);

	virtual void ReleaseUploadBuffers();

	bool IsActive() const;
	void SetActive(bool IsActive);

	UINT GetMaxVertexCount() const;

	void SetVertexCount(UINT VertexCount);
	UINT GetVertexCount() const;

	void SetPosition(UINT Index, const XMFLOAT3& Position);
	const XMFLOAT3& GetPosition(UINT Index) const;

	void SetSize(UINT Index, const XMFLOAT2& Size);
	const XMFLOAT2& GetSize(UINT Index) const;

	void SetAlphaColor(UINT Index, float AlphaColor);
	float GetAlphaColor(UINT Index) const;

	void SetCellIndex(UINT Index, float CellIndex);
	float GetCellIndex(UINT Index) const;

	void SetMaterial(const shared_ptr<CMaterial>& Material);

	shared_ptr<CUIAnimationController> GetAnimationController() const;

	void SetChild(const shared_ptr<CQuadObject>& ChildObject);
};
