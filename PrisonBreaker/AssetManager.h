#pragma once

class CObject;
class CMesh;
class CSkinnedMesh;
class CTexture;
class CShader;
class CMaterial;
class CAnimation;

class CAssetManager : public CSingleton<CAssetManager>
{
	friend class CSingleton;

private:
	string							           m_assetPath;
									             
	unordered_map<string, CMesh*>              m_meshes;
	unordered_map<string, CTexture*>           m_textures;
	unordered_map<string, CShader*>            m_shaders;
	unordered_map<string, CMaterial*>          m_materials;
	unordered_map<string, vector<CAnimation*>> m_animations; // [modelName][animation]

private:
	CAssetManager();
	virtual ~CAssetManager();

	void LoadMeshes(ID3D12Device* d3d12Device, ID3D12GraphicsCommandList* d3d12GraphicsCommandList, const string& fileName);
	void LoadTextures(ID3D12Device* d3d12Device, ID3D12GraphicsCommandList* d3d12GraphicsCommandList, const string& fileName);
	void LoadShaders(ID3D12Device* d3d12Device, ID3D12RootSignature* D3D12RootSignature);
	void LoadMaterials(ID3D12Device* d3d12Device, ID3D12GraphicsCommandList* d3d12GraphicsCommandList, const string& fileName);

public:
	const string& GetAssetPath();

	CMesh* GetMesh(const string& key);
	int GetMeshCount();

	CTexture* CreateTexture(const string& key);
	CTexture* GetTexture(const string& key);
	int GetTextureCount();

	CShader* GetShader(const string& key);
	int GetShaderCount();

	CMaterial* CreateMaterial(const string& key);
	CMaterial* GetMaterial(const string& key);
	int GetMaterialCount();

	void LoadAnimations(ID3D12Device* d3d12Device, ID3D12GraphicsCommandList* d3d12GraphicsCommandList, const string& fileName);
	const vector<CAnimation*>& GetAnimations(const string& key);
	int GetAnimationCount(const string& key);

	void Init(ID3D12Device* d3d12Device, ID3D12GraphicsCommandList* d3d12GraphicsCommandList, ID3D12RootSignature* D3D12RootSignature);

	void CreateShaderResourceViews(ID3D12Device* d3d12Device);

	void ReleaseUploadBuffers();
};
