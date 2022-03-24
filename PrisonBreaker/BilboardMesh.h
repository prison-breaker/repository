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

	// 이 클래스는 GPU의 가상주소로 사용되기 때문에 값을 읽는 연산(Get)은 매우 느리므로 사용을 자제하자!
	// + 이 클래스는 CUIAnimationController에서만 GPU의 가상주소로 사용되지 않기 때문에 유일하게 Get을 사용할 수 있다.
	void SetPosition(const XMFLOAT3& Position);
	const XMFLOAT3& GetPosition() const;

	void SetSize(const XMFLOAT2& Size);
	const XMFLOAT2& GetSize() const;

	void SetCellIndex(UINT CellIndex);
	UINT GetCellIndex() const;
};
