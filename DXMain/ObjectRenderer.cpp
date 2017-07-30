#include "stdafx.h"
#include "ObjectRenderer.h"

static bool gbTerrainWireFrame{ false };

bool CObjectRenderer::Begin(){
	TWBARMGR->AddBoolBar("TOOL_MODE", "TerrainControl", "Terrain_WireFrame", &gbTerrainWireFrame);

	//rasterizer state
	D3D11_RASTERIZER_DESC descRasterizer;
	ZeroMemory(&descRasterizer, sizeof(D3D11_RASTERIZER_DESC));
	//descRasterizer.FillMode = D3D11_FILL_SOLID;
	descRasterizer.FillMode = D3D11_FILL_WIREFRAME;
	descRasterizer.CullMode = D3D11_CULL_NONE;
	GLOBALVALUEMGR->GetDevice()->CreateRasterizerState(&descRasterizer, &m_pTestRSState);
	//rasterizer state


	m_pTerrainRenderContainer = RCSELLER->GetTagRenderContainer()[tag::TAG_TERRAIN]["terrain"];
	m_pSkyBoxRenderContainer = RCSELLER->GetTagRenderContainer()[tag::TAG_SPACE]["skybox"];

	//skybox depth stencil
	D3D11_DEPTH_STENCIL_DESC descDepth;
	descDepth.DepthEnable = FALSE;
	descDepth.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	descDepth.DepthFunc = D3D11_COMPARISON_LESS;
	descDepth.StencilEnable = TRUE;
	descDepth.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	descDepth.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	const D3D11_DEPTH_STENCILOP_DESC noSkyStencilOp = { D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_EQUAL };
	descDepth.FrontFace = noSkyStencilOp;
	descDepth.BackFace = noSkyStencilOp;
	GLOBALVALUEMGR->GetDevice()->CreateDepthStencilState(&descDepth, &m_pd3dDepthStencilState);

	return true;
}

bool CObjectRenderer::End(){
	m_pTerrainRenderContainer = nullptr;
	m_pSkyBoxRenderContainer = nullptr;
	if (m_pd3dDepthStencilState)m_pd3dDepthStencilState->Release();
	if (m_pd3dTempDepthStencilState)m_pd3dTempDepthStencilState->Release();

	return true;
}

void CObjectRenderer::SetShaderState(){
	
}

void CObjectRenderer::CleanShaderState(){

	m_pTerrainRenderContainer->ClearObjectList();

	for (auto RenderContainer : RCSELLER->GetTagRenderContainer()[tag::TAG_DYNAMIC_OBJECT]) {
		RenderContainer.second->ClearObjectList();
	}
	for (auto RenderContainer : RCSELLER->GetTagRenderContainer()[tag::TAG_STATIC_OBJECT]) {
		RenderContainer.second->ClearObjectList();
	}
	for (auto RenderContainer : RCSELLER->GetStempRenderContainer()[tag::TAG_DYNAMIC_OBJECT]) {
		RenderContainer.second->ClearObjectList();
	}
	for (auto RenderContainer : RCSELLER->GetStempRenderContainer()[tag::TAG_STATIC_OBJECT]) {
		RenderContainer.second->ClearObjectList();
	}
}

void CObjectRenderer::UpdateShaderState(){

}

void CObjectRenderer::Excute( CCamera* pCamera){
	//scene의 모든 Part의 rendercontainer안에 part list Render!
	//RenderSkyBox();
	
	if(gbTerrainWireFrame){
		ID3D11RasterizerState* pPrv;
		GLOBALVALUEMGR->GetDeviceContext()->RSGetState(&pPrv);
		GLOBALVALUEMGR->GetDeviceContext()->RSSetState(m_pTestRSState);
		m_pTerrainRenderContainer->Render(pCamera);
		GLOBALVALUEMGR->GetDeviceContext()->RSSetState(pPrv);
	}
	else {
		m_pTerrainRenderContainer->Render(pCamera);
	}

	
	for (auto RenderContainer : RCSELLER->GetTagRenderContainer()[tag::TAG_DYNAMIC_OBJECT]) {
		RenderContainer.second->Render(pCamera);
	}
	for (auto RenderContainer : RCSELLER->GetTagRenderContainer()[tag::TAG_STATIC_OBJECT]) {
		RenderContainer.second->Render(pCamera);
	}
	for (auto RenderContainer : RCSELLER->GetStempRenderContainer()[tag::TAG_DYNAMIC_OBJECT]) {
		RenderContainer.second->Render(pCamera);
	}
	for (auto RenderContainer : RCSELLER->GetStempRenderContainer()[tag::TAG_STATIC_OBJECT]) {
		RenderContainer.second->Render(pCamera);
	}
	
	CleanShaderState();
}
void CObjectRenderer::ExcuteShadowRender( CCamera* pCamera)
{
	m_pTerrainRenderContainer->Render_NoPS(pCamera);
	m_pTerrainRenderContainer->ClearObjectList();

	for (auto RenderContainer : RCSELLER->GetTagRenderContainer()[tag::TAG_DYNAMIC_OBJECT]) {
		RenderContainer.second->Render_NoPS(pCamera);
		RenderContainer.second->ClearObjectList();
	}
	for (auto RenderContainer : RCSELLER->GetTagRenderContainer()[tag::TAG_STATIC_OBJECT]) {
		RenderContainer.second->Render_NoPS(pCamera);
		RenderContainer.second->ClearObjectList();
	}
	for (auto RenderContainer : RCSELLER->GetStempRenderContainer()[tag::TAG_DYNAMIC_OBJECT]) {
		RenderContainer.second->Render_NoPS(pCamera);
		RenderContainer.second->ClearObjectList();
	}
	for (auto RenderContainer : RCSELLER->GetStempRenderContainer()[tag::TAG_STATIC_OBJECT]) {
		RenderContainer.second->Render_NoPS(pCamera);
		RenderContainer.second->ClearObjectList();
	}
}

void CObjectRenderer::RenderSkyBox(){
	//GLOBALVALUEMGR->GetDeviceContext()->OMGetDepthStencilState(&m_pd3dTempDepthStencilState, &m_TempStencil);
	//GLOBALVALUEMGR->GetDeviceContext()->OMSetDepthStencilState(m_pd3dDepthStencilState, 0);
	m_pSkyBoxRenderContainer->Render(nullptr);
	m_pSkyBoxRenderContainer->ClearObjectList();
	//GLOBALVALUEMGR->GetDeviceContext()->OMSetDepthStencilState(m_pd3dTempDepthStencilState, m_TempStencil);

}


CObjectRenderer::CObjectRenderer() : DXObject("objectrenderer"){

}

CObjectRenderer::~CObjectRenderer(){

}
