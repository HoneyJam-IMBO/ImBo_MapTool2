#include "stdafx.h"
#include "PositionInfoManager.h"

vector<CPositionInfoData> CPositionInfoManager::m_vPositionInfoData;
float CPositionInfoManager::m_fActionSpeed = 1.0f;

void TW_CALL PISaveButtonCallback(void * clientData) {
	//CPositionInfoManager* pPositionManager = reinterpret_cast<CPositionInfoManager*>(clientData);
	wstring wsName;
	cout << "Save_PositionInfo - name : " << endl;
	wcin >> wsName;
	CPositionInfoManager::SaveData(wsName);
}
void TW_CALL PILoadButtonCallback(void * clientData) {
	//CPositionInfoManager* pPositionManager = reinterpret_cast<CPositionInfoManager*>(clientData);
	string sName;
	cout << "Load_PositionInfo - name : " << endl;
	cin >> sName;
	CPositionInfoManager::LoadData(sName);
}
void CPositionInfoManager::Begin(){
	const char* barName{ "TOOL_MODE" };
	TWBARMGR->AddButtonCB(barName, "PositionInfo", "Save_poi", PISaveButtonCallback, nullptr);
	TWBARMGR->AddButtonCB(barName, "PositionInfo", "Load_poi", PILoadButtonCallback, nullptr);
	TWBARMGR->AddMinMaxBarRW(barName, "PositionInfo", "Speed_poi", &m_fActionSpeed, 0.001f, 100.f, 0.001f);
}

void CPositionInfoManager::End(){
	m_vPositionInfoData.clear();
}

void CPositionInfoManager::DeletePositionInfoData(int index){
	if (m_vPositionInfoData.size() < index) return;
	auto Iter = m_vPositionInfoData.begin();
	for (int i = 0; i < index; ++i) {
		Iter++;
	}
	m_vPositionInfoData.erase(Iter);

}

void CPositionInfoManager::SaveData(wstring name){
	
	EXPORTER->Begin(wroot_path + name);

	EXPORTER->WriteFloat(m_fActionSpeed); EXPORTER->WriteSpace();
	EXPORTER->WriteInt(m_vPositionInfoData.size()); EXPORTER->WriteSpace();
	for (auto Info : m_vPositionInfoData) {
		XMFLOAT3 xmf3Pos;
		XMStoreFloat3(&xmf3Pos, Info.GetPosition());
		//pos
		EXPORTER->WriteFloat(xmf3Pos.x); EXPORTER->WriteSpace();
		EXPORTER->WriteFloat(xmf3Pos.y); EXPORTER->WriteSpace();
		EXPORTER->WriteFloat(xmf3Pos.z); EXPORTER->WriteSpace();

		XMFLOAT4 xmf4Qua;
		XMStoreFloat4(&xmf4Qua, Info.GetQuaternion());

		//qua
		EXPORTER->WriteFloat(xmf4Qua.x); EXPORTER->WriteSpace();
		EXPORTER->WriteFloat(xmf4Qua.y); EXPORTER->WriteSpace();
		EXPORTER->WriteFloat(xmf4Qua.z); EXPORTER->WriteSpace();
		EXPORTER->WriteFloat(xmf4Qua.w); EXPORTER->WriteSpace();
		EXPORTER->WriteEnter();
	}

	EXPORTER->End();
}

void CPositionInfoManager::LoadData(string name){
	m_vPositionInfoData.clear();

	IMPORTER->Begin(root_path + name);

	m_fActionSpeed = IMPORTER->ReadFloat();
	int nCnt = IMPORTER->ReadInt();
	for (int i = 0; i < nCnt; ++i) {
		XMFLOAT3 xmf3Pos;
		//pos
		xmf3Pos.x = IMPORTER->ReadFloat();
		xmf3Pos.y = IMPORTER->ReadFloat();
		xmf3Pos.z = IMPORTER->ReadFloat();

		XMFLOAT4 xmf4Qua;
		//qua
		xmf4Qua.x = IMPORTER->ReadFloat(); 
		xmf4Qua.y = IMPORTER->ReadFloat(); 
		xmf4Qua.z = IMPORTER->ReadFloat(); 
		xmf4Qua.w = IMPORTER->ReadFloat(); 

		m_vPositionInfoData.push_back(CPositionInfoData(xmf3Pos, xmf4Qua));
	}

	IMPORTER->End();
}

void CPositionInfoManager::ProcessInput(CCamera* pCamera){
	if (INPUTMGR->KeyDown(VK_R)) {
		XMVECTOR xmvQua = XMQuaternionRotationMatrix(pCamera->GetWorldMtx());
		CPositionInfoManager::PushPositionInfoData(CPositionInfoData(pCamera->GetPosition(), xmvQua));
	}
	else if (INPUTMGR->KeyDown(VK_T)) {
		size_t i = CPositionInfoManager::GetInfoCnt();
		if (i > 0) CPositionInfoManager::DeletePositionInfoData(i - 1);
	}
	else if (INPUTMGR->KeyDown(VK_Y)) {
		CPositionInfoManager::ClearPositionInfoData();
	}
	else if (INPUTMGR->KeyDown(VK_F)) {
		pCamera->ActionCamStart();
	}
}

void CPositionInfoManager::RenderAllPositionInfo(){
	for (auto Info : m_vPositionInfoData) {
		XMMATRIX xmMtx = XMMatrixAffineTransformation(XMVectorSet(1, 1, 1, 1), XMQuaternionIdentity(), Info.GetQuaternion(), Info.GetPosition());
		DEBUGER->RegistCoordinateSys(xmMtx);
	}
}
