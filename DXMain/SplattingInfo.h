#pragma once
#include "DXObject.h"
#include "Texture.h"

#define BLEND_DATA_NUM 512*512
#define BLEND_WIDTH 512
#define BLEND_HEIGHT 512

class CSplattingInfoManager;

class CSplattingInfo;
struct LoadFileStructSP {
	CSplattingInfo* m_pSplatting;
	wstring path;
};
class CSplattingInfo : public DXObject{
public:
	void Begin();
	virtual bool End();
	virtual void SetShaderState();
	virtual void CleanShaderState();
	virtual void UpdateShaderState();//blend info ÆíÁý

	//void SetBlendInfoTexture( CTexture* pTexture) { m_pBlendInfoTexture = pTexture; }
	//ID3D11ShaderResourceView* GetBlendInfoSRV() { return m_pBlendInfoTexture->GetShaderResourceView(); }
	//void SetDetailTexture( CTexture* pTexture) { m_pDetailTexture = pTexture; }
	//ID3D11ShaderResourceView* GetDetailSRV() { return m_pDetailTexture->GetShaderResourceView(); }
	void SetBlendInfoTextuePath(const WCHAR* path) { m_pBlendInfoTexturePath = path; }
	const WCHAR* GetBlendInfoTexturePath() { return m_pBlendInfoTexturePath.c_str(); }
	void SetDetailTextuePath(const WCHAR* path) { m_pDetailTexturePath = path; }
	const WCHAR* GetDetailTexturePath() { return m_pDetailTexturePath.c_str(); }
	float* GetBlendInfo() { return m_pBlendInfo; }
	void SetBlendInfo(float* pData) { m_pBlendInfo = pData; }
	static CSplattingInfo* CreateSplattingInfo(CSplattingInfoManager* pSplattingManager, const WCHAR* pDetailTextureName);
	static CSplattingInfo* CreateSplattingInfo(CSplattingInfoManager* pSplattingManager, const WCHAR* pDetailTextureName, const WCHAR* pBlendInfoTextureName);

	void SetSplattingManager(CSplattingInfoManager* pSplattingManager) { m_pSplattingInfoManager = pSplattingManager; }
	CSplattingInfoManager* GetSplattingManager() { return m_pSplattingInfoManager; }
	void SetIndex(UINT index);
	UINT GetIndex() { return m_nIndex; }

	void CreateControllUI();
private:
	vector<LoadFileStructSP> m_vLoadFileStruct;
	// CTexture> m_pBlendInfoTexture{ nullptr };
	// CTexture> m_pDetailTexture{ nullptr };
	wstring m_pBlendInfoTexturePath;
	wstring m_pDetailTexturePath;

	CSplattingInfoManager* m_pSplattingInfoManager{ nullptr };

	float* m_pBlendInfo{ nullptr };
	UINT m_nIndex{ 0 };
public:
	CSplattingInfo();
	~CSplattingInfo();
};