#pragma once
#include "BilboardMesh.h"

class CCamera;
class CMaterial;
class CUIAnimationController;

class CBilboardObject : public enable_shared_from_this<CBilboardObject>
{
protected:
	bool				               m_IsActive{};
								       
	vector<shared_ptr<CMaterial>>      m_Materials{};
								       
	UINT						       m_VertexCount{};
								       
	ComPtr<ID3D12Resource>	           m_D3D12VertexBuffer{};
	ComPtr<ID3D12Resource>	           m_D3D12VertexUploadBuffer{};
	D3D12_VERTEX_BUFFER_VIEW           m_D3D12VertexBufferView{};
								       
	CBilboardMesh*				       m_MappedImageInfo{};

	shared_ptr<CUIAnimationController> m_UIAnimationController{};

public:
	CBilboardObject() = default;
	virtual ~CBilboardObject() = default;

	static shared_ptr<CBilboardObject> LoadObjectInfoFromFile(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, tifstream& InFile);
	static void LoadAnimationInfoFromFile(tifstream& InFile, const shared_ptr<CBilboardObject>& Model);

	virtual void Animate(float ElapsedTime);
	virtual void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera, RENDER_TYPE RenderType);

	virtual void ReleaseUploadBuffers();

	bool IsActive() const;
	void SetActive(bool IsActive);

	UINT GetVertexCount() const;

	void SetPosition(const XMFLOAT3& Position, UINT Index);
	void SetSize(const XMFLOAT2& Size, UINT Index);

	void SetCellIndex(UINT CellIndex, UINT Index);

	void SetMaterial(const shared_ptr<CMaterial>& Material);

	void SetAnimationClip(UINT ClipNum);
	void SetKeyFrameIndex(UINT ClipNum, UINT KeyFrameIndex);
};
