#pragma once
#include "QuadObject.h"

class CSkyBox : public CQuadObject
{
public:
	CSkyBox(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList);
	virtual ~CSkyBox() = default;

	virtual void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera, RENDER_TYPE RenderType);
};

//=========================================================================================================================

class CTree : public CQuadObject
{
public:
	CTree(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList);
	virtual ~CTree() = default;
};
