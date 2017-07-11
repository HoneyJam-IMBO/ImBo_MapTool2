#include "stdafx.h"
#include "GameObject.h"

#include "RenderContainer.h"
#include "RenderContainerSeller.h"
#include "FileBasedMesh.h"
#include "TerrainContainer.h"
#include "Animater.h"

//SetSelectMeshCallback
void TW_CALL SetSelectMeshCallback(const void * value, void * clientData) {
	if (nullptr == clientData) return;
	CGameObject* pObj = reinterpret_cast<CGameObject*>(clientData);
	float index = *static_cast<const float*>(value);
	pObj->SetSelectMeshIndex(index);
	pObj->CreateMenuMeshTextureUI();
	for (int i = 0; i < pObj->GetRenderContainer()->GetvMesh().size(); ++i) {
		if (i == pObj->GetSelectMeshIndex()) {
			pObj->GetRenderContainer()->GetMesh(i)->SetMeshMaterial(RESOURCEMGR->GetMaterial("RED"));
			continue;
		}
		pObj->GetRenderContainer()->GetMesh(i)->SetMeshMaterial(RESOURCEMGR->GetMaterial("DEFAULT"));
	}
}
void TW_CALL GetSelectMeshCallback(void * value, void * clientData) {
	if (nullptr == clientData) return;
	CGameObject* pObj = reinterpret_cast<CGameObject*>(clientData);
	*static_cast<float *>(value) = pObj->GetSelectMeshIndex();
	float x = *static_cast<float *>(value);
}

void TW_CALL SetMeshTextureButtonCallback(void * clientData) {
	CGameObject* pObj = reinterpret_cast<CGameObject*>(clientData);
	pObj->CreateMenuMeshTextureUI();

	//make texture/ set texture
	//pObj->GetRenderContainer()->GetMesh(pObj->GetSelectMeshIndex());
}
void TW_CALL LoadTextureFileCallback(void* clientData) {
	/*
	pSampler = newCSampler>(m_pd3dDevice, m_pd3dDeviceContext);
	pSampler->Begin(PS_TEXTURE_SAMPLER, BIND_PS);
	m_mSampler.insert(pairSampler("DEFAULT", pSampler));
	UINT DefaultSlot = { PS_TEXTURE };
	UINT DefaultFlag = { BIND_PS };
	D3DX11CreateShaderResourceViewFromFile(m_pd3dDevice, _T("../../Assets/default.jpg"), NULL, NULL, &pd3dSRV, NULL);
	pTexture = newCTexture>(m_pd3dDevice, m_pd3dDeviceContext);
	pTexture->Begin(pd3dSRV, pSampler, DefaultSlot, DefaultFlag);
	m_mTexture.insert(pairTexture("DEFAULT", pTexture));
	*/
	StructLoadTextureFile* pData = reinterpret_cast<StructLoadTextureFile*>(clientData);
	//string path = "../inputdata/";
	//path += pData->m_sName;
	wstring wPath{ L"" };
	wPath.assign(pData->m_sName.cbegin(), pData->m_sName.cend());

	char name[64];
	sprintf(name, "Test%d", dynamic_cast<CFileBasedMesh*>(pData->m_pMesh)->GetMeshIndex());
	pData->m_pMesh->SetMeshTexture(0, RESOURCEMGR->CreateTexture(name, (WCHAR*)wPath.c_str()));

	pData->m_pMesh->SetMeshMaterial(RESOURCEMGR->GetMaterial("DEFAULT"));
}
bool CGameObject::Begin() {

	XMStoreFloat4x4(&m_xmf4x4World, XMMatrixIdentity());

	m_pRenderContainer = RCSELLER->GetRenderContainer(m_name);
	if (m_pRenderContainer->GetMesh())//mesh가 있으면
	{//aabb 해당 mesh에서 aabb를 얻어온다.
		m_OriBoundingBox = m_pRenderContainer->GetMesh()->GetAABB();
	}
	else {//없으면 최대 최소 aabb를 얻어온다.
		BoundingBox::CreateFromPoints(m_OriBoundingBox, XMVectorSet(+10.f, +10.f, +10.f, 0.f), XMVectorSet(-10.f, -10.f, -10.f, 0.f));
	}

	if (m_pRenderContainer->GetAnimater()) m_pAnimater = m_pRenderContainer->GetAnimater();

	XMStoreFloat4(&m_xmf4Quaternion, XMQuaternionIdentity());
	return true;
}
bool CGameObject::End() {
	m_vObjectActiveOBBs.clear();
	//-------------------------------component---------------------------
	ClearComponents();
	//-------------------------------component---------------------------
	return true;
}

void CGameObject::Animate(float fTimeElapsed) {
	//if (m_pAnimater) {
	//	//obb animate
	//	m_vObjectActiveOBBs.clear();
	//	for (auto OBB : m_pAnimater->GetActiveOBBs()) {
	//		OBB.Transform(OBB, m_pAnimater->GetMeshOffsetMtx()*GetWorldMtx());
	//		m_vObjectActiveOBBs.push_back(OBB);
	//		DEBUGER->RegistOBB(OBB);
	//	}
	//}

	DEBUGER->RegistCoordinateSys(GetWorldMtx());

	//모든 컴포넌트를 돌면서 Update실행
	for (auto i : m_mapComponents) {
		i.second->Update(fTimeElapsed);
	}
}
void CGameObject::Move(XMVECTOR xmvDir, float fDistance) {
	
	XMVECTOR pos;
	pos = GetPosition() + XMVector3Normalize(xmvDir)*fDistance;

	//추가 
	if (m_indexNaviMesh != -1) {
		XMFLOAT3 xmf3Pos;
		XMStoreFloat3(&xmf3Pos, pos);
		//미리 가봐서 
		if (CNaviObjectManager::IsIntersection(xmf3Pos.x, xmf3Pos.z, m_indexNaviMesh)) {
			//그 자리가 아직 내 navi mesh안이라면
			xmf3Pos.y = CNaviObjectManager::GetHeight(XMFLOAT2(xmf3Pos.x, xmf3Pos.z), m_indexNaviMesh);
		}
		else {
			//아니면 index를 새로 구하는데 만약 navi mesh위가 아니라면
			int new_index = CNaviObjectManager::GetIndex(xmf3Pos.x, xmf3Pos.z, m_indexNaviMesh);
			if (new_index != -1) {
				//만약 다른 navi mesh위에 내가 위치하게 됬다면.
				//navi mesh index를 갱신하고 내 높이도 갱신
				m_indexNaviMesh = new_index;
				xmf3Pos.y = CNaviObjectManager::GetHeight(XMFLOAT2(xmf3Pos.x, xmf3Pos.z), m_indexNaviMesh);
			}
			else {
				//만약 내가 navi mesh위에 있지 않다면
				//나는 이동하지 않는다.
				pos = GetPosition();
			}
		}
	}
	

	
	SetPosition(pos);
}
void CGameObject::Rotate(XMMATRIX xmMtx) {
	XMMATRIX xmmtxRotate = XMMatrixMultiply(xmMtx, XMLoadFloat4x4(&m_xmf4x4World));
	XMStoreFloat4x4(&m_xmf4x4World, xmmtxRotate);
	SetQuaternion(XMQuaternionRotationMatrix(GetWorldMtx()));
}
void CGameObject::Rotate(float x, float y, float z) {
	XMMATRIX xmmtxRotate;
	if (x != 0.0f)
	{
		xmmtxRotate = XMMatrixRotationAxis(GetRight(), (float)XMConvertToRadians(x));
		XMStoreFloat4x4(&m_xmf4x4World, XMMatrixMultiply(xmmtxRotate, XMLoadFloat4x4(&m_xmf4x4World)));
		//SetRotationQuaternion(XMQuaternionRotationAxis(GetRight(), x));
	}
	if (y != 0.0f)
	{
		//플레이어의 로컬 y-축을 기준으로 회전하는 행렬을 생성한다.
		xmmtxRotate = XMMatrixRotationAxis(GetUp(), (float)XMConvertToRadians(y));
		XMStoreFloat4x4(&m_xmf4x4World, XMMatrixMultiply(xmmtxRotate, XMLoadFloat4x4(&m_xmf4x4World)));
		//SetRotationQuaternion(XMQuaternionRotationAxis(GetUp(), y));
	}
	if (z != 0.0f)
	{
		//플레이어의 로컬 z-축을 기준으로 회전하는 행렬을 생성한다.
		xmmtxRotate = XMMatrixRotationAxis(GetLook(), (float)XMConvertToRadians(z));
		XMStoreFloat4x4(&m_xmf4x4World, XMMatrixMultiply(xmmtxRotate, XMLoadFloat4x4(&m_xmf4x4World)));
		//SetRotationQuaternion(XMQuaternionRotationAxis(GetLook(), z));
	}
	
	XMStoreFloat4(&m_xmf4Quaternion, XMQuaternionMultiply(XMQuaternionRotationRollPitchYaw(x, y, z), XMLoadFloat4(&m_xmf4Quaternion)));
	//SetQuaternion(XMQuaternionRotationMatrix(GetWorldMtx()));
}

void CGameObject::RotateWorldAxis(float x, float y, float z) {
	XMMATRIX xmmtxRotate;
	if (x != 0.0f)
	{
		xmmtxRotate = XMMatrixRotationAxis(XMVectorSet(1.f,0.f,0.f,0.f), (float)XMConvertToRadians(x));
		XMStoreFloat4x4(&m_xmf4x4World, XMMatrixMultiply(xmmtxRotate, XMLoadFloat4x4(&m_xmf4x4World)));
		XMStoreFloat4(&m_xmf4Quaternion, XMQuaternionMultiply(XMQuaternionRotationAxis(GetRight(), x), XMLoadFloat4(&m_xmf4Quaternion)));
	}
	if (y != 0.0f)
	{
		//플레이어의 로컬 y-축을 기준으로 회전하는 행렬을 생성한다.
		xmmtxRotate = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), (float)XMConvertToRadians(y));
		XMStoreFloat4x4(&m_xmf4x4World, XMMatrixMultiply(xmmtxRotate, XMLoadFloat4x4(&m_xmf4x4World)));
		XMStoreFloat4(&m_xmf4Quaternion, XMQuaternionMultiply(XMQuaternionRotationAxis(GetUp(), y), XMLoadFloat4(&m_xmf4Quaternion)));
	}
	if (z != 0.0f)
	{
		//플레이어의 로컬 z-축을 기준으로 회전하는 행렬을 생성한다.
		xmmtxRotate = XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), (float)XMConvertToRadians(z));
		XMStoreFloat4x4(&m_xmf4x4World, XMMatrixMultiply(xmmtxRotate, XMLoadFloat4x4(&m_xmf4x4World)));
		XMStoreFloat4(&m_xmf4Quaternion, XMQuaternionMultiply(XMQuaternionRotationAxis(GetLook(), z), XMLoadFloat4(&m_xmf4Quaternion)));
	}

	m_xmf3Rotate = XMFLOAT3{ m_xmf3Rotate.x + x, m_xmf3Rotate.y + y, m_xmf3Rotate.z + z };
	//SetQuaternion(XMQuaternionRotationMatrix(GetWorldMtx()));
}
void CGameObject::SetPosition(XMVECTOR pos) {
	XMStoreFloat3(&m_xmf3Position, pos);

	m_xmf4x4World._41 = m_xmf3Position.x;
	m_xmf4x4World._42 = m_xmf3Position.y;
	m_xmf4x4World._43 = m_xmf3Position.z;
	if (m_pTerrainContainer) {
		m_xmf3Position.y = m_xmf4x4World._42 = GetTerrainHeight();
		 //GetTerrainHeight();
	}
}
void CGameObject::SetPositionX(const float pos){
	m_xmf4x4World._41 = pos;
	m_xmf3Position.x = pos;
}
void CGameObject::SetPositionY(const float pos) {
	m_xmf4x4World._42 = pos;
	m_xmf3Position.y = pos;
}
void CGameObject::SetPositionZ(const float pos) {
	m_xmf4x4World._43 = pos;
	m_xmf3Position.z = pos;
}
void CGameObject::SetWorldMtx(XMMATRIX mtxWorld) {
	XMStoreFloat4x4(&m_xmf4x4World, mtxWorld);
	SetQuaternion(XMQuaternionRotationMatrix(mtxWorld));
	SetPosition(XMVectorSet(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43, 1.0f));
	//SetScale(XMVectorSet(1.f, 1.f, 1.f, 1.f));
}

void CGameObject::SetScale(XMVECTOR xmv){
	XMStoreFloat4(&m_xmf4Scale, xmv);
	SetWorldMtx(XMMatrixAffineTransformation(GetScale(), XMQuaternionIdentity(), GetQuaternion(), GetPosition()));
}

void CGameObject::SetRotation(XMMATRIX mtxRotation){
	XMFLOAT4X4 xmf4x4;
	XMStoreFloat4x4(&xmf4x4, mtxRotation);
	m_xmf4x4World._11 = xmf4x4._11, m_xmf4x4World._12 = xmf4x4._12, m_xmf4x4World._13 = xmf4x4._13;
	m_xmf4x4World._21 = xmf4x4._21, m_xmf4x4World._22 = xmf4x4._22, m_xmf4x4World._23 = xmf4x4._23;
	m_xmf4x4World._31 = xmf4x4._31, m_xmf4x4World._32 = xmf4x4._32, m_xmf4x4World._33 = xmf4x4._33;

}

void CGameObject::SetRight(XMVECTOR xmvRight) {
	XMFLOAT3 xmfRight;
	XMStoreFloat3(&xmfRight, xmvRight);
	m_xmf4x4World._11 = xmfRight.x;
	m_xmf4x4World._12 = xmfRight.y;
	m_xmf4x4World._13 = xmfRight.z;
}
void CGameObject::SetUp(XMVECTOR xmvUp) {
	XMFLOAT3 xmfUp;
	XMStoreFloat3(&xmfUp, xmvUp);
	m_xmf4x4World._21 = xmfUp.x;
	m_xmf4x4World._22 = xmfUp.y;
	m_xmf4x4World._23 = xmfUp.z;
}
void CGameObject::SetLook(XMVECTOR xmvLook) {
	XMFLOAT3 xmfLook;
	XMStoreFloat3(&xmfLook, xmvLook);
	m_xmf4x4World._31 = xmfLook.x;
	m_xmf4x4World._32 = xmfLook.y;
	m_xmf4x4World._33 = xmfLook.z;
}
//--------------------------------getter--------------------------------
XMVECTOR CGameObject::GetRight() {
	XMFLOAT3 xmvPos;

	xmvPos.x = m_xmf4x4World._11;
	xmvPos.y = m_xmf4x4World._12;
	xmvPos.z = m_xmf4x4World._13;

	return XMLoadFloat3(&xmvPos);
}
XMVECTOR CGameObject::GetUp() {
	XMFLOAT3 xmvPos;

	xmvPos.x = m_xmf4x4World._21;
	xmvPos.y = m_xmf4x4World._22;
	xmvPos.z = m_xmf4x4World._23;

	return XMLoadFloat3(&xmvPos);
}
XMVECTOR CGameObject::GetLook() {
	XMFLOAT3 xmvPos;

	xmvPos.x = m_xmf4x4World._31;
	xmvPos.y = m_xmf4x4World._32;
	xmvPos.z = m_xmf4x4World._33;

	return XMLoadFloat3(&xmvPos);
}
XMVECTOR CGameObject::GetPosition() {
	XMFLOAT3 xmvPos;

	xmvPos.x = m_xmf4x4World._41;
	xmvPos.y = m_xmf4x4World._42;
	xmvPos.z = m_xmf4x4World._43;

	return XMLoadFloat3(&xmvPos);
}
XMMATRIX CGameObject::GetWorldMtx() {
	return XMLoadFloat4x4(&m_xmf4x4World);
}
//--------------------------------getter--------------------------------

//-------------------------------componenet----------------------------
//같은 Family의 component는 set할 수 없다. 
bool CGameObject::SetComponent(CComponent* pComponenet) {
	if (!pComponenet) return false;

	//component id검사 이상한 id이면 패스
	//family검사도 해준다.
	for (int i = 0; i < COMPONENTID_END; ++i) {
		if (i == pComponenet->GetID()) {
			for (auto data : m_mapComponents) {
				//family id가 같은것이 있다면 안넣어 줌
				if (data.second->GetFamilyID() == pComponenet->GetFamilyID())
					return false;
			}

			//최종적으로 검사 다 끝나면 넣어줌
			m_mapComponents.insert(pairComponent(pComponenet->GetID(), pComponenet));
			pComponenet->SetOwner(this);
			return true;
		}
	}

	return false;
}

//Get
CComponent* CGameObject::GetComponenet(const component_id& componenetID) {
	mapComponent::const_iterator  find_iterator = m_mapComponents.find(componenetID);
	return find_iterator->second;
}
const CComponent* CGameObject::GetComponenet(const component_id& componenetID)const {
	mapComponent::const_iterator  find_iterator = m_mapComponents.find(componenetID);
	return find_iterator->second;
}

//Clear
void CGameObject::ClearComponents() {
	//모든 컴포넌트를 돌면서 Update실행
	for (auto i : m_mapComponents) {
		i.second->End();
		delete i.second;
	}
	m_mapComponents.clear();
}
//-------------------------------componenet----------------------------

//instance buffer controll base
void CGameObject::SetBufferInfo(void** ppMappedResources, int& nInstance,  CCamera* pCamera) {
	//형변환
	VS_VB_INSTANCE *pnInstances = (VS_VB_INSTANCE *)ppMappedResources[0];
	
	//transpose 이후 정보 주입
	if (m_pAnimater) {
		pnInstances[nInstance].m_xmmtxWorld = XMMatrixTranspose(m_pAnimater->GetMeshOffsetMtx()*GetWorldMtx());
		return;
	}
	pnInstances[nInstance].m_xmmtxWorld = XMMatrixTranspose(GetWorldMtx());

}
void CGameObject::RegistToContainer() {
	m_pRenderContainer->AddObject(this);
}

void CGameObject::RegistToDebuger(){
	BoundingBox BoundingBox;
	GetMainBoundingBox(BoundingBox);
	DEBUGER->RegistAABB(BoundingBox, m_utag);
}

//void CGameObject::SetRenderContainer(CRenderContainerSeller * pSeller) {
//	m_pRenderContainer = pSeller->GetRenderContainer(m_objectID);
//	if (m_pRenderContainer->GetMesh())//mesh가 있으면
//	{//aabb 해당 mesh에서 aabb를 얻어온다.
//		m_OriBoundingBox = m_pRenderContainer->GetMesh()->GetBoundingBox();
//	}
//	else {//없으면 최대 최소 aabb를 얻어온다.
//		BoundingBox::CreateFromPoints(m_OriBoundingBox, XMLoadFloat3(&XMFLOAT3(+FLT_MAX, +FLT_MAX, +FLT_MAX)), XMLoadFloat3(&XMFLOAT3(-FLT_MAX, -FLT_MAX, -FLT_MAX)));
//	}
//}
float CGameObject::GetTerrainHeight(){

	return m_pTerrainContainer->GetHeight(GetPosition(), m_indexNaviMesh);
//	return 100.0f;
}
//flustum culling
bool CGameObject::IsVisible( CCamera* pCamera){
	BoundingBox BoundingBox;
	
	m_bIsVisible = (m_bActive) ? true : false;
	if (m_bActive){
		GetMainBoundingBox(BoundingBox);
		if (pCamera) m_bIsVisible = pCamera->IsInFrustum(BoundingBox);
	}
	return(m_bIsVisible);
}

bool CGameObject::IsCollision(CGameObject * pOther) {
	if (m_pAnimater == nullptr && pOther->GetAnimater() == nullptr) {
		return IsCollisionMeshToMesh(pOther);
	}
	else if (m_pAnimater && pOther->GetAnimater()) {
		return IsCollisionAnimToAnim(pOther);
	}
	else if (m_pAnimater) {
		return IsCollisionAnimToMesh(pOther);
	}
	else if (pOther->GetAnimater()) {
		return pOther->IsCollisionAnimToMesh(this);
	}
}

bool CGameObject::IsCollisionMeshToMesh(CGameObject* pOther){
	CMesh* pMesh1 = GetRenderContainer()->GetMesh(0);
	CMesh * pMesh2 = pOther->GetRenderContainer()->GetMesh(0);

	BoundingBox Myaabb = pMesh1->GetAABB();
	BoundingBox Otheraabb = pMesh2->GetAABB();
	Myaabb.Transform(Myaabb, GetWorldMtx());
	Otheraabb.Transform(Otheraabb, pOther->GetWorldMtx());

	bool val = Myaabb.Intersects(Otheraabb);
	if (val) {
		//mesh - mesh
		int MynObb = pMesh1->GetOBBCnt();
		int OthernObb = pMesh2->GetOBBCnt();
		int nCollisio{ 0 };

		if (MynObb > 0 && OthernObb > 0) {
			val = false;
			for (auto MyObb : pMesh1->GetvOBBObject()) {
				for (auto OtherObb : pMesh2->GetvOBBObject()) {
					BoundingOrientedBox Myobb = MyObb.GetOBB();
					Myobb.Transform(Myobb, GetWorldMtx());
					DEBUGER->RegistOBB(Myobb, GetUTag());

					BoundingOrientedBox Otherobb = OtherObb.GetOBB();
					Otherobb.Transform(Otherobb, pOther->GetWorldMtx());
					DEBUGER->RegistOBB(Otherobb, pOther->GetUTag());

					if (Myobb.Intersects(Otherobb)) //return true;
						nCollisio++;
				}
			}
			if (nCollisio > 0)
				val = true;
		}
		else if (MynObb > 0) {
			val = false;
			int nCollisio{ 0 };
			for (auto MyObb : pMesh1->GetvOBBObject()) {

				BoundingOrientedBox Myobb = MyObb.GetOBB();
				Myobb.Transform(Myobb, GetWorldMtx());
				DEBUGER->RegistOBB(Myobb, GetUTag());
				if (Otheraabb.Intersects(Myobb)) //return true;
					nCollisio++;
			}
			if (nCollisio > 0)
				val = true;
		}
		else if (OthernObb > 0) {
			val = false;
			int nCollisio{ 0 };
			for (auto OtherObb : pMesh2->GetvOBBObject()) {

				BoundingOrientedBox Otherobb = OtherObb.GetOBB();
				Otherobb.Transform(Otherobb, pOther->GetWorldMtx());
				DEBUGER->RegistOBB(Otherobb, pOther->GetUTag());
				if (Myaabb.Intersects(Otherobb)) //return true;
					nCollisio++;
			}
			if (nCollisio > 0)
				val = true;
		}

	}
	return val;
}

bool CGameObject::IsCollisionAnimToAnim(CGameObject * pOther){
	//animation - animation
	CAnimater* pOtherAnimater = pOther->GetAnimater();
	BoundingBox Myaabb = m_pAnimater->GetMainAABB()->GetAABB();
	BoundingBox Otheraabb = pOtherAnimater->GetMainAABB()->GetAABB();

	Myaabb.Transform(Myaabb, m_pAnimater->GetMeshOffsetMtx()*GetWorldMtx());
	Otheraabb.Transform(Otheraabb, pOtherAnimater->GetMeshOffsetMtx() * pOther->GetWorldMtx());

	bool val = Myaabb.Intersects(Otheraabb);
	if (val) {
		CAnimationInfo* pMyAnimationInfo = m_pAnimater->GetCurAnimationInfo();
		CAnimationInfo* pOtherAnimationInfo = pOtherAnimater->GetCurAnimationInfo();

		int MynObb = pMyAnimationInfo->GetActiveOBB().size();
		int OthernObb = pOtherAnimationInfo->GetActiveOBB().size();
		int nCollisio{ 0 };

		if (MynObb > 0 && OthernObb > 0) {//animater - animater
			val = false;
			
			for (auto pMyOBB : pMyAnimationInfo->GetActiveOBB()) {
				for (auto pOtherOBB : pOtherAnimationInfo->GetActiveOBB()) {
					BoundingOrientedBox Myobb = pMyOBB->GetOBB();
					XMMATRIX xmmtxOffset = m_pAnimater->GetMeshOffsetMtx();
					XMMATRIX xmmtxFrame = pMyAnimationInfo->GetCurFrameMtx(pMyOBB->GetMyJointIndex());
					Myobb.Transform(Myobb, xmmtxFrame * xmmtxOffset * GetWorldMtx());
					DEBUGER->RegistOBB(Myobb, GetUTag());

					BoundingOrientedBox Otherobb = pOtherOBB->GetOBB();
					xmmtxOffset = pOtherAnimater->GetMeshOffsetMtx();
					xmmtxFrame = pOtherAnimationInfo->GetCurFrameMtx(pOtherOBB->GetMyJointIndex());
					Otherobb.Transform(Otherobb, xmmtxFrame * xmmtxOffset * pOther->GetWorldMtx());
					DEBUGER->RegistOBB(Otherobb, pOther->GetUTag());

					if (Myobb.Intersects(Otherobb)) //return true;
						nCollisio++;
				}
			}
			if (nCollisio > 0)
				val = true;
		}
		else if (MynObb > 0) {//animater - aabb
			val = false;
			int nCollisio{ 0 };
			CAnimationInfo* pMyAnimationInfo = m_pAnimater->GetCurAnimationInfo();
			for (auto pMyOBB : m_pAnimater->GetCurAnimationInfo()->GetActiveOBB()) {

				BoundingOrientedBox Myobb = pMyOBB->GetOBB();
				XMMATRIX xmmtxOffset = m_pAnimater->GetMeshOffsetMtx();
				XMMATRIX xmmtxFrame = pMyAnimationInfo->GetCurFrameMtx(pMyOBB->GetMyJointIndex());
				Myobb.Transform(Myobb, xmmtxFrame * xmmtxOffset * GetWorldMtx());
				DEBUGER->RegistOBB(Myobb, GetUTag());
				if (Otheraabb.Intersects(Myobb)) //return true;
					nCollisio++;
			}
			if (nCollisio > 0)
				val = true;
		}
		else if (OthernObb > 0) {//aabb - animater
			val = false;
			int nCollisio{ 0 };
			CAnimationInfo* pOtherAnimationInfo = pOtherAnimater->GetCurAnimationInfo();
			for (auto pOtherOBB : pOtherAnimater->GetCurAnimationInfo()->GetActiveOBB()) {

				BoundingOrientedBox Otherobb = pOtherOBB->GetOBB();
				XMMATRIX xmmtxOffset = pOtherAnimater->GetMeshOffsetMtx();
				XMMATRIX xmmtxFrame = pOtherAnimationInfo->GetCurFrameMtx(pOtherOBB->GetMyJointIndex());
				Otherobb.Transform(Otherobb, xmmtxFrame * xmmtxOffset * pOther->GetWorldMtx());
				DEBUGER->RegistOBB(Otherobb, pOther->GetUTag());

				if (Myaabb.Intersects(Otherobb)) //return true;
					nCollisio++;
			}
			if (nCollisio > 0)
				val = true;
		}

	}
	return val;
}

bool CGameObject::IsCollisionAnimToMesh(CGameObject * pOther){

	CMesh* pOtherMesh = pOther->GetRenderContainer()->GetMesh(0);
	CAnimationInfo* pMyAnimationInfo = m_pAnimater->GetCurAnimationInfo();

	BoundingBox Myaabb = m_pAnimater->GetMainAABB()->GetAABB();
	BoundingBox Otheraabb = pOtherMesh->GetAABB();

	Myaabb.Transform(Myaabb, m_pAnimater->GetMeshOffsetMtx()*GetWorldMtx());
	Otheraabb.Transform(Otheraabb, pOther->GetWorldMtx());

	bool val = Myaabb.Intersects(Otheraabb);
	if (val) {

		//animation - mesh
		if (m_pAnimater) {
			int MynObb = pMyAnimationInfo->GetActiveOBB().size();
			int OthernObb = pOtherMesh->GetOBBCnt();
			int nCollisio{ 0 };

			if (MynObb > 0 && OthernObb > 0) {//animater - mesh
				val = false;
				for (auto pOBB : pMyAnimationInfo->GetActiveOBB()) {
					for (auto OtherObb : pOtherMesh->GetvOBBObject()) {
						BoundingOrientedBox Myobb = pOBB->GetOBB();
						XMMATRIX xmmtxOffset = m_pAnimater->GetMeshOffsetMtx();
						XMMATRIX xmmtxFrame = pMyAnimationInfo->GetCurFrameMtx(pOBB->GetMyJointIndex());
						Myobb.Transform(Myobb, xmmtxFrame * xmmtxOffset * GetWorldMtx());
						DEBUGER->RegistOBB(Myobb, UTAG_COLLISION);

						BoundingOrientedBox Otherobb = OtherObb.GetOBB();
						Otherobb.Transform(Otherobb, pOther->GetWorldMtx());
						DEBUGER->RegistOBB(Otherobb, UTAG_COLLISION);

						if (Myobb.Intersects(Otherobb)) //return true;
							nCollisio++;
					}
				}
				if (nCollisio > 0)
					val = true;
			}
			else if (MynObb > 0) {//animater - aabb
				val = false;
				int nCollisio{ 0 };
				for (auto pOBB : pMyAnimationInfo->GetActiveOBB()) {

					BoundingOrientedBox Myobb = pOBB->GetOBB();
					XMMATRIX xmmtxOffset = m_pAnimater->GetMeshOffsetMtx();
					XMMATRIX xmmtxFrame = pMyAnimationInfo->GetCurFrameMtx(pOBB->GetMyJointIndex());
					Myobb.Transform(Myobb, xmmtxFrame * xmmtxOffset * GetWorldMtx());
					DEBUGER->RegistOBB(Myobb, UTAG_COLLISION);
					if (Otheraabb.Intersects(Myobb)) //return true;
						nCollisio++;
				}
				if (nCollisio > 0)
					val = true;
			}
			else if (OthernObb > 0) {//aabb - mesh
				val = false;
				int nCollisio{ 0 };
				for (int i = 0; i < OthernObb; ++i) {

					BoundingOrientedBox Otherobb = pOther->GetRenderContainer()->GetMesh(0)->GetOBBObject(i).GetOBB();
					Otherobb.Transform(Otherobb, pOther->GetWorldMtx());
					DEBUGER->RegistOBB(Otherobb, UTAG_COLLISION);
					if (Myaabb.Intersects(Otherobb)) //return true;
						nCollisio++;
				}
				if (nCollisio > 0)
					val = true;
			}
		}
		//mesh - animation
		else if (pOther->GetAnimater()) {
			CAnimater* pOtherAniamter = pOther->GetAnimater();
			int MynObb = GetRenderContainer()->GetMesh(0)->GetOBBCnt();
			int OthernObb = pOtherAniamter->GetCurAnimationInfo()->GetActiveOBB().size();
			int nCollisio{ 0 };
			CAnimationInfo* pOtherAnimationInfo = pOtherAniamter->GetCurAnimationInfo();

			if (MynObb > 0 && OthernObb > 0) {//mesh - animater
				val = false;
				for (auto pOtherOBB : pOtherAniamter->GetCurAnimationInfo()->GetActiveOBB()) {
					for (int j = 0; j < MynObb; ++j) {
						BoundingOrientedBox Otherobb = pOtherOBB->GetOBB();
						XMMATRIX xmmtxOffset = pOtherAniamter->GetMeshOffsetMtx();
						XMMATRIX xmmtxFrame = pOtherAnimationInfo->GetCurFrameMtx(pOtherOBB->GetMyJointIndex());
						Otherobb.Transform(Otherobb, xmmtxFrame * xmmtxOffset * pOther->GetWorldMtx());
						DEBUGER->RegistOBB(Otherobb, UTAG_COLLISION);

						BoundingOrientedBox Myobb = GetRenderContainer()->GetMesh(0)->GetOBBObject(j).GetOBB();
						Myobb.Transform(Myobb, GetWorldMtx());
						DEBUGER->RegistOBB(Myobb, UTAG_COLLISION);

						if (Myobb.Intersects(Otherobb)) //return true;
							nCollisio++;
					}
				}
				if (nCollisio > 0)
					val = true;
			}
			else if (MynObb > 0) {//mesh - aabb
				val = false;
				int nCollisio{ 0 };
				for (int i = 0; i < MynObb; ++i) {

					BoundingOrientedBox Myobb = GetRenderContainer()->GetMesh(0)->GetOBBObject(i).GetOBB();
					Myobb.Transform(Myobb, GetWorldMtx());
					DEBUGER->RegistOBB(Myobb, UTAG_COLLISION);
					if (Myobb.Intersects(Otheraabb)) //return true;
						nCollisio++;
				}
				if (nCollisio > 0)
					val = true;

			}
			else if (OthernObb > 0) {//aabb - mesh

				val = false;
				int nCollisio{ 0 };
				for (auto pOBB : m_pAnimater->GetCurAnimationInfo()->GetActiveOBB()) {

					BoundingOrientedBox Myobb = pOBB->GetOBB();
					XMMATRIX xmmtxOffset = m_pAnimater->GetMeshOffsetMtx();
					XMMATRIX xmmtxFrame = pOtherAnimationInfo->GetCurFrameMtx(pOBB->GetMyJointIndex());
					Myobb.Transform(Myobb, xmmtxFrame * xmmtxOffset * GetWorldMtx());
					DEBUGER->RegistOBB(Myobb, UTAG_COLLISION);
					if (Otheraabb.Intersects(Myobb)) //return true;
						nCollisio++;
				}
				if (nCollisio > 0)
					val = true;
			}
		}
	}

	return val;
}

void CGameObject::GetMainBoundingBox(BoundingBox& out){
	out = m_OriBoundingBox;
	if (m_pAnimater) {
		out = m_pAnimater->GetMainAABB()->GetAABB();
		out.Transform(out, m_pAnimater->GetMeshOffsetMtx()*GetWorldMtx());
		return;
	}
	//else {
	//	out = m_pRenderContainer->GetMesh()->GetAABB();
	//}

	out.Transform(out, GetWorldMtx());
}

bool CGameObject::CheckPickObject(XMVECTOR xmvProjPickPos, XMVECTOR xmvProjRayDir, float & distance){
	//XMMATRIX xmMtxViewInverse = UPDATER->GetCamera()->GetWorldMtx();
	//XMVECTOR xmvWorldCameraStartPos = UPDATER->GetCamera()->GetPosition();
	//XMVECTOR xmvWorldPickPos = XMVector3TransformCoord(xmvProjPickPos, xmMtxViewInverse);
	//XMVECTOR xmvWorldRayDir = XMVector3Normalize(xmvWorldPickPos - xmvWorldCameraStartPos);
	//BoundingBox BoundingBox;
	//GetMainBoundingBox(BoundingBox);
	//return BoundingBox.Intersects(xmvWorldCameraStartPos, xmvWorldRayDir, distance);

	//BoundingBox BoundingBox;// = m_OriBoundingBox;
	//GetMainBoundingBox(BoundingBox);
	//return BoundingBox.Intersects(xmvWorldCameraStartPos, xmvWorldPickPos, distance);
		//위의 두 벡터 start pos, ray dir는 proj공간의 녀석들이다. 여기에 view mtx의 inverce를 곱하면 world가 나온다

		//들어온건 world ray, world pos다.
		if (m_pAnimater) {
			XMVECTOR xmvWorldCameraStartPos = UPDATER->GetCamera()->GetPosition();
			XMVECTOR xmvWorldPickPos = XMVector3TransformCoord(xmvProjPickPos, UPDATER->GetCamera()->GetWorldMtx());
			XMVECTOR xmvWorldRayDir = XMVector3Normalize(xmvWorldPickPos - xmvWorldCameraStartPos);

			BoundingBox BoundingBox;// = m_OriBoundingBox;
			GetMainBoundingBox(BoundingBox);
			return BoundingBox.Intersects(xmvWorldCameraStartPos, xmvWorldPickPos, distance);
		}
		
		XMMATRIX xmmtxViewWorldInverce = UPDATER->GetCamera()->GetWorldMtx();
		xmmtxViewWorldInverce = xmmtxViewWorldInverce * XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_xmf4x4World));
		XMVECTOR xmvModelRayStartPos = XMVector3TransformCoord(XMVectorSet(0, 0, 0, 1), xmmtxViewWorldInverce);
		XMVECTOR xmvModelPickPos = XMVector3TransformCoord(xmvProjPickPos, xmmtxViewWorldInverce);

		XMVECTOR xmvModelRayDir = XMVector3Normalize(xmvModelPickPos - xmvModelRayStartPos);
		//XMVector3Normalize(XMVector3TransformCoord(xmvRayDir, xmmtxWorldInverce));
		//XMVECTOR xmvModelCameraStartPos = XMVector3TransformCoord(xmvWorldCameraStartPos, xmmtxWorldInverce);
		
		if (m_pRenderContainer->CheckPickMesh(xmvModelRayStartPos, xmvModelRayDir, distance)) {
			XMVECTOR xmvModelPickResultPos = xmvModelRayStartPos + xmvModelRayDir*distance;
			XMVECTOR xmvWorldPickResultPos = XMVector3TransformCoord(xmvModelPickResultPos, GetWorldMtx());
			XMVECTOR xmvPickResult = XMVector3Length(xmvWorldPickResultPos - UPDATER->GetCamera()->GetPosition());
			XMFLOAT4 xmf4Result;
			XMStoreFloat4(&xmf4Result, xmvPickResult);
			distance = xmf4Result.x;
			return true;
		};
	
	//xmvWorldCameraStartPos는 view space의 picking pos다.

	
	//XMMATRIX xmmtxViewInverse = XMMatrixInverse(nullptr, UPDATER->GetCamera()->GetViewMtx());
	//XMVECTOR xmvPickPosition = XMVector3TransformCoord(xmvWorldCameraStartPos, xmmtxViewInverse);
	//XMVECTOR xmvwRayDir = XMVector3Normalize(xmvPickPosition - UPDATER->GetCamera()->GetPosition());
	//
	////veiw camera pos -> world camera pos
	//XMVECTOR xmvWorldStartPos = XMVector3TransformCoord(XMVectorSet(0, 0, 0, 1), xmmtxViewInverse);
	//XMVECTOR xmvCameraWorld = UPDATER->GetCamera()->GetPosition();
	////view ray -> world ray
	//XMVECTOR xmvWorldPickingPos = XMVector3TransformCoord(xmvWorldCameraStartPos, xmmtxViewInverse);
	//XMVECTOR xmvWorldRayDir = XMVector3Normalize(xmvWorldPickingPos - xmvCameraWorld);
	
//	BoundingBox BoundingBox;
//	GetMainBoundingBox(BoundingBox);
//	return BoundingBox.Intersects(xmvWorldCameraStartPos, xmvRayDir, distance);
}

void CGameObject::PickingProc(){
	CreateObjectUI();
	
	CreateMeshUI();

	if (m_pAnimater) {
		m_pAnimater->CreateAnimationUI();
	}
}
void TW_CALL DeleteObjectCallback(void* clientData) {
	CGameObject* pData = reinterpret_cast<CGameObject*>(clientData);
	UPDATER->GetSpaceContainer()->RemoveObject(pData);
}
void TW_CALL GetUTAGCallback(void * value, void * clientData) {
	if (nullptr == clientData) return;
	CGameObject* pObj = reinterpret_cast<CGameObject*>(clientData);
	float tag = (float)pObj->GetUTag();
	*static_cast<float *>(value) = tag;
}

void TW_CALL SetUTAGCallback(const void * value, void * clientData) {
	if (nullptr == clientData) return;
	CGameObject* pObj = reinterpret_cast<CGameObject*>(clientData);
	int index = *static_cast<const float*>(value);
	UPDATER->GetSpaceContainer()->RemoveObject(pObj);
	pObj->SetUTag((utag)index);

	pObj->SetTerrainContainer(nullptr);
	if (index == 3) {
		pObj->SetNaviMeshIndex();
		pObj->SetTerrainContainer(UPDATER->GetTerrainContainer());
	}


	UPDATER->GetSpaceContainer()->AddObject(pObj);
}
void TW_CALL SetALLUTAGCallback(const void * value, void * clientData) {
	if (nullptr == clientData) return;
	CGameObject* pObj = reinterpret_cast<CGameObject*>(clientData);
	int index = *static_cast<const float*>(value);

	for (int i = 0; i < UPDATER->GetSpaceContainer()->GetSpaceNum(); ++i) {
		auto list = UPDATER->GetSpaceContainer()->GetAllSpace()[i]->GetmlpObject()[pObj->GetTag()];
		for (auto pData : list) {
			if (pData->GetName() == pObj->GetName()) {
				UPDATER->GetSpaceContainer()->RemoveObject(pData);
				pData->SetUTag((utag)index);
				UPDATER->GetSpaceContainer()->AddObject(pData);
			}
		}
	}
	
}
static float fMaterial{ 0.f };
void CGameObject::CreateObjectUI(){
	const char* barName = "PickingBar";
	TWBARMGR->DeleteBar(barName);
	TWBARMGR->AddBar(barName);
	//set param
	TWBARMGR->SetBarSize(barName, 250, 500);
	TWBARMGR->SetBarPosition(barName, 0, 0);
	TWBARMGR->SetBarColor(barName, 255, 0, 0);
	TWBARMGR->SetBarContained(barName, true);
	TWBARMGR->SetBarMovable(barName, false);
	TWBARMGR->SetBarResizable(barName, false);
	//set param
	TWBARMGR->AddButtonCB(barName, "Control", "Delete", DeleteObjectCallback, this);
	float maxUtag = utag::UTAG_END - 1;
	TWBARMGR->AddMinMaxBarCB(barName, "Control", "UTAG", SetUTAGCallback, GetUTAGCallback, this,
		0.f, maxUtag, 1.f);
	TWBARMGR->AddMinMaxBarCB(barName, "Control", "ALL_UTAG", SetALLUTAGCallback, GetUTAGCallback, this,
		0.f, maxUtag, 1.f);
	TWBARMGR->AddRotationBar(barName, "Rotation World", "Rotate", this);
	TWBARMGR->AddPositionBar(barName, "Position", "Position", this, 0.f, UPDATER->GetSpaceContainer()->GetSize() - 1.0f, 1.0f);
	TWBARMGR->AddScaleBar(barName, "Scale", "Scale", this, 0.1f, 100.f, 0.1f);

	TWBARMGR->AddMinMaxBarRW(barName, "Material", "Material", &fMaterial, 0.0f, 2.f, 0.05f);

}

void CGameObject::CreateMeshUI(){
	const char* barName = "MeshInfo";
	TWBARMGR->DeleteBar(barName);
	TWBARMGR->AddBar(barName);
	//set param
	TWBARMGR->SetBarSize(barName, 250, 200);
	TWBARMGR->SetBarPosition(barName, 750, 0);
	TWBARMGR->SetBarColor(barName, 255, 0, 255);
	TWBARMGR->SetBarContained(barName, true);
	TWBARMGR->SetBarMovable(barName, false);
	TWBARMGR->SetBarResizable(barName, false);
	//set param

	TWBARMGR->AddMinMaxBarCB(barName, "SelectMesh", "SelectMeshIndex",
		SetSelectMeshCallback, GetSelectMeshCallback, this, 0.f, m_pRenderContainer->GetvMesh().size() - 1, 1.f);

//	TWBARMGR->AddButtonCB(barName, "SetSelect", "MeshTexture", SetMeshTextureButtonCallback, this);
	//char menuName[64];
	//int i{ 0 };
	//for (auto pMesh : m_pRenderContainer->GetvMesh()) {
	//	sprintf(menuName, "Mesh%d", i);
	//	TWBARMGR->AddButtonCB(barName, "SetTextureButton", menuName, SetMeshTextureButtonCallback, m_pRenderContainer->GetMesh(i++) );
	//}
}

void CGameObject::CreateMenuMeshTextureUI(){

	const char* barName{ "LoadTextureFile" };
	TWBARMGR->AddBar(barName);

	vector<wstring> vFile;
	DIRECTORYFINDER->GetFiles(vFile, L"../../Assets", true, true, L".jpg");
	DIRECTORYFINDER->GetFiles(vFile, L"../../Assets", true, true, L".JPG");
	DIRECTORYFINDER->GetFiles(vFile, L"../../Assets", true, true, L".png");
	DIRECTORYFINDER->GetFiles(vFile, L"../../Assets", true, true, L".PNG");

	const char* groupName = "TextureFile";
	char menuName[256];
	int cnt{ 0 };
	m_vStructLoadTextureFile.resize(vFile.size());
	for (auto data : vFile) {
		string s{ "" };
		s.assign(data.cbegin(), data.cend());
		m_vStructLoadTextureFile[cnt] = StructLoadTextureFile{ m_pRenderContainer->GetMesh(m_indexSelectMesh), s };
		sprintf(menuName, "%s", s.c_str());
		TWBARMGR->AddButtonCB(barName, groupName, menuName, LoadTextureFileCallback, &m_vStructLoadTextureFile[cnt]);
		cnt++;
	}
}

void CGameObject::SaveInfo(){
	//tag
	
	EXPORTER->WriteUINT(m_tag); EXPORTER->WriteSpace();
	EXPORTER->WriteUINT(m_utag);
	EXPORTER->WriteEnter();
	//name
	EXPORTER->WriteCHAR(m_name.c_str());
	EXPORTER->WriteEnter();
	//worldmtx
	EXPORTER->WriteFloat4x4(m_xmf4x4World);
	EXPORTER->WriteEnter();
}

void CGameObject::LoadInfo(){
	//tag
	m_tag = (tag)IMPORTER->ReadUINT();
	m_utag = (utag)IMPORTER->ReadUINT();

	//name
	m_name = IMPORTER->Readstring();
	
	//worldmtx
	m_xmf4x4World = IMPORTER->ReadFloat4x4();
	SetWorldMtx(XMLoadFloat4x4(&m_xmf4x4World));
}

CGameObject* CGameObject::CreateObject(string name, tag t, XMMATRIX xmmtxWorld){
	CGameObject* pObject = new CGameObject(name, t);
	pObject->Begin();
	pObject->SetWorldMtx(xmmtxWorld);
	return pObject;
}

void CGameObject::SetNaviMeshIndex(int index){
	if (CNaviObjectManager::IsValiableIndex(index)) {
		m_indexNaviMesh = index;
		SetPosition(CNaviObjectManager::GetNaviMeshPosition(index));
	}
}

void CGameObject::SetNaviMeshIndex(){
	m_indexNaviMesh = CNaviObjectManager::GetValiableIndex(GetPosition());

	if (m_indexNaviMesh == -1) SetNaviMeshIndex(0);
	else SetNaviMeshIndex(m_indexNaviMesh);
}

//생성자는 위에서부터 
CGameObject::CGameObject(string name, tag t, utag ut) : CObject(name, t, ut) {
	XMStoreFloat4x4(&m_xmf4x4World, XMMatrixIdentity());
}
CGameObject ::~CGameObject() {

}