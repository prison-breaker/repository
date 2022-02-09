#include "stdafx.h"
#include "SkyBox.h"

CSkyBox::CSkyBox(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList)
{
	vector<CBilboardMesh> Vertices{};
	XMFLOAT3 Position{};
	XMFLOAT2 Size{ 20.0f, 20.0f };

	Vertices.reserve(6);

	Position = { 0.0f, 0.0f, -10.0f };
	Vertices.emplace_back(Position, Size);

	Position = { 0.0f, 0.0f, +10.0f };
	Vertices.emplace_back(Position, Size);

	Position = { +10.0f, 0.0f, 0.0f };
	Vertices.emplace_back(Position, Size);

	Position = { -10.0f, 0.0f, 0.0f };
	Vertices.emplace_back(Position, Size);

	Position = { 0.0f, +10.0f, 0.0f };
	Vertices.emplace_back(Position, Size);

	Position = { 0.0f, -10.0f, 0.0f };
	Vertices.emplace_back(Position, Size);

	UINT Stride{ sizeof(CBilboardMesh) };

	m_D3D12PositionBuffer = DX::CreateBufferResource(D3D12Device, D3D12GraphicsCommandList, Vertices.data(), Stride * (UINT)Vertices.size(),
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, m_D3D12PositionUploadBuffer.GetAddressOf());

	m_D3D12PositionBufferView.BufferLocation = m_D3D12PositionBuffer->GetGPUVirtualAddress();
	m_D3D12PositionBufferView.StrideInBytes = Stride;
	m_D3D12PositionBufferView.SizeInBytes = Stride * (UINT)Vertices.size();
}

void CSkyBox::Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera)
{
	D3D12_VERTEX_BUFFER_VIEW VertexBufferViews[] = { m_D3D12PositionBufferView };

	D3D12GraphicsCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
	D3D12GraphicsCommandList->IASetVertexBuffers(0, 1, VertexBufferViews);

	for (UINT i = 0; i < 6; ++i)
	{
		if (m_Texture)
		{
			m_Texture->UpdateShaderVariable(D3D12GraphicsCommandList, ROOT_PARAMETER_DIFFUSEMAP, i);
		}

		D3D12GraphicsCommandList->DrawInstanced(1, 1, i, 0);
	}
}
