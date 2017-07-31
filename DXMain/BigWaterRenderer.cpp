#include "stdafx.h"
#include "BigWaterRenderer.h"

CBigWaterRenderer::CBigWaterRenderer()
	: DXObject("bigwaterrenderer")
{
}


CBigWaterRenderer::~CBigWaterRenderer()
{
}

bool CBigWaterRenderer::Begin()
{
	const char* barName = "Effects";
	const char* groupName = "BigWater";
	TWBARMGR->AddSeparator(barName, groupName, "SurfaceInfo");
	TWBARMGR->AddColorBar4F(barName, "BigWater", "BaseWaterColor", &m_xmf4WaterColor);
	TWBARMGR->AddMinMaxBarRW(barName, "BigWater", "fresnelMode", &m_fresnelMode, 0.f, 2.f, 1.f);
	TWBARMGR->AddMinMaxBarRW(barName, "BigWater", "xDrawfactor", &m_xDrawfactor, 0.f, 50.f, 0.01f);
	TWBARMGR->AddMinMaxBarRW(barName, "BigWater", "fPowFactor", &m_fPowFactor, 0.f, 50.f, 0.01f);
	TWBARMGR->AddMinMaxBarRW(barName, "BigWater", "fWaveHeight", &m_fWaterHeight, 0.f, 100.f, 0.01f);
	

	TWBARMGR->AddSeparator(barName, groupName, "BaseBumpMap");
	TWBARMGR->AddMinMaxBarRW(barName, "BigWater", "BaseBumpMapScale", &m_BumpMapBaseUVScale, 0.f, 100.f, 0.01f);
	TWBARMGR->AddMinMaxBarRW(barName, "BigWater", "WindDirX", &m_windDir.x, -1.f, 1.f, 0.01f);
	TWBARMGR->AddMinMaxBarRW(barName, "BigWater", "WindDirY", &m_windDir.y, -1.f, 1.f, 0.01f);
	TWBARMGR->AddMinMaxBarRW(barName, "BigWater", "WindForce", &m_windForce, 0.f, 1000.f, 0.01f);

	TWBARMGR->AddSeparator(barName, groupName, "DetailBumpMap");
	TWBARMGR->AddMinMaxBarRW(barName, "BigWater", "DetailBumpMapScale", &m_BumpMapDetailUVScale, 0.f, 100.f, 0.01f);
	TWBARMGR->AddMinMaxBarRW(barName, "BigWater", "WindDir2X", &m_windDir2.x, -1.f, 1.f, 0.01f);
	TWBARMGR->AddMinMaxBarRW(barName, "BigWater", "WindDir2Y", &m_windDir2.y, -1.f, 1.f, 0.01f);
	TWBARMGR->AddMinMaxBarRW(barName, "BigWater", "WindForce2", &m_windForce2, 0.f, 1000.f, 0.01f);
	//spec
	TWBARMGR->AddSeparator(barName, groupName, "SpecInfo");
	TWBARMGR->AddMinMaxBarRW(barName, "BigWater", "specExp", &m_specExp, 0.f, 50.f, 0.01f);
	TWBARMGR->AddMinMaxBarRW(barName, "BigWater", "specIntensity", &m_specIntensity, 0.f, 5.f, 0.01f);

	ResizeBuffer();

	//rasterizer state
	D3D11_RASTERIZER_DESC descRasterizer;
	ZeroMemory(&descRasterizer, sizeof(D3D11_RASTERIZER_DESC));
	descRasterizer.FillMode = D3D11_FILL_SOLID;
	//descRasterizer.FillMode = D3D11_FILL_WIREFRAME;
	descRasterizer.CullMode = D3D11_CULL_NONE;
	GLOBALVALUEMGR->GetDevice()->CreateRasterizerState(&descRasterizer, &m_pTestRSState);
	//rasterizer state

	//D3D11_BLEND_DESC d3dBlendDesc;
	//ZeroMemory(&d3dBlendDesc, sizeof(D3D11_BLEND_DESC));
	//ZeroMemory(&d3dBlendDesc, sizeof(D3D11_BLEND_DESC));
	//d3dBlendDesc.AlphaToCoverageEnable = false;
	//d3dBlendDesc.IndependentBlendEnable = false;
	//d3dBlendDesc.RenderTarget[0].BlendEnable = true;
	////d3dBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	////d3dBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	////d3dBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	//d3dBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	//d3dBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	//d3dBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	//d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	//d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	//d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	//d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	//
	//GLOBALVALUEMGR->GetDevice()->CreateBlendState(&d3dBlendDesc, &m_pAlphaBlendState);

	D3D11_BLEND_DESC descBlend;
	ZeroMemory(&descBlend, sizeof(D3D11_BLEND_DESC));
	descBlend.RenderTarget[0].BlendEnable = true;
	descBlend.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	descBlend.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	descBlend.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	
	descBlend.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	descBlend.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	descBlend.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	descBlend.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	//blend state
	GLOBALVALUEMGR->GetDevice()->CreateBlendState(&descBlend, &m_pAlphaBlendState);


	//깊이 스텐실 스테이트
	//거울을 렌더링할 깊이스텐실 스테이트
	D3D11_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = true;
	//d3dDepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	d3dDepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	d3dDepthStencilDesc.StencilEnable = true;
	d3dDepthStencilDesc.StencilReadMask = 0xff;
	d3dDepthStencilDesc.StencilWriteMask = 0xff;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	auto pDevice = GLOBALVALUEMGR->GetDevice();
	pDevice->CreateDepthStencilState(&d3dDepthStencilDesc, &m_pd3dMirrorToStencilState);
	//거울에 반사된 객체 렌더링 할 깊이 스텐실 스테이트
	ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = true;
	d3dDepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	d3dDepthStencilDesc.StencilEnable = true;
	d3dDepthStencilDesc.StencilReadMask = 0xff;
	d3dDepthStencilDesc.StencilWriteMask = 0xff;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	pDevice->CreateDepthStencilState(&d3dDepthStencilDesc, &m_pd3dReflectDepthStencilState);

	ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = false;
	d3dDepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D11_COMPARISON_NEVER;
	d3dDepthStencilDesc.StencilEnable = true;
	d3dDepthStencilDesc.StencilReadMask = 0xFF;
	d3dDepthStencilDesc.StencilWriteMask = 0xFF;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	pDevice->CreateDepthStencilState(&d3dDepthStencilDesc, &m_pd3dReflectSkyDepthStencilState);

	m_pWaterSurface = new CGameObject("bigwatersurface");
	m_pWaterSurface->Begin();

	m_pWaterPSBuffer = CBuffer::CreateConstantBuffer(1, sizeof(stRenderSurfaceInfo), 0, BIND_PS, NULL);
	m_pWaterInfoPSBuffer = CBuffer::CreateConstantBuffer(1, sizeof(stBigWaterInfo), 0, BIND_PS, NULL);
	m_pWaterColor = CBuffer::CreateConstantBuffer(1, sizeof(XMFLOAT4), 0, BIND_PS, NULL);

	m_pWaterBaseBumpMap = CTexture::CreateTexture(L"../../Assets/water_bump_map01.jpg", 5, BIND_PS);
	m_pWaterDetailBumpMap = CTexture::CreateTexture(L"../../Assets/water_bump_map03.jpg", 6, BIND_PS);
	return true;
}

bool CBigWaterRenderer::End()
{
	m_pWaterSurface->End();
	delete m_pWaterSurface;

	ReleaseBuffer();

	m_pWaterPSBuffer->End();	delete m_pWaterPSBuffer;  m_pWaterPSBuffer = nullptr;
	m_pWaterInfoPSBuffer->End();	delete m_pWaterInfoPSBuffer;  m_pWaterInfoPSBuffer = nullptr;
	m_pWaterColor->End();	delete m_pWaterColor;	m_pWaterColor = nullptr;

	m_pWaterBaseBumpMap->End();
	delete m_pWaterBaseBumpMap;
	m_pWaterDetailBumpMap->End();
	delete m_pWaterDetailBumpMap;
	//m_pWaterPSBuffer->End();	delete m_pWaterPSBuffer;  m_pWaterPSBuffer = nullptr;
	//m_pReflractionVSBuffer->End();	m_pReflractionVSBuffer = nullptr;

	return true;
}

void CBigWaterRenderer::RenderBigWater(CCamera* pCamera, ID3D11RenderTargetView* pRTV, ID3D11DepthStencilView* pDSV, vector<CTexture*>& vTexture) {
	size_t size = RCSELLER->GetTagRenderContainer()[tag::TAG_BIGWATER]["bigwater"]->GetObjectList().size();
	if (size > 0) {
		GLOBALVALUEMGR->GetDeviceContext()->ClearDepthStencilView(m_pDSVDepthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		RenderWater1(pCamera);//물 기본 바탕 그리기
		RenderWaterSurface(pCamera);//물 표면 그리기
		RenderRefractions(pCamera);//굴절 그리기

		for (auto pTexture : vTexture) {
			pTexture->SetShaderState();
		}
		RenderWater2(pCamera, pRTV, pDSV);//물 표면, 굴절 맵을 이용해 물 표면 랜더
		for (auto pTexture : vTexture) {
			pTexture->CleanShaderState();
		}
	}
	else {
		m_bBigWater = false;
	}
}

void CBigWaterRenderer::SaveBigWaterInfo(){
	EXPORTER->WriteBool(m_bBigWater);
	if (m_bBigWater) {
		auto pBigWater = *(RCSELLER->GetTagRenderContainer()[tag::TAG_BIGWATER]["bigwater"]->GetObjectList().begin());
		
		
	}
}

void CBigWaterRenderer::RenderWater1(CCamera* pCamera) {//big water는 하나만 존재허심
	//물을 그릴 때 depth stencil....을 하려고 했는데 씨바. 이거 stencil있니?
	//stencil제작험
	//물 그릴 떄 stencil 마킹
	//2. 거울을 그린다. 스텐실 버퍼만 변경 랜더 타겟에 출력하지 않음.!

	//water color 
	XMFLOAT4* pColor = (XMFLOAT4*)m_pWaterColor->Map();
	pColor[0] = m_xmf4WaterColor;
	m_pWaterColor->Unmap();
	m_pWaterColor->SetShaderState();

	ID3D11DepthStencilState* pTmpDSS{ nullptr };
	UINT StencilRef{ 0 };
	auto pDeviceContext = GLOBALVALUEMGR->GetDeviceContext();
	pDeviceContext->OMGetDepthStencilState(&pTmpDSS, &StencilRef);
	pDeviceContext->OMSetDepthStencilState(m_pd3dMirrorToStencilState, 2);

	RCSELLER->GetTagRenderContainer()[tag::TAG_BIGWATER]["bigwater"]->Render(pCamera);

	//state 원상 복귀
	pDeviceContext->OMSetDepthStencilState(pTmpDSS, StencilRef);
}

void CBigWaterRenderer::RenderWaterSurface(CCamera* pCamera) {
	auto pBigWater = RCSELLER->GetTagRenderContainer()[tag::TAG_BIGWATER]["bigwater"]->GetObjectList().begin();
	stBigWaterInfo* pWaterInfo = (stBigWaterInfo*)m_pWaterInfoPSBuffer->Map();
	pWaterInfo[0].nFlag = 1;
	pWaterInfo[0].fHeight = (*pBigWater)->GetPositionY();
	m_pWaterInfoPSBuffer->Unmap();
	m_pWaterInfoPSBuffer->SetShaderState();

	//표면 맵을 제작한다.
	ID3D11RenderTargetView *pd3dRTVs[1] = { m_pRTVReflection };
	//float fClearColor[4] = { xmf4Xolor.x, xmf4Xolor.y, xmf4Xolor.z, xmf4Xolor.w };
	float fClearColor[4] = { 0.f, 0.f, 0.f, 1.f };
	if (m_pRTVReflection) GLOBALVALUEMGR->GetDeviceContext()->ClearRenderTargetView(m_pRTVReflection, fClearColor);

	GLOBALVALUEMGR->GetDeviceContext()->OMSetRenderTargets(1, pd3dRTVs, m_pDSVDepthStencil);

	//카메라의 행렬을 임시로 저장한다.
	XMVECTOR xmvPos = pCamera->GetPosition();
	XMVECTOR xmvLookAt = pCamera->GetLook();

	//bigwater의 위치를 이용해 plane을 제작한다.
	//auto pBigWater = RCSELLER->GetTagRenderContainer()[tag::TAG_BIGWATER]["bigwater"]->GetObjectList().begin();
	XMVECTOR xmvPlane = XMPlaneFromPointNormal((*pBigWater)->GetPosition(), XMVectorSet(0, 1, 0, 0));

	//그 plane을 이용해서 카메라 반사
	pCamera->UpdateReflectionViewMtx(xmvPlane);

	//그 카메라 이용해서 애들 랜더
	UPDATER->GetSpaceContainer()->PrepareRender(pCamera, TAG_TERRAIN | TAG_STATIC_OBJECT | TAG_DYNAMIC_OBJECT | TAG_LIGHT);
	pCamera->SetShaderState();
	//render
	auto pObjectRenderer = RENDERER->GetObjectRenderer();
	ID3D11RasterizerState* pPrv;
	GLOBALVALUEMGR->GetDeviceContext()->RSGetState(&pPrv);
	GLOBALVALUEMGR->GetDeviceContext()->RSSetState(m_pTestRSState);
	pObjectRenderer->Excute(pCamera);
	GLOBALVALUEMGR->GetDeviceContext()->RSSetState(pPrv);
	pObjectRenderer->RenderSkyBox();
	UPDATER->GetSkyBoxContainer()->GetSkyBox()->RegistToContainer();
	//clear
	//그 카메라 이용해서 애들 랜더

	//다 하고 카메라 원상복귀
	pCamera->SetLookAt(xmvPos, xmvLookAt, XMVectorSet(0, 1, 0, 0));
	pCamera->UpdateViewMtx();
	pCamera->UpdateShaderState();

	//debug
	DEBUGER->AddTexture(XMFLOAT2(500, 500),XMFLOAT2(750, 750), m_pSRVReflection);
}

void CBigWaterRenderer::RenderRefractions(CCamera* pCamera){

}

void CBigWaterRenderer::RenderWater2(CCamera* pCamera, ID3D11RenderTargetView* pRTV, ID3D11DepthStencilView* pDSV) {
	auto pDeviceContext = GLOBALVALUEMGR->GetDeviceContext();
	auto pBigWater = RCSELLER->GetTagRenderContainer()[tag::TAG_BIGWATER]["bigwater"]->GetObjectList().begin();
	
	//rtv dsv set
	ID3D11RenderTargetView *pd3dRTVs[1] = { pRTV };
	pDeviceContext->OMSetRenderTargets(1, pd3dRTVs, pDSV);
	
	//bump map set
	m_pWaterBaseBumpMap->SetShaderState();
	m_pWaterDetailBumpMap->SetShaderState();

	//Surface render에 필요한 정보 set
	stRenderSurfaceInfo* pRenderSurfaceInfo = (stRenderSurfaceInfo*)m_pWaterPSBuffer->Map();
	pRenderSurfaceInfo[0].ViewProjMatrix = XMMatrixTranspose(pCamera->GetViewMtx() * pCamera->GetProjectionMtx());
	pRenderSurfaceInfo[0].RefViewProjMatrix = XMMatrixTranspose(pCamera->GetReflectionViewMtx() * pCamera->GetProjectionMtx());
	XMStoreFloat3(&pRenderSurfaceInfo[0].eyeVector, pCamera->GetPosition());
	pRenderSurfaceInfo[0].fresnelMode = m_fresnelMode;
	pRenderSurfaceInfo[0].fPowFactor = m_fPowFactor;
	pRenderSurfaceInfo[0].xDrawfactor = m_xDrawfactor;
	pRenderSurfaceInfo[0].windDir = m_windDir;
	pRenderSurfaceInfo[0].windForce = m_windForce;
	pRenderSurfaceInfo[0].windDir2 = m_windDir2;
	pRenderSurfaceInfo[0].windForce2 = m_windForce2;

	m_progress += TIMEMGR->GetTimeElapsed();
	pRenderSurfaceInfo[0].progress = m_progress;
	pRenderSurfaceInfo[0].BumpMapBaseUVScale = m_BumpMapBaseUVScale;
	pRenderSurfaceInfo[0].BumpMapDetailUVScale = m_BumpMapDetailUVScale;
	XMStoreFloat3(&pRenderSurfaceInfo[0].DirToLight, -UPDATER->GetDirectionalLight()->GetLook());
	pRenderSurfaceInfo[0].DirLightColor = UPDATER->GetDirectionalLight()->GetColor();
	pRenderSurfaceInfo[0].specExp = m_specExp;
	pRenderSurfaceInfo[0].specIntensity = m_specIntensity;
	pRenderSurfaceInfo[0].fWaterHeight = m_fWaterHeight;
	m_pWaterPSBuffer->Unmap();
	m_pWaterPSBuffer->SetShaderState();
	
	
	ID3D11ShaderResourceView* pSRV[] = { m_pSRVReflection };
	pDeviceContext->PSSetShaderResources(4, 1, pSRV);
	
	//state set
	ID3D11DepthStencilState* pTmp{ nullptr };
	UINT StencilRef{ 0 };
	pDeviceContext->OMGetDepthStencilState(&pTmp, &StencilRef);
	pDeviceContext->OMSetDepthStencilState(m_pd3dReflectDepthStencilState, 2);
	ID3D11BlendState* pTmpBlendState{ nullptr };
	float pPreBlendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	float pBlendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };

	UINT TmpBlendSampleMask{ 0 };
	pDeviceContext->OMGetBlendState(&pTmpBlendState, pPreBlendFactor, &TmpBlendSampleMask);
	pDeviceContext->OMSetBlendState(m_pAlphaBlendState, pBlendFactor, 0xffffffff);
	
	
	m_pWaterSurface->SetWorldMtx((*pBigWater)->GetWorldMtx());
	m_pWaterSurface->RegistToContainer();//water surface 1개 set하고 그림 
	RCSELLER->GetTagRenderContainer()[tag::TAG_BIGWATER]["bigwatersurface"]->Render(pCamera);

	for (auto RenderContainer : RCSELLER->GetTagRenderContainer()[tag::TAG_BIGWATER]) {
		RenderContainer.second->ClearObjectList();
	}

	//state 원상 복귀
	pDeviceContext->OMSetDepthStencilState(pTmp, StencilRef);
	pDeviceContext->OMSetBlendState(pTmpBlendState, pPreBlendFactor, TmpBlendSampleMask);
}
void CBigWaterRenderer::ResizeBuffer()
{
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
	//d3dTexture2DDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	D3D11_SHADER_RESOURCE_VIEW_DESC d3dSRVDesc;
	::ZeroMemory(&d3dSRVDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	d3dSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	d3dSRVDesc.Texture2D.MipLevels = 1;

	D3D11_RENDER_TARGET_VIEW_DESC d3dRTVDesc;
	::ZeroMemory(&d3dRTVDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
	d3dRTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	d3dRTVDesc.Texture2D.MipSlice = 0;
	d3dTexture2DDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	d3dTexture2DDesc.Format = d3dSRVDesc.Format = d3dRTVDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

	// 반사맵
	GLOBALVALUEMGR->GetDevice()->CreateTexture2D(&d3dTexture2DDesc, nullptr, &m_pTXTReflection);
	GLOBALVALUEMGR->GetDevice()->CreateRenderTargetView(m_pTXTReflection, &d3dRTVDesc, &m_pRTVReflection);
	GLOBALVALUEMGR->GetDevice()->CreateShaderResourceView(m_pTXTReflection, &d3dSRVDesc, &m_pSRVReflection);
	// 굴절맵
	GLOBALVALUEMGR->GetDevice()->CreateTexture2D(&d3dTexture2DDesc, nullptr, &m_pTXTRefraction);
	GLOBALVALUEMGR->GetDevice()->CreateRenderTargetView(m_pTXTRefraction, &d3dRTVDesc, &m_pRTVRefraction);
	GLOBALVALUEMGR->GetDevice()->CreateShaderResourceView(m_pTXTRefraction, &d3dSRVDesc, &m_pSRVRefraction);

	// Create the depth stencil view 
	D3D11_DEPTH_STENCIL_VIEW_DESC d3dDepthStencilViewDesc;
	ZeroMemory(&d3dDepthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	d3dDepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	d3dDepthStencilViewDesc.Texture2D.MipSlice = 0;

	d3dTexture2DDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	d3dDepthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	d3dSRVDesc.Format = DXGI_FORMAT_R32_FLOAT;
	d3dTexture2DDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
	GLOBALVALUEMGR->GetDevice()->CreateTexture2D(&d3dTexture2DDesc, nullptr, &m_pTXTDepthStencil);
	GLOBALVALUEMGR->GetDevice()->CreateDepthStencilView(m_pTXTDepthStencil, &d3dDepthStencilViewDesc, &m_pDSVDepthStencil);
	GLOBALVALUEMGR->GetDevice()->CreateShaderResourceView(m_pTXTDepthStencil, &d3dSRVDesc, &m_pSRVDepthStencil);
}

void CBigWaterRenderer::ReleaseBuffer()
{
	if (m_pTXTReflection) m_pTXTReflection->Release();
	if (m_pRTVReflection) m_pRTVReflection->Release();
	if (m_pSRVReflection) m_pSRVReflection->Release();

	if (m_pTXTRefraction) m_pTXTRefraction->Release();
	if (m_pRTVRefraction) m_pRTVRefraction->Release();
	if (m_pSRVRefraction) m_pSRVRefraction->Release();
}

void CBigWaterRenderer::SetShaderState()
{
}

void CBigWaterRenderer::CleanShaderState()
{
}

void CBigWaterRenderer::UpdateShaderState()
{
}
