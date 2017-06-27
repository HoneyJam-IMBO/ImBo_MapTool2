#include "stdafx.h"
#include "NaviVertex.h"


void CNaviVertex::Begin()
{
}

void CNaviVertex::End(){

}

CNaviVertex * CNaviVertex::CreateNaviVertex(XMFLOAT3 pos){
	CNaviVertex* pVertex = new CNaviVertex;
	pVertex->SetPosition(pos);

	return pVertex;
}

void CNaviVertex::RefreshPosition(){
	for (auto pNaviObject : m_vpNaviObject) {
		pNaviObject->RefreshPlane();
	}
}

void TW_CALL DeleteVertexButtonCallback(void * clientData) {
	CNaviVertex* pVertex = reinterpret_cast<CNaviVertex*>(clientData);

	//control ui 삭제
	const char* pName = "NaviMeshControlBar";
	char* pGroupName = "NaviVertexControl";
	char pMenuName[64];
	sprintf(pMenuName, "Vertex%d", pVertex->GetIndex());
	TWBARMGR->DeleteVar(pName, pMenuName);

	//나와 연관된 애들에게서 나를 삭제
	//auto Iter = pVertex->GetNaviObjects().begin();
	//while (Iter != pVertex->GetNaviObjects().end()) {
	//	(*Iter)->DeleteVertexCallback();
	//	CNaviObjectManager::DeleteNaviObject((*Iter)->GetNaviObjectID());
	//	Iter = Iter++;
	//}
	for (auto pNaviObject : pVertex->GetNaviObjects()) {
		pNaviObject->DeleteVertexCallback(pVertex);
		
		CNaviObjectManager::DeleteNaviObject(pNaviObject->GetNaviObjectID());
	}
	//자신을 manager의 vertex들을 지운다.
	CNaviObjectManager::DeleteVertex(pVertex->GetIndex());
}

void TW_CALL GetXCallback(void * value, void * clientData) {
	if (nullptr == clientData) return;
	CNaviVertex* pObj = reinterpret_cast<CNaviVertex*>(clientData);
	float x = (float)pObj->GetPosition().x;
	*static_cast<float *>(value) = x;
}

void TW_CALL SetXCallback(const void * value, void * clientData) {
	if (nullptr == clientData) return;
	CNaviVertex* pObj = reinterpret_cast<CNaviVertex*>(clientData);
	float x = *static_cast<const float*>(value);
	pObj->SetPositionX(x);
	pObj->RefreshPosition();
}


void TW_CALL GetYCallback(void * value, void * clientData) {
	if (nullptr == clientData) return;
	CNaviVertex* pObj = reinterpret_cast<CNaviVertex*>(clientData);
	float x = (float)pObj->GetPosition().y;
	*static_cast<float *>(value) = x;
}

void TW_CALL SetYCallback(const void * value, void * clientData) {
	if (nullptr == clientData) return;
	CNaviVertex* pObj = reinterpret_cast<CNaviVertex*>(clientData);
	float x = *static_cast<const float*>(value);
	pObj->SetPositionY(x);
	pObj->RefreshPosition();
}

void TW_CALL GetZCallback(void * value, void * clientData) {
	if (nullptr == clientData) return;
	CNaviVertex* pObj = reinterpret_cast<CNaviVertex*>(clientData);
	float x = (float)pObj->GetPosition().z;
	*static_cast<float *>(value) = x;
}

void TW_CALL SetZCallback(const void * value, void * clientData) {
	if (nullptr == clientData) return;
	CNaviVertex* pObj = reinterpret_cast<CNaviVertex*>(clientData);
	float x = *static_cast<const float*>(value);
	pObj->SetPositionZ(x);
	pObj->RefreshPosition();
}
void CNaviVertex::CreateControlUI(){
	const char* pName = "NaviMeshControlBar";
	char* pGroupName = "NaviVertexControl";
	char pMenuName[64];
	sprintf(pMenuName, "Vertex%d", m_index);
	TWBARMGR->AddButtonCB(pName, pGroupName, pMenuName, DeleteVertexButtonCallback, this);
	sprintf(pMenuName, "Vertex%d_x", m_index);
	TWBARMGR->AddMinMaxBarCB(pName, pGroupName, pMenuName, SetXCallback, GetXCallback, this, 
		0.f, UPDATER->GetSpaceContainer()->GetSize(), 0.1f);
	sprintf(pMenuName, "Vertex%d_y", m_index);
	TWBARMGR->AddMinMaxBarCB(pName, pGroupName, pMenuName, SetYCallback, GetYCallback, this,
		0.f, UPDATER->GetSpaceContainer()->GetSize(), 0.1f);
	sprintf(pMenuName, "Vertex%d_z", m_index);
	TWBARMGR->AddMinMaxBarCB(pName, pGroupName, pMenuName, SetZCallback, GetZCallback, this,
		0.f, UPDATER->GetSpaceContainer()->GetSize(), 0.1f);

}

void CNaviVertex::DeleteNaviObject(int index){
	auto Iter = m_vpNaviObject.begin();

	for (auto pNaviObject : m_vpNaviObject) {
		if (pNaviObject->GetNaviObjectID() == index) break;
		Iter++;
	}
	m_vpNaviObject.erase(Iter);
	//지웠는데 내가 가진 object가 없다면 난 허공에 뜬 점이 되니까 지워줌
	if (m_vpNaviObject.empty()) {
		CNaviObjectManager::DeleteVertex(m_index);
	}
	//여기서 다른 객체가 아직 이 정점을 들고있어서 문제가 될 수있는데 
	//그 경우에는 이 매커니즘에서 이미 오류가 난거기 때문에
	//이 로직에는 이상이 없는 것이다.
}
