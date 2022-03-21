#pragma once

class CBilboardMesh
{
private:
	XMFLOAT3 m_Position{};
	XMFLOAT2 m_Size{};
	
	XMUINT2  m_CellCount{};
	UINT	 m_CellIndex{};

public:
	CBilboardMesh(const XMFLOAT3& Position, const XMFLOAT2& Size);
	CBilboardMesh(const XMFLOAT3& Position, const XMFLOAT2& Size, const XMUINT2& CellCount, UINT CellIndex);
	~CBilboardMesh() = default;

	// �� Ŭ������ GPU�� �����ּҷ� ���Ǳ� ������ ���� �д� ����(Get)�� �ſ� �����Ƿ� ����� ��������!
	void SetPosition(const XMFLOAT3& Position);

	void SetCellIndex(UINT CellIndex);
};
