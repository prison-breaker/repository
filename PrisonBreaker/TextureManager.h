#pragma once
#include "Material.h"

class CTextureManager
{
private:
	unordered_map<tstring, shared_ptr<CTexture>> m_Textures{};
	ComPtr<ID3D12DescriptorHeap>				 m_D3D12SrvDescriptorHeap{};

private:
	CTextureManager() = default;
	~CTextureManager() = default;

public:
	static CTextureManager* GetInstance();

	void CreateCbvSrvUavDescriptorHeaps(ID3D12Device* D3D12Device);
	void CreateShaderResourceViews(ID3D12Device* D3D12Device);

	void ReleaseUploadBuffers();

	void RegisterTexture(const tstring& TextureName, const shared_ptr<CTexture>& Texture);
	shared_ptr<CTexture> GetTexture(const tstring& TextureName);

	UINT GetRegisteredTextureCount() const;

	void SetDescriptorHeap(ID3D12GraphicsCommandList* D3D12GraphicsCommandList);
};
