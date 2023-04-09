#include "pch.h"
#include "Asset.h"

CAsset::CAsset() :
    m_name()
{
}

CAsset::~CAsset()
{
}

void CAsset::SetName(const string& name)
{
    m_name = name;
}

const string& CAsset::GetName()
{
    return m_name;
}

void CAsset::CreateShaderVariables(ID3D12Device* d3d12Device, ID3D12GraphicsCommandList* d3d12GraphicsCommandList)
{
}

void CAsset::UpdateShaderVariables(ID3D12GraphicsCommandList* d3d12GraphicsCommandList)
{
}

void CAsset::ReleaseShaderVariables()
{
}
