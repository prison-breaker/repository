#pragma once
#include "Object.h"

class CUI : public CObject
{
private:
	bool m_isFixed;      // ī�޶��� ������ ���� �ʾ� ��ġ�� ������������ ���� ����
	bool m_isCursorOver; // ���� Ŀ���� ��ġ�� �� ��ü ���� �ִ����� ���� ����

public:
	CUI();
	virtual ~CUI();
	
	static CUI* Load(ID3D12Device* d3d12Device, ID3D12GraphicsCommandList* d3d12GraphicsCommandList, ifstream& in);

	void SetFixed(bool isFixed);
	bool IsFixed();

	bool IsCursorOver();

	void SetPosition(const XMFLOAT2& position);
	XMFLOAT2 GetPosition();

	void SetSize(const XMFLOAT2& size);
	XMFLOAT2 GetSize();

	virtual void OnCursorOver();		    // ���� Ŀ���� ��ġ�� �� UI ���� ���� �� ȣ��
	virtual void OnCursorLeftButtonDown();  // ���� Ŀ���� ��ġ�� �� UI ���� �ְ�, ���� ���콺 ��ư�� ������ �� ȣ��
	virtual void OnCursorLeftButtonUp();    // ���� Ŀ���� ��ġ�� ��� ����, �� UI�� ������ ���Ȱ� ���� �����ӿ� ������ �� ȣ��
	virtual void OnCursorLeftButtonClick(); // ���� Ŀ���� ��ġ�� �� UI ���� �ְ�, ������ �������� ���� �����ӿ� ������ �� ȣ��

	virtual void LateUpdate();

	virtual void Render(ID3D12GraphicsCommandList* d3d12GraphicsCommandList, CCamera* camera);

private:
	void CheckCursorOver();
};
