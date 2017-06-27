#include "stdafx.h"
#include "Stemp.h"
#include "StempManager.h"

void CStemp::Begin(){
	m_nIndex = m_pStempManager->GetStemps().size();
}

bool CStemp::End(){
	m_pStempTexture->End();
	m_pStempTexture = nullptr;

	return true;
}

void CStemp::SetShaderState(){
	RENDERER->GetTerrainRenderContainer()->AddVolatileTexture(m_pStempTexture);
	if(GLOBALVALUEMGR->GetToolMode() == TOOL_MODE_TERRAIN || GLOBALVALUEMGR->GetToolMode() == TOOL_MODE_SPLATTING)
		DEBUGER->AddTexture(XMFLOAT2(250, 150), XMFLOAT2(350, 250), m_pStempTexture->GetShaderResourceView());
}

void CStemp::CleanShaderState(){
}

void CStemp::SetStempData(float* pStempData){
	m_pStempData = pStempData;
}

void CStemp::SetStempData(UINT index, float data){

}

void CStemp::IncreaseTerrain(float* pHeightData, TERRAIN_PICPOS_RENDER_INFO* pPicposRenderInfo){
	int terrain_width = m_pStempManager->GetTerrainContainer()->GetTerrainWidth();
	int terrain_length = m_pStempManager->GetTerrainContainer()->GetTerrainLength();

	int x = (1 - pPicposRenderInfo->PickPos.x) * (terrain_width - 1);//height map의 index
	int y = (1 - pPicposRenderInfo->PickPos.y) * (terrain_length - 1);//height map의 index

	int r = (pPicposRenderInfo->Extent * terrain_width);//stemp의 r
	//ZeroMemory(m_pHeightData, sizeof(Pixel) * (m_nWidth - 1) * (m_nLength - 1));
	for (int i = -r; i < r; ++i) {
		for (int j = -r; j < r; ++j) {
			int index = ((x + i) + (y + j)*terrain_length);//height map의 index
			if (index < 0) continue;
			if (index >= (terrain_width * terrain_length)) continue;

			int is = i + r;//0~ 2*r
			int js = j + r;
			int scale = (int)(m_nStempWidth/2) / r;//0-r 을 256size로 변환 시킬 scale
			int stempIndex = (is*scale) + (js*scale) * m_nStempLength;
			
			float heightMapData = (pHeightData[index]);
			if ((heightMapData + m_pStempData[stempIndex] * pPicposRenderInfo->Height) > 1){
				pHeightData[index] = 1;
				continue;
			}
			heightMapData = heightMapData + m_pStempData[stempIndex] * pPicposRenderInfo->Height;
			pHeightData[index] = (heightMapData);
		}
	}
}

void CStemp::DecreaseTerrain(float* pHeightData, TERRAIN_PICPOS_RENDER_INFO* pPicposRenderInfo){
	int terrain_width = m_pStempManager->GetTerrainContainer()->GetTerrainWidth();
	int terrain_length = m_pStempManager->GetTerrainContainer()->GetTerrainLength();

	int x = (1 - pPicposRenderInfo->PickPos.x) * (terrain_width - 1);
	int y = (1 - pPicposRenderInfo->PickPos.y) * (terrain_length - 1);

	int r = (pPicposRenderInfo->Extent * terrain_width);
	//ZeroMemory(m_pHeightData, sizeof(Pixel) * (m_nWidth - 1) * (m_nLength - 1));
	for (int i = -r; i < r; ++i) {
		for (int j = -r; j < r; ++j) {
			int index = ((x + i) + (y + j)*terrain_length);
			if (index < 0) continue;
			if (index >= (terrain_width * terrain_length)) continue;

			int is = i + r;//0~ 2*r
			int js = j + r;
			int scale = (int)(m_nStempWidth / 2) / r;//0-r 을 256size로 변환 시킬 scale
			int stempIndex = (is*scale) + (js*scale) * m_nStempLength;
			
			float heightMapData = (pHeightData[index]);
			if ((heightMapData - m_pStempData[stempIndex] * pPicposRenderInfo->Height) < 0){
				pHeightData[index] = 0;
				continue;
			}
			heightMapData = heightMapData - m_pStempData[stempIndex] * pPicposRenderInfo->Height;
			pHeightData[index] = (heightMapData);
		}
	}
}

void CStemp::IncBrushTerrain(float* pHeightData, TERRAIN_PICPOS_RENDER_INFO* pPicposRenderInfo){
	int terrain_width = m_pStempManager->GetTerrainContainer()->GetTerrainWidth();
	int terrain_length = m_pStempManager->GetTerrainContainer()->GetTerrainLength();


	int x = (1 - pPicposRenderInfo->PickPos.x) * (terrain_width - 1);
	int y = (1 - pPicposRenderInfo->PickPos.y) * (terrain_length - 1);
	int r = (pPicposRenderInfo->Extent * terrain_width);

	for (int i = -r; i < r; ++i) {
		for (int j = -r; j < r; ++j) {
			int index = ((x + i) + (y + j)*terrain_length);
			if (index < 0) continue;
			if (index >= (terrain_width * terrain_length)) continue;

			int is = i + r;//0~ 2*r
			int js = j + r;
			int scale = (int)(m_nStempWidth / 2) / r;//0-r 을 256size로 변환 시킬 scale
			int stempIndex = (is*scale) + (js*scale) * m_nStempLength;
			
			float heightMapData = (pHeightData[index]);
			//현재 heightmap이 나보다 낮으면! 
			if (heightMapData < m_pStempData[stempIndex] * pPicposRenderInfo->Height) {
				//나로 set하고 
				heightMapData = m_pStempData[stempIndex] * pPicposRenderInfo->Height;
				//저장
				pHeightData[index] = (heightMapData);
			}
			//아니면 그대로 둠
		}
	}
}

void CStemp::DecBrushTerrain(float * pHeightData, TERRAIN_PICPOS_RENDER_INFO* pPicposRenderInfo){
	int terrain_width = m_pStempManager->GetTerrainContainer()->GetTerrainWidth();
	int terrain_length = m_pStempManager->GetTerrainContainer()->GetTerrainLength();

	int x = (1 - pPicposRenderInfo->PickPos.x) * (terrain_width - 1);
	int y = (1 - pPicposRenderInfo->PickPos.y) * (terrain_length - 1);
	int r = (pPicposRenderInfo->Extent * terrain_width);

	for (int i = -r; i < r; ++i) {
		for (int j = -r; j < r; ++j) {
			int index = ((x + i) + (y + j)*terrain_length);
			if (index < 0) continue;
			if (index >= (terrain_width * terrain_length)) continue;

			int is = i + r;//0~ 2*r
			int js = j + r;
			int scale = (int)(m_nStempWidth / 2) / r;//0-r 을 256size로 변환 시킬 scale
			int stempIndex = (is*scale) + (js*scale) * m_nStempLength;

			float heightMapData = (pHeightData[index]);
			//현재 heightmap이 나의 -보다 높으면!! 
			if (heightMapData > -m_pStempData[stempIndex] * pPicposRenderInfo->Height) {
				//나로 set하고 
				heightMapData = -m_pStempData[stempIndex] * pPicposRenderInfo->Height;
				//저장
				pHeightData[index] = (heightMapData);
			}
			//아니면 그대로 둠
		}
	}
}

void CStemp::SetTerrain(float* pHeightData, TERRAIN_PICPOS_RENDER_INFO* pPicposRenderInfo){
	int terrain_width = m_pStempManager->GetTerrainContainer()->GetTerrainWidth();
	int terrain_length = m_pStempManager->GetTerrainContainer()->GetTerrainLength();

	int x = (1 - pPicposRenderInfo->PickPos.x) * (terrain_width - 1);
	int y = (1 - pPicposRenderInfo->PickPos.y) * (terrain_length - 1);

	int r = (pPicposRenderInfo->Extent * terrain_width);
	//ZeroMemory(m_pHeightData, sizeof(Pixel) * (m_nWidth - 1) * (m_nLength - 1));
	for (int i = -r; i < r; ++i) {
		for (int j = -r; j < r; ++j) {
			int index = ((x + i) + (y + j)*terrain_length);
			if (index < 0) continue;
			if (index >= (terrain_width * terrain_length)) continue;

			int is = i + r;//0~ 2*r
			int js = j + r;
			int scale = (int)(m_nStempWidth / 2) / r;//0-2r 을 stemp size
			//256size로 변환 시킬 scale
			int stempIndex = (is*scale) + (js*scale) * m_nStempLength;

			pHeightData[index] = (m_pStempData[stempIndex] * pPicposRenderInfo->Height);
		}
	}
}

CStemp * CStemp::CreateStemp(wstring name, CStempManager* pStempManager){
	CStemp* pStemp = new CStemp;

	pStemp->SetStempManager(pStempManager);
	pStemp->SetStempTexture(CTexture::CreateTexture(name, 3, BIND_PS));
	
	int width, length = 0;

	Pixel24* pPixelStempData = IMPORTER->ReadBitmap24(name.c_str(), width, length);
	float* pStempData = new float[width * length];
	for (int i = 0; i < width*length; ++i) {
		float data = pPixelStempData[(width*length-1) - i].r;//0-256
		data /= 256; //0 - 1
		//data *= 1000;//0 - 1000
		//data -= 1000;//-1000 - 1000
		pStempData[i] = data;
	}
	pStemp->SetStempWidth(width);
	pStemp->SetStempLength(length);
	pStemp->SetStempData(pStempData);
	pStemp->Begin();
	return pStemp;
}

CStemp::CStemp() : DXObject("stemp"){
}

CStemp::~CStemp()
{
}
