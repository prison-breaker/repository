#pragma once

class CObject;

class CMesh;
class CMaterial;

class CComponent;
class CStateMachine;
class CRigidBody;
class CAnimator;
class CTransform;
class CSpriteRenderer;
class CCollider;

class CCamera;

struct LoadedModel
{
	CObject*   m_rootFrame;
	CAnimator* m_animator;
};

class CObject
{
private:
	static UINT		    m_nextInstanceID;
	UINT			    m_instanceID;
					    
	string              m_name;
					    
	bool			    m_isActive;
	bool			    m_isDeleted;

	CMesh*			    m_mesh;
	vector<CMaterial*>  m_materials;

	vector<CComponent*> m_components;

	CObject*		    m_parent;
	vector<CObject*>    m_children;

public:
	CObject();
	CObject(const CObject& rhs) = delete;
	virtual ~CObject();

	static LoadedModel Load(ID3D12Device* d3d12Device, ID3D12GraphicsCommandList* d3d12GraphicsCommandList, const string& fileName);

	UINT GetInstanceID();

	void SetName(const string& name);
	const string& GetName();

	void SetActive(bool isActive);
	bool IsActive();

	void SetDeleted(bool isDeleted);
	bool IsDeleted();

	void SetMesh(CMesh* mesh);
	CMesh* GetMesh();

	void AddMaterial(CMaterial* material);
	const vector<CMaterial*>& GetMaterials();

	CComponent* CreateComponent(COMPONENT_TYPE componentType);
	void SetComponent(COMPONENT_TYPE componentType, CComponent* newComponent);
	template <typename T>
	T* GetComponent()
	{
		for (int i = static_cast<int>(COMPONENT_TYPE::STATE_MACHINE); i < static_cast<int>(COMPONENT_TYPE::COUNT); ++i)
		{
			if (m_components[i] != nullptr)
			{
				if (typeid(T).raw_name() == typeid(*m_components[i]).raw_name())
				{
					return reinterpret_cast<T*>(m_components[i]);
				}
			}
		}

		return nullptr;
	}
	const vector<CComponent*>& GetComponents();

	CObject* GetParent();

	void AddChild(CObject* object);
	const vector<CObject*>& GetChildren();

	virtual void Init();

	virtual void CreateShaderVariables(ID3D12Device* d3d12Device, ID3D12GraphicsCommandList* d3d12GraphicsCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* d3d12GraphicsCommandList);
	virtual void ReleaseShaderVariables();

	virtual void ReleaseUploadBuffers();

	CObject* FindFrame(const string& name);

	CObject* CheckRayIntersection(const XMFLOAT3& rayOrigin, const XMFLOAT3& rayDirection, float& hitDistance, float maxDistance);

	bool IsVisible(CCamera* camera);

	virtual void OnCollisionEnter(CObject* collidedObject);
	virtual void OnCollision(CObject* collidedObject);
	virtual void OnCollisionExit(CObject* collidedObject);

	virtual void Update();
	virtual void LateUpdate();

	virtual void PreRender(ID3D12GraphicsCommandList* d3d12GraphicsCommandList, CCamera* camera);
	virtual void Render(ID3D12GraphicsCommandList* d3d12GraphicsCommandList, CCamera* camera);

private:
	static CObject* LoadFrame(ID3D12Device* d3d12Device, ID3D12GraphicsCommandList* d3d12GraphicsCommandList, ifstream& in);
};
