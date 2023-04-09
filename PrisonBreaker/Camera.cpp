#include "pch.h"
#include "Camera.h"

#include "TimeManager.h"

#include "Player.h"

CCamera::CCamera(CAMERA_TYPE type) :
	m_type(type),
	m_isZoomIn(),
	m_magnification(1.0f),
	m_d3d12Viewport(),
	m_d3d12ScissorRect(),
	m_viewMatrix(Matrix4x4::Identity()),
	m_projectionMatrix(Matrix4x4::Identity()),
	m_right(1.0f, 0.0f, 0.0f),
	m_up(0.0f, 1.0f, 0.0f),
	m_forward(0.0f, 0.0f, 1.0f),
	m_position(),
	m_offset(0.0f, 0.0f, -3.0f),
	m_speed(15.0f),
	m_frustum(),
	m_d3d12Buffer(),
	m_mappedData(),
	m_target(),
	m_light()
{
}

CCamera::~CCamera()
{
	ReleaseShaderVariables();
}

CAMERA_TYPE CCamera::GetType()
{
	return m_type;
}

void CCamera::SetZoomIn(bool isZoomIn)
{
	m_isZoomIn = isZoomIn;
}

bool CCamera::IsZoomIn()
{
	return m_isZoomIn;
}

void CCamera::SetMagnification(float magnification)
{
	m_magnification = clamp(magnification, 1.0f, 1.5f);
}

float CCamera::GetMagnification()
{
	return m_magnification;
}

void CCamera::SetViewport(int TopLeftX, int TopLeftY, UINT Width, UINT Height, float MinDepth, float MaxDepth)
{
	m_d3d12Viewport.TopLeftX = static_cast<float>(TopLeftX);
	m_d3d12Viewport.TopLeftY = static_cast<float>(TopLeftY);
	m_d3d12Viewport.Width = static_cast<float>(Width);
	m_d3d12Viewport.Height = static_cast<float>(Height);
	m_d3d12Viewport.MinDepth = MinDepth;
	m_d3d12Viewport.MaxDepth = MaxDepth;
}

void CCamera::SetScissorRect(LONG Left, LONG Top, LONG Right, LONG Bottom)
{
	m_d3d12ScissorRect.left = Left;
	m_d3d12ScissorRect.top = Top;
	m_d3d12ScissorRect.right = Right;
	m_d3d12ScissorRect.bottom = Bottom;
}

const XMFLOAT4X4& CCamera::GetViewMatrix()
{
	return m_viewMatrix;
}

const XMFLOAT4X4& CCamera::GetProjectionMatrix()
{
	return m_projectionMatrix;
}

const XMFLOAT3& CCamera::GetRight()
{
	return m_right;
}

const XMFLOAT3& CCamera::GetUp()
{
	return m_up;
}

const XMFLOAT3& CCamera::GetFoward()
{
	return m_forward;
}

void CCamera::SetPosition(const XMFLOAT3& Position)
{
	m_position = Position;

	RegenerateViewMatrix();
}

const XMFLOAT3& CCamera::GetPosition()
{
	return m_position;
}

void CCamera::SetTarget(CPlayer* target)
{
	m_target = target;
}

CPlayer* CCamera::GetTarget()
{
	return m_target;
}

void CCamera::SetLight(Light* light)
{
	m_light = light;
}

Light* CCamera::GetLight()
{
	return m_light;
}

void CCamera::CreateShaderVariables(ID3D12Device* d3d12Device, ID3D12GraphicsCommandList* d3d12GraphicsCommandList)
{
	UINT bytes = (sizeof(CB_CAMERA) + 255) & ~255;

	m_d3d12Buffer = DX::CreateBufferResource(d3d12Device, d3d12GraphicsCommandList, nullptr, bytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, nullptr);
	DX::ThrowIfFailed(m_d3d12Buffer->Map(0, nullptr, reinterpret_cast<void**>(&m_mappedData)));
}

void CCamera::UpdateShaderVariables(ID3D12GraphicsCommandList* d3d12GraphicsCommandList)
{
	XMStoreFloat4x4(&m_mappedData->m_viewMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_viewMatrix)));
	XMStoreFloat4x4(&m_mappedData->m_projectionMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_projectionMatrix)));
	memcpy(&m_mappedData->m_position, &m_position, sizeof(XMFLOAT3));

	d3d12GraphicsCommandList->SetGraphicsRootConstantBufferView(static_cast<UINT>(ROOT_PARAMETER_TYPE::CAMERA), m_d3d12Buffer->GetGPUVirtualAddress());
}

void CCamera::ReleaseShaderVariables()
{
	m_d3d12Buffer->Unmap(0, nullptr);
}

void CCamera::RSSetViewportsAndScissorRects(ID3D12GraphicsCommandList* d3d12GraphicsCommandList)
{
	d3d12GraphicsCommandList->RSSetViewports(1, &m_d3d12Viewport);
	d3d12GraphicsCommandList->RSSetScissorRects(1, &m_d3d12ScissorRect);
}

void CCamera::GenerateViewMatrix(const XMFLOAT3& Position, const XMFLOAT3& Look)
{
	m_position = Position;
	m_forward = Look;

	RegenerateViewMatrix();
}

void CCamera::RegenerateViewMatrix()
{
	XMFLOAT3 worldUp = XMFLOAT3(0.0f, 1.0f, 0.0f);

	m_forward = Vector3::Normalize(m_forward);
	m_right = Vector3::CrossProduct(worldUp, m_forward, false);
	m_up = Vector3::CrossProduct(m_forward, m_right, false);
	m_viewMatrix = Matrix4x4::LookToLH(m_position, m_forward, worldUp);

	GenerateBoundingFrustum();
}

void CCamera::GenerateOrthographicsProjectionMatrix(float ViewWidth, float ViewHeight, float NearZ, float FarZ)
{
	m_projectionMatrix = Matrix4x4::OrthographicFovLH(ViewWidth, ViewHeight, NearZ, FarZ);
}

void CCamera::GeneratePerspectiveProjectionMatrix(float FOVAngleY, float AspectRatio, float NearZ, float FarZ)
{
	m_projectionMatrix = Matrix4x4::PerspectiveFovLH(XMConvertToRadians(FOVAngleY), AspectRatio, NearZ, FarZ);
}

void CCamera::GenerateBoundingFrustum()
{
	// 원근 투영 변환 행렬에서 절두체를 생성한다(절두체는 카메라 좌표계로 표현된다).
	m_frustum.CreateFromMatrix(m_frustum, XMLoadFloat4x4(&m_projectionMatrix));

	// 카메라 변환 행렬의 역행렬을 구한다.
	XMMATRIX inverseViewMatrix = XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_viewMatrix));

	// 절두체를 카메라 변환 행렬의 역행렬로 변환한다(이제 절두체는 월드 좌표계로 표현된다).
	m_frustum.Transform(m_frustum, inverseViewMatrix);
}

bool CCamera::IsInBoundingFrustum(const BoundingBox& BoundingBox)
{
	return m_frustum.Intersects(BoundingBox);
}

void CCamera::Move(const XMFLOAT3& Shift)
{
	SetPosition(Vector3::Add(m_position, Shift));
}

void CCamera::Rotate(float Pitch, float Yaw, float Roll)
{
	if (!Math::IsZero(Pitch))
	{
		XMFLOAT4X4 rotationMatrix = Matrix4x4::RotationAxis(m_right, Pitch);

		m_forward = Vector3::TransformNormal(m_forward, rotationMatrix);
	}

	if (!Math::IsZero(Yaw))
	{
		XMFLOAT4X4 rotationMatrix = Matrix4x4::RotationAxis(m_up, Yaw);

		m_forward = Vector3::TransformNormal(m_forward, rotationMatrix);
	}

	if (!Math::IsZero(Roll))
	{
		XMFLOAT4X4 rotationMatrix = Matrix4x4::RotationAxis(m_forward, Roll);

		m_forward = Vector3::TransformNormal(m_forward, rotationMatrix);
	}

	RegenerateViewMatrix();
}

void CCamera::Update()
{
	if (m_target != nullptr)
	{
		const XMFLOAT3& rotation = m_target->GetRotation();
		XMFLOAT4X4 transformMatrix = Matrix4x4::Multiply(Matrix4x4::RotationYawPitchRoll(rotation.x, 0.0f, 0.0f), m_target->GetWorldMatrix());
		XMFLOAT3 focusPosition = XMFLOAT3(transformMatrix._41, transformMatrix._42 + 4.5f, transformMatrix._43);

		if (m_magnification > 1.0f)
		{
			XMFLOAT3 zoomDirection = Vector3::Normalize(Vector3::Add(XMFLOAT3(transformMatrix._11, transformMatrix._12, transformMatrix._13), XMFLOAT3(transformMatrix._31, transformMatrix._32, transformMatrix._33)));
		
			zoomDirection = Vector3::ScalarProduct(m_magnification, zoomDirection, false);
			focusPosition = Vector3::Add(focusPosition, zoomDirection);
		}

		XMFLOAT3 newOffset = Vector3::TransformNormal(m_offset, transformMatrix);
		XMFLOAT3 newPosition = Vector3::Add(focusPosition, newOffset);
		XMFLOAT3 direction = Vector3::Subtract(newPosition, m_position);
		float shift = Vector3::Length(direction) * m_speed * DT;

		if (shift > 0.0f)
		{
			direction = Vector3::Normalize(direction);
			m_position = Vector3::Add(m_position, Vector3::ScalarProduct(shift, direction, false));
			m_forward = Vector3::Subtract(focusPosition, m_position);
		}
	}

	RegenerateViewMatrix();
}
