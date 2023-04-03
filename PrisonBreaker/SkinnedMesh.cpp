#include "pch.h"
#include "SkinnedMesh.h"

#include "Object.h"

CSkinnedMesh::CSkinnedMesh(const CMesh& rhs) : CMesh(rhs),
	m_boneCount(),
	m_d3d12BoneIndexBuffer(),
	m_d3d12BoneIndexUploadBuffer(),
	m_d3d12BoneIndexBufferView(),
	m_d3d12BoneWeightBuffer(),
	m_d3d12BoneWeightUploadBuffer(),
	m_d3d12BoneWeightBufferView(),
	m_boneOffsetMatrixes(),
	m_d3d12BoneOffsetMatrixes(),
	m_mappedBoneOffsetMatrixes(),
	m_boneFrameCache(),
	m_d3d12BoneTransformMatrixes(),
	m_mappedBoneTransformMatrixes()
{
}

CSkinnedMesh::~CSkinnedMesh()
{
}

void CSkinnedMesh::SetBoneInfo(vector<CObject*>* boneFrameCache, const ComPtr<ID3D12Resource>& d3d12BoneTransformMatrixes, XMFLOAT4X4* mappedBoneTransformMatrixes)
{
	m_boneFrameCache = boneFrameCache;
	m_d3d12BoneTransformMatrixes = d3d12BoneTransformMatrixes;
	m_mappedBoneTransformMatrixes = mappedBoneTransformMatrixes;
}

void CSkinnedMesh::CSkinnedMesh::LoadSkinInfo(ID3D12Device* d3d12Device, ID3D12GraphicsCommandList* d3d12GraphicsCommandList, ifstream& in)
{
	string str;

	while (true)
	{
		File::ReadStringFromFile(in, str);

		if (str == "<Name>")
		{
			File::ReadStringFromFile(in, m_name);
		}
		else if (str == "<BoneOffsetMatrixes>")
		{
			in.read(reinterpret_cast<char*>(&m_boneCount), sizeof(int));

			if (m_boneCount > 0)
			{
				m_boneOffsetMatrixes.resize(m_boneCount);
				in.read(reinterpret_cast<char*>(m_boneOffsetMatrixes.data()), sizeof(XMFLOAT4X4) * m_boneCount);

				int bytes = ((sizeof(XMFLOAT4X4) * MAX_BONES) + 255) & ~255;

				m_d3d12BoneOffsetMatrixes = DX::CreateBufferResource(d3d12Device, d3d12GraphicsCommandList, nullptr, bytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, nullptr);
				m_d3d12BoneOffsetMatrixes->Map(0, nullptr, reinterpret_cast<void**>(&m_mappedBoneOffsetMatrixes));

				for (int i = 0; i < m_boneCount; ++i)
				{
					XMStoreFloat4x4(&m_mappedBoneOffsetMatrixes[i], XMMatrixTranspose(XMLoadFloat4x4(&m_boneOffsetMatrixes[i])));
				}
			}
		}
		else if (str == "<BoneIndices>")
		{
			int vertexCount = 0;

			in.read(reinterpret_cast<char*>(&vertexCount), sizeof(int));

			if (vertexCount > 0)
			{
				vector<XMINT4> boneIndices(vertexCount);

				in.read(reinterpret_cast<char*>(boneIndices.data()), sizeof(XMUINT4) * vertexCount);

				m_d3d12BoneIndexBuffer = DX::CreateBufferResource(d3d12Device, d3d12GraphicsCommandList, boneIndices.data(), sizeof(XMUINT4) * vertexCount, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, m_d3d12BoneIndexUploadBuffer.GetAddressOf());
				m_d3d12BoneIndexBufferView.BufferLocation = m_d3d12BoneIndexBuffer->GetGPUVirtualAddress();
				m_d3d12BoneIndexBufferView.StrideInBytes = sizeof(XMUINT4);
				m_d3d12BoneIndexBufferView.SizeInBytes = sizeof(XMUINT4) * vertexCount;
			}
		}
		else if (str == "<BoneWeights>")
		{
			int vertexCount = 0;

			in.read(reinterpret_cast<char*>(&vertexCount), sizeof(int));

			if (vertexCount > 0)
			{
				vector<XMFLOAT4> boneWeights(vertexCount);

				in.read(reinterpret_cast<char*>(boneWeights.data()), sizeof(XMFLOAT4) * vertexCount);

				m_d3d12BoneWeightBuffer = DX::CreateBufferResource(d3d12Device, d3d12GraphicsCommandList, boneWeights.data(), sizeof(XMFLOAT4) * vertexCount, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, m_d3d12BoneWeightUploadBuffer.GetAddressOf());
				m_d3d12BoneWeightBufferView.BufferLocation = m_d3d12BoneWeightBuffer->GetGPUVirtualAddress();
				m_d3d12BoneWeightBufferView.StrideInBytes = sizeof(XMFLOAT4);
				m_d3d12BoneWeightBufferView.SizeInBytes = sizeof(XMFLOAT4) * vertexCount;
			}
		}
		else if (str =="</SkinnedMesh>")
		{
			break;
		}
	}
}

void CSkinnedMesh::UpdateShaderVariables(ID3D12GraphicsCommandList* d3d12GraphicsCommandList)
{
	for (int i = 0; i < m_boneFrameCache->size(); ++i)
	{
		XMStoreFloat4x4(&m_mappedBoneTransformMatrixes[i], XMMatrixTranspose(XMLoadFloat4x4(&(*m_boneFrameCache)[i]->GetWorldMatrix())));
	}

	d3d12GraphicsCommandList->SetGraphicsRootConstantBufferView((UINT)ROOT_PARAMETER_TYPE::BONE_OFFSET, m_d3d12BoneOffsetMatrixes->GetGPUVirtualAddress());
	d3d12GraphicsCommandList->SetGraphicsRootConstantBufferView((UINT)ROOT_PARAMETER_TYPE::BONE_TRANSFORM, m_d3d12BoneTransformMatrixes->GetGPUVirtualAddress());
}

void CSkinnedMesh::ReleaseUploadBuffers()
{
	if (m_d3d12BoneIndexUploadBuffer != nullptr)
	{
		m_d3d12BoneIndexUploadBuffer.Reset();
	}

	if (m_d3d12BoneWeightUploadBuffer != nullptr)
	{
		m_d3d12BoneWeightUploadBuffer.Reset();
	}
}

void CSkinnedMesh::Render(ID3D12GraphicsCommandList* d3d12GraphicsCommandList, int subSetIndex)
{
	UpdateShaderVariables(d3d12GraphicsCommandList);

	D3D12_VERTEX_BUFFER_VIEW vertexBufferViews[] = { m_d3d12PositionBufferView, m_d3d12NormalBufferView, m_d3d12TangentBufferView, m_d3d12BiTangentBufferView, m_d3d12TexCoordBufferView, m_d3d12BoneIndexBufferView, m_d3d12BoneWeightBufferView };

	d3d12GraphicsCommandList->IASetVertexBuffers(0, _countof(vertexBufferViews), vertexBufferViews);
	d3d12GraphicsCommandList->IASetPrimitiveTopology(m_d3d12PrimitiveTopology);

	int bufferSize = (int)m_d3d12IndexBuffers.size();

	if (bufferSize > 0 && subSetIndex < bufferSize)
	{
		d3d12GraphicsCommandList->IASetIndexBuffer(&m_d3d12IndexBufferViews[subSetIndex]);
		d3d12GraphicsCommandList->DrawIndexedInstanced((UINT)m_indices[subSetIndex].size(), 1, 0, 0, 0);
	}
	else
	{
		d3d12GraphicsCommandList->DrawInstanced((UINT)m_positions.size(), 1, 0, 0);
	}
}
