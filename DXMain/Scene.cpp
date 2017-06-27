#include "stdafx.h"
#include "Scene.h"
#include "DirectXFramework.h"

bool CScene::Begin(string path){


	return true;
}

bool CScene::End(){
	
	return true;
}

void CScene::Animate(float fTimeElapsed){

}

CScene * CScene::CreateScene(string name, CDirectXFramework* pFramework){
	CScene* pScene = new CScene(name);
	pScene->LoadScene(name);
	pScene->SetFrameWork(pFramework);
	return pScene;
	//return nullptr;
}

void CScene::LoadScene(string path){
	m_sName = GetFileName(path);
	char meshRoot[256] = { "../../Assets/SceneResource/" };

	m_wsMeshRoot;
	strcat(meshRoot, m_sName.c_str());
	string sMeshRoot{ meshRoot };

	m_wsMeshRoot.assign(sMeshRoot.cbegin(), sMeshRoot.cend());
	LoadResource(m_wsMeshRoot);
	
	IMPORTER->Begin(path);
	//output path
	wstring wsOutputPath = IMPORTER->ReadWstring();
	//scene name
	wstring wsSceneName = IMPORTER->ReadWstring();
	m_sName.assign(wsSceneName.cbegin(), wsSceneName.cend());

	UPDATER->LoadSpaceInfo();
	UPDATER->LoadTerrainInfo(wsOutputPath, wsSceneName);
	UPDATER->LoadObjectsInfo();
	//effect info
	RENDERER->LoadEffectInfo(wsOutputPath, wsSceneName);

	////test
	//CGameObject* pObject = new CGameObject("water", tag::TAG_REFLECTION);
	//pObject->Begin();
	////pObject->SetPosition(XMVectorSet(space_size / 2, space_size / 2, space_size / 2, 0));
	//pObject->SetScale(XMVectorSet(4, 4, 4, 1));
	//pObject->SetPosition(XMVectorSet(100, 3, 100, 0));
	//UPDATER->GetSpaceContainer()->AddObject(pObject);
	////test

	//object info save
	//m_pSpaceContainer->WriteObjects();
	//EXPORTER->WriteEnter();

	IMPORTER->End();

	GLOBALVALUEMGR->SetLoad(true);

}

void CScene::LoadResource(wstring wsMeshRoot){
	//read resource root 
	m_wsMeshRoot = wsMeshRoot;

	RESOURCEMGR->ReleaseAllResource();
	RCSELLER->ClearStempRenderContainer();

	//해당 dir아래의 모든 gjm파일을 로드
	vector<wstring> vFile;
	DIRECTORYFINDER->GetFiles(vFile, m_wsMeshRoot, true, true, L".gjm");
	DIRECTORYFINDER->GetFiles(vFile, m_wsMeshRoot, true, true, L".GJM");

	string name;
	for (auto fileName : vFile) {
		string sPath{ "" };
		sPath.assign(fileName.begin(), fileName.end());
		/*여기서 file 이름을 가지고 name을 정하도록 한다.
		file 명에서 확장자를 제거하면 될 것이다.
		sprintf(name, "파 일 명", i);*/
		name = GetFileName(sPath);
		RESOURCEMGR->CreateMultiMesh(sPath, name);
		//sprintf(name, "StempMesh%d", i++);
	}

	//RESOURCEMGR->CreateMultiMesh("../../Assets/Model/TestPlayer/test_player_noOBB.gjm", "test_player");
	//RESOURCEMGR->CreateTexture("DEFAULT", _T("../../Assets/default.jpg"), PS_TEXTURE, BIND_PS);

	
	RCSELLER->CreateStempRenderContainer();

	//RCSELLER->GetRenderContainer("test_player")->GetMaterials().clear();
	//RCSELLER->GetRenderContainer("test_player")->AddMaterial(RESOURCEMGR->GetMaterial("FBX"));
}
