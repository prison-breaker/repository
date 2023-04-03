#pragma once

class CPlayer;
struct Light;

struct CB_CAMERA
{
	XMFLOAT4X4 m_viewMatrix;
	XMFLOAT4X4 m_projectionMatrix;
			 
	XMFLOAT3   m_position;
};

class CCamera
{
	friend class CCameraManager;

protected:
	CAMERA_TYPE			   m_type;

	bool				   m_isZoomIn;
	float				   m_magnification;

	D3D12_VIEWPORT         m_d3d12Viewport;
	D3D12_RECT	           m_d3d12ScissorRect;
				           
	XMFLOAT4X4	           m_viewMatrix;
	XMFLOAT4X4	           m_projectionMatrix;
				              
	XMFLOAT3	           m_right;
	XMFLOAT3	           m_up;
	XMFLOAT3	           m_forward;
	XMFLOAT3	           m_position;
				              
	XMFLOAT3	           m_offset;
	float		           m_speed;
						   
	BoundingFrustum        m_frustum;
						   
	ComPtr<ID3D12Resource> m_d3d12Buffer;
	CB_CAMERA*			   m_mappedData;

	CPlayer*			   m_target;
	Light*				   m_light;

protected:
	// 이 객체의 생성은 오로지 CCameraManager에 의해서만 일어난다.
	// 단, 이 객체를 상속 받은 자식 클래스의 생성자에서 이 클래스의 생성자를 호출해야하므로 접근 지정자를 protected로 설정하였다.
	CCamera(CAMERA_TYPE type);

public:
	~CCamera();

	CAMERA_TYPE GetType();

	void SetZoomIn(bool isZoomIn);
	bool IsZoomIn();

	void SetMagnification(float magnification);
	float GetMagnification();

	void SetViewport(int TopLeftX, int TopLeftY, UINT Width, UINT Height, float MinDepth, float MaxDepth);
	void SetScissorRect(LONG Left, LONG Top, LONG Right, LONG Bottom);

	const XMFLOAT4X4& GetViewMatrix();
	const XMFLOAT4X4& GetProjectionMatrix();

	const XMFLOAT3& GetRight();
	const XMFLOAT3& GetUp();
	const XMFLOAT3& GetFoward();

	void SetPosition(const XMFLOAT3& Position);
	const XMFLOAT3& GetPosition();

	void SetTarget(CPlayer* target);
	CPlayer* GetTarget();

	void SetLight(Light* light);
	Light* GetLight();

	void CreateShaderVariables(ID3D12Device* d3d12Device, ID3D12GraphicsCommandList* d3d12GraphicsCommandList);
	void UpdateShaderVariables(ID3D12GraphicsCommandList* d3d12GraphicsCommandList);
	void ReleaseShaderVariables();

	void RSSetViewportsAndScissorRects(ID3D12GraphicsCommandList* d3d12GraphicsCommandList);

	void GenerateViewMatrix(const XMFLOAT3& Position, const XMFLOAT3& Look);
	void RegenerateViewMatrix();

	void GenerateBoundingFrustum();
	bool IsInBoundingFrustum(const BoundingBox& BoundingBox);

	void GenerateOrthographicsProjectionMatrix(float ViewWidth, float ViewHeight, float NearZ, float FarZ);
	void GeneratePerspectiveProjectionMatrix(float FOVAngleY, float AspectRatio, float NearZ, float FarZ);

	void Move(const XMFLOAT3& Shift);
	void Rotate(float Pitch, float Yaw, float Roll);

	void Update();
};
