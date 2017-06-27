#pragma once

#include "NaviObjectManager.h"
#include "Scene.h"
//seller
#include "RenderContainerSeller.h"
//object
#include "FreeCamera.h"
#include "ThirdPersonCamera.h"
#include "Bunny.h"
#include "TestCube.h"
//player
#include "Player.h"
//light
#include "PointLight.h"
#include "SpotLight.h"
#include "CapsuleLight.h"
#include "DirectionalLight.h"

static XMFLOAT4 xmf4DirectionalLightColor;

class CSceneMain;
struct LoadFileStruct {
	CSceneMain* m_pScene;
	string Filename;
};

class CSceneMain :public CScene{

public:
	CNaviObjectManager* m_pNaviObjectManager{ nullptr };
	//-----------------------------------scene--------------------------
	virtual bool Begin();
	virtual bool End();

	virtual void Animate(float fTimeElapsed);

	virtual void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual void ProcessInput(float fTimeElapsed);
	CGameObject* PickObjectPointedByCursor(UINT nMessageID, int xClient, int yClient);
	//-----------------------------------scene--------------------------

	//animation tool을 위한 함수
	void CreateControllObject(string path);
	void CreateLoadFileUI();
	void AddFBXAnimationInfo(string path);
	void CreateAddInfoUI();
	void ClearAllFBXObject();

	void ClearAllFBXUI();

	void CreateBaseTextureSelectUI();
	void CreateObjectPositioningUI();
	
	void ObjectPositioning();

	//Container Creater
	void CreateSceneContainers();
	void CreateTerrainContainer();
	void CreateSkyBoxContainer();
	//void DeleteTerrainContainer();
	//void DeleteSkyBoxContainer();

	CTestObject* GetFBXObject() { return m_pFBXObject; }
	void CreatePositioningObject();
	void CreatePositioningStempObject();

	void SetMeshRoot(wstring wsMeshRoot);
	void SaveScene(string sceneName);
	void LoadScene(string path);
	void CreateSceneListUI();

	virtual void LoadResource(wstring wsMeshRoot);//추가
private:
	
	//tool 변수
	//모든 생성 가능한 객체를 벡터로 미리 만들어 둔다.
	//이 안의 모든 객체를 버튼으로써 제작한다.
	vector<CGameObject*> m_vpObjectList;
	vector<LoadFileStruct> m_LoadFileStruct;

	//fbx object
	CTestObject* m_pFBXObject{ nullptr };
	int m_MeshCnt{ 0 };
	//tool 변수

	//player
	//CGameObject* m_pPlayer{ nullptr };
	//player
public:
	CSceneMain(CDirectXFramework* pFrameWork);
	~CSceneMain();
};