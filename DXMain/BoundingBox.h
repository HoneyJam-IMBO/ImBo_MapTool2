#pragma once

#include "GameObject.h"
//#include "AnimationInfo.h"

struct VS_VB_BOUNDINGBOX_INSTANCE {
	XMFLOAT3 m_xmf3Pos;
	XMFLOAT4 m_xmf4Quaternion;
	XMFLOAT4 m_xmf4Extend;
};

class CBoundingBox : public CGameObject{
public:
	bool Begin(XMVECTOR Position, XMVECTOR Extend, XMVECTOR Quaternion = XMQuaternionIdentity());
	virtual bool End() { return CGameObject::End(); };

	//debug buffer controll
	virtual void SetDebugBufferInfo(void** ppMappedResources, int& nInstance,  CCamera* pCamera);
	//buffer controll
	virtual void SetBufferInfo(void** ppMappedResources, int& nInstance,  CCamera* pCamera);

	void SetBoundingBoxInfo(BoundingBox& aabb, utag ut = UTAG_DEFAULT);
	void SetBoundingBoxInfo(BoundingOrientedBox& obb, utag ut = UTAG_DEFAULT);
	void SetBoundingBoxInfo(CBoundingBox& boundingBox);

	BoundingBox GetAABB();
	BoundingOrientedBox GetOBB();

	virtual bool IsVisible( CCamera* pCamera) { return true; };//계층구조의 녀석들은 다시 만들어줄 필요가 있음

	//test
	float& GetMin() { return m_fMin; }
	float& GetMax() { return m_fMax; }
	void SetMin(float t) { m_fMin = t; }
	void SetMax(float t) { m_fMax = t; }
	void SetMyJointIndex(int index) { m_nMyJointIndex = index; }
	int GetMyJointIndex() { return m_nMyJointIndex; }
	void SetMyIndex(int index) { m_nMyIndex = index; }
	int GetMyIndex() { return m_nMyIndex; }

	bool GetToolActive() { return m_bToolActive; }
	void SetToolActive(bool b) { m_bToolActive = b; }
	//void SetAnimater(CAnimationInfo* pAnimationInfo) { m_pAnimationInfo = pAnimationInfo; }
private:
	//test
	float m_fMin;
	float m_fMax;

	int m_nMyJointIndex{ 0 };
	int m_nMyIndex{ 0 };

	bool m_bToolActive{ true };
	//CAnimationInfo* m_pAnimationInfo;
public:
	CBoundingBox();
	~CBoundingBox();
};
