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

void CBilboardObject::SetTexture(const shared_ptr<CTexture>& Texture)
{
	if (Texture)
	{
		m_Texture = Texture;
	}
}
