
#include "stdafx.h"
#include "Updater.h"

bool CUpdater::Begin() {
	//--------------------------------------space-------------------------------------
	//space
	SetSpaceContainer(CSpaceContainer::CreateSpaceContainer(512, 0));
	//terrain
	SetTerrainContainer(CTerrainContainer::CreateTerrainContainer(L"Temp", 512, 512, 256, UPDATER->GetSpaceContainer(), true));
	//skybox
	SetSkyBoxContainer(CSkyBoxContainer::CreateSkyBoxContainer(L"Temp", 0, UPDATER->GetSpaceContainer()));

	//directional light
	m_pDirectionalLight = new CDirectionalLight;
	m_pDirectionalLight->Begin(DIRECTIONAL_AMBIENT_LIGHT{
		XMFLOAT4(1.0f, -1.0f, 1.0f, 0.0f),XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f) , XMFLOAT4(1.5f, 1.5f, 1.5f, 1.f),//dir
		XMFLOAT4(0.4f, 0.4f, 0.4f, 1.f), XMFLOAT4(0.1f, 0.1f, 0.1f, 1.f), XMFLOAT4(0.25f, 0.25f, 0.25f, 1.f), XMFLOAT4(0.1f, 0.1f, 0.1f, 1.f)//ambient
	});
	m_pDirectionalLight->SetPosition(XMVectorSet(m_pSpaceContainer->GetSize() / 2.f, m_pSpaceContainer->GetSize(), m_pSpaceContainer->GetSize() / 2.f, 0.f));
	m_pDirectionalLight->Rotate(30.f, 0.f, 0.f);
	//-------------------------------------space-------------------------------------
	return true;
}

bool CUpdater::End() {
	//terrain
	if (m_pTerrainContainer) {
		m_pTerrainContainer->End();
		delete m_pTerrainContainer;
	}
	//skybox
	if (m_pSkyBoxContainer) {
		m_pSkyBoxContainer->End();
		delete m_pSkyBoxContainer;
	}
	//space
	if (m_pSpaceContainer) {
		m_pSpaceContainer->End();
		delete m_pSpaceContainer;
	}

	//directional light
	if (m_pDirectionalLight) {
		m_pDirectionalLight->End();
		delete m_pDirectionalLight;
	}
	m_pDirectionalLight = nullptr;
	ReleseInstance();
	return true;
}

void CUpdater::PreUpdate(float fTimeElapsed){
	m_pCamera->Update(fTimeElapsed);
}

void CUpdater::Update(float fTimeElapsed) {
	
	//-----------------------------------space------------------------------
	//directional light 등록
	m_pDirectionalLight->RegistToContainer();

	m_pSpaceContainer->Animate(fTimeElapsed);
	//m_pSpaceContainer->PrepareRender(m_pCamera);
	if (m_pTerrainContainer)m_pTerrainContainer->Update(m_pCamera);//이건 사실 할 필요가 없는것.
	if (m_pSkyBoxContainer)m_pSkyBoxContainer->Update(m_pCamera, fTimeElapsed);
	//-----------------------------------space------------------------------
}

void CUpdater::PhisicsUpdate(float fTimeElapsed){
	m_pSpaceContainer->PhisicsUpdate(fTimeElapsed);
}

void CUpdater::ChangeSceneContainers() {
	if (m_pSpaceContainer)m_pSpaceContainer->ChangeSpaceData();
	if (m_pTerrainContainer) m_pTerrainContainer->ChangeSpaceData();

	//directional light
	m_pDirectionalLight->SetPosition(XMVectorSet(m_pSpaceContainer->GetSize() / 2.f, m_pSpaceContainer->GetSize(), m_pSpaceContainer->GetSize() / 2.f, 0.f));
	//if(m_pSkyBoxContainer)m_pSkyBoxContainer->ChangeSpaceData();
}

void CUpdater::LoadSpaceInfo(){
	//clear all objects
	m_pSpaceContainer->ClearAllObjects();

	//space info
	float space_size = IMPORTER->ReadFloat();
	float space_lv = IMPORTER->ReadFloat();
	m_pSpaceContainer->SetSpaceSize(space_size);
	m_pSpaceContainer->SetSpaceLevel(space_lv);
	m_pSpaceContainer->ChangeSpaceData();

	//terrain onoff/ skybox onoff
	bool bTerrainContainer = IMPORTER->ReadBool();
	m_pTerrainContainer->SetActive(bTerrainContainer);
	m_pTerrainContainer->ChangeSpaceData();

	bool bSkyBoxContainer = IMPORTER->ReadBool();
	m_pSkyBoxContainer->SetActive(bSkyBoxContainer);

}

void CUpdater::SaveSpaceInfo(){
	//space info
	EXPORTER->WriteFloat(UPDATER->GetSpaceContainer()->GetSpaceSize()); EXPORTER->WriteSpace();
	EXPORTER->WriteFloat(UPDATER->GetSpaceContainer()->GetSpaceLevel());
	EXPORTER->WriteEnter();
	//terrain onoff/ skybox onoff
	bool bTerrainContainer = UPDATER->GetTerrainContainer() != nullptr;
	bool bSkyBoxContainer = UPDATER->GetSkyBoxContainer() != nullptr;
	EXPORTER->WriteBool(bTerrainContainer); EXPORTER->WriteSpace();
	EXPORTER->WriteBool(bSkyBoxContainer);
	EXPORTER->WriteEnter();

}

void CUpdater::LoadTerrainInfo(wstring wsOutputPath, wstring wsSceneName){
	if (m_pTerrainContainer->GetActive()) {//terrain container가 있다면..
		//추가
		CNaviObjectManager::LoadData();

		//추가
		float zn = IMPORTER->ReadFloat();
		m_pTerrainContainer->SetTessFacterZn(zn);
		float zf = IMPORTER->ReadFloat();
		m_pTerrainContainer->SetTessFacterZf(zf);
		float max_facter_value = IMPORTER->ReadFloat();
		m_pTerrainContainer->SetTessFacterMaxFV(max_facter_value);
		float min_facter_value = IMPORTER->ReadFloat();
		m_pTerrainContainer->SetTessFacterMinFV(min_facter_value);

		//m_pTerrainContainer->SetSceneName(wsSceneName);//이름은 나중에 바꾸도록 한다.
		//base texture path
		XMFLOAT3 xmf3Scale;
		//이렇게 되어야 함
		int width = IMPORTER->ReadInt();
		UPDATER->GetTerrainContainer()->SetTerrainWidth(width);
		int length = IMPORTER->ReadInt();
		UPDATER->GetTerrainContainer()->SetTerrainLength(length);
		float fHeightScale = IMPORTER->ReadFloat();
		UPDATER->GetTerrainContainer()->SetHeightScale(fHeightScale);

		wstring wsBaseTexturePath = IMPORTER->ReadWstring();
		UPDATER->GetTerrainContainer()->SetBaseTexture(wsBaseTexturePath);
		//height map texture name

		//터레인 추가분
		//height map data save
		int nData = IMPORTER->ReadInt();//전체 heigmtmap data의 수
		float* pData = new float[nData];//height map의 data
		for (int i = 0; i < nData; ++i) {
			pData[i] = IMPORTER->ReadFloat();
		}
		UPDATER->GetTerrainContainer()->SetHeightData(pData);
		UPDATER->GetTerrainContainer()->ReadyHeightMap();
		//터레인 추가분

		//normal map texture name
		wstring wsNormalDataName = wsOutputPath + wsSceneName + L"NormalMap.bmp";
		UPDATER->GetTerrainContainer()->SetNormalData(IMPORTER->ReadBitmap24(wsNormalDataName.c_str(), width, length));//nomalmap
		UPDATER->GetTerrainContainer()->SetNormalMapTexture(CTexture::CreateTexture(wsNormalDataName.c_str(), PS_SLOT_NORMALMAP, BIND_PS));

		//create splatting info
		int nSplatting = IMPORTER->ReadInt();
		UPDATER->GetTerrainContainer()->GetSplattingInfoManager()->ClearSplattingInfo();
		for (int i = 0; i<nSplatting; ++i) {
			//splatting의 detail texture는 path로 받는다.
			wstring wsDetailTexturePath = IMPORTER->ReadWstring();

			//splatting의 blending info는 [scene name]이름을 토대로 가공한다.
			//[scene name] + [splatting blending info] + [index]
			WCHAR wcBlendinfoPath[256];
			wsprintf(wcBlendinfoPath, L"%s%sBlendInfo%d.bmp", wsOutputPath.c_str(), wsSceneName.c_str(), i);

			UPDATER->GetTerrainContainer()->GetSplattingInfoManager()->CreateSplattingInfo(wsDetailTexturePath.c_str(), wcBlendinfoPath);
		}
		//create splatting info
	}
}

void CUpdater::SaveTerrainInfo(wstring wsOutputPath, wstring wsSceneName){

	if (m_pTerrainContainer->GetActive()) {//terrain container가 있다면..
		//추가
		CNaviObjectManager::SaveData();

		//추가
		float zn = m_pTerrainContainer->GetTessFacterZn();
		EXPORTER->WriteFloat(zn); EXPORTER->WriteSpace();
		float zf = m_pTerrainContainer->GetTessFacterZf();
		EXPORTER->WriteFloat(zf); EXPORTER->WriteSpace();
		float max_facter_value = m_pTerrainContainer->GetTessFacterMaxFV();
		EXPORTER->WriteFloat(max_facter_value); EXPORTER->WriteSpace();
		float min_facter_value = m_pTerrainContainer->GetTessFacterMaxFV();
		EXPORTER->WriteFloat(min_facter_value); EXPORTER->WriteSpace();
		EXPORTER->WriteEnter();

		//base texture path
		float fHeightScale = UPDATER->GetTerrainContainer()->GetHeightScale();
		int width = UPDATER->GetTerrainContainer()->GetTerrainWidth();
		int length = UPDATER->GetTerrainContainer()->GetTerrainLength();
		EXPORTER->WriteInt(width); EXPORTER->WriteSpace();
		EXPORTER->WriteInt(length); EXPORTER->WriteSpace();
		EXPORTER->WriteFloat(fHeightScale); EXPORTER->WriteSpace();
		EXPORTER->WriteEnter();
		EXPORTER->WriteCHAR(UPDATER->GetTerrainContainer()->GetBaseTexture()->GetsPath().c_str());
		EXPORTER->WriteEnter();

		//height map data save
		int nData = width*length;
		EXPORTER->WriteInt(nData); EXPORTER->WriteSpace();//전체 heigmtmap data의 수
		float* pData = m_pTerrainContainer->GetHeightData();//height map의 data
		for (int i = 0; i < nData; ++i) {
			EXPORTER->WriteFloat(pData[i]); EXPORTER->WriteSpace();
		}
		EXPORTER->WriteEnter();

		//normal map texture name
		wstring wsNormalDataName = wsOutputPath + wsSceneName + L"NormalMap.bmp";
		EXPORTER->MakeBitmap24(wsNormalDataName.c_str(), UPDATER->GetTerrainContainer()->GetNormalData(), width, length);
		//이 두개는 scene name을 가지고 가공
		//[scene name] + height map 이렇게

		int nSplatting = UPDATER->GetTerrainContainer()->GetSplattingInfoManager()->GetSplattingInfos().size();
		EXPORTER->WriteInt(nSplatting);
		EXPORTER->WriteEnter();
		for (int i = 0; i<nSplatting; ++i) {
			//splatting의 detail texture는 path로 받는다.
			wstring path = UPDATER->GetTerrainContainer()->GetSplattingInfoManager()->GetSplattingInfos()[i]->GetDetailTexturePath();
			EXPORTER->WriteWstring(path);
			EXPORTER->WriteEnter();
			//splatting의 blending info는 [scene name]이름을 토대로 가공한다.
			//[scene name] + [splatting blending info] + [index]
			WCHAR splattingName[256];
			wsprintf(splattingName, L"%s%sBlendInfo%d.bmp", wsOutputPath.c_str(), wsSceneName.c_str(), i);
			EXPORTER->MakeBitmap24(splattingName, UPDATER->GetTerrainContainer()->GetSplattingInfoManager()->GetSplattingInfos()[i]->GetBlendInfo(), 512, 512);
		}
		//splatting texture name
	}

}

void CUpdater::LoadObjectsInfo(){
	m_pDirectionalLight->LoadInfo();
	m_pSpaceContainer->LoadObjectInfos();
}

void CUpdater::SaveObjectsInfo(){
	m_pDirectionalLight->SaveInfo();
	m_pSpaceContainer->SaveObjectInfos();
}

CGameObject * CUpdater::PickObject(XMVECTOR xmvWorldCameraStartPos, XMVECTOR xmvRayDir, float & distanse){
	float fHitDistance = FLT_MAX;
	float fNearHitDistance = FLT_MAX;
	CGameObject* pNearObj = nullptr;

	pNearObj = m_pSpaceContainer->PickObject(xmvWorldCameraStartPos, xmvRayDir, fNearHitDistance);
	
	if (m_pDirectionalLight->CheckPickObject(xmvWorldCameraStartPos, xmvRayDir, fHitDistance)) {
		if (fNearHitDistance > fHitDistance) {
			pNearObj = m_pDirectionalLight;
			fNearHitDistance = fHitDistance;
		}
	}
	distanse = fNearHitDistance;

	return pNearObj;
}

CUpdater::CUpdater() :CSingleTonBase<CUpdater>("updatersingleton") {

}

CUpdater::~CUpdater() {

}
