#include "stdafx.h"
#include "BilboardObject.h"

void CBilboardObject::Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera)
{

}

void CBilboardObject::ReleaseUploadBuffers()
{
	if (m_D3D12PositionUploadBuffer)
	{
		m_D3D12PositionUploadBuffer.ReleaseAndGetAddressOf();
	}
}

void CBilboardObject::SetMaterial(const shared_ptr<CMaterial>& Material)
{
	if (Material)
	{
		m_Materials.push_back(Material);
	}
}
