#pragma once
#include "BilboardMesh.h"
#include "StateMachine.h"

class CCamera;
class CMaterial;
class CUIAnimationController;

class CBilboardObject : public enable_shared_from_this<CBilboardObject>
{
protected:
	bool				                       m_IsActive{};
								               
	vector<shared_ptr<CMaterial>>              m_Materials{};
								               
	UINT						               m_VertexCount{};
								               
	ComPtr<ID3D12Resource>	                   m_D3D12VertexBuffer{};
	ComPtr<ID3D12Resource>	                   m_D3D12VertexUploadBuffer{};
	D3D12_VERTEX_BUFFER_VIEW                   m_D3D12VertexBufferView{};
								               
	CBilboardMesh*				               m_MappedImageInfo{};

	shared_ptr<CStateMachine<CBilboardObject>> m_StateMachine{};
	shared_ptr<CUIAnimationController>         m_UIAnimationController{};

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

	void SetMaterial(const shared_ptr<CMaterial>& Material);

	UINT GetVertexCount() const;

	void SetPosition(UINT Index, const XMFLOAT3& Position);
	void SetSize(UINT Index, const XMFLOAT2& Size);
	void SetCellIndex(UINT Index, UINT CellIndex);

	void SetAnimationClip(UINT ClipNum);
	void SetKeyFrameIndex(UINT ClipNum, UINT KeyFrameIndex);
};
