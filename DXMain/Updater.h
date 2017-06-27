#pragma once

#include "SingleTon.h"

//
#include "Camera.h"
#include "SpaceContainer.h"
#include "TerrainContainer.h"
#include "SkyBoxContainer.h"

class CDirectXFramework;

class CUpdater : public CSingleTonBase<CUpdater> {

public:
	bool Begin();
	bool End();

	//func
	void PreUpdate(float fTimeElapsed);
	void Update(float fTimeElapsed);
	void PhisicsUpdate(float fTimeElapsed);
	//func

	void SetTerrainContainer(CTerrainContainer* pTerrainContainer) { m_pTerrainContainer = pTerrainContainer; }
	CTerrainContainer* GetTerrainContainer() { return m_pTerrainContainer; }
	void SetSpaceContainer(CSpaceContainer* pSpaceContainer) { m_pSpaceContainer = pSpaceContainer; }
	CSpaceContainer* GetSpaceContainer() { return m_pSpaceContainer; }
	void SetSkyBoxContainer(CSkyBoxContainer* pSkyBoxContainer) { m_pSkyBoxContainer = pSkyBoxContainer; }
	CSkyBoxContainer* GetSkyBoxContainer() { return m_pSkyBoxContainer; }

	void SetCamera( CCamera* pCamera) { m_pCamera = pCamera; }
	 CCamera* GetCamera() { return m_pCamera; }
	void ChangeSceneContainers();

	//directional light
	void SetDirectionalLight(CDirectionalLight* pDirectionalLight) { m_pDirectionalLight = pDirectionalLight; }
	CDirectionalLight* GetDirectionalLight() { return m_pDirectionalLight; }
	//directional light

	void LoadSpaceInfo();
	void SaveSpaceInfo();
	void LoadTerrainInfo(wstring wsOutputPath, wstring wsSceneName);
	void SaveTerrainInfo(wstring wsOutputPath, wstring wsSceneName);
	void LoadObjectsInfo();
	void SaveObjectsInfo();

	//picking
	CGameObject* PickObject(XMVECTOR xmvWorldCameraStartPos, XMVECTOR xmvRayDir, float& distanse);
private:
	//directional light
	CDirectionalLight* m_pDirectionalLight{ nullptr };
	//directional light

	 CCamera* m_pCamera{ nullptr };
	//space
	CSpaceContainer* m_pSpaceContainer{ nullptr };
	//space
	//terrain
	CTerrainContainer* m_pTerrainContainer{ nullptr };
	//terrain
	//skybox
	CSkyBoxContainer* m_pSkyBoxContainer{ nullptr };
	//skybox

public:
	CUpdater();
	virtual ~CUpdater();
};
