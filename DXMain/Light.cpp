#include "stdafx.h"
#include "Light.h"

bool CLight::Begin() { 

	return CGameObject::Begin();
};

//instance buffer controll base
void CLight::SetBufferInfo(void** ppMappedResources, int& nInstance,  CCamera* pCamera) {
	//형변환
	VS_VB_INSTANCE *pnInstances = (VS_VB_INSTANCE *)ppMappedResources[0];
	//transpose 이후 정보 주입
	pnInstances[nInstance].m_xmmtxWorld = XMMatrixTranspose(GetWorldMtx());

}

bool CLight::CheckPickObject(XMVECTOR xmvProjPickPos, XMVECTOR xmvRayDir, float & distance){
	XMVECTOR xmvWorldCameraStartPos = UPDATER->GetCamera()->GetPosition();
	XMVECTOR xmvWorldPickPos = XMVector3TransformCoord(xmvProjPickPos, UPDATER->GetCamera()->GetWorldMtx());
	XMVECTOR xmvWorldRayDir = XMVector3Normalize(xmvWorldPickPos - xmvWorldCameraStartPos);

	BoundingBox BoundingBox;// = m_OriBoundingBox;
	GetMainBoundingBox(BoundingBox);
	return BoundingBox.Intersects(xmvWorldCameraStartPos, xmvWorldRayDir, distance);

	//BoundingBox BoundingBox;
	//GetMainBoundingBox(BoundingBox);
	//return BoundingBox.Intersects(xmvWorldCameraStartPos, xmvRayDir, distance);
	//return false;
}



