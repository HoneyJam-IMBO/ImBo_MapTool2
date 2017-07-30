#pragma once

#include "SingleTon.h"

//
#include "DirectionalLight.h"
#include "TerrainContainer.h"
#include "SkyBoxContainer.h"
#include "ObjectRenderer.h"
#include "BigWaterRenderer.h"
#include "AORenderer.h"
#include "LightRenderer.h"
#include "BloomDownScale.h"
#include "Bloom.h"
#include "Blur.h"
#include "PostProcessingFinalPass.h"
#include "SSLR.h"

#include "SSRF.h"

#include "Shadow.h"

class CDirectXFramework;

class CRenderer : public CSingleTonBase<CRenderer> {

public:
	bool Begin();
	bool End();

	void Render( CCamera* pCamera);
	void PostProcessing( CCamera* pCamera);
	void Update(float fTimeElapsed);

	void SetForwardRenderTargets();
	void SetMainRenderTargetView();
	void SetRenderTargetViews(UINT nRenderTarget, ID3D11RenderTargetView** pd3dRTVs, ID3D11DepthStencilView* pd3ddsvDepthStencil);

	bool CreateSwapChain();
	bool CreateRenderTargetView();
	void ReleaseForwardRenderTargets();
	void ClearDepthStencilView(ID3D11DepthStencilView* pDepthStencilView);

	bool ResizeBuffer();
	void SetFramework(CDirectXFramework* pFramework) { m_pFramework = pFramework; }

	CRenderContainer* GetTerrainRenderContainer() { return m_pObjectRenderer->GetTerrainRenderContainer(); }
	CRenderContainer* GetSkyBoxRenderContainer() { return m_pObjectRenderer->GetSkyBoxRenderContainer(); }
	CObjectRenderer* GetObjectRenderer() { return m_pObjectRenderer; }
	CBigWaterRenderer* GetBigWaterRenderer() { return m_pBigWaterRenderer; }
	CShadow* GetShadow() { return m_pShadow; }

	void SaveEffectInfo(wstring wsOutputPath, wstring wsSceneName);
	void LoadEffectInfo(wstring wsOutputPath, wstring wsSceneName);
	//void SetTerrainContainer(CTerrainContainer* pTerrainContainer) { m_pTerrainContainer = pTerrainContainer; }
	//void SetSkyBoxContainer(CSkyBoxContainer* pSkyBoxContainer) { m_pSkyBoxContainer = pSkyBoxContainer; }
	//void SetDirectionalLight(CDirectionalLight* pDirectionalLight) { m_pDirectionalLIght = pDirectionalLight; }
private:

	CDirectXFramework* m_pFramework{ nullptr };
	CRenderContainer* m_pTerrainRenderContainer{ nullptr };
	CRenderContainer* m_pSkyBoxRenderContainer{ nullptr };

	ID3D11RenderTargetView	*	m_pd3dRenderTargetView{ nullptr };
	
	ID3D11DepthStencilState* m_pd3dDepthStencilState{ nullptr };
	ID3D11BlendState* m_pd3dRefBlendState{ nullptr };

	ID3D11DepthStencilView		*m_pd3ddsvReadOnlyDepthStencil{ nullptr };

	//---------------------------deferred rendering----------------------
	ID3D11Texture2D			 *m_pd3dtxtDepthStencil{ nullptr };
	ID3D11Texture2D			 *m_pd3dtxtColorSpecInt{ nullptr };//0
	ID3D11Texture2D			 *m_pd3dtxtNormal{ nullptr };//1
	ID3D11Texture2D			 *m_pd3dtxtSpecPow{ nullptr };//2


	ID3D11ShaderResourceView *m_pd3dsrvDepthStencil{ nullptr };
	ID3D11ShaderResourceView *m_pd3dsrvColorSpecInt{ nullptr };//0
	ID3D11ShaderResourceView *m_pd3dsrvNormal{ nullptr };//1
	ID3D11ShaderResourceView *m_pd3dsrvSpecPow{ nullptr };//2

	ID3D11DepthStencilView	 *m_pd3ddsvDepthStencil{ nullptr };
	ID3D11RenderTargetView   *m_pd3drtvColorSpecInt{ nullptr };//0
	ID3D11RenderTargetView   *m_pd3drtvNormal{ nullptr };//1
	ID3D11RenderTargetView   *m_pd3drtvSpecPow{ nullptr };//2

	vector< CTexture*> m_vObjectLayerResultTexture;
	vector< CTexture*> m_vLightLayerResultTexture;

	//--------------------------light map render target----------------
	ID3D11Texture2D			 *m_pd3dtxtLight{ nullptr };
	ID3D11ShaderResourceView *m_pd3dsrvLight{ nullptr };
	ID3D11RenderTargetView   *m_pd3drtvLight{ nullptr };
	//--------------------------light map render target----------------

	////--------------------------light render target----------------
	//ID3D11Texture2D			 *m_pd3dtxtLightedColor{ nullptr };
	//ID3D11ShaderResourceView *m_pd3dsrvLightedColor{ nullptr };
	//ID3D11RenderTargetView   *m_pd3drtvLightedColor{ nullptr };
	////--------------------------light render target----------------
	// CTexture> m_LightedColorTexture;

	//-------------------------layer-------------------------
	CObjectRenderer* m_pObjectRenderer{ nullptr };
	CBigWaterRenderer* m_pBigWaterRenderer{ nullptr };
	CAORenderer* m_pAORenderer{ nullptr };
	CLightRenderer* m_pLightRenderer{ nullptr };
	CSSRF* m_pRefrectionRenderer{ nullptr };
	CBloomDownScale * m_pBloomDownScale{ nullptr };
	CBloom* m_pBloom{ nullptr };
	CBlur* m_p16to1Blur{ nullptr };
	CBlur* m_p4to1Blur{ nullptr };
	CPostProcessingFinalPass* m_pPostProcessingFinalPass{ nullptr };
	CSSLR* m_pSSLR{ nullptr };

	CShadow* m_pShadow{ nullptr };
	//-------------------------layer-------------------------

	//effects
	//back  buffer color
	float m_fBackBufferClearColor[4];
	//ssao
	bool m_bSSAOOnOff{ true };
	float m_fSSAORadius{ 50.f };
	float m_fSSAOOffsetRadius{ 10.f };
	//bloom
	bool m_bBloom{ true };
	float m_fBLOOMThreshold{ 1.5f };
	float m_fBLOOMMiddleGrey{ 0.1f };
	float m_fBLOOMWhite{ 1.3f };
	float m_fBLOOMScale{ 1.5f };
	//sslr
	bool m_bSSLROnOff{ true };
	float m_fSSLRMaxSunDist{ 1.8f };
	float m_fSSLRInitDecay{ 0.05f };
	float m_fSSLRDistDecay{ 0.05f };
	float m_fSSLRMaxDeltaLen{ 0.01f };
	//effects

public:
	CRenderer();
	virtual ~CRenderer();

//set get
	//ssao
	void SetSSAORadius(float fSSAORadius) { m_fSSAORadius = fSSAORadius; };
	float GetSSAORadius() { return m_fSSAORadius; }
	void SetSSAOOffsetRadius(float SSAOOffsetRadius) { m_fSSAOOffsetRadius = SSAOOffsetRadius; }
	float GetSSAOOffsetRadius() { return m_fSSAOOffsetRadius; }
	//bloom
	void SetBLOOMThreshold(float BLOOMThreshold) { m_fBLOOMThreshold = BLOOMThreshold; }
	float GetBLOOMThreshold() { return m_fBLOOMThreshold; }
	void SetBLOOMMiddleGrey(float BLOOMMiddleGrey) { m_fBLOOMMiddleGrey = BLOOMMiddleGrey; }
	float GetBLOOMMiddleGrey() { return m_fBLOOMMiddleGrey; }
	void SetBLOOMWhite(float BLOOMWhith) { m_fBLOOMWhite = BLOOMWhith; }
	float GetBLOOMWhite() { return m_fBLOOMWhite; }
	void SetBLOOMScale(float BLOOMScale) { m_fBLOOMScale = BLOOMScale; }
	float GetBLOOMScale() { return m_fBLOOMScale; }
	//sslr
	void SetSSLROnOff(bool SSLROnOff) { m_bSSLROnOff = SSLROnOff; }
	bool GetSSLROnOff() { return m_bSSLROnOff; }
	void SetSSLRMaxSunDist(float SSLRMaxSunDist) { m_fSSLRMaxSunDist = SSLRMaxSunDist; }
	float GetSSLRMaxSunDist() { return m_fSSLRMaxSunDist; }
	void SetSSLRInitDecay(float SSLRInitDecay) { m_fSSLRInitDecay = SSLRInitDecay; }
	float GetSSLRInitDecay() { return m_fSSLRInitDecay; }
	void SetSSLRDistDecay(float SSLRDistDecay) { m_fSSLRDistDecay = SSLRDistDecay; }
	float GetSSLRDistDecay() { return m_fSSLRDistDecay; }
	void SetSSLRMaxDeltaLen(float fSSLRMaxDeltaLen) { m_fSSLRMaxDeltaLen = fSSLRMaxDeltaLen; }
	float GetSSLRMaxDeltaLen() { return m_fSSLRMaxDeltaLen; }
//set get
};
