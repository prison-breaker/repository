#include "pch.h"
#include "CameraManager.h"

#include "Core.h"

#include "TimeManager.h"
#include "InputManager.h"

#include "Camera.h"

CCameraManager::CCameraManager() :
	m_cameras()
{
}

CCameraManager::~CCameraManager()
{
}

CCamera* CCameraManager::GetMainCamera()
{
	return m_cameras[0];
}

CCamera* CCameraManager::GetUICamera()
{
	return m_cameras[1];
}

const vector<CCamera*>& CCameraManager::GetCameras()
{
	return m_cameras;
}

void CCameraManager::Init(ID3D12Device* d3d12Device, ID3D12GraphicsCommandList* d3d12GraphicsCommandList)
{
	const XMFLOAT2& resolution = CCore::GetInstance()->GetResolution();
	CCamera* camera = new CCamera(CAMERA_TYPE::MAIN);

	camera->SetViewport(0, 0, (UINT)resolution.x, (UINT)resolution.y, 0.0f, 1.0f);
	camera->SetScissorRect(0, 0, (LONG)resolution.x, (LONG)resolution.y);
	camera->GeneratePerspectiveProjectionMatrix(90.0f, resolution.x / resolution.y, 1.0f, 200.0f);
	camera->GenerateViewMatrix(XMFLOAT3(0.0f, 5.0f, 10.0f), XMFLOAT3(0.0f, 0.0f, -1.0f));
	camera->CreateShaderVariables(d3d12Device, d3d12GraphicsCommandList);
	m_cameras.push_back(camera);

	camera = new CCamera(CAMERA_TYPE::UI);
	camera->SetViewport(0, 0, (UINT)resolution.x, (UINT)resolution.y, 0.0f, 1.0f);
	camera->SetScissorRect(0, 0, (LONG)resolution.x, (LONG)resolution.y);
	camera->GenerateOrthographicsProjectionMatrix(resolution.x, resolution.y, 0.0f, 30.0f);
	camera->GenerateViewMatrix(XMFLOAT3(0.0f, 0.0f, -10.0f), XMFLOAT3(0.0f, 0.0f, 1.0f));
	camera->CreateShaderVariables(d3d12Device, d3d12GraphicsCommandList);
	m_cameras.push_back(camera);

	camera = new CCamera(CAMERA_TYPE::LIGHT);
	camera->SetViewport(0, 0, DEPTH_BUFFER_WIDTH, DEPTH_BUFFER_HEIGHT, 0.0f, 1.0f);
	camera->SetScissorRect(0, 0, DEPTH_BUFFER_WIDTH, DEPTH_BUFFER_HEIGHT);
	camera->CreateShaderVariables(d3d12Device, d3d12GraphicsCommandList);
	m_cameras.push_back(camera);
}

void CCameraManager::Update()
{
	//// 현재 윈도우가 포커싱 상태인지 알아낸다.
	//HWND hWnd = GetFocus();

	//if (hWnd != nullptr)
	//{
	//	// 윈도우 영역 계산
	//	RECT rect = {};

	//	GetWindowRect(hWnd, &rect);

	//	// 마우스 커서 위치 계산
	//	POINT oldCursor = { (LONG)(rect.right / 2), (LONG)(rect.bottom / 2) };
	//	POINT cursor = {};

	//	// 이 함수는 윈도우 전체 영역을 기준으로 커서의 위치를 계산한다.
	//	GetCursorPos(&cursor);

	//	XMFLOAT2 delta = {};

	//	delta.x = (cursor.x - oldCursor.x) * 20.0f * DT;
	//	delta.y = (cursor.y - oldCursor.y) * 20.0f * DT;

	//	SetCursorPos(oldCursor.x, oldCursor.y);

	//	m_cameras[0]->Rotate(delta.y, delta.x, 0.0f);

	//	XMFLOAT3 shift = {};

	//	if (KEY_HOLD(KEY::UP))
	//	{
	//		shift = Vector3::Add(shift, Vector3::ScalarProduct(30.0f * DT, m_cameras[0]->GetFoward(), false));
	//	}

	//	if (KEY_HOLD(KEY::DOWN))
	//	{
	//		shift = Vector3::Add(shift, Vector3::ScalarProduct(-30.0f * DT, m_cameras[0]->GetFoward(), false));
	//	}

	//	if (KEY_HOLD(KEY::LEFT))
	//	{
	//		shift = Vector3::Add(shift, Vector3::ScalarProduct(-30.0f * DT, m_cameras[0]->GetRight(), false));
	//	}

	//	if (KEY_HOLD(KEY::RIGHT))
	//	{
	//		shift = Vector3::Add(shift, Vector3::ScalarProduct(30.0f * DT, m_cameras[0]->GetRight(), false));
	//	}

	//	m_cameras[0]->Move(shift);
	//}

	m_cameras[0]->Update();
}
