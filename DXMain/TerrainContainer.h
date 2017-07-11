#pragma once
#include "Object.h"
#include "Terrain.h"
#include "StempManager.h"
#include "SplattingInfoManager.h"

enum STEMP_MODE {
	STEMP_MODE_SET,
	STEMP_MODE_INDE,
	STEMP_MODE_BRUSH
};

//추가
struct TERRAIN_TESS_FACTER{
	XMFLOAT3 camera_pos;
	float zn{ 500.f };
	float zf{ 1000.f };
	float max_facter_value{ 20.f };
	float min_facter_value{ 10.f };
	float space_lavel_pow{ 1.f };
};

struct TERRAIN_GLOBAL_VALUE {
	float OneSpaceSizeRcp{ 0 };// 1/하나의 공간의 크기
	float OneSideSpaceNumRcp{ 0 };// 1/한의 사이드에 있는 공간 수 
	float OneSideSpaceNum{ 0 };//1 / 한의 사이드에 있는 공간 수 
	float HeightScale{ 0 };//높이값 scale
};
class CSpaceContainer;

class CTerrainContainer : public CObject {
public:
	void Begin();
	bool End();

	void PrepareRender();
	float GetHeight(XMVECTOR xmvPos, int index = -1);
	float GetHeight(XMFLOAT2 xmf2Pos, int index = -1);

	void SetPicpos(float x, float y);
	void SetRenderRadius(float r);

	void Update( CCamera* pCamera);
	CGameObject* PickObjects(XMVECTOR xmvWorldCameraStartPos, XMVECTOR xmvRayDir, float& distance);
	void ReadyHeightMap();
	void SetBufferInfo();
	void IncreasePickPosHeight();
	void DecreasePickPosHeight();
	void IncBrushPickPosHeight();
	void DecBrushPickPosHeight();
	void SetPickPosHeight();

	void SetStempMode(STEMP_MODE mode) { m_StempMode = mode; }
	void SetBaseTexture(wstring path);
	void SetHeightMapTexture(wstring path);
	CTexture* GetBaseTexture() { return m_pBaseTexture; };

	void CreateSplattingInfo();
	CSplattingInfoManager* GetSplattingInfoManager() { return m_pSplattingInfoManager; }
	CStempManager* GetStempManager() { return m_pStempManager; }
	//CRenderContainer* GetTerrainRenderContainer() { return m_pTerrainRenderContainer; }

	XMFLOAT2 GetCurPickPos() { return m_xmf2CurPickPos; }

	void CreateNormalMap();

	static CTerrainContainer* CreateTerrainContainer(LPCTSTR pSceneName, int nWidth, int nLength, float HeightScale, CSpaceContainer* pSpaceContainer, bool isTool = false);
	static CTerrainContainer* CreateTerrainContainer(wstring wsOutputPath, wstring SceneName, int nWidth, int nLength, CSpaceContainer* pSpaceContainer, bool isTool = false);

	void SetTerrainWidth(int nWidth) { m_nWidth = nWidth; }
	int GetTerrainWidth() { return m_nWidth; }
	void SetTerrainLength(int nLength) { m_nLength = nLength; }
	int GetTerrainLength() { return m_nLength; }
	void SetHeightScale(float scale);
	void SetScale(XMFLOAT3 xmf3Scale) { m_xmf3Scale = xmf3Scale; }
	float GetHeightScale() { return m_xmf3Scale.y; }
	void SetTerrainScale(XMFLOAT3 xmf3Scale) { m_xmf3Scale = xmf3Scale; }
	XMFLOAT3 GetTerrainScale() { return m_xmf3Scale; }
	void SetSpaceContainer(CSpaceContainer* pSpaceContainer) { m_pSpaceContainer = pSpaceContainer; }
	CSpaceContainer* GetSpaceContainer() { return m_pSpaceContainer; }
	void SetSceneName(wstring name) { m_wsSceneName = name; }
	wstring GetSceneName() { return m_wsSceneName; }
	void SetOutputPath(wstring name) { m_wsOutputPath = name; }
	wstring GetOutputPath() { return m_wsOutputPath; }
	void SetIsTool(bool b) { m_bIsTool = b; }
	bool GetIsTool() { return m_bIsTool; }
	float* GetHeightData() { return m_pHeightData; }
	Pixel24* GetNormalData() { return m_pNormalData; }
	void SetHeightData(float* pHeightData);
	void SetNormalData(Pixel24* pNormalData);
	void SetHeightMapTexture( CTexture* pHeightMapTexture);
	void SetNormalMapTexture( CTexture* pNormalMapTexture);

	void CreateResetTextures(LPCTSTR pTerrainName);
	void CreateTerrainTextures(LPCTSTR pTerrainName);
	void CreateTerrainMesh(CSpaceContainer* pSpaceContainer);

	void ChangeSpaceData();
	void DeleteAllTerrainObjects();

	void SetActive(bool b);
	bool GetActive() { return m_bActive; }

	//추가
	void SetTerrainTessFacter(float zn, float zf, float space_level_pow, float max_facter_value, float min_facter_value, XMFLOAT3 xmf3CameraPos);
	void SetTessFacterZn(float zn) { m_pTessFacterData->zn = zn; }
	void SetTessFacterZf(float zf) { m_pTessFacterData->zf = zf; }
	void SetTessFacterSLP(float space_level_pow) { m_pTessFacterData->space_lavel_pow = space_level_pow; }
	void SetTessFacterMaxFV(float max_facter_value) { m_pTessFacterData->max_facter_value = max_facter_value; }
	void SetTessFacterMinFV(float min_facter_value) { m_pTessFacterData->min_facter_value = min_facter_value; }

	float GetTessFacterZn() { return m_pTessFacterData->zn; }
	float GetTessFacterZf() { return m_pTessFacterData->zf; }
	float GetTessFacterSLP() { return m_pTessFacterData->space_lavel_pow; }
	float GetTessFacterMaxFV() { return m_pTessFacterData->max_facter_value; }
	float GetTessFacterMinFV() { return m_pTessFacterData->min_facter_value; }

	void SaveHeightMap();
private:
	//추가
	 CBuffer* m_pTessFacterBuffer{ nullptr };
	TERRAIN_TESS_FACTER* m_pTessFacterData{ nullptr };

	bool m_bIsTool{ false };
	bool m_bActive{ true };
	wstring m_wsSceneName;
	wstring m_wsOutputPath;
	XMFLOAT2 m_xmf2CurPickPos{ 0.f, 0.f };
	STEMP_MODE m_StempMode{ STEMP_MODE_SET };

	CStempManager * m_pStempManager{ nullptr };
	TERRAIN_GLOBAL_VALUE* m_pGlobalTerrainData{ nullptr };
	 CBuffer* m_pGlobalTerrainBuffer{ nullptr };

	ID3D11RasterizerState* m_pd3dSpaceRSState{ nullptr };
	ID3D11RasterizerState* m_pd3dTempRSState{ nullptr };
	
	CSpaceContainer* m_pSpaceContainer{ nullptr };
	//CGlobalTerrain* m_pGlobalTerrain{ nullptr };
	//CRenderContainer* m_pTerrainRenderContainer{ nullptr };
	
	int m_nWidth{ 0 };
	int m_nLength{ 0 };
	XMFLOAT3 m_xmf3Scale;

	//Pixel24* m_pHeightData{ nullptr };
	float* m_pHeightData{ nullptr };
	Pixel24* m_pNormalData{ nullptr };

	//Pixel24* m_pNormalData{ nullptr };
	 CTexture* m_pBaseTexture{ nullptr };
	 CTexture* m_pHeightMapTexture{ nullptr };
	 CTexture* m_pNormalTexture{ nullptr };
	vector<CTerrain*> m_vpTerrain;//pick object를 위한 녀석
	//안의 terrain들은 space에서 관리되고 사라지니까 end 및 delete할 필요가 없다.

	//splatting infos
	CSplattingInfoManager* m_pSplattingInfoManager{ nullptr };
	
public:
	CTerrainContainer();
	~CTerrainContainer();
};