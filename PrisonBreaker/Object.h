#pragma once

class CObject;

class CMesh;
class CMaterial;

class CCollider;
class CRigidBody;
class CStateMachine;
class CAnimator;

class CCamera;

struct LoadedModel
{
	CObject*   m_rootFrame;
	CAnimator* m_animator;
};

class CObject
{
private:
	static UINT		   m_nextInstanceID;
	UINT			   m_instanceID;

	string             m_name;

	bool			   m_isActive;
	bool			   m_isDeleted;

	XMFLOAT4X4		   m_worldMatrix;
	XMFLOAT4X4		   m_transformMatrix;

	CMesh*			   m_mesh;
	vector<CMaterial*> m_materials;

	CCollider*         m_collider;
	CRigidBody*		   m_rigidBody;
	CStateMachine*     m_stateMachine;
	CAnimator*		   m_animator;

	CObject*		   m_parent;
	vector<CObject*>   m_children;

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

	const XMFLOAT4X4& GetWorldMatrix();

	void SetTransformMatrix(const XMFLOAT4X4& transformMatrix);
	const XMFLOAT4X4& GetTransformMatrix();

	void SetRight(const XMFLOAT3& right);
	XMFLOAT3 GetRight();

	void SetUp(const XMFLOAT3& up);
	XMFLOAT3 GetUp();

	void SetForward(const XMFLOAT3& forward);
	XMFLOAT3 GetForward();

	void SetPosition(const XMFLOAT3& position);
	XMFLOAT3 GetPosition();

	void SetMesh(CMesh* mesh);
	CMesh* GetMesh();

	void SetMaterial(CMaterial* material);
	const vector<CMaterial*>& GetMaterials();

	void CreateCollider();
	CCollider* GetCollider();

	void CreateRigidBody();
	CRigidBody* GetRigidBody();

	void SetAnimator(CAnimator* animator);
	CAnimator* GetAnimator();

	void CreateStateMachine();
	CStateMachine* GetStateMachine();

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

	void UpdateLocalCoord(const XMFLOAT3& forward);
	void UpdateTransform();

	void Move(const XMFLOAT3& Direction, float Distance);
	void Scale(float Pitch, float Yaw, float Roll);
	void Rotate(float Pitch, float Yaw, float Roll);
	void Rotate(const XMFLOAT3& Axis, float Angle);

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
