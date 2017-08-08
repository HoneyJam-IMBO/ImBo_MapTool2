#pragma once
//이 자식은 현재 나의 position과 quaternion을 저장할 때 쓸 data다
class CPositionInfoData {
public:
	//XMFLOAT3 GetPosition() { return m_xmf3Position; }
	XMVECTOR GetPosition() { return XMLoadFloat3(&m_xmf3Position); }
	XMVECTOR GetQuaternion() { return XMLoadFloat4(&m_xmf4Quaternion); }

	void SetPositionInfo(XMFLOAT3 xmf3Pos) { m_xmf3Position = xmf3Pos; }
	void SetQuaternion(XMFLOAT4 xmf4Qua) { m_xmf4Quaternion = xmf4Qua; }
	void SetPositionInfo(XMVECTOR xmvPos) { XMStoreFloat3(&m_xmf3Position, xmvPos); }
	void SetQuaternion(XMVECTOR xmvQua) { XMStoreFloat4(&m_xmf4Quaternion, xmvQua); }

private:
	XMFLOAT3 m_xmf3Position;
	XMFLOAT4 m_xmf4Quaternion;

public:
	CPositionInfoData(XMFLOAT3 xmf3Pos, XMFLOAT4 xmf4Qua) : m_xmf3Position(xmf3Pos), m_xmf4Quaternion(xmf4Qua) {};
	CPositionInfoData(XMVECTOR xmvPos, XMVECTOR xmvQua){
		XMStoreFloat3(&m_xmf3Position, xmvPos);
		XMStoreFloat4(&m_xmf4Quaternion, xmvQua); 
	};
	~CPositionInfoData() {};

};