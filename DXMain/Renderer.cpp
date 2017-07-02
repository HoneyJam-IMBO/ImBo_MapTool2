
#include "stdafx.h"
#include "Renderer.h"


bool CRenderer::Begin() {

	D3D11_DEPTH_STENCIL_DESC descDepth;
	descDepth.DepthEnable = TRUE;
	descDepth.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	descDepth.DepthFunc = D3D11_COMPARISON_LESS;
	descDepth.StencilEnable = TRUE;
	descDepth.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	descDepth.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	const D3D11_DEPTH_STENCILOP_DESC stencilMarkOp = { D3D11_STENCIL_OP_REPLACE, D3D11_STENCIL_OP_REPLACE, D3D11_STENCIL_OP_REPLACE, D3D11_COMPARISON_ALWAYS };
	descDepth.FrontFace = stencilMarkOp;
	descDepth.BackFace = stencilMarkOp;
	GLOBALVALUEMGR->GetDevice()->CreateDepthStencilState(&descDepth, &m_pd3dDepthStencilState);
	if (false == CreateSwapChain()) return false;

	const char* barName = "Effects";
	TWBARMGR->AddBar(barName);
	//set param
	TWBARMGR->SetBarSize(barName, 250, 250);
	TWBARMGR->SetBarPosition(barName, 0, 300);
	TWBARMGR->SetBarColor(barName, 255, 0, 255);
	TWBARMGR->SetBarContained(barName, true);
	TWBARMGR->SetBarMovable(barName, false);
	TWBARMGR->SetBarResizable(barName, false);
	//set param

	//back buffer color
	TWBARMGR->AddColorBar4F(barName, "BACKBUFFER", "COLOR", &m_fBackBufferClearColor);
	//ssao
	TWBARMGR->AddMinMaxBarRW(barName, "SSAO", "Radius", &m_fSSAORadius, 1.0f, 1000.f, 0.5f);
	TWBARMGR->AddMinMaxBarRW(barName, "SSAO", "OffsetRadius", &m_fSSAOOffsetRadius, 1.0f, 100.f, 0.1f);
	TWBARMGR->AddBoolBar(barName, "SSAO", "SSAO_Switch", &m_bSSAOOnOff);
	//ssao
	//bloom
	TWBARMGR->AddMinMaxBarRW(barName, "BLOOM", "Threshold", &m_fBLOOMThreshold, 0.0f, 10.f, 0.001f);
	TWBARMGR->AddMinMaxBarRW(barName, "BLOOM", "MiddleGrey", &m_fBLOOMMiddleGrey, 0.0f, 4.f, 0.001f);
	TWBARMGR->AddMinMaxBarRW(barName, "BLOOM", "White", &m_fBLOOMWhite, 0.0f, 4.f, 0.001f);
	TWBARMGR->AddMinMaxBarRW(barName, "BLOOM", "BloomScale", &m_fBLOOMScale, 0.f, 100.f, 0.1f);
	TWBARMGR->AddBoolBar(barName, "BLOOM", "BLOOM_Switch", &m_bBloom);
	//bloom
	//sslr
	TWBARMGR->AddMinMaxBarRW(barName, "SSLR", "MaxSunDist", &m_fSSLRMaxSunDist, 0.0f, 3, 0.001f);
	TWBARMGR->AddMinMaxBarRW(barName, "SSLR", "InitDecay", &m_fSSLRInitDecay, 0.0f, 4.f, 0.001f);
	TWBARMGR->AddMinMaxBarRW(barName, "SSLR", "DistDecay", &m_fSSLRDistDecay, 0.f, 4, 0.001f);
	TWBARMGR->AddMinMaxBarRW(barName, "SSLR", "MaxDeltaLen", &m_fSSLRMaxDeltaLen, 0.001f, 0.05, 0.0001f);
	TWBARMGR->AddBoolBar(barName, "SSLR", "SSLR_Switch", &m_bSSLROnOff);
	//sslr

	//layer
	m_pObjectRenderer = new CObjectRenderer();
	m_pObjectRenderer->Begin();
	m_pAORenderer = new CAORenderer();
	m_pAORenderer->Begin();

	m_pLightRenderer = new CLightRenderer();
	m_pLightRenderer->Begin();

	m_pRefrectionRenderer = new CSSRF();
	m_pRefrectionRenderer->Begin();
	//post processing
	m_pBloomDownScale = new CBloomDownScale();
	m_pBloomDownScale->Begin();
	m_pBloom = new CBloom();
	m_pBloom->Begin();
	m_p16to1Blur = new CBlur();
	m_p16to1Blur->Begin();
	m_p4to1Blur = new CBlur();
	m_p4to1Blur->Begin();
	m_pPostProcessingFinalPass = new CPostProcessingFinalPass();
	m_pPostProcessingFinalPass->Begin();
	m_pSSLR = new CSSLR();
	m_pSSLR->Begin();
	//layer

	//shadow
	m_pShadow = new CShadow();
	m_pShadow->Begin();
	
	// render target과 depth-stencil buffer 생성/ deferred texture 생성
	if (!CreateRenderTargetView()) {
		MessageBox(GLOBALVALUEMGR->GethWnd(), TEXT("RenderTarget이나 Depth-Stencil 버퍼 생성이 실패했습니다. 프로그램을 종료합니다."), TEXT("프로그램 구동 실패"), MB_OK);
		return false;
	}
	return true;
}

bool CRenderer::End() {
	if (m_pd3dRenderTargetView) m_pd3dRenderTargetView->Release();
	if (m_pd3ddsvReadOnlyDepthStencil) m_pd3ddsvReadOnlyDepthStencil->Release();
	
	ReleaseForwardRenderTargets();

	//layer
	if (m_pObjectRenderer) {
		m_pObjectRenderer->End();
		delete m_pObjectRenderer;
	}
	if (m_pAORenderer) {
		m_pAORenderer->End();
		delete m_pAORenderer;
	}
	if (m_pLightRenderer) {
		m_pLightRenderer->End();
		delete m_pLightRenderer;
	}
	if (m_pRefrectionRenderer) {
		m_pRefrectionRenderer->End();
		delete m_pRefrectionRenderer;
	}
	if (m_pBloomDownScale) {
		m_pBloomDownScale->End();
		delete m_pBloomDownScale;
	}
	if (m_pBloom) {
		m_pBloom->End();
		delete m_pBloom;
	}
	if (m_p16to1Blur) {
		m_p16to1Blur->End();
		delete m_p16to1Blur;
	}
	if (m_p4to1Blur) {
		m_p4to1Blur->End();
		delete m_p4to1Blur;
	}
	if (m_pPostProcessingFinalPass) {
		m_pPostProcessingFinalPass->End();
		delete m_pPostProcessingFinalPass;
	}
	if (m_pSSLR) {
		m_pSSLR->End();
		delete m_pSSLR;
	}

	//shadow
	if (m_pShadow) {
		m_pShadow->End();
		delete m_pShadow;
	}
	ReleseInstance();
	return true;
}

void CRenderer::Render( CCamera* pCamera) {

	RESOURCEMGR->GetSampler("WRAP_LINEAR")->SetShaderState();
	RESOURCEMGR->GetSampler("WRAP_POINT")->SetShaderState();
	//CLAMP
	RESOURCEMGR->GetSampler("CLAMP_LINEAR")->SetShaderState();
	RESOURCEMGR->GetSampler("CLAMP_POINT")->SetShaderState();
	RESOURCEMGR->GetSampler("SHADOW")->SetShaderState();

	//shadow render
	m_pShadow->RenderShadowMap(pCamera);


	//clear buff
	//CLEAR
	ClearDepthStencilView(m_pd3ddsvDepthStencil);
	SetForwardRenderTargets();//gbuff가 될 rtv/ dsv set
	GLOBALVALUEMGR->GetDeviceContext()->OMSetDepthStencilState(m_pd3dDepthStencilState, 1);
	//CLEAR

	//object
	//object positioning 객체 랜더
	if (GLOBALVALUEMGR->GetToolMode() == TOOL_MODE_OBJECTPOSITIONING) {
		
		if (GLOBALVALUEMGR->GetPositioningObject()) {
			if (UPDATER->GetTerrainContainer()) {
				XMFLOAT2 xmf2CurPicPos = UPDATER->GetTerrainContainer()->GetCurPickPos();
				float fHeight = UPDATER->GetTerrainContainer()->GetHeight(xmf2CurPicPos);

				GLOBALVALUEMGR->GetPositioningObject()->SetPosition(XMVectorSet(xmf2CurPicPos.x, fHeight, xmf2CurPicPos.y, 1.0));
				GLOBALVALUEMGR->GetPositioningObject()->RegistToContainer();
			}
			else {
				GLOBALVALUEMGR->GetPositioningObject()->SetPosition(XMVectorSet(UPDATER->GetSpaceContainer()->GetSpaceSize()/2.f, 0, UPDATER->GetSpaceContainer()->GetSpaceSize() / 2.f, 1.0));
				GLOBALVALUEMGR->GetPositioningObject()->RegistToContainer();
			}
		}
	}
	UPDATER->GetSpaceContainer()->PrepareRender(pCamera);

	DEBUGER->start_Timemeasurement();
	pCamera->SetShaderState();
	m_pObjectRenderer->Excute(pCamera);

	
	DEBUGER->end_Timemeasurement(L"object_render");
	//OBJECT RENDER
	
	//SSAO
	//이건 light map이라고 생각하자
	SetRenderTargetViews(1, &m_pd3drtvLight, m_pd3ddsvReadOnlyDepthStencil);
	for (auto texture : m_vObjectLayerResultTexture) {
		texture->SetShaderState();
	}

	ID3D11ShaderResourceView* pAmbientOcclution;// = RESOURCEMGR->GetTexture("DEFAULT")->GetShaderResourceView();
	if (m_bSSAOOnOff) {
		DEBUGER->start_Timemeasurement();
		float SSAO_OffsetRadius = m_fSSAOOffsetRadius;//m_pFramework->GetCurScene()->GetSSAOOffsetRadius();
		float SSAO_Radius = m_fSSAORadius;//m_pFramework->GetCurScene()->GetSSAORadius();
		pAmbientOcclution = m_pAORenderer->Excute(pCamera, SSAO_OffsetRadius, SSAO_Radius);
		pAmbientOcclution = m_p4to1Blur->Excute(pAmbientOcclution);
		//ID3D11UnorderedAccessView* pAoUavMiniDepthNormal = m_pAORenderer->GetUavAoMiniDepthNormal();
		DEBUGER->end_Timemeasurement(L"ssao");
	}
	GLOBALVALUEMGR->GetDeviceContext()->PSSetShaderResources(4, 1, &pAmbientOcclution);
	//SSAO

	//LIGHT RENDER
	DEBUGER->start_Timemeasurement();
	if (m_bBloom == false) {
		SetMainRenderTargetView();
	}
	m_pLightRenderer->Excute(pCamera, m_pShadow);
	for (auto texture : m_vObjectLayerResultTexture) {
		texture->CleanShaderState();
	}
	DEBUGER->end_Timemeasurement(L"light");
	//LIGHT RENDER

	//post processing
	if (m_bBloom) {
		//water
		DEBUGER->start_Timemeasurement();
		m_pRefrectionRenderer->Excute(pCamera, m_pd3drtvLight, m_pd3ddsvReadOnlyDepthStencil, m_pd3dsrvLight, m_pd3dsrvDepthStencil, m_pd3dsrvNormal);
		DEBUGER->end_Timemeasurement(L"ssrf");

		//SSLR
		if (m_bSSLROnOff && m_bSSAOOnOff) {
			DEBUGER->start_Timemeasurement();
			if (UPDATER->GetDirectionalLight()) {
				D3D11_VIEWPORT oldvp;
				UINT num = 1;
				GLOBALVALUEMGR->GetDeviceContext()->RSGetViewports(&num, &oldvp);
				ID3D11RasterizerState* pPrevRSState;
				GLOBALVALUEMGR->GetDeviceContext()->RSGetState(&pPrevRSState);

				XMVECTOR xmvSunDir = UPDATER->GetDirectionalLight()->GetLook();
				XMFLOAT3 xmf3Color = UPDATER->GetDirectionalLight()->GetColor();
				float fOffsetSunPos = UPDATER->GetDirectionalLight()->GetOffsetLength();
				float fMaxSunDist = m_fSSLRMaxSunDist;// m_pFramework->GetCurScene()->GetSSLRMaxSunDist();
				float fInitDecay = m_fSSLRInitDecay;// m_pFramework->GetCurScene()->GetSSLRInitDecay();
				float fDistDecay = m_fSSLRDistDecay;// m_pFramework->GetCurScene()->GetSSLRDistDecay();
				float fMaxDeltaLen = m_fSSLRMaxDeltaLen;//m_pFramework->GetCurScene()->GetSSLRMaxDeltaLen();

				DEBUGER->AddDepthTexture(XMFLOAT2(800, 800), XMFLOAT2(900, 900), pAmbientOcclution);
				m_pSSLR->Excute(pCamera, m_pd3drtvLight, pAmbientOcclution, xmvSunDir, xmf3Color,
					fOffsetSunPos, fMaxSunDist, fInitDecay, fDistDecay, fMaxDeltaLen);

				// Restore the states
				GLOBALVALUEMGR->GetDeviceContext()->RSSetViewports(num, &oldvp);
				GLOBALVALUEMGR->GetDeviceContext()->RSSetState(pPrevRSState);
				if (pPrevRSState)pPrevRSState->Release();
			}
			DEBUGER->end_Timemeasurement(L"sslr");
		}//sslr 모드가 true이면 sslr 실행 
		//SSLR

		//bloom
		DEBUGER->start_Timemeasurement();
		SetMainRenderTargetView();
		m_vLightLayerResultTexture[0]->SetShaderState();
		PostProcessing(pCamera);
		m_vObjectLayerResultTexture[0]->CleanShaderState();
		for (auto texture : m_vLightLayerResultTexture) {
			texture->CleanShaderState();
		}
		DEBUGER->end_Timemeasurement(L"postprocessing");
		//bloom
	}
	else {
		m_pRefrectionRenderer->ClearObjects();
	}
	//post processing
	

	//skybox
	pCamera->SetShaderState();
	m_pObjectRenderer->RenderSkyBox();

	//debuge
	if (INPUTMGR->GetDebugMode()) {
		DEBUGER->DebugRender(pCamera);
	}

	//DEBUG
	if (INPUTMGR->GetDebugMode()) {
		ID3D11Buffer* pGBufferUnpackingBuffer = pCamera->GetGBufferUnpackingBuffer();
		GLOBALVALUEMGR->GetDeviceContext()->PSSetConstantBuffers(PS_UNPACKING_SLOT, 1, &pGBufferUnpackingBuffer);

		//if(testBotton){
		DEBUGER->AddTexture(XMFLOAT2(0, 0), XMFLOAT2(200, 200), m_pd3dsrvColorSpecInt);
		DEBUGER->AddTexture(XMFLOAT2(0, 200), XMFLOAT2(200, 400), m_pd3dsrvNormal);
		DEBUGER->AddTexture(XMFLOAT2(0, 400), XMFLOAT2(200, 600), m_pd3dsrvLight);
		DEBUGER->AddTexture(XMFLOAT2(0, 800), XMFLOAT2(200, 800), m_pd3dsrvDepthStencil);
		//debug
		DEBUGER->AddTexture(XMFLOAT2(200, 0), XMFLOAT2(400, 200), pAmbientOcclution);
		//DEBUGER->AddDepthTexture(XMFLOAT2(500, 0), XMFLOAT2(750, 150), m_pd3dsrvDepthStencil);
		//DEBUGER->AddTexture(XMFLOAT2(300, 0), XMFLOAT2(600, 300), m_pd3dsrvDepthStencil);

		//이건 꼭 여기서 해줘야함.

		DEBUGER->RenderTexture();
		DEBUGER->RenderText();
		//DEBUGER->ClearDebuger();
	}
	else {
		DEBUGER->ClearDebuger();
	}
	//DEBUG

	//UI
	TWBARMGR->Render();
	//UI

	//PRESENT
	// If the swap chain already exists, resize it.
	HRESULT hr = GLOBALVALUEMGR->GetSwapChain()->Present(0, 0);
	//PRESENT
	if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
	{
		DEBUGER->DebugMessageBox("explosion", "d");
		return;
	}
	

}
void CRenderer::Update(float fTimeElapsed) {
	m_pBloomDownScale->SetAdaptation(fTimeElapsed);
}
void CRenderer::PostProcessing( CCamera* pCamera) {
	float fBloomThreshold = m_fBLOOMThreshold;// m_pFramework->GetCurScene()->GetBLOOMThreshold();
	float fMiddleGrey = m_fBLOOMMiddleGrey;// m_pFramework->GetCurScene()->GetBLOOMMiddleGrey();
	float fWhite = m_fBLOOMWhite;// m_pFramework->GetCurScene()->GetBLOOMWhite();
	float fBloomScale = m_fBLOOMScale;// m_pFramework->GetCurScene()->GetBLOOMScale();

	m_pBloomDownScale->Excute(pCamera, fBloomThreshold);
	ID3D11ShaderResourceView* pBloomImage = m_pBloom->Excute(pCamera);
	pBloomImage = m_p16to1Blur->Excute(pBloomImage);

	GLOBALVALUEMGR->GetDeviceContext()->PSSetShaderResources(2, 1, &pBloomImage);

	//rtv에 풀스크린 드로우 
	m_pPostProcessingFinalPass->Excute(pCamera, fMiddleGrey, fWhite, fBloomScale);

	//적응을 하기위한 이전avgLum과 지금 계산한 avgLum을 교환
	m_pBloomDownScale->SwapAdaptationBuff();

}
void CRenderer::ClearDepthStencilView(ID3D11DepthStencilView* pDepthStencilView) {
	GLOBALVALUEMGR->GetDeviceContext()->ClearDepthStencilView(pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}
void CRenderer::SetForwardRenderTargets() {
	ID3D11RenderTargetView *pd3dRTVs[RENDER_TARGET_NUMBER] = { m_pd3drtvColorSpecInt, m_pd3drtvNormal, m_pd3drtvSpecPow };
	//float fClearColor[4] = { xmf4Xolor.x, xmf4Xolor.y, xmf4Xolor.z, xmf4Xolor.w };
	float fClearColor[4] = { 0.f, 0.f, 0.f, 0.f };

	if (m_pd3drtvColorSpecInt) GLOBALVALUEMGR->GetDeviceContext()->ClearRenderTargetView(m_pd3drtvColorSpecInt, m_fBackBufferClearColor);
	if (m_pd3drtvNormal) GLOBALVALUEMGR->GetDeviceContext()->ClearRenderTargetView(m_pd3drtvNormal, fClearColor);
	if (m_pd3drtvSpecPow) GLOBALVALUEMGR->GetDeviceContext()->ClearRenderTargetView(m_pd3drtvSpecPow, fClearColor);

	SetRenderTargetViews(RENDER_TARGET_NUMBER, pd3dRTVs, m_pd3ddsvDepthStencil);
}
void CRenderer::SetMainRenderTargetView() {
	GLOBALVALUEMGR->GetDeviceContext()->OMSetRenderTargets(1, &m_pd3dRenderTargetView, m_pd3ddsvReadOnlyDepthStencil);
}
void CRenderer::SetRenderTargetViews(UINT nRenderTarget, ID3D11RenderTargetView** pd3dRTVs, ID3D11DepthStencilView* pd3ddsvDepthStencil) {
	GLOBALVALUEMGR->GetDeviceContext()->OMSetRenderTargets(nRenderTarget, pd3dRTVs, pd3ddsvDepthStencil);
}


bool CRenderer::CreateSwapChain()
{
	return true;
}

bool CRenderer::CreateRenderTargetView() {

	HRESULT hResult = S_OK;
	ID3D11Texture2D *pd3dBackBuffer{ nullptr };
	if (FAILED(hResult = GLOBALVALUEMGR->GetSwapChain()->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *)&pd3dBackBuffer))) return(false);
	if (FAILED(hResult = GLOBALVALUEMGR->GetDevice()->CreateRenderTargetView(pd3dBackBuffer, NULL, &m_pd3dRenderTargetView))) return(false);
	if (pd3dBackBuffer) pd3dBackBuffer->Release();
	{
		//----------------------------------------Resource Desc-----------------------------------------//
		D3D11_SHADER_RESOURCE_VIEW_DESC d3dSRVDesc;
		::ZeroMemory(&d3dSRVDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		d3dSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		d3dSRVDesc.Texture2D.MipLevels = 1;
		//d3dSRVDesc.Format = DXGI_FORMAT_R32_FLOAT;
		//----------------------------------------Resource Desc-----------------------------------------//
		//----------------------------------------TextUre Desc-----------------------------------------//
		D3D11_TEXTURE2D_DESC d3dTexture2DDesc;
		::ZeroMemory(&d3dTexture2DDesc, sizeof(D3D11_TEXTURE2D_DESC));
		d3dTexture2DDesc.Width = GLOBALVALUEMGR->GetrcClient().right;
		d3dTexture2DDesc.Height = GLOBALVALUEMGR->GetrcClient().bottom;
		d3dTexture2DDesc.MipLevels = 1;
		d3dTexture2DDesc.ArraySize = 1;
		d3dTexture2DDesc.SampleDesc.Count = 1;
		d3dTexture2DDesc.SampleDesc.Quality = 0;
		d3dTexture2DDesc.Usage = D3D11_USAGE_DEFAULT;
		d3dTexture2DDesc.CPUAccessFlags = 0;
		d3dTexture2DDesc.MiscFlags = 0;
		d3dTexture2DDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		//d3dTexture2DDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		//----------------------------------------TextUre Desc-----------------------------------------//
		//----------------------------------------Render Desc-----------------------------------------//
		D3D11_RENDER_TARGET_VIEW_DESC d3dRTVDesc;
		::ZeroMemory(&d3dRTVDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
		d3dRTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		d3dRTVDesc.Texture2D.MipSlice = 0;
		//----------------------------------------TextUre Desc-----------------------------------------//
		//----------------------------------------Uav Desc---------------------------------------------//
		D3D11_UNORDERED_ACCESS_VIEW_DESC d3dUAVDesc;
		ZeroMemory(&d3dUAVDesc, sizeof(d3dUAVDesc));
		d3dUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;		
		//----------------------------------------Uav Desc---------------------------------------------//
		// Create the depth stencil view 
		D3D11_DEPTH_STENCIL_VIEW_DESC d3dDepthStencilViewDesc;
		ZeroMemory(&d3dDepthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
		d3dDepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		d3dDepthStencilViewDesc.Texture2D.MipSlice = 0;

		ReleaseForwardRenderTargets();

		//--------------------------------------Scene0 DSV Create-----------------------------------------//
		d3dTexture2DDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		d3dDepthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
		d3dSRVDesc.Format = DXGI_FORMAT_R32_FLOAT;
		d3dUAVDesc.Format = DXGI_FORMAT_R32_FLOAT;
		d3dTexture2DDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
		GLOBALVALUEMGR->GetDevice()->CreateTexture2D(&d3dTexture2DDesc, nullptr, &m_pd3dtxtDepthStencil);
		GLOBALVALUEMGR->GetDevice()->CreateDepthStencilView(m_pd3dtxtDepthStencil, &d3dDepthStencilViewDesc, &m_pd3ddsvDepthStencil);
		GLOBALVALUEMGR->GetDevice()->CreateShaderResourceView(m_pd3dtxtDepthStencil, &d3dSRVDesc, &m_pd3dsrvDepthStencil);
		
		//real depth stencil
		d3dDepthStencilViewDesc.Flags = D3D11_DSV_READ_ONLY_DEPTH;
		if (FAILED(hResult = GLOBALVALUEMGR->GetDevice()->CreateDepthStencilView(m_pd3dtxtDepthStencil, &d3dDepthStencilViewDesc, &m_pd3ddsvReadOnlyDepthStencil))) return(false);
		//--------------------------------------Scene0 DSV Create-----------------------------------------//
		
		d3dTexture2DDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		d3dTexture2DDesc.Format = d3dSRVDesc.Format = d3dRTVDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

		//--------------------------------------Scene1 RTV Create-----------------------------------------//
		GLOBALVALUEMGR->GetDevice()->CreateTexture2D(&d3dTexture2DDesc, nullptr, &m_pd3dtxtColorSpecInt);
		GLOBALVALUEMGR->GetDevice()->CreateShaderResourceView(m_pd3dtxtColorSpecInt, &d3dSRVDesc, &m_pd3dsrvColorSpecInt);
		GLOBALVALUEMGR->GetDevice()->CreateRenderTargetView(m_pd3dtxtColorSpecInt, &d3dRTVDesc, &m_pd3drtvColorSpecInt);
		//--------------------------------------Scene1 RTV Create-----------------------------------------//

		//--------------------------------------Scene2 RTV Create-----------------------------------------//
		GLOBALVALUEMGR->GetDevice()->CreateTexture2D(&d3dTexture2DDesc, nullptr, &m_pd3dtxtNormal);
		GLOBALVALUEMGR->GetDevice()->CreateRenderTargetView(m_pd3dtxtNormal, &d3dRTVDesc, &m_pd3drtvNormal);
		GLOBALVALUEMGR->GetDevice()->CreateShaderResourceView(m_pd3dtxtNormal, &d3dSRVDesc, &m_pd3dsrvNormal);
		//--------------------------------------Scene2 RTV Create-----------------------------------------//

		//--------------------------------------Scene3 RTV Create-----------------------------------------//
		GLOBALVALUEMGR->GetDevice()->CreateTexture2D(&d3dTexture2DDesc, nullptr, &m_pd3dtxtSpecPow);
		GLOBALVALUEMGR->GetDevice()->CreateRenderTargetView(m_pd3dtxtSpecPow, &d3dRTVDesc, &m_pd3drtvSpecPow);
		GLOBALVALUEMGR->GetDevice()->CreateShaderResourceView(m_pd3dtxtSpecPow, &d3dSRVDesc, &m_pd3dsrvSpecPow);
		//--------------------------------------Scene3 RTV Create-----------------------------------------//

		//자기 texture set -> sampler set위함
		//---------------------make texture---------------------
		//texture set to light rendercontainer
		
		ID3D11ShaderResourceView *pd3dSRV = { m_pd3dsrvDepthStencil};
		UINT Slot = { 0 };
		UINT BindFlag = { BIND_PS | BIND_CS };
		 CTexture* pTexture = CTexture::CreateTexture(pd3dSRV, Slot, BindFlag);
		m_vObjectLayerResultTexture.push_back(pTexture);

		pd3dSRV = { m_pd3dsrvColorSpecInt };
		Slot = { 1 };
		BindFlag = { BIND_PS | BIND_CS };
		pTexture = CTexture::CreateTexture(pd3dSRV, Slot, BindFlag);
		m_vObjectLayerResultTexture.push_back(pTexture);

		pd3dSRV = { m_pd3dsrvNormal };
		Slot = { 2 };
		BindFlag = { BIND_PS | BIND_CS };
		pTexture = CTexture::CreateTexture(pd3dSRV, Slot, BindFlag);
		m_vObjectLayerResultTexture.push_back(pTexture);
		
		pd3dSRV = { m_pd3dsrvSpecPow };
		Slot = { 3 };
		BindFlag = { BIND_PS | BIND_CS };
		pTexture = CTexture::CreateTexture(pd3dSRV, Slot, BindFlag);
		m_vObjectLayerResultTexture.push_back(pTexture);
		//---------------------make texture---------------------


		//light texture제작
		GLOBALVALUEMGR->GetDevice()->CreateTexture2D(&d3dTexture2DDesc, nullptr, &m_pd3dtxtLight);
		GLOBALVALUEMGR->GetDevice()->CreateRenderTargetView(m_pd3dtxtLight, &d3dRTVDesc, &m_pd3drtvLight);
		GLOBALVALUEMGR->GetDevice()->CreateShaderResourceView(m_pd3dtxtLight, &d3dSRVDesc, &m_pd3dsrvLight);

		//make texture
		UINT LightTexSlot = { 0 };
		UINT LightTexBindFlag = { BIND_PS | BIND_CS };
		pTexture = CTexture::CreateTexture(m_pd3dsrvLight, LightTexSlot, LightTexBindFlag);
		m_vLightLayerResultTexture.push_back(pTexture);
		//light texture제작

		////lighted color texture제작
		//GLOBALVALUEMGR->GetDevice()->CreateTexture2D(&d3dTexture2DDesc, nullptr, &m_pd3dtxtLight);
		//GLOBALVALUEMGR->GetDevice()->CreateRenderTargetView(m_pd3dtxtLight, &d3dRTVDesc, &m_pd3drtvLight);
		//GLOBALVALUEMGR->GetDevice()->CreateShaderResourceView(m_pd3dtxtLight, &d3dSRVDesc, &m_pd3dsrvLight);
		//
		////make texture
		//UINT LightedColorTexSlot = { 0 };
		//UINT LightedColorTexBindFlag = { BIND_PS | BIND_CS };
		//m_LightedColorTexture = CTexture::CreateTexture(m_pd3dsrvLight, LightedColorTexSlot, LightedColorTexBindFlag);
		////lighted colortexture제작
	}
	m_pAORenderer->ResizeBuffer();
	m_pRefrectionRenderer->ResizeBuffer();
	m_pBloomDownScale->ResizeBuffer();
	m_pBloom->ResizeBuffer();
	m_p16to1Blur->ResizeBuffer(4);
	m_p4to1Blur->ResizeBuffer(2);
	m_pSSLR->ResizeBuffer();
	m_pShadow->ResizeBuffer();

	return true;
}

void CRenderer::ReleaseForwardRenderTargets() {
	if (m_pd3dtxtLight) m_pd3dtxtLight->Release();
	m_pd3dtxtLight = nullptr;

	//texture end
	for (auto pTexture : m_vObjectLayerResultTexture) {
		pTexture->End();
		delete pTexture;
	}
	m_vObjectLayerResultTexture.clear();

	for (auto pTexture : m_vLightLayerResultTexture) {
		pTexture->End();
		delete pTexture;
	}
	m_vLightLayerResultTexture.clear();

	if (m_pd3dtxtColorSpecInt) m_pd3dtxtColorSpecInt->Release();//0
	m_pd3dtxtColorSpecInt = nullptr;

	if (m_pd3dtxtNormal) m_pd3dtxtNormal->Release();//1
	m_pd3dtxtNormal = nullptr;

	if (m_pd3dtxtSpecPow) m_pd3dtxtSpecPow->Release();//2
	m_pd3dtxtSpecPow = nullptr;

	if (m_pd3dtxtDepthStencil) m_pd3dtxtDepthStencil->Release();
	m_pd3dtxtDepthStencil = nullptr;

	//if (m_pd3dsrvColorSpecInt) m_pd3dsrvColorSpecInt->Release();//0
	m_pd3dsrvColorSpecInt = nullptr;

	//if (m_pd3dsrvNormal) m_pd3dsrvNormal->Release();//1
	m_pd3dsrvNormal = nullptr;

	//if (m_pd3dsrvSpecPow) m_pd3dsrvSpecPow->Release();//2
	m_pd3dsrvSpecPow = nullptr;

	//if (m_pd3dsrvDepthStencil) m_pd3dsrvDepthStencil->Release();
	m_pd3dsrvDepthStencil = nullptr;

	if (m_pd3drtvColorSpecInt) m_pd3drtvColorSpecInt->Release();//0
	m_pd3drtvColorSpecInt = nullptr;

	if (m_pd3drtvNormal) m_pd3drtvNormal->Release();//1
	m_pd3drtvNormal = nullptr;

	if (m_pd3drtvSpecPow) m_pd3drtvSpecPow->Release();//2
	m_pd3drtvSpecPow = nullptr;

	if (m_pd3ddsvDepthStencil) m_pd3ddsvDepthStencil->Release();
	m_pd3ddsvDepthStencil = nullptr;

	if (m_pd3drtvLight) m_pd3drtvLight->Release();
	m_pd3drtvLight = nullptr;


}


bool CRenderer::ResizeBuffer() {
	if (m_pd3dRenderTargetView) m_pd3dRenderTargetView->Release();
	if (m_pd3ddsvReadOnlyDepthStencil) m_pd3ddsvReadOnlyDepthStencil->Release();

	if (FAILED(GLOBALVALUEMGR->GetSwapChain()->ResizeBuffers(1, 0, 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0)))
		MessageBox(nullptr, TEXT(""), TEXT(""), MB_OK);

	//resize rtv size, deferred texture size
	CreateRenderTargetView();

	return true;
}

void CRenderer::SaveEffectInfo(wstring wsOutputPath, wstring wsSceneName){
	//back buffer color
	EXPORTER->WriteFloat(m_fBackBufferClearColor[0]); EXPORTER->WriteSpace();
	EXPORTER->WriteFloat(m_fBackBufferClearColor[1]); EXPORTER->WriteSpace();
	EXPORTER->WriteFloat(m_fBackBufferClearColor[2]); EXPORTER->WriteSpace();
	EXPORTER->WriteFloat(m_fBackBufferClearColor[3]); EXPORTER->WriteSpace();
	EXPORTER->WriteEnter();

	//ssao
	EXPORTER->WriteFloat(m_fSSAORadius); EXPORTER->WriteSpace();
	EXPORTER->WriteFloat(m_fSSAOOffsetRadius);
	EXPORTER->WriteEnter();
	//bloom
	EXPORTER->WriteFloat(m_fBLOOMThreshold); EXPORTER->WriteSpace();
	EXPORTER->WriteFloat(m_fBLOOMMiddleGrey); EXPORTER->WriteSpace();
	EXPORTER->WriteFloat(m_fBLOOMWhite); EXPORTER->WriteSpace();
	EXPORTER->WriteFloat(m_fBLOOMScale);
	EXPORTER->WriteEnter();
	//sslr
	EXPORTER->WriteBool(m_bSSLROnOff); EXPORTER->WriteSpace();
	EXPORTER->WriteFloat(m_fSSLRMaxSunDist); EXPORTER->WriteSpace();
	EXPORTER->WriteFloat(m_fSSLRInitDecay); EXPORTER->WriteSpace();
	EXPORTER->WriteFloat(m_fSSLRDistDecay); EXPORTER->WriteSpace();
	EXPORTER->WriteFloat(m_fSSLRMaxDeltaLen);
	EXPORTER->WriteEnter();

	m_pRefrectionRenderer->SaveData();

	//shadow
	m_pShadow->SaveShadow(wsOutputPath, wsSceneName);
}

void CRenderer::LoadEffectInfo(wstring wsOutputPath, wstring wsSceneName){
	//back buffer color
	m_fBackBufferClearColor[0] = IMPORTER->ReadFloat();
	m_fBackBufferClearColor[1] = IMPORTER->ReadFloat();
	m_fBackBufferClearColor[2] = IMPORTER->ReadFloat();
	m_fBackBufferClearColor[3] = IMPORTER->ReadFloat();

	//ssao
	float fSSAORadius = IMPORTER->ReadFloat();
	SetSSAORadius(fSSAORadius);
	float fSSAOOffsetRadius = IMPORTER->ReadFloat();
	SetSSAOOffsetRadius(fSSAOOffsetRadius);

	//bloom
	float fBLOOMThreshold = IMPORTER->ReadFloat();
	SetBLOOMThreshold(fBLOOMThreshold);
	float fBLOOMMiddleGrey = IMPORTER->ReadFloat();
	SetBLOOMMiddleGrey(fBLOOMMiddleGrey);
	float fBLOOMWhite = IMPORTER->ReadFloat();
	SetBLOOMWhite(fBLOOMWhite);
	float fBLOOMScale = IMPORTER->ReadFloat();
	SetBLOOMScale(fBLOOMScale);
	//sslr
	bool bSSLROnOff = IMPORTER->ReadBool();
	SetSSLROnOff(bSSLROnOff);
	float fSSLRMaxSunDist = IMPORTER->ReadFloat();
	SetSSLRMaxSunDist(fSSLRMaxSunDist);
	float fSSLRInitDecay = IMPORTER->ReadFloat();
	SetSSLRInitDecay(fSSLRInitDecay);
	float fSSLRDistDecay = IMPORTER->ReadFloat();
	SetSSLRDistDecay(fSSLRDistDecay);
	float fSSLRMaxDeltaLen = IMPORTER->ReadFloat();
	SetSSLRMaxDeltaLen(fSSLRMaxDeltaLen);

	//ssrf
	float fMinDepthBias			= IMPORTER->ReadFloat();
	float fMaxDepthBias			=IMPORTER->ReadFloat();;
	float fEdgeDistThreshold	=IMPORTER->ReadFloat();;
	float fReflectionScale		=IMPORTER->ReadFloat();;
	float fViewAngleThreshold	=IMPORTER->ReadFloat();;
	float fPixelScale			=IMPORTER->ReadFloat();;
	float fNumStepScale			=IMPORTER->ReadFloat();;
	m_pRefrectionRenderer->SetMinDepthBias(fMinDepthBias);
	m_pRefrectionRenderer->SetMaxDepthBias(fMaxDepthBias);
	m_pRefrectionRenderer->SetEdgeDistThreshold(fEdgeDistThreshold);
	m_pRefrectionRenderer->SetReflectionScale(fReflectionScale);
	m_pRefrectionRenderer->SetViewAngleThreshold(fViewAngleThreshold);
	m_pRefrectionRenderer->SetPixelScale(fPixelScale);
	m_pRefrectionRenderer->SetNumStepScale(fNumStepScale);

	m_pShadow->LoadShadow(wsOutputPath, wsSceneName);
}

CRenderer::CRenderer() :CSingleTonBase<CRenderer>("rendereringleton") {

}

CRenderer::~CRenderer() {

}
