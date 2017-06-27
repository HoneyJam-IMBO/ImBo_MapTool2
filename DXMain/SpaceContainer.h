#pragma once
#include "Object.h"
#include "Space.h"
#include "Camera.h"
#include "DirectionalLight.h"
//#include "SceneMain.h"
//#include "RenderContainerSeller.h"

class CSpaceContainer : public CObject {
public:
	void Begin();
	bool End();

	//space controller
	void Animate(float fTimeElapsed);
	void PhisicsUpdate(float fTimeElapsed);
	void PrepareRenderOneSpace( CCamera* pCamera, UINT renderFlag = TAG_TERRAIN | TAG_STATIC_OBJECT | TAG_DYNAMIC_OBJECT | TAG_LIGHT, int render_space = -1);
	void PrepareRender( CCamera* pCamera, UINT renderFlag = TAG_TERRAIN | TAG_STATIC_OBJECT | TAG_DYNAMIC_OBJECT | TAG_LIGHT | TAG_REFLECTION);
	//animate하다가 해당 공간을 벗어난 객체 임시 저장소
	void AddBlockObjectList(CGameObject* pObject);
	void AddObject(CGameObject* pObject);
	void RemoveObject(CGameObject* pObject);
	void RemoveObject(string name);
	void ClearBlockObjectList();
	int SearchSpace(XMVECTOR xmvPos);
	void AddSpace(UINT index, CSpace* pSpace);
	void ChangeSpaceData();
	//모든 lay 충돌 검사된 객체 중 가장 가장 가까운 객체
	CGameObject* PickObject(XMVECTOR xmvWorldCameraStartPos, XMVECTOR xmvRayDir, float& distanse);
	//space controller

	static CSpaceContainer* CreateSpaceContainer(int size, int lv);

	void ClearAllObjects();
	void SaveObjectInfos();
	void LoadObjectInfos();
private:
	//space들을 관리한다.
	CSpace** m_ppSpace{ nullptr };
	//쿼드 트리의 루트노드에 해당하는 startSpace 관리
	CSpace* m_pStartSpace{ nullptr };
	//block object list
	list<CGameObject*> m_lpBlockObject;

	//전체 공간 크기
	float m_size{ 0 };
	//공간의 레벨 
	float m_level{ 0 };
	//한 사이드에 있는 공간의 개수
	int m_oneSideSpaceNum{ 0 };
	//공간 하나의 크기
	int m_oneSpaceSize{ 0 };
	//공간의 개수
	int m_nSpace{ 0 };

public:
	CSpaceContainer();
	~CSpaceContainer();

	//set get
	CSpace** GetAllSpace() { return m_ppSpace; }
	list<CGameObject*>& GetBlockObjectList() { return m_lpBlockObject; }
	CSpace* GetStartSpace() { return m_pStartSpace; }
	UINT GetSpaceNum() { return m_nSpace; }
	UINT GetSize() { return m_size; }
	UINT GetLevel() { return m_level; }
	UINT GetOneSideSpaceNum() { return m_oneSideSpaceNum; }
	UINT GetOneSpaceSize() { return m_oneSpaceSize; }

	int GetSpaceSize() { return m_size; }
	int GetSpaceLevel() { return m_level; }
	void SetSpaceSize(int size) { m_size = size; }
	void SetSpaceLevel(int lv) { m_level = lv; }
	//set get
};