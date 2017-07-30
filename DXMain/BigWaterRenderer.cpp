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
	ResizeBuffer();

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


	//���� ���ٽ� ������Ʈ
	//�ſ��� �������� ���̽��ٽ� ������Ʈ
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
	//�ſ￡ �ݻ�� ��ü ������ �� ���� ���ٽ� ������Ʈ
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

	m_pWaterPSBuffer = CBuffer::CreateConstantBuffer(1, 2*sizeof(XMMATRIX), 0, BIND_PS, NULL);
	m_pWaterInfoPSBuffer = CBuffer::CreateConstantBuffer(1, sizeof(stBigWaterInfo), 0, BIND_PS, NULL);
	return true;
}

bool CBigWaterRenderer::End()
{
	m_pWaterSurface->End();
	delete m_pWaterSurface;

	ReleaseBuffer();

	m_pWaterPSBuffer->End();	delete m_pWaterPSBuffer;  m_pWaterPSBuffer = nullptr;
	m_pWaterInfoPSBuffer->End();	delete m_pWaterInfoPSBuffer;  m_pWaterInfoPSBuffer = nullptr;
	//m_pWaterPSBuffer->End();	delete m_pWaterPSBuffer;  m_pWaterPSBuffer = nullptr;
	//m_pReflractionVSBuffer->End();	m_pReflractionVSBuffer = nullptr;

	return true;
}

void CBigWaterRenderer::RenderBigWater(CCamera* pCamera, ID3D11RenderTargetView* pRTV, ID3D11DepthStencilView* pDSV) {
	size_t size = RCSELLER->GetTagRenderContainer()[tag::TAG_BIGWATER]["bigwater"]->GetObjectList().size();
	if (size > 0) {
		GLOBALVALUEMGR->GetDeviceContext()->ClearDepthStencilView(m_pDSVDepthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		RenderWater1(pCamera);//�� �⺻ ���� �׸���
		RenderWaterSurface(pCamera);//�� ǥ�� �׸���
		RenderRefractions(pCamera);//���� �׸���
		RenderWater2(pCamera, pRTV, pDSV);//�� ǥ��, ���� ���� �̿��� �� ǥ�� ����
	}
	else {
		m_bBigWater = false;
	}
}

void CBigWaterRenderer::RenderWater1(CCamera* pCamera) {//big water�� �ϳ��� �������
	//���� �׸� �� depth stencil....�� �Ϸ��� �ߴµ� ����. �̰� stencil�ִ�?
	//stencil������
	//�� �׸� �� stencil ��ŷ
	//2. �ſ��� �׸���. ���ٽ� ���۸� ���� ���� Ÿ�ٿ� ������� ����.!


	ID3D11DepthStencilState* pTmp{ nullptr };
	UINT StencilRef{ 0 };
	auto pDeviceContext = GLOBALVALUEMGR->GetDeviceContext();
	pDeviceContext->OMGetDepthStencilState(&pTmp, &StencilRef);
	pDeviceContext->OMSetDepthStencilState(m_pd3dMirrorToStencilState, 2);

	RCSELLER->GetTagRenderContainer()[tag::TAG_BIGWATER]["bigwater"]->Render(pCamera);

	//state ���� ����
	pDeviceContext->OMSetDepthStencilState(pTmp, StencilRef);
}

void CBigWaterRenderer::RenderWaterSurface(CCamera* pCamera) {
	auto pBigWater = RCSELLER->GetTagRenderContainer()[tag::TAG_BIGWATER]["bigwater"]->GetObjectList().begin();
	stBigWaterInfo* pWaterInfo = (stBigWaterInfo*)m_pWaterInfoPSBuffer->Map();
	pWaterInfo[0].nFlag = 1;
	pWaterInfo[0].fHeight = (*pBigWater)->GetPositionY();
	m_pWaterInfoPSBuffer->Unmap();
	m_pWaterInfoPSBuffer->SetShaderState();

	//ǥ�� ���� �����Ѵ�.
	ID3D11RenderTargetView *pd3dRTVs[1] = { m_pRTVReflection };
	//float fClearColor[4] = { xmf4Xolor.x, xmf4Xolor.y, xmf4Xolor.z, xmf4Xolor.w };
	float fClearColor[4] = { 0.f, 0.f, 0.f, 1.f };
	if (m_pRTVReflection) GLOBALVALUEMGR->GetDeviceContext()->ClearRenderTargetView(m_pRTVReflection, fClearColor);

	GLOBALVALUEMGR->GetDeviceContext()->OMSetRenderTargets(1, pd3dRTVs, m_pDSVDepthStencil);

	//ī�޶��� ����� �ӽ÷� �����Ѵ�.
	XMVECTOR xmvPos = pCamera->GetPosition();
	XMVECTOR xmvLookAt = pCamera->GetLook();

	//bigwater�� ��ġ�� �̿��� plane�� �����Ѵ�.
	//auto pBigWater = RCSELLER->GetTagRenderContainer()[tag::TAG_BIGWATER]["bigwater"]->GetObjectList().begin();
	XMVECTOR xmvPlane = XMPlaneFromPointNormal((*pBigWater)->GetPosition(), XMVectorSet(0, 1, 0, 0));

	//�� plane�� �̿��ؼ� ī�޶� �ݻ�
	pCamera->UpdateReflectionViewMtx(xmvPlane);

	//�� ī�޶� �̿��ؼ� �ֵ� ����
	UPDATER->GetSpaceContainer()->PrepareRender(pCamera, TAG_TERRAIN | TAG_STATIC_OBJECT | TAG_DYNAMIC_OBJECT | TAG_LIGHT);
	pCamera->SetShaderState();
	//render
	auto pObjectRenderer = RENDERER->GetObjectRenderer();
	pObjectRenderer->Excute(pCamera);
	pObjectRenderer->RenderSkyBox();
	UPDATER->GetSkyBoxContainer()->GetSkyBox()->RegistToContainer();
	//clear
	//�� ī�޶� �̿��ؼ� �ֵ� ����

	//�� �ϰ� ī�޶� ���󺹱�
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
	ID3D11RenderTargetView *pd3dRTVs[1] = { pRTV };
	pDeviceContext->OMSetRenderTargets(1, pd3dRTVs, pDSV);
	
	XMMATRIX* pMtx = (XMMATRIX*)m_pWaterPSBuffer->Map();
	pMtx[0] = XMMatrixTranspose(pCamera->GetViewMtx() * pCamera->GetProjectionMtx());
	pMtx[1] = XMMatrixTranspose(pCamera->GetReflectionViewMtx() * pCamera->GetProjectionMtx());
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
	FLOAT prevBlendFactor[4];
	UINT TmpBlendSampleMask{ 0 };
	pDeviceContext->OMGetBlendState(&pTmpBlendState, prevBlendFactor, &TmpBlendSampleMask);
	pDeviceContext->OMSetBlendState(m_pAlphaBlendState, nullptr, 0xffffffff);
	
	
	m_pWaterSurface->SetWorldMtx((*pBigWater)->GetWorldMtx());
	m_pWaterSurface->RegistToContainer();//water surface 1�� set�ϰ� �׸� 
	RCSELLER->GetTagRenderContainer()[tag::TAG_BIGWATER]["bigwatersurface"]->Render(pCamera);

	for (auto RenderContainer : RCSELLER->GetTagRenderContainer()[tag::TAG_BIGWATER]) {
		RenderContainer.second->ClearObjectList();
	}

	//state ���� ����
	pDeviceContext->OMSetDepthStencilState(pTmp, StencilRef);
	pDeviceContext->OMSetBlendState(pTmpBlendState, prevBlendFactor, TmpBlendSampleMask);
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

	// �ݻ��
	GLOBALVALUEMGR->GetDevice()->CreateTexture2D(&d3dTexture2DDesc, nullptr, &m_pTXTReflection);
	GLOBALVALUEMGR->GetDevice()->CreateRenderTargetView(m_pTXTReflection, &d3dRTVDesc, &m_pRTVReflection);
	GLOBALVALUEMGR->GetDevice()->CreateShaderResourceView(m_pTXTReflection, &d3dSRVDesc, &m_pSRVReflection);
	// ������
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
