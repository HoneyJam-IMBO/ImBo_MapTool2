#pragma once
#include "DXObject.h"
#include "Stemp.h"

struct TERRAIN_PICPOS_RENDER_INFO {
	XMFLOAT2 PickPos{ 0.f, 0.f };//pick pos/ space_size;
	float Extent{ 0.f };//원의 r/space_size;
	float Height{ 1.f };//height scale에 대한 비율
	UINT ToolMode;
	UINT pad[3];
};

struct Pixel24;
class CBuffer;
class CTerrainContainer;

class CStempManager : public DXObject {
public:
	void Begin();
	virtual bool End();

	virtual void SetShaderState();
	virtual void CleanShaderState();
	virtual void UpdateShaderState();

	void IncreaseTerrain(float* pHeightMapData);
	void DecreaseTerrain(float* pHeightMapData);
	void IncBrushTerrain(float* pHeightMapData);
	void DecBrushTerrain(float* pHeightMapData);
	void SetTerrain(float* pHeightMapData);

	void IncreaseTerrainCos(float* pHeightMapData);
	void DecreaseTerrainCos(float* pHeightMapData);
	void IncBrushTerrainCos(float* pHeightMapData);
	void DecBrushTerrainCos(float* pHeightMapData);
	void SetTerrainCos(float* pHeightMapData);

	void SetPickPos(XMFLOAT2 xmf2Pos) {	m_pPicposRenderInfo->PickPos = xmf2Pos; }
	XMFLOAT2 GetPickPos() { return m_pPicposRenderInfo->PickPos; }
	void SetExtent(float extent) { m_pPicposRenderInfo->Extent = (extent / m_fSpaceSize); }
	float GetExtent() { return m_pPicposRenderInfo->Extent; }
	TERRAIN_PICPOS_RENDER_INFO* GetTerrainPickPosRenderInfo() { return m_pPicposRenderInfo; }
	vector<CStemp*>& GetStemps() { return m_vStemp; }

	void SetTerrainContainer(CTerrainContainer* pTerrainContainer) { m_pTerrainContainer = pTerrainContainer; }
	CTerrainContainer* GetTerrainContainer() { return m_pTerrainContainer; }
	void SetCurStempIndex(int index);
	void CreateStemp(wstring name);

	void SetSpaceSize(float fSize) { m_fSpaceSize = fSize; }
	static CStempManager* CreateStempManager(float fSpaceSize, CTerrainContainer* pTerrainContainer);
private:
	CTerrainContainer* m_pTerrainContainer{ nullptr };
	float m_fExtent{ 50.f };
	float m_fHeight{ 0.2f };
	TERRAIN_PICPOS_RENDER_INFO* m_pPicposRenderInfo{ nullptr };
	 CBuffer* m_pPicposRenderInfoBuffer{ nullptr };

	vector<CStemp*> m_vStemp;
	float m_nCurStemp{ 0.f };
	float m_fSpaceSize{ 0.f };
public:
	CStempManager();
	~CStempManager();
};