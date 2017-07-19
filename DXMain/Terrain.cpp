#include "stdafx.h"
#include "Terrain.h"
#include "TerrainContainer.h"

bool CTerrain::Begin() {
	//object_id set
//	m_objectID = object_id::OBJECT_TERRAIN;
	SetPositionY(128);

	if (m_pTerrainContainer->GetIsTool()) {
		CGameObject::Begin();
		return true;
	}
	else {
		XMStoreFloat4x4(&m_xmf4x4World, XMMatrixIdentity());
		XMStoreFloat4(&m_xmf4Quaternion, XMQuaternionIdentity());
		m_pRenderContainer = RCSELLER->GetRenderContainer("terrain");

		float fx = static_cast<float>(m_pTerrainContainer->GetSpaceContainer()->GetOneSpaceSize());
		float fy = static_cast<float>(m_pTerrainContainer->GetSpaceContainer()->GetSpaceSize());
		float fz = static_cast<float>(m_pTerrainContainer->GetSpaceContainer()->GetOneSpaceSize());
		BoundingBox::CreateFromPoints(m_OriBoundingBox, XMVectorSet(0.f, 0.f, 0.f, 0.f), XMVectorSet(fx, fy, fz, 0.f));

		return true;
	}
}
bool CTerrain::End() {

	return CGameObject::End();
}

void CTerrain::SetTerrainContainer(CTerrainContainer * pTerrainContainer){
	m_pTerrainContainer = pTerrainContainer;
}

#define OFFSET 5

bool CTerrain::CheckPickObject(XMVECTOR xmvProjCameraStartPos, XMVECTOR xmvProjRayDir, float & distance){
	float fHitDistance = FLT_MAX;
	float fNearHitDistance = FLT_MAX;
	bool bIntersection = false;

	XMMATRIX xmMtxViewInverse = UPDATER->GetCamera()->GetWorldMtx();
	XMVECTOR xmvWorldCameraStartPos = UPDATER->GetCamera()->GetPosition();
	XMVECTOR xmvWorldRayDir = XMVector3Normalize(XMVector3TransformCoord(xmvProjCameraStartPos, xmMtxViewInverse) - xmvWorldCameraStartPos);
	float start_x = m_xmf3Position.x;
	float start_z = m_xmf3Position.z;
	auto pSpaceContainer = m_pTerrainContainer->GetSpaceContainer();
	int one_space_size = pSpaceContainer->GetOneSpaceSize();
	int loop_num = one_space_size / OFFSET;
	for (int i = 0; i < loop_num; ++i) {
		for (int j = 0; j < loop_num; ++j) {
			float pivot_x = start_x + i * OFFSET;
			float pivot_z = start_z + j * OFFSET;
			//0 0 
			XMVECTOR v0 = XMVectorSet(pivot_x			, m_pTerrainContainer->GetHeight(XMFLOAT2(pivot_x, pivot_z))					, pivot_z			, 1.0f);
			//1 0
			XMVECTOR v1 = XMVectorSet(pivot_x + OFFSET	, m_pTerrainContainer->GetHeight(XMFLOAT2(pivot_x + OFFSET, pivot_z))			, pivot_z			, 1.0f);
			//0 1
			XMVECTOR v2 = XMVectorSet(pivot_x			, m_pTerrainContainer->GetHeight(XMFLOAT2(pivot_x, pivot_z + OFFSET))			, pivot_z + OFFSET	, 1.0f);
			//1 1
			XMVECTOR v3 = XMVectorSet(pivot_x + OFFSET	, m_pTerrainContainer->GetHeight(XMFLOAT2(pivot_x + OFFSET, pivot_z + OFFSET))	, pivot_z + OFFSET	, 1.0f);

			if (TriangleTests::Intersects(xmvWorldCameraStartPos, xmvWorldRayDir, v0, v1, v2, fHitDistance)) {//ray와 충돌했다면
				if (fNearHitDistance > fHitDistance) {//이전의 가장 가까운 녀석과 비교
					fNearHitDistance = fHitDistance;
					distance = fHitDistance;//더 가까우면 가장 가까운 객체 변경
					bIntersection = true;
				}
			}
			if (TriangleTests::Intersects(xmvWorldCameraStartPos, xmvWorldRayDir, v1, v2, v3, fHitDistance)) {//ray와 충돌했다면
				if (fNearHitDistance > fHitDistance) {//이전의 가장 가까운 녀석과 비교
					fNearHitDistance = fHitDistance;
					distance = fHitDistance;//더 가까우면 가장 가까운 객체 변경
					bIntersection = true;
				}
			}
		}
	}
	XMFLOAT4 xmf3TerrainPickPos;
	if (bIntersection) {
		XMVECTOR xmvWorldRayStart = UPDATER->GetCamera()->GetPosition();
		XMVECTOR xmvPickPosition = XMVector3TransformCoord(xmvProjCameraStartPos, xmMtxViewInverse);
		XMVECTOR xmvRayDir = XMVector3Normalize(xmvPickPosition - xmvWorldRayStart);
		XMStoreFloat4(&xmf3TerrainPickPos, xmvWorldRayStart + xmvRayDir * distance);
		//xmf3TerrainPickPos.y = m_pTerrainContainer->GetHeight(XMFLOAT2(xmf3TerrainPickPos.x, xmf3TerrainPickPos.z));
		//XMVECTOR xmvTerrainPickPos = XMLoadFloat4(&xmf3TerrainPickPos);
		//XMFLOAT4 result;

		//XMStoreFloat4(&result, XMVector3Length(xmvTerrainPickPos - xmvWorldRayStart));
		//distance = result.x;

		m_pTerrainContainer->SetPicpos(xmf3TerrainPickPos.x, xmf3TerrainPickPos.z);
	}
	else {

	}
	return bIntersection;

	//XMVECTOR xmvWorldPickPos = XMVector3TransformCoord(xmvProjCameraStartPos, xmMtxViewInverse);
	//XMVECTOR xmvWorldRayDir = XMVector3Normalize(xmvWorldPickPos - xmvWorldCameraStartPos);
	//BoundingBox BoundingBox;
	//GetMainBoundingBox(BoundingBox);
	//bool b = BoundingBox.Intersects(xmvWorldCameraStartPos, xmvWorldRayDir, distance);
	//
	//
	//if (b) {
	//	XMVECTOR xmvWorldRayStart = UPDATER->GetCamera()->GetPosition();
	//	XMVECTOR xmvPickPosition = XMVector3TransformCoord(xmvProjCameraStartPos, xmMtxViewInverse);
	//	XMVECTOR xmvRayDir = XMVector3Normalize(xmvPickPosition - xmvWorldRayStart);
	//	XMStoreFloat4(&xmf3TerrainPickPos, xmvWorldRayStart + xmvRayDir * distance);
	//	//xmf3TerrainPickPos.y = m_pTerrainContainer->GetHeight(XMFLOAT2(xmf3TerrainPickPos.x, xmf3TerrainPickPos.z));
	//	//XMVECTOR xmvTerrainPickPos = XMLoadFloat4(&xmf3TerrainPickPos);
	//	//XMFLOAT4 result;
	//	
	//	//XMStoreFloat4(&result, XMVector3Length(xmvTerrainPickPos - xmvWorldRayStart));
	//	//distance = result.x;
	//
	//	m_pTerrainContainer->SetPicpos(xmf3TerrainPickPos.x, xmf3TerrainPickPos.z);
	//}
	//else {
	//
	//}
	//return b;

	//XMMATRIX xmmtxViewInverse = XMMatrixInverse(nullptr, UPDATER->GetCamera()->GetViewMtx());
	//
	////veiw camera pos -> world camera pos
	//XMVECTOR xmvWorldStartPos = XMVector3TransformCoord(XMVectorSet(0, 0, 0, 1), xmmtxViewInverse);
	//XMVECTOR xmvCameraWorld = UPDATER->GetCamera()->GetPosition();
	////view ray -> world ray
	//XMVECTOR xmvWorldPickingPos = XMVector3TransformCoord(xmvWorldCameraStartPos, xmmtxViewInverse);
	//XMVECTOR xmvWorldRayDir = XMVector3Normalize(xmvWorldPickingPos - xmvWorldStartPos);
	//BoundingBox BoundingBox;
	//GetMainBoundingBox(BoundingBox);
	//return BoundingBox.Intersects(xmvWorldStartPos, xmvRayDir, distance);

	//BoundingBox BoundingBox;
	//GetMainBoundingBox(BoundingBox);
	//bool b = BoundingBox.Intersects(xmvWorldCameraStartPos, xmvRayDir, distance);
	//XMFLOAT4 xmf3TerrainPickPos;
	//if (b) {
	//	XMStoreFloat4(&xmf3TerrainPickPos, xmvWorldCameraStartPos + xmvRayDir * distance);
	//	m_pTerrainContainer->SetPicpos(xmf3TerrainPickPos.x, xmf3TerrainPickPos.z);
	//}
	//else {
	//
	//}
	//return b;
}

CTerrain* CTerrain::CreateTerrain(CTerrainContainer * pTerrainContainer, int x, int y){
	CTerrain* pTerrain = new CTerrain;
	pTerrain->SetTerrainContainer(pTerrainContainer);
	pTerrain->Begin();
	pTerrain->SetPosition(XMVectorSet(x*pTerrainContainer->GetSpaceContainer()->GetOneSpaceSize(), 
		0.f, y*pTerrainContainer->GetSpaceContainer()->GetOneSpaceSize(), 0.f));
	pTerrainContainer->GetSpaceContainer()->AddObject(pTerrain);

	return pTerrain;
}

void CTerrain::SaveInfo()
{
}

void CTerrain::LoadInfo()
{
}

CTerrain::CTerrain() : CGameObject("terrain", tag::TAG_TERRAIN) {

}
CTerrain::~CTerrain() {

}