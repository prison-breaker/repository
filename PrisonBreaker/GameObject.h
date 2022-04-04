#pragma once
#include "Mesh.h"
#include "Material.h"
#include "Camera.h"
#include "AnimationController.h"
#include "StateMachine.h"

class CGameObject;

struct LOADED_MODEL_INFO
{
	shared_ptr<CGameObject>			        m_Model{};
									        
	vector<shared_ptr<CAnimationClip>>      m_AnimationClips{};

	vector<vector<shared_ptr<CGameObject>>> m_BoneFrameCaches{}; // [SkinnedMesh][Bone]
	vector<shared_ptr<CSkinnedMesh>>	    m_SkinnedMeshCaches{};
};

class CGameObject : public enable_shared_from_this<CGameObject>
{
protected:
	bool				             m_IsActive{};
	
	tstring					         m_FrameName{};
		
	float							 m_Speed{};

	XMFLOAT4X4		                 m_WorldMatrix{ Matrix4x4::Identity() };
	XMFLOAT4X4				         m_TransformMatrix{ Matrix4x4::Identity() };
							         
	shared_ptr<CMesh>	             m_Mesh{};
	vector<shared_ptr<CMaterial>>    m_Materials{};
	shared_ptr<BoundingBox>			 m_BoundingBox{};

	shared_ptr<CAnimationController> m_AnimationController{};

	vector<shared_ptr<CGameObject>>  m_ChildObjects{};

public:
	CGameObject() = default;
	virtual ~CGameObject() = default;

	static shared_ptr<LOADED_MODEL_INFO> LoadObjectFromFile(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, const tstring& FileName, unordered_map<tstring, shared_ptr<CMesh>>& MeshCaches, unordered_map<tstring, shared_ptr<CMaterial>>& MaterialCaches);
	static shared_ptr<CGameObject> LoadModelInfoFromFile(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, tifstream& InFile, unordered_map<tstring, shared_ptr<CMesh>>& MeshCaches, unordered_map<tstring, shared_ptr<CMaterial>>& MaterialCaches);
	static void LoadAnimationInfoFromFile(tifstream& InFile, const shared_ptr<LOADED_MODEL_INFO>& ModelInfo);

	virtual void CreateShaderVariables(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* D3D12GraphicsCommandList);
	virtual void ReleaseShaderVariables();

	virtual void ReleaseUploadBuffers();

	virtual void Move(const XMFLOAT3& Direction, float Distance);

	virtual void Animate(float ElapsedTime);

	virtual void Render(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera, RENDER_TYPE RenderType);

	shared_ptr<CGameObject> FindFrame(const tstring& FrameName);
	shared_ptr<CSkinnedMesh> FindSkinnedMesh(const tstring& SkinnedMeshName);

	void SetActive(bool IsActive);
	bool IsActive() const;

	void SetSpeed(float Speed);
	float GetSpeed() const;

	const XMFLOAT4X4& GetWorldMatrix() const;

	void SetTransformMatrix(const XMFLOAT4X4& TransformMatrix);
	const XMFLOAT4X4& GetTransformMatrix() const;

	void SetRight(const XMFLOAT3& Right);
	XMFLOAT3 GetRight() const;

	void SetUp(const XMFLOAT3& Up);
	XMFLOAT3 GetUp() const;

	void SetLook(const XMFLOAT3& Look);
	XMFLOAT3 GetLook() const;

	void SetPosition(const XMFLOAT3& Position);
	XMFLOAT3 GetPosition() const;

	void SetMesh(const shared_ptr<CMesh>& Mesh);
	void SetMaterial(const shared_ptr<CMaterial>& Material);

	void SetBoundingBox(const shared_ptr<BoundingBox>& BoundingBox);
	shared_ptr<BoundingBox> GetBoundingBox();

	void SetChild(const shared_ptr<CGameObject>& ChildObject);

	void SetAnimationController(ID3D12Device* D3D12Device, ID3D12GraphicsCommandList* D3D12GraphicsCommandList, const shared_ptr<LOADED_MODEL_INFO>& ModelInfo);
	CAnimationController* GetAnimationController() const;

	void SetAnimationClip(UINT ClipNum);

	bool IsVisible(CCamera* Camera) const;

	void UpdateBoundingBox();
	void UpdateTransform(const XMFLOAT4X4& ParentMatrix);

	void Scale(float Pitch, float Yaw, float Roll);
	void Rotate(float Pitch, float Yaw, float Roll);
	void Rotate(const XMFLOAT3& Axis, float Angle);

	void RenderBoundingBox(ID3D12GraphicsCommandList* D3D12GraphicsCommandList, CCamera* Camera);
};
