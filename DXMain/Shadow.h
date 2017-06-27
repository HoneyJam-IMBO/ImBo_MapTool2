#pragma once
#include "DXObject.h"

//struct stStaticShadowInfo {
//	XMMATRIX xmmtxViewProj[MAX_SPACE_NUM];
//};
//struct SPACE_GLOBAL_VALUE {
//	float OneSpaceSizeRcp;//하나의 공간의 크기
//	float OneSideSpaceNum;//한의 사이드에 있는 공간 수
//	UINT pad[2];
//};
struct stShadowInfo {
	float bias{ 0.039f };
	float bias_offset{ 0.0105f };
	//UINT pad[2];
	float kernelhalf{ 1.f };
	float dist_sum{ 0.07f };
	XMMATRIX xmmtxViewProj;//동적 카메라
};

class CShadow :
	public DXObject
{
public:
	bool Begin();
	virtual bool End();

	virtual void SetShaderState();
	virtual void CleanShaderState();

	virtual void UpdateShaderState();

	ID3D11ShaderResourceView* RenderShadowMap( CCamera* pCamera = nullptr);

	void ResizeBuffer();
	void ReleaseBuffer();

	void SetBias(float bias) { m_pShadowInfo->bias = bias; }
	void SetBiasOffset(float bias_offset) { m_pShadowInfo->bias_offset = bias_offset; }

	void CreateShadowControlUI();
	void SaveShadow(wstring wsOutputPath, wstring wsSceneName);
	void LoadShadow(wstring wsOutputPath, wstring wsSceneName);
private:
	bool m_bShadow{ true };

	// CTexture> m_pShadowTexture{ nullptr };
	ID3D11Texture2D			 *m_pd3dtxtShadow{ nullptr };
	ID3D11ShaderResourceView *m_pd3dsrvShadow{ nullptr };
	ID3D11DepthStencilView	 *m_pd3ddsvShadow{ nullptr };

	ID3D11RasterizerState* m_pd3dRSShader{ nullptr };
	//shadow buffer
	 CBuffer* m_pShadowBuf{ nullptr };
	//shadow buffer
	// CBuffer> m_pStaticShadowBuf{ nullptr };
	// CBuffer> m_pGlobalTerrainBuffer{ nullptr };

	stShadowInfo* m_pShadowInfo{ nullptr };
	

	//map tool
	//float m_SelectSpace{ 0 };
	//ID3D11Texture2D			 *m_pd3dtxtRenderShadow{ nullptr };
	//ID3D11ShaderResourceView *m_pd3dsrvRenderShadow{ nullptr };
	//ID3D11RenderTargetView	 *m_pd3drtvRenderShadow{ nullptr };
	//CDebugTexture* m_pDebugTextureObj{ nullptr };
	//map tool
private:
	XMFLOAT4X4			m_xmmtxShadowVP;
	 CBuffer*	m_pShadowVPBuffer;
	 CCamera* m_pCamera{ nullptr };
public:
	CShadow();
	~CShadow();
};

