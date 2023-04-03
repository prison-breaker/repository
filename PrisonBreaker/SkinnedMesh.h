#pragma once
#include "Mesh.h"

class CObject;

class CSkinnedMesh : public CMesh
{
	friend class CAssetManager;

private:
	int						   m_boneCount;
							   
	ComPtr<ID3D12Resource>	   m_d3d12BoneIndexBuffer;
	ComPtr<ID3D12Resource>	   m_d3d12BoneIndexUploadBuffer;
	D3D12_VERTEX_BUFFER_VIEW   m_d3d12BoneIndexBufferView;
							   
	ComPtr<ID3D12Resource>	   m_d3d12BoneWeightBuffer;
	ComPtr<ID3D12Resource>	   m_d3d12BoneWeightUploadBuffer;
	D3D12_VERTEX_BUFFER_VIEW   m_d3d12BoneWeightBufferView;
							   
	vector<XMFLOAT4X4>		   m_boneOffsetMatrixes;
	ComPtr<ID3D12Resource>	   m_d3d12BoneOffsetMatrixes;
	XMFLOAT4X4*				   m_mappedBoneOffsetMatrixes;

	// �Ʒ� �������� ������ ���� �������� ��ü�� Animator ������Ʈ�� �ִ� ������ �����ϴ� �������̴�.
	vector<CObject*>*		   m_boneFrameCache;

	ComPtr<ID3D12Resource>	   m_d3d12BoneTransformMatrixes;
	XMFLOAT4X4*				   m_mappedBoneTransformMatrixes;

private:
	CSkinnedMesh(const CMesh& rhs);

public:
	virtual ~CSkinnedMesh();

	void SetBoneInfo(vector<CObject*>* boneFrameCache, const ComPtr<ID3D12Resource>& d3d12BoneTransformMatrixes, XMFLOAT4X4* mappedBoneTransformMatrixes);

	void LoadSkinInfo(ID3D12Device* d3d12Device, ID3D12GraphicsCommandList* d3d12GraphicsCommandList, ifstream& in);

	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* d3d12GraphicsCommandList);

	virtual void ReleaseUploadBuffers();

	virtual void Render(ID3D12GraphicsCommandList* d3d12GraphicsCommandList, int subSetIndex);
};
