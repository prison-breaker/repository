#pragma once
#include "BilboardObject.h"

class CSkyBox : public CBilboardObject
{
public:
	CSkyBox(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList);
	virtual ~CSkyBox() = default;

	virtual void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera);
};
