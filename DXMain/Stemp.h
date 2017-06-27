#pragma once
#include "DXObject.h"

struct Pixel24;
class CTexture;

class CStempManager;
struct TERRAIN_PICPOS_RENDER_INFO;

class CStemp : public DXObject{
public:
	void Begin();
	virtual bool End();

	virtual void SetShaderState();
	virtual void CleanShaderState();

	void SetStempData(float* pStempData);
	void SetStempData(UINT index, float data);
	void IncreaseTerrain(float* pHeightMapData, TERRAIN_PICPOS_RENDER_INFO* pPicposRenderInfo);
	void DecreaseTerrain(float* pHeightMapData, TERRAIN_PICPOS_RENDER_INFO* pPicposRenderInfo);
	void IncBrushTerrain(float* pHeightMapData, TERRAIN_PICPOS_RENDER_INFO* pPicposRenderInfo);
	void DecBrushTerrain(float* pHeightMapData, TERRAIN_PICPOS_RENDER_INFO* pPicposRenderInfo);
	void SetTerrain(float* pHeightMapData, TERRAIN_PICPOS_RENDER_INFO* pPicposRenderInfo);
	void SetStempManager(CStempManager* pStempManager) { m_pStempManager = pStempManager; }
	void SetStempTexture( CTexture* pTexture) { m_pStempTexture = pTexture; }
	static CStemp* CreateStemp(wstring name, CStempManager* pStempManager);

	void SetStempWidth(int width) { m_nStempWidth = width; }
	void SetStempLength(int length) { m_nStempLength = length; }
private:
	float* m_pStempData{ nullptr };
	CStempManager* m_pStempManager{ nullptr };
	int m_nIndex{ 0 };

	int m_nStempWidth{ 0 };
	int m_nStempLength{ 0 };
	 CTexture* m_pStempTexture{ nullptr };

	//help func
	int ByteToInt(byte b) {
		float value = b;//0~256
		value /= 256;//0~1
		value *= 2000;//0~2000
		value -= 1000;//-1000~1000
		return value;
	}
	byte IntToByte(int i) {
		float tmp = i;//-1000~1000
		tmp += 1000;//0~2000
		tmp /= 2000;//0~1
		tmp *= 256;//0~256
		byte value = tmp;
		return value;
	}
	//help func
public:
	CStemp();
	virtual ~CStemp();
};