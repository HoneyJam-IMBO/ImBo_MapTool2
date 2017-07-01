#include "stdafx.h"
#include "Shadow.h"

static  CTexture* pTestTex{ nullptr };

bool CShadow::Begin(){
	TWBARMGR->AddBoolBar("Effects", "SHADOW", "SHADOW_Switch", &m_bShadow);

	ReleaseBuffer();
	//m_pShadowVPBuffer = CBuffer::CreateConstantBuffer(1, sizeof(XMFLOAT4X4), 0, BIND_VS | BIND_DS , NULL);
	m_pCamera = new CFreeCamera;
	//------------------------------------------카메라 제작--------------------------------------
	float width = GLOBALVALUEMGR->GetrcClient().right;
	float height = GLOBALVALUEMGR->GetrcClient().bottom;

	m_pCamera->Begin();
	m_pCamera->GenerateProjectionMatrix(60.0f * XM_PI / 180.0f
		, float(GLOBALVALUEMGR->GetrcClient().right) / float(GLOBALVALUEMGR->GetrcClient().bottom)
		, 10.0f
		,3000.0f);

	//viewprojection행렬 제작 / 광원
	XMVECTOR eye = { 500.0f, 500.0f, -500.0f, 0.0f };
	XMVECTOR at = { 500, 0.0f, 0, 0.0f };
	XMVECTOR up = { 0.0f, 1.0f, 0.0f, 0.0f };
	m_pCamera->SetLookAt(eye, at, up);

	D3D11_RASTERIZER_DESC descRasterizer;
	ZeroMemory(&descRasterizer, sizeof(D3D11_RASTERIZER_DESC));
	descRasterizer.FillMode = D3D11_FILL_SOLID;
	descRasterizer.CullMode = D3D11_CULL_NONE;
	GLOBALVALUEMGR->GetDevice()->CreateRasterizerState(&descRasterizer, &m_pd3dRSShader);

	m_pShadowBuf = RESOURCEMGR->CreateConstantBuffer("ShadowInfoBuffer", 1, sizeof(stShadowInfo), 4, BIND_PS);
	//m_pStaticShadowBuf = RESOURCEMGR->CreateConstantBuffer("StaticShadowInfoBuffer", 1, sizeof(stStaticShadowInfo), 9, BIND_PS);
	//global buffer 제작
	//m_pGlobalTerrainBuffer = RESOURCEMGR->GetBuffer("SpaceCB");

	m_pShadowInfo = new stShadowInfo;

	_TCHAR ppstrShadowMapTextureNames[MAX_SPACE_NUM][128];
	UINT nSpace = MAX_SPACE_NUM;
	for (UINT i = 0; i<nSpace; ++i) {
		WCHAR wcShadowMapPath[256];
		wsprintf(ppstrShadowMapTextureNames[i], L"../../Assets/default.jpg");
	}

	//m_pShadowTexture = CTexture::CreateTexture(nSpace, ppstrShadowMapTextureNames, 9, BIND_PS);
	//maptool
	//maptool


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
	////----------------------------------------Render Desc-----------------------------------------//
	//D3D11_RENDER_TARGET_VIEW_DESC d3dRTVDesc;
	//::ZeroMemory(&d3dRTVDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
	//d3dRTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	//d3dRTVDesc.Texture2D.MipSlice = 0;
	////----------------------------------------TextUre Desc-----------------------------------------//
	//d3dTexture2DDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	//d3dTexture2DDesc.Format = d3dSRVDesc.Format = d3dRTVDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	//GLOBALVALUEMGR->GetDevice()->CreateTexture2D(&d3dTexture2DDesc, nullptr, &m_pd3dtxtRenderShadow);
	//GLOBALVALUEMGR->GetDevice()->CreateShaderResourceView(m_pd3dtxtRenderShadow, &d3dSRVDesc, &m_pd3dsrvRenderShadow);
	//GLOBALVALUEMGR->GetDevice()->CreateRenderTargetView(m_pd3dtxtRenderShadow, &d3dRTVDesc, &m_pd3drtvRenderShadow);

	//m_pDebugTextureObj = new CDebugTexture;
	//m_pDebugTextureObj->Begin();
	//maptool
	//maptool
	return true;
}

bool CShadow::End()
{
	//if (m_pGlobalTerrainBuffer) m_pGlobalTerrainBuffer = nullptr;
	if (m_pShadowBuf) m_pShadowBuf = nullptr;
	//if (m_pStaticShadowBuf) m_pStaticShadowBuf = nullptr;

	if (m_pCamera) {
		m_pCamera->End();
		delete m_pCamera;
	}
	m_pCamera = nullptr;
	//static shadow map clean
	//if (m_pShadowTexture){ 
	//	m_pShadowTexture->CleanShaderState();
	//	m_pShadowTexture->End();
	//	m_pShadowTexture = nullptr;
	//}
	//if (m_pGlobalTerrainBuffer) {
	//	m_pGlobalTerrainBuffer->CleanShaderState();
	//	m_pGlobalTerrainBuffer->End();
	//	m_pGlobalTerrainBuffer = nullptr;
	//}
	return true;
}

void CShadow::SetShaderState(){
	//동적 그림자 srv
	ID3D11ShaderResourceView* pSRV[] = { m_pd3dsrvShadow };
	if (pTestTex) {
		pTestTex->SetShaderState();
	}
	else
		GLOBALVALUEMGR->GetDeviceContext()->PSSetShaderResources(5, 1, pSRV);
	//동적 그림자 srv

	//이건 동적 그림자 카메라
	stShadowInfo* pData = (stShadowInfo*)m_pShadowBuf->Map();
	pData->xmmtxViewProj = XMMatrixTranspose(m_pCamera->GetViewMtx()*m_pCamera->GetProjectionMtx());
	pData->bias = m_pShadowInfo->bias * 0.00001;
	pData->bias_offset = m_pShadowInfo->bias_offset * 0.00001;
	pData->kernelhalf = m_pShadowInfo->kernelhalf;
	pData->dist_sum = m_pShadowInfo->dist_sum;

	m_pShadowBuf->Unmap();
	m_pShadowBuf->SetShaderState();
	//이건 동적 그림자 카메라

	//m_pStaticShadowBuf->SetShaderState();
}

void CShadow::CleanShaderState(){
	//m_pStaticShadowBuf->CleanShaderState();
	ID3D11ShaderResourceView* pSRV[] = { nullptr };
	GLOBALVALUEMGR->GetDeviceContext()->PSSetShaderResources(5, 1, pSRV);
	m_pShadowBuf->CleanShaderState();
}

void CShadow::UpdateShaderState(){

}

ID3D11ShaderResourceView * CShadow::RenderShadowMap( CCamera* pCamera){

	GLOBALVALUEMGR->GetDeviceContext()->ClearDepthStencilView(m_pd3ddsvShadow, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	if (m_bShadow) {
		DEBUGER->start_Timemeasurement();
		UPDATER->GetSpaceContainer()->PrepareRender(m_pCamera, TAG_DYNAMIC_OBJECT | TAG_STATIC_OBJECT | TAG_TERRAIN);

		ID3D11RasterizerState* pPrevRSState;
		GLOBALVALUEMGR->GetDeviceContext()->RSGetState(&pPrevRSState);
		GLOBALVALUEMGR->GetDeviceContext()->RSSetState(m_pd3dRSShader);


		float offset = UPDATER->GetDirectionalLight()->GetOffsetLength();
		XMVECTOR xmvDirectionalLightDir = UPDATER->GetDirectionalLight()->GetLook();
		//XMVECTOR xmvCameraPos = pCamera->GetPosition();
		//XMVECTOR eye = xmvCameraPos + xmvDirectionalLightDir*offset;
		//XMVECTOR at = xmvCameraPos;
		UINT SelectSpace = 0;//m_SelectSpace;
		UINT nSpace = UPDATER->GetSpaceContainer()->GetSpaceNum();
		XMVECTOR at;
		XMFLOAT3 xmf3Pos;
		if (SelectSpace == 0) {
			float space_size = UPDATER->GetSpaceContainer()->GetSize();
			XMVECTOR xmPos = XMVectorSet(space_size / 2, space_size / 2, space_size / 2, 0.f);
			//float pos_offset = (UPDATER->GetSpaceContainer()->GetOneSpaceSize() / 2);
			//xmPos += XMVectorSet(pos_offset, pos_offset, pos_offset, 0);
			XMStoreFloat3(&xmf3Pos, xmPos);
			at = XMVectorSet(xmf3Pos.x, xmf3Pos.y, xmf3Pos.z, 0);
			////select space가 0이면 camera를 쫓아 다닌다.
			//if(pCamera) XMStoreFloat3(&xmf3Pos, pCamera->GetPosition());
			//else xmf3Pos = XMFLOAT3{ 0.f, 0.f, 0.f };
			//
			//at = XMVectorSet(xmf3Pos.x, 128.f, xmf3Pos.z, 0);
		}
		else if (SelectSpace <= nSpace) {
			//space num보다 작다면
			//select space index -1 이 space의 index이다 
			XMVECTOR xmPos = UPDATER->GetSpaceContainer()->GetAllSpace()[SelectSpace - 1]->GetPosition();
			float pos_offset = (UPDATER->GetSpaceContainer()->GetOneSpaceSize() / 2);;
			xmPos += XMVectorSet(pos_offset, pos_offset, pos_offset, 0);
			XMStoreFloat3(&xmf3Pos, xmPos);
			at = XMVectorSet(xmf3Pos.x, 128.f, xmf3Pos.z, 0);
		}
		XMVECTOR eye = at + xmvDirectionalLightDir*offset;
		XMVECTOR up = { 0.0f, 1.0f, 0.0f, 0.0f };
		m_pCamera->SetLookAt(eye, xmvDirectionalLightDir, up);
		m_pCamera->SetViewport(0, 0, 4096, 4096, 0.0f, 1.0f);

		m_pCamera->UpdateShaderState();
		m_pCamera->SetShaderState();

		//UPDATER->GetSpaceContainer()->PrepareRenderOneSpace(m_pCamera, RTAG_TERRAIN | RTAG_DYNAMIC_OBJECT | RTAG_STATIC_OBJECT, SelectSpace - 1);

		ID3D11RenderTargetView*	rtNULL = nullptr;
		GLOBALVALUEMGR->GetDeviceContext()->OMSetRenderTargets(1, &rtNULL, m_pd3ddsvShadow);
		//GLOBALVALUEMGR->GetDeviceContext()->PSSetShaderResources(m_TextureStartSlot, 1, &m_pd3dsrvShadow);
		RENDERER->GetObjectRenderer()->ExcuteShadowRender(m_pCamera);

		DEBUGER->AddDepthTexture(XMFLOAT2(500, 0), XMFLOAT2(750, 150), m_pd3dsrvShadow);
		//DEBUGER->AddDepthTexture(XMFLOAT2(500, 150), XMFLOAT2(750, 300), m_pShadowTexture->GetShaderResourceView());
		m_pCamera->SetViewport(0, 0, GLOBALVALUEMGR->GetrcClient().right, GLOBALVALUEMGR->GetrcClient().bottom, 0.0f, 1.0f);
		GLOBALVALUEMGR->GetDeviceContext()->RSSetState(pPrevRSState);
		DEBUGER->end_Timemeasurement(L"sahdow");

		if (INPUTMGR->KeyDown(VK_Y)) {
			if (pTestTex) pTestTex->End();
			pTestTex = nullptr;
		}
		if (INPUTMGR->KeyDown(VK_T)) {
			if (pTestTex) pTestTex->End();
			D3D11_TEXTURE2D_DESC d3dTexture2DDesc;
			::ZeroMemory(&d3dTexture2DDesc, sizeof(D3D11_TEXTURE2D_DESC));
			d3dTexture2DDesc.Width = 4096;
			d3dTexture2DDesc.Height = 4096;
			d3dTexture2DDesc.MipLevels = 1;
			d3dTexture2DDesc.ArraySize = 1;
			d3dTexture2DDesc.SampleDesc.Count = 1;
			d3dTexture2DDesc.SampleDesc.Quality = 0;
			d3dTexture2DDesc.Usage = D3D11_USAGE_DEFAULT;
			d3dTexture2DDesc.CPUAccessFlags = 0;
			d3dTexture2DDesc.MiscFlags = 0;
			d3dTexture2DDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			d3dTexture2DDesc.Format = DXGI_FORMAT_R32_FLOAT;
			ID3D11Texture2D* pd3dtxtSaveShadow;
			GLOBALVALUEMGR->GetDevice()->CreateTexture2D(&d3dTexture2DDesc, nullptr, &pd3dtxtSaveShadow);

			GLOBALVALUEMGR->GetDeviceContext()->CopyResource(pd3dtxtSaveShadow, m_pd3dtxtShadow);

			D3DX11SaveTextureToFileA(GLOBALVALUEMGR->GetDeviceContext(), pd3dtxtSaveShadow, D3DX11_IFF_DDS, "../shadow.dds");
			TexMetadata metadata;
			ScratchImage image;
			LoadFromDDSFile(L"../shadow.dds", DDS_FLAGS_NONE, &metadata, image);
			pTestTex = CTexture::CreateTexture(L"../shadow.dds", 5, BIND_PS, nullptr);
		}
		if (pTestTex) {
			DEBUGER->AddDepthTexture(XMFLOAT2(500, 300), XMFLOAT2(750, 450), pTestTex->GetShaderResourceView());
		}
	}
	return m_pd3dsrvShadow;

	
}

void CShadow::ResizeBuffer()
{
	ReleaseBuffer();

	D3D11_DEPTH_STENCIL_VIEW_DESC d3dDepthStencilViewDesc;
	ZeroMemory(&d3dDepthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	d3dDepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	d3dDepthStencilViewDesc.Texture2D.MipSlice = 0;
	d3dDepthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;

	D3D11_TEXTURE2D_DESC d3dTexture2DDesc;
	::ZeroMemory(&d3dTexture2DDesc, sizeof(D3D11_TEXTURE2D_DESC));
	d3dTexture2DDesc.Width = 4096;
	d3dTexture2DDesc.Height = 4096;
	d3dTexture2DDesc.MipLevels = 1;
	d3dTexture2DDesc.ArraySize = 1;
	d3dTexture2DDesc.SampleDesc.Count = 1;
	d3dTexture2DDesc.SampleDesc.Quality = 0;
	d3dTexture2DDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dTexture2DDesc.CPUAccessFlags = 0;
	d3dTexture2DDesc.MiscFlags = 0;
	d3dTexture2DDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
	d3dTexture2DDesc.Format = DXGI_FORMAT_R32_TYPELESS;

	D3D11_SHADER_RESOURCE_VIEW_DESC d3dSRVDesc;
	::ZeroMemory(&d3dSRVDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	d3dSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	d3dSRVDesc.Texture2D.MipLevels = 1;
	d3dSRVDesc.Format = DXGI_FORMAT_R32_FLOAT;
	//d3dTexture2DDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
	GLOBALVALUEMGR->GetDevice()->CreateTexture2D(&d3dTexture2DDesc, nullptr, &m_pd3dtxtShadow);
	GLOBALVALUEMGR->GetDevice()->CreateDepthStencilView(m_pd3dtxtShadow, &d3dDepthStencilViewDesc, &m_pd3ddsvShadow);
	GLOBALVALUEMGR->GetDevice()->CreateShaderResourceView(m_pd3dtxtShadow, &d3dSRVDesc, &m_pd3dsrvShadow);
}

void CShadow::ReleaseBuffer()
{
	if (m_pd3dtxtShadow) m_pd3dtxtShadow->Release();
	if (m_pd3dsrvShadow) m_pd3dsrvShadow->Release();
	if (m_pd3ddsvShadow) m_pd3ddsvShadow->Release();
}

void CShadow::CreateShadowControlUI(){
	const char* barName = "PickingBar";
	TWBARMGR->AddMinMaxBarRW(barName, "SHADOW", "bias", &m_pShadowInfo->bias, 0.0f, 1.0f, 0.00001f);
	TWBARMGR->AddMinMaxBarRW(barName, "SHADOW", "bias_offset", &m_pShadowInfo->bias_offset, 0.0f, 0.1, 0.000001f);
	TWBARMGR->AddMinMaxBarRW(barName, "SHADOW", "kernelhalf", &m_pShadowInfo->kernelhalf, 0.0f, 4.0f, 1.f);
	TWBARMGR->AddMinMaxBarRW(barName, "SHADOW", "dist_sum", &m_pShadowInfo->dist_sum, 0.001f, 0.1f, 0.001f);

	//TWBARMGR->AddMinMaxBarRW(barName, "SHADOW", "Select_Space", &m_SelectSpace, 0.f, UPDATER->GetSpaceContainer()->GetSpaceNum(), 1.0f);
}

void CShadow::SaveShadow(wstring wsOutputPath, wstring wsSceneName){
	EXPORTER->WriteFloat(m_pShadowInfo->bias); EXPORTER->WriteSpace();
	EXPORTER->WriteFloat(m_pShadowInfo->bias_offset); EXPORTER->WriteSpace();
	EXPORTER->WriteFloat(m_pShadowInfo->dist_sum); EXPORTER->WriteSpace();
	EXPORTER->WriteFloat(m_pShadowInfo->kernelhalf); EXPORTER->WriteSpace();
	EXPORTER->WriteEnter();
	//위 shadow 인자들을 저장하고
	
	//float precSelect = m_SelectSpace;
	////모든 공간에 대해서
	//for(int i =0; i<UPDATER->GetSpaceContainer()->GetSpaceNum(); ++i){
	//	WCHAR wcShadowMapPath[256];
	//	wsprintf(wcShadowMapPath, L"%s%sShadowMap%d.dds", wsOutputPath.c_str(), wsSceneName.c_str(), i);
	//	
	//	m_SelectSpace = i + 1;
	//	RenderShadowMap();
	//
	//	//shadow map을 set하고 r32g32b32a32 rtv를 set해서 
	//	ID3D11RenderTargetView *pd3dRTVs[1] = { m_pd3drtvRenderShadow };
	//	//float fClearColor[4] = { xmf4Xolor.x, xmf4Xolor.y, xmf4Xolor.z, xmf4Xolor.w };
	//	float fClearColor[4] = { 0.f, 0.f, 0.f, 0.f };
	//	if (m_pd3drtvRenderShadow) GLOBALVALUEMGR->GetDeviceContext()->ClearRenderTargetView(m_pd3drtvRenderShadow, fClearColor);
	//	GLOBALVALUEMGR->GetDeviceContext()->OMSetRenderTargets(1, pd3dRTVs, nullptr);
	//
	//	//camera 설정
	//
	//	//camera 설정
	//
	//	//그림을 그린다. 
	//	m_pDebugTextureObj->SetTextureInfo(XMFLOAT2(0,0), XMFLOAT2(GLOBALVALUEMGR->GetrcClient().right, GLOBALVALUEMGR->GetrcClient().bottom));
	//	 CTexture* pTexture = CTexture::CreateTexture(m_pd3dsrvShadow);
	//	RCSELLER->GetRenderContainer(TAG_DEBUG, "debugtexture")->AddObject(m_pDebugTextureObj);
	//	RCSELLER->GetRenderContainer(TAG_DEBUG, "debugtexture")->AddTexture(pTexture);
	//	RCSELLER->GetRenderContainer(TAG_DEBUG, "debugtexture")->Render(nullptr);
	//	RCSELLER->GetRenderContainer(TAG_DEBUG, "debugtexture")->ClearObjectList();
	//	RCSELLER->GetRenderContainer(TAG_DEBUG, "debugtexture")->ClearTextures();
	//	pd3dRTVs[0] = { nullptr };
	//	GLOBALVALUEMGR->GetDeviceContext()->OMSetRenderTargets(1, pd3dRTVs, nullptr);
	//
	//	//그 rtv를 저장한다. 
	//	EXPORTER->MakeSRVTexture(m_pd3dsrvRenderShadow, wcShadowMapPath);
	//}
	//
	//m_SelectSpace = precSelect;
}

void CShadow::LoadShadow(wstring wsOutputPath, wstring wsSceneName){
	m_pShadowInfo->bias = IMPORTER->ReadFloat();
	m_pShadowInfo->bias_offset = IMPORTER->ReadFloat();
	m_pShadowInfo->dist_sum = IMPORTER->ReadFloat();
	m_pShadowInfo->kernelhalf = IMPORTER->ReadFloat();


	//_TCHAR ppstrShadowMapTextureNames[MAX_SPACE_NUM][128];
	//UINT nSpace = UPDATER->GetSpaceContainer()->GetSpaceNum();
	//for (UINT i = 0; i<nSpace; ++i) {
	//	WCHAR wcShadowMapPath[256];
	//	wsprintf(ppstrShadowMapTextureNames[i], L"%s%sShadowMap%d.dds", wsOutputPath.c_str(), wsSceneName.c_str(), i);
	//}
	//m_pShadowTexture->End();
	//m_pShadowTexture = nullptr;
	//
	//m_pShadowTexture = CTexture::CreateTexture(nSpace, ppstrShadowMapTextureNames, 9, BIND_PS);
	////static shadow map set
	//if (m_pShadowTexture) m_pShadowTexture->SetShaderState();

	//float offset = UPDATER->GetDirectionalLight()->GetOffsetLength();
	//XMVECTOR xmvDirectionalLightDir = UPDATER->GetDirectionalLight()->GetLook();
	//UINT SelectSpace = m_SelectSpace;
	//XMVECTOR at;
	//XMFLOAT3 xmf3Pos;
	//stStaticShadowInfo* pData = (stStaticShadowInfo*)m_pStaticShadowBuf->Map();
	////이건 정적 그림자 카메라 버퍼
	//for (int i = 0; i < UPDATER->GetSpaceContainer()->GetSpaceNum(); ++i) {
	//	XMVECTOR xmPos = UPDATER->GetSpaceContainer()->GetAllSpace()[i]->GetPosition();
	//	float pos_offset = (UPDATER->GetSpaceContainer()->GetOneSpaceSize() / 2);;
	//	xmPos += XMVectorSet(pos_offset, pos_offset, pos_offset, 0);
	//	XMStoreFloat3(&xmf3Pos, xmPos);
	//	at = XMVectorSet(xmf3Pos.x, 0, xmf3Pos.z, 0);
	//	XMVECTOR eye = at + xmvDirectionalLightDir*offset;
	//	XMVECTOR up = { 0.0f, 1.0f, 0.0f, 0.0f };
	//	m_pCamera->SetLookAt(eye, at, up);
	//
	//	pData->xmmtxViewProj[i] = XMMatrixTranspose(m_pCamera->GetViewMtx()*m_pCamera->GetProjectionMtx());
	//}
	//
	//m_pStaticShadowBuf->Unmap();
	//m_pStaticShadowBuf->SetShaderState();

	//SPACE_GLOBAL_VALUE* pSpaceData = (SPACE_GLOBAL_VALUE*)m_pGlobalTerrainBuffer->Map();
	////pData = m_pGlobalTerrainData;
	//pSpaceData->OneSideSpaceNum = UPDATER->GetSpaceContainer()->GetOneSideSpaceNum();
	//pSpaceData->OneSpaceSizeRcp = UPDATER->GetSpaceContainer()->GetOneSpaceSize();
	//m_pGlobalTerrainBuffer->Unmap();
	//m_pGlobalTerrainBuffer->SetShaderState();
}

CShadow::CShadow()
	: DXObject("shadow"){

}

CShadow::~CShadow(){

}
