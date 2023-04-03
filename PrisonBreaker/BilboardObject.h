#pragma once
#include "Object.h"

struct QuadInfo
{
	XMFLOAT3 m_position;
	XMFLOAT2 m_size;

	XMUINT2  m_spriteSize;
	float	 m_spriteIndex;

	XMFLOAT3 m_color;
};

class CBilboardObject abstract : public CObject
{
protected:
	int						 m_maxVertexCount;
	int						 m_vertexCount;

	ComPtr<ID3D12Resource>	 m_d3d12VertexBuffer;
	ComPtr<ID3D12Resource>	 m_d3d12VertexUploadBuffer;
	D3D12_VERTEX_BUFFER_VIEW m_d3d12VertexBufferView;
	QuadInfo*				 m_mappedQuadInfo;

public:
	CBilboardObject(); 
	virtual ~CBilboardObject();

	virtual void ReleaseUploadBuffers();

	virtual void Render(ID3D12GraphicsCommandList* d3d12GraphicsCommandList, CCamera* camera);
};

//=========================================================================================================================

class CSkyBox : public CBilboardObject
{
public:
	CSkyBox(ID3D12Device* d3d12Device, ID3D12GraphicsCommandList* d3d12GraphicsCommandList);
	virtual ~CSkyBox();

	virtual void Render(ID3D12GraphicsCommandList* d3d12GraphicsCommandList, CCamera* camera);
};

//=========================================================================================================================

class CTree : public CBilboardObject
{
public:
	CTree(ID3D12Device* d3d12Device, ID3D12GraphicsCommandList* d3d12GraphicsCommandList, SCENE_TYPE sceneType);
	virtual ~CTree();
};
