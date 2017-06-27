#pragma once

#include "NaviVertex.h"
#include "NaviObject.h"
#include "SingleTon.h"

class CBuffer;

//NaviObject를 관리한다. picking결과를 처리하며 
//gs의 정점 데이터를 관리한다.
//
class CNaviObjectManager{
public:
	static bool Begin();
	static bool End();

	//NaviObject를 전부 관리하지만 일단 render만
	static void Render();
	static void PickingProc(XMFLOAT3 xmf3PickingPos);

	static void AddNaviObject(CNaviObject* pNaviObject) { m_vNaviObject.push_back(pNaviObject); }
	static void AddNaviVertex(CNaviVertex* pNaviVertex) { m_vpNaviVertex.push_back(pNaviVertex); }
	static void ClearNaviObject() { m_vNaviObject.clear(); }
	static void ClearNaviVertex() { m_vpNaviVertex.clear(); }

	//navi vertex의 index는 이녀석이 부여한다.
	static void ResetVertexIndex();
	static vector<CNaviVertex*>& GetNaviVertices() { return m_vpNaviVertex; }
	static void DeleteVertex(int index);
	static void DeleteNaviObject(int index);

	static float GetHeight(XMFLOAT2 pos, int index);
	static void CreateNaviObject();

	static bool IsValiableIndex(int index);
	static XMVECTOR GetNaviMeshPosition(int index);
	static int GetValiableIndex(XMVECTOR pos);
	static bool IsIntersection(float x, float z, int index);
	static int GetIndex(float x, float z, int index);

	static void LoadData();
	static void SaveData();
	//static bool Is
private:
	static void RenderUI();
	static void ClearUI();

	static bool m_bAutoCreateNaviObject;
	//navimesh를 제작할 때 사용할 control vertex 최대 3개
	//static int m_ControlVertexs[3];//index 뿐
	static CNaviVertex* m_ControlVertexs[3];//index 뿐
	static int m_ControlVertexCount;

	static vector<CNaviObject*> m_vNaviObject;

	static vector<CNaviVertex*> m_vpNaviVertex;
	static CBuffer* m_pGSNaviMeshVertexBuffer;
	static CBuffer* m_pNaviMeshInstancingBuffer;

	static CMesh* m_pNaviMesh;
	static CRenderShader* m_pNaviObjectShader;
public:
	CNaviObjectManager();
	~CNaviObjectManager();
};
