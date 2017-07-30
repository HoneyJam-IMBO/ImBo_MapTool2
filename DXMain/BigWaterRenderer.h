#pragma once
#include "DXObject.h"

struct stBigWaterInfo {
	UINT nFlag{ 0 };
	float fHeight{ 0.f };
	UINT pad[2];
};
class CBigWaterRenderer :
	public DXObject {

public:
	CBigWaterRenderer();
	~CBigWaterRenderer();

public:
	bool Begin();
	virtual bool End();

	virtual void SetShaderState();
	virtual void CleanShaderState();

	virtual void UpdateShaderState();

	//ID3D11ShaderResourceView* RenderReflectionMap(CCamera* pCamera, ID3D11DepthStencilView* pDepthStencilView, CObjectRenderer* objRenderer);
	//ID3D11ShaderResourceView* RenderRefractionMap(CCamera* pCamera, ID3D11DepthStencilView* pDepthStencilView, CObjectRenderer* objRenderer);
	void RenderBigWater(CCamera* pCamera, ID3D11RenderTargetView* pRTV, ID3D11DepthStencilView* pDSV);

	void	ResizeBuffer();
	void	ReleaseBuffer();

	void SetbBigWater(bool b) { m_bBigWater = b; }
	bool GetbBigWater() { return m_bBigWater; }
private:
	bool m_bBigWater{ false };
	void RenderWater1(CCamera* pCamera);//물 기본 바탕 그리기
	void RenderWaterSurface(CCamera* pCamera);//물 표면 그리기
	void RenderRefractions(CCamera* pCamera);//굴절 그리기
	void RenderWater2(CCamera* pCamera, ID3D11RenderTargetView* pRTV, ID3D11DepthStencilView* pDSV);//물 표면, 굴절 맵을 이용해 물 표면 랜더


	//void	CalReflectionViewProj(CCamera* pCamera);
private:
	CGameObject* m_pWaterSurface{ nullptr };
	CBuffer*			m_pWaterInfoPSBuffer;//reflection map제작 때 사용하는 버퍼
	CBuffer*			m_pWaterPSBuffer;//water surface 그릴 때 사용하는 버퍼
	CBuffer*			m_pReflractionVSBuffer;
	CRenderShader*				m_pWaterShader{ nullptr };
	CMesh*						m_pWaterMesh{ nullptr };

	ID3D11Texture2D			 *m_pTXTReflection{ nullptr };
	ID3D11ShaderResourceView *m_pSRVReflection{ nullptr };
	ID3D11RenderTargetView	 *m_pRTVReflection{ nullptr };

	ID3D11Texture2D			 *m_pTXTRefraction{ nullptr };
	ID3D11ShaderResourceView *m_pSRVRefraction{ nullptr };
	ID3D11RenderTargetView	 *m_pRTVRefraction{ nullptr };

	ID3D11Texture2D			 *m_pTXTDepthStencil{ nullptr };
	ID3D11ShaderResourceView *m_pSRVDepthStencil{ nullptr };
	ID3D11DepthStencilView	 *m_pDSVDepthStencil{ nullptr };

	//깊이 스텐실 스테이트
	ID3D11DepthStencilState			*m_pd3dMirrorToStencilState;
	ID3D11DepthStencilState			*m_pd3dReflectDepthStencilState;
	ID3D11DepthStencilState			*m_pd3dReflectSkyDepthStencilState;

	ID3D11BlendState*	m_pAlphaBlendState;
};