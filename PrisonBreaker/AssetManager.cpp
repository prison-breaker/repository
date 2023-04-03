#include "pch.h"
#include "AssetManager.h"

#include "Core.h"

#include "SkinnedMesh.h"
#include "NavMesh.h"
#include "Texture.h"
#include "ObjectShader.h"
#include "DepthWriteShader.h"
#include "BilboardShader.h"
#include "WireFrameShader.h"
#include "Material.h"
#include "Animation.h"

CAssetManager::CAssetManager() :
	m_assetPath(),
	m_meshes(),
	m_textures(),
	m_shaders(),
	m_materials(),
	m_animations()
{
}

CAssetManager::~CAssetManager()
{
	Utility::SafeDelete(m_meshes);
	Utility::SafeDelete(m_textures);
	Utility::SafeDelete(m_shaders);
	Utility::SafeDelete(m_materials);

	for (auto& p : m_animations)
	{
		Utility::SafeDelete(p.second);
	}
}

void CAssetManager::LoadMeshes(ID3D12Device* d3d12Device, ID3D12GraphicsCommandList* d3d12GraphicsCommandList, const string& fileName)
{
	string filePath = m_assetPath + "Mesh\\" + fileName;
	ifstream in(filePath, ios::binary);
	string str;

	while (true)
	{
		File::ReadStringFromFile(in, str);

		if (str == "<Meshes>")
		{
			int meshCount = 0;

			in.read(reinterpret_cast<char*>(&meshCount), sizeof(int));

			if (meshCount > 0)
			{
				// + 1: NavMesh
				m_meshes.reserve(meshCount + 1);
				cout << fileName << " �ε� ����...\n";
			}
		}
		else if (str == "<Mesh>")
		{
			CMesh* mesh = new CMesh();

			mesh->Load(d3d12Device, d3d12GraphicsCommandList, in);
			m_meshes.emplace(mesh->GetName(), mesh);
		}
		else if (str == "<SkinnedMesh>")
		{
			// <RefName>
			File::ReadStringFromFile(in, str);
			File::ReadStringFromFile(in, str);

			CSkinnedMesh* skinnedMesh = new CSkinnedMesh(*GetMesh(str));

			skinnedMesh->LoadSkinInfo(d3d12Device, d3d12GraphicsCommandList, in);
			m_meshes.emplace(skinnedMesh->GetName(), skinnedMesh);
		}
		else if (str == "</Meshes>")
		{
			cout << fileName << " �ε� �Ϸ�...(�޽� ����: " << m_meshes.size() << ")\n\n";
			break;
		}
	}

	// NavMesh
	CNavMesh* navMesh = new CNavMesh();

	navMesh->Load(d3d12Device, d3d12GraphicsCommandList, "NavMesh.bin");
	m_meshes.emplace(navMesh->GetName(), navMesh);
}

void CAssetManager::LoadTextures(ID3D12Device* d3d12Device, ID3D12GraphicsCommandList* d3d12GraphicsCommandList, const string& fileName)
{
	string filePath = m_assetPath + "Texture\\" + fileName;
	ifstream in(filePath, ios::binary);
	string str;

	while (true)
	{
		File::ReadStringFromFile(in, str);

		if (str == "<Textures>")
		{
			int textureCount = 0;

			in.read(reinterpret_cast<char*>(&textureCount), sizeof(int));

			if (textureCount > 0)
			{
				m_textures.reserve(textureCount);
				cout << fileName << " �ε� ����...\n";
			}
		}
		else if (str == "<Texture>")
		{
			CTexture* texture = new CTexture();

			texture->Load(d3d12Device, d3d12GraphicsCommandList, in);
			m_textures.emplace(texture->GetName(), texture);
		}
		else if (str == "</Textures>")
		{
			cout << fileName << " �ε� �Ϸ�...(�ؽ�ó ����: " << m_textures.size() << ")\n\n";
			break;
		}
	}

	// DepthWrite Texture
	const XMFLOAT2& resolution = CCore::GetInstance()->GetResolution();
	CTexture* texture = new CTexture();

	texture->SetName("DepthWrite");
	texture->Create(d3d12Device, (UINT64)DEPTH_BUFFER_WIDTH, (UINT)DEPTH_BUFFER_HEIGHT, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, DXGI_FORMAT_R32_FLOAT, D3D12_CLEAR_VALUE{ DXGI_FORMAT_R32_FLOAT, { 1.0f, 1.0f, 1.0f, 1.0f } }, TEXTURE_TYPE::SHADOW_MAP);
	m_textures.emplace(texture->GetName(), texture);

	// PostProcessing Texture
}

void CAssetManager::LoadShaders(ID3D12Device* d3d12Device, ID3D12RootSignature* D3D12RootSignature)
{
	// �������� �ʿ��� ���̴� ��ü(PSO)�� �����Ѵ�.
	CShader* shader = new CDepthWriteShader();

	shader->SetName("DepthWrite");
	shader->CreatePipelineStates(d3d12Device, D3D12RootSignature, 3);
	m_shaders.emplace(shader->GetName(), shader);

	shader = new CObjectShader();
	shader->SetName("Object");
	shader->CreatePipelineStates(d3d12Device, D3D12RootSignature, 2);
	m_shaders.emplace(shader->GetName(), shader);

	shader = new CBilboardShader();
	shader->SetName("Bilboard");
	shader->CreatePipelineStates(d3d12Device, D3D12RootSignature, 2);
	m_shaders.emplace(shader->GetName(), shader);

	shader = new CWireFrameShader();
	shader->SetName("WireFrame");
	shader->CreatePipelineStates(d3d12Device, D3D12RootSignature, 1);
	m_shaders.emplace(shader->GetName(), shader);
}

void CAssetManager::LoadMaterials(ID3D12Device* d3d12Device, ID3D12GraphicsCommandList* d3d12GraphicsCommandList, const string& fileName)
{
	string filePath = m_assetPath + "Material\\" + fileName;
	ifstream in(filePath, ios::binary);
	string str;

	while (true)
	{
		File::ReadStringFromFile(in, str);

		if (str == "<Materials>")
		{
			int materialCount = 0;

			in.read(reinterpret_cast<char*>(&materialCount), sizeof(int));

			if (materialCount > 0)
			{
				m_materials.reserve(materialCount);
				cout << fileName << " �ε� ����...\n";
			}
		}
		else if (str == "<Material>")
		{
			CMaterial* material = new CMaterial();

			material->Load(d3d12Device, d3d12GraphicsCommandList, in);
			m_materials.emplace(material->GetName(), material);
		}
		else if (str == "</Materials>")
		{
			cout << fileName << " �ε� �Ϸ�...(���͸��� ����: " << m_materials.size() << ")\n\n";
			break;
		}
	}
}

void CAssetManager::LoadAnimations(ID3D12Device* d3d12Device, ID3D12GraphicsCommandList* d3d12GraphicsCommandList, const string& fileName)
{
	// str.length() - 14 : _Animation.bin
	string modelName = fileName.substr(0, fileName.length() - 14);

	if (m_animations.find(modelName) == m_animations.end())
	{
		string filePath = m_assetPath + "Animation\\" + fileName;
		ifstream in(filePath, ios::binary);
		string str;

		while (true)
		{
			File::ReadStringFromFile(in, str);

			if (str == "<Animations>")
			{
				int animationCount = 0;

				in.read(reinterpret_cast<char*>(&animationCount), sizeof(int));
				m_animations[modelName].reserve(animationCount);
				cout << fileName << " �ִϸ��̼� �ε� ����...\n";
			}
			else if (str == "<Animation>")
			{
				CAnimation* animation = new CAnimation();

				animation->Load(in);
				m_animations[modelName].push_back(animation);
			}
			else if (str == "</Animations>")
			{
				cout << fileName << " �ִϸ��̼� �ε� �Ϸ�...\n";
				break;
			}
		}
	}
}

const string& CAssetManager::GetAssetPath()
{
	return m_assetPath;
}

CMesh* CAssetManager::GetMesh(const string& key)
{
	CMesh* mesh = nullptr;

	if (m_meshes.find(key) != m_meshes.end())
	{
		mesh = m_meshes[key];
	}

	return mesh;
}

int CAssetManager::GetMeshCount()
{
	return (int)m_meshes.size();
}

CTexture* CAssetManager::CreateTexture(const string& key)
{
	CTexture* texture = GetTexture(key);

	if (texture == nullptr)
	{
		texture = new CTexture();
		m_textures.emplace(key, texture);
	}

	return texture;
}

CTexture* CAssetManager::GetTexture(const string& key)
{
	CTexture* texture = nullptr;

	if (m_textures.find(key) != m_textures.end())
	{
		texture = m_textures[key];
	}

	return texture;
}

int CAssetManager::GetTextureCount()
{
	return (int)m_textures.size();
}

CShader* CAssetManager::GetShader(const string& key)
{
	CShader* shader = nullptr;

	if (m_shaders.find(key) != m_shaders.end())
	{
		shader = m_shaders[key];
	}

	return shader;
}

int CAssetManager::GetShaderCount()
{
	return (int)m_shaders.size();
}

CMaterial* CAssetManager::CreateMaterial(const string& key)
{
	CMaterial* material = GetMaterial(key);

	if (material == nullptr)
	{
		material = new CMaterial();
		m_materials.emplace(key, material);
	}

	return material;
}

CMaterial* CAssetManager::GetMaterial(const string& key)
{
	CMaterial* material = nullptr;

	if (m_materials.find(key) != m_materials.end())
	{
		material = m_materials[key];
	}

	return material;
}

int CAssetManager::GetMaterialCount()
{
	return (int)m_materials.size();
}

const vector<CAnimation*>& CAssetManager::GetAnimations(const string& key)
{	
	return m_animations[key];
}

int CAssetManager::GetAnimationCount(const string& key)
{
	return (int)m_animations[key].size();
}

void CAssetManager::Init(ID3D12Device* d3d12Device, ID3D12GraphicsCommandList* d3d12GraphicsCommandList, ID3D12RootSignature* D3D12RootSignature)
{
	// ������Ʈ ������ ����� �ǿ��� ���� ���丮�� �����ϸ� Visual Studio���� ���� ��, �ش� ��θ� �۾� �����丮�� �����Ѵ�.
	// ������, Debug�� ����� ������ ���� �����ϴ� ��쿡��, �ش� ���� ������ ��ΰ� �۾� ���丮�� �����ǹǷ�, ���� �۾� ���丮����
	// ���� ������ �� �� ���� ��, Release\\Asset\\���� �̵��Ͽ� ���ҽ��� ������ �� �ֵ��� �����.
	// ��, � ���� �����ϴ��� �۾� ���丮�� ��ġ�ϵ��� ����� �ش�.
	char assetPath[255] = {};

	GetCurrentDirectoryA(255, assetPath);

	// ���� ���� ��θ� ���Ѵ�.
	for (int i = (int)strlen(assetPath) - 1; i >= 0; --i)
	{
		if (assetPath[i] == '\\')
		{
			assetPath[i] = '\0';

			break;
		}
	}

	strcat_s(assetPath, 255, "\\Release\\Asset\\");
	m_assetPath = assetPath;

	LoadMeshes(d3d12Device, d3d12GraphicsCommandList, "Meshes.bin");
	LoadTextures(d3d12Device, d3d12GraphicsCommandList, "Textures.bin");
	LoadShaders(d3d12Device, D3D12RootSignature);
	LoadMaterials(d3d12Device, d3d12GraphicsCommandList, "Materials.bin");
}

void CAssetManager::CreateShaderResourceViews(ID3D12Device* d3d12Device)
{
	D3D12_CPU_DESCRIPTOR_HANDLE d3d12CpuDescriptorHandle = CCore::GetInstance()->GetCbvSrvUavDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
	D3D12_GPU_DESCRIPTOR_HANDLE d3d12GpuDescriptorHandle = CCore::GetInstance()->GetCbvSrvUavDescriptorHeap()->GetGPUDescriptorHandleForHeapStart();
	UINT descriptorIncrementSize = d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	for (const auto& texture : m_textures)
	{
		d3d12Device->CreateShaderResourceView(texture.second->GetTexture(), nullptr, d3d12CpuDescriptorHandle);
		texture.second->SetGpuDescriptorHandle(d3d12GpuDescriptorHandle);

		d3d12CpuDescriptorHandle.ptr += descriptorIncrementSize;
		d3d12GpuDescriptorHandle.ptr += descriptorIncrementSize;
	}
}

void CAssetManager::ReleaseUploadBuffers()
{
	for (const auto& p : m_meshes)
	{
		p.second->ReleaseUploadBuffers();
	}

	for (const auto& p : m_textures)
	{
		p.second->ReleaseUploadBuffers();
	}
}
