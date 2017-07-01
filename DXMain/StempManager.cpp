#include "stdafx.h"
#include "StempManager.h"

void CStempManager::Begin(){
	//모든 stemp제작
	vector<wstring> vFile;
	DIRECTORYFINDER->GetFiles(vFile, L"../inputdata/Stemp", true, true, L".bmp");
	DIRECTORYFINDER->GetFiles(vFile, L"../inputdata/Stemp", true, true, L".BMP");
	
	//const char* groupName = "File";
	for (auto data : vFile) {
		CreateStemp(data);
	}
	//모든 stemp제작

	m_pPicposRenderInfoBuffer = CBuffer::CreateConstantBuffer(1, sizeof(TERRAIN_PICPOS_RENDER_INFO), 4, BIND_PS);
	m_pPicposRenderInfo = new TERRAIN_PICPOS_RENDER_INFO;
	m_pPicposRenderInfo->Extent = 50.f / m_fSpaceSize;
	m_pPicposRenderInfo->Height = m_fHeight;

	TWBARMGR->AddMinMaxBarRW("TOOL_MODE", "StempControll", "STEMP_EXTENT", &m_fExtent, 0.1f, 512.f, 0.1f);
	TWBARMGR->AddMinMaxBarRW("TOOL_MODE", "StempControll", "STEMP_HEIGHT", &m_fHeight, 0.01f, 1.f, 0.01f);
	TWBARMGR->AddMinMaxBarRW("TOOL_MODE", "StempControll", "STEMP_INDEX", &m_nCurStemp, 0.f, m_vStemp.size()-1, 1.f);
	
}

bool CStempManager::End(){
	//TWBARMGR->DeleteVar("TOOL_MODE", "STEMP_EXTENT");
	//TWBARMGR->DeleteVar("TOOL_MODE", "STEMP_INDEX");

	if (m_pPicposRenderInfo) delete m_pPicposRenderInfo;
	m_pPicposRenderInfo = nullptr;
	if (m_pPicposRenderInfoBuffer) {
		m_pPicposRenderInfoBuffer->End();
		delete m_pPicposRenderInfoBuffer;
	}
	m_pPicposRenderInfoBuffer = nullptr;

	for (auto pStemp : m_vStemp) {
		pStemp->End();
		delete pStemp;
	}
	m_vStemp.clear();

	return true;
}

void CStempManager::SetShaderState(){
	RENDERER->GetTerrainRenderContainer()->AddVolatileBuffer(m_pPicposRenderInfoBuffer);
	m_vStemp[m_nCurStemp]->SetShaderState();
}

void CStempManager::CleanShaderState(){
}

void CStempManager::UpdateShaderState(){
	TERRAIN_PICPOS_RENDER_INFO* pPicposRenderInfo = (TERRAIN_PICPOS_RENDER_INFO*)m_pPicposRenderInfoBuffer->Map();
	pPicposRenderInfo->PickPos.x = (m_pPicposRenderInfo->PickPos.x);
	pPicposRenderInfo->PickPos.y = 1 - (m_pPicposRenderInfo->PickPos.y);
	pPicposRenderInfo->Extent = m_pPicposRenderInfo->Extent = m_fExtent / m_fSpaceSize;
	pPicposRenderInfo->Height = m_pPicposRenderInfo->Height = m_fHeight;
	pPicposRenderInfo->ToolMode = (UINT)GLOBALVALUEMGR->GetToolMode();
	m_pPicposRenderInfoBuffer->Unmap();
	if (GLOBALVALUEMGR->GetToolMode() == TOOL_MODE_TERRAIN || GLOBALVALUEMGR->GetToolMode() == TOOL_MODE_SPLATTING){
		DEBUGER->AddText(10, 350, 200, YT_Color(255,255,0,255), L"stemp index : f1 + f2 -\n stemp extend : f3 + f4 -");
		if (INPUTMGR->KeyDown(VK_F1_)) {
			if (m_nCurStemp + 1 > m_vStemp.size() - 1) m_nCurStemp = m_vStemp.size() - 1;
			else m_nCurStemp++;
		}
		else if (INPUTMGR->KeyDown(VK_F2_)) {
			if (m_nCurStemp - 1 < 0) m_nCurStemp = 0;
			else m_nCurStemp--;
		}
		else if (INPUTMGR->OnlyKeyDown(VK_F3_)) {
			if (m_fExtent + 1 < 513) m_fExtent+= 0.1;
		}
		else if (INPUTMGR->OnlyKeyDown(VK_F4_)) {
			if (m_fExtent - 1 > 0.1)m_fExtent -= 0.1;
		}
		else if (INPUTMGR->OnlyKeyDown(VK_F7_)) {
			float x = (m_fHeight + 0.001);
			if (x < 1)
				m_fHeight = x;
		}
		else if (INPUTMGR->OnlyKeyDown(VK_F8_)) {
			float x = (m_fHeight - 0.001);
			if (x > 0)
				m_fHeight = x;
		}
	}
}

void CStempManager::IncreaseTerrain(float * pHeightMapData){
	m_vStemp[m_nCurStemp]->IncreaseTerrain(pHeightMapData, m_pPicposRenderInfo);
}

void CStempManager::DecreaseTerrain(float* pHeightMapData){
	m_vStemp[m_nCurStemp]->DecreaseTerrain(pHeightMapData, m_pPicposRenderInfo);
}

void CStempManager::IncBrushTerrain(float * pHeightMapData){
	m_vStemp[m_nCurStemp]->IncBrushTerrain(pHeightMapData, m_pPicposRenderInfo);
}

void CStempManager::DecBrushTerrain(float * pHeightMapData){
	m_vStemp[m_nCurStemp]->DecBrushTerrain(pHeightMapData, m_pPicposRenderInfo);
}

void CStempManager::SetTerrain(float * pHeightMapData){
	m_vStemp[m_nCurStemp]->SetTerrain(pHeightMapData, m_pPicposRenderInfo);
}

void CStempManager::IncreaseTerrainCos(float * pHeightMapData){
	DWORD dwVtxCntX = 512;
	DWORD dwVtxCntZ = 512;
	int iIndex{ 0 };
	int terrain_width = GetTerrainContainer()->GetTerrainWidth();
	int terrain_length = GetTerrainContainer()->GetTerrainLength();


	int centerX = (1 - m_pPicposRenderInfo->PickPos.x) * (terrain_width - 1);//height map의 index
	int centerY = (1 - m_pPicposRenderInfo->PickPos.y) * (terrain_length - 1);//height map의 index

	XMVECTOR xmvCenter = XMVectorSet(centerX, centerY, 0, 0);
	int r = m_fExtent;//stemp의 r
	
	for (int i = -r; i < r; ++i) {
		for (int j = -r; j < r; ++j) {
			int curX = (centerX + i);
			int curY = (centerY + j);
			int index = (curX + curY * terrain_length);//height map의 index
			if (curX > terrain_length - 1)continue;
			if (curY > terrain_length - 1)continue;
			if (index < 0) continue;
			if (index >= (terrain_width * terrain_length)) continue;
		
			XMVECTOR xmvCur = XMVectorSet(curX, curY, 0, 0);
			XMVECTOR xmvDis = xmvCur - xmvCenter;
			XMVECTOR xmvDistance = XMVector2Length(xmvDis);
			float fDistance;
			XMStoreFloat(&fDistance, xmvDistance);

			float fHeightDelta = cosf((fDistance / m_fExtent) * XMConvertToRadians(90.f)) * m_fHeight;

			if (fHeightDelta > 0) {
				if (pHeightMapData[index] + fHeightDelta < 1)
					pHeightMapData[index] += fHeightDelta;
			}
		}
	}

}

void CStempManager::DecreaseTerrainCos(float * pHeightMapData){
	DWORD dwVtxCntX = 512;
	DWORD dwVtxCntZ = 512;
	int iIndex{ 0 };
	int terrain_width = GetTerrainContainer()->GetTerrainWidth();
	int terrain_length = GetTerrainContainer()->GetTerrainLength();


	int centerX = (1 - m_pPicposRenderInfo->PickPos.x) * (terrain_width - 1);//height map의 index
	int centerY = (1 - m_pPicposRenderInfo->PickPos.y) * (terrain_length - 1);//height map의 index

	XMVECTOR xmvCenter = XMVectorSet(centerX, centerY, 0, 0);
	int r = m_fExtent;//stemp의 r

	for (int i = -r; i < r; ++i) {
		for (int j = -r; j < r; ++j) {
			int curX = (centerX + i);
			int curY = (centerY + j);
			int index = (curX + curY * terrain_length);//height map의 index
			if (curX > terrain_length - 1)continue;
			if (curY > terrain_length - 1)continue;
			if (index < 0) continue;
			if (index >= (terrain_width * terrain_length)) continue;

			XMVECTOR xmvCur = XMVectorSet(curX, curY, 0, 0);
			XMVECTOR xmvDis = xmvCur - xmvCenter;
			XMVECTOR xmvDistance = XMVector2Length(xmvDis);
			float fDistance;
			XMStoreFloat(&fDistance, xmvDistance);

			float fHeightDelta = cosf((fDistance / m_fExtent) * XMConvertToRadians(90.f)) * m_fHeight;

			if (fHeightDelta > 0) {
				if (pHeightMapData[index] - fHeightDelta > -1)
					pHeightMapData[index] -= fHeightDelta;
			}
		}
	}
}

void CStempManager::IncBrushTerrainCos(float * pHeightMapData){
	DWORD dwVtxCntX = 512;
	DWORD dwVtxCntZ = 512;
	int iIndex{ 0 };
	int terrain_width = GetTerrainContainer()->GetTerrainWidth();
	int terrain_length = GetTerrainContainer()->GetTerrainLength();


	int centerX = (1 - m_pPicposRenderInfo->PickPos.x) * (terrain_width - 1);//height map의 index
	int centerY = (1 - m_pPicposRenderInfo->PickPos.y) * (terrain_length - 1);//height map의 index

	XMVECTOR xmvCenter = XMVectorSet(centerX, centerY, 0, 0);
	int r = m_fExtent;//stemp의 r

	for (int i = -r; i < r; ++i) {
		for (int j = -r; j < r; ++j) {
			int curX = (centerX + i);
			int curY = (centerY + j);
			int index = (curX + curY * terrain_length);//height map의 index
			if (curX > terrain_length - 1)continue;
			if (curY > terrain_length - 1)continue;
			if (index < 0) continue;
			if (index >= (terrain_width * terrain_length)) continue;

			XMVECTOR xmvCur = XMVectorSet(curX, curY, 0, 0);
			XMVECTOR xmvDis = xmvCur - xmvCenter;
			XMVECTOR xmvDistance = XMVector2Length(xmvDis);
			float fDistance;
			XMStoreFloat(&fDistance, xmvDistance);

			float fHeightDelta = cosf((fDistance / m_fExtent) * XMConvertToRadians(90.f)) * m_fHeight;

			if (fHeightDelta > 0) {
				if (pHeightMapData[index] < fHeightDelta)
					pHeightMapData[index] = fHeightDelta;
			}
		}
	}
}

void CStempManager::DecBrushTerrainCos(float * pHeightMapData){
	DWORD dwVtxCntX = 512;
	DWORD dwVtxCntZ = 512;
	int iIndex{ 0 };
	int terrain_width = GetTerrainContainer()->GetTerrainWidth();
	int terrain_length = GetTerrainContainer()->GetTerrainLength();


	int centerX = (1 - m_pPicposRenderInfo->PickPos.x) * (terrain_width - 1);//height map의 index
	int centerY = (1 - m_pPicposRenderInfo->PickPos.y) * (terrain_length - 1);//height map의 index

	XMVECTOR xmvCenter = XMVectorSet(centerX, centerY, 0, 0);
	int r = m_fExtent;//stemp의 r

	for (int i = -r; i < r; ++i) {
		for (int j = -r; j < r; ++j) {
			int curX = (centerX + i);
			int curY = (centerY + j);
			int index = (curX + curY * terrain_length);//height map의 index
			if (curX > terrain_length - 1)continue;
			if (curY > terrain_length - 1)continue;
			if (index < 0) continue;
			if (index >= (terrain_width * terrain_length)) continue;

			XMVECTOR xmvCur = XMVectorSet(curX, curY, 0, 0);
			XMVECTOR xmvDis = xmvCur - xmvCenter;
			XMVECTOR xmvDistance = XMVector2Length(xmvDis);
			float fDistance;
			XMStoreFloat(&fDistance, xmvDistance);

			float fHeightDelta = cosf((fDistance / m_fExtent) * XMConvertToRadians(90.f)) * m_fHeight;

			if (fHeightDelta > 0) {
				if (pHeightMapData[index] > -fHeightDelta)
					pHeightMapData[index] = -fHeightDelta;
			}
		}
	}
}

void CStempManager::SetTerrainCos(float * pHeightMapData){
	DWORD dwVtxCntX = 512;
	DWORD dwVtxCntZ = 512;
	int iIndex{ 0 };
	int terrain_width = GetTerrainContainer()->GetTerrainWidth();
	int terrain_length = GetTerrainContainer()->GetTerrainLength();


	int centerX = (1 - m_pPicposRenderInfo->PickPos.x) * (terrain_width - 1);//height map의 index
	int centerY = (1 - m_pPicposRenderInfo->PickPos.y) * (terrain_length - 1);//height map의 index

	XMVECTOR xmvCenter = XMVectorSet(centerX, centerY, 0, 0);
	int r = m_fExtent;//stemp의 r

	for (int i = -r; i < r; ++i) {
		for (int j = -r; j < r; ++j) {
			int curX = (centerX + i);
			int curY = (centerY + j);
			int index = (curX + curY * terrain_length);//height map의 index
			if (curX > terrain_length - 1)continue;
			if (curY > terrain_length - 1)continue;
			if (index < 0) continue;
			if (index >= (terrain_width * terrain_length)) continue;

			XMVECTOR xmvCur = XMVectorSet(curX, curY, 0, 0);
			XMVECTOR xmvDis = xmvCur - xmvCenter;
			XMVECTOR xmvDistance = XMVector2Length(xmvDis);
			float fDistance;
			XMStoreFloat(&fDistance, xmvDistance);

			float fHeightDelta = cosf((fDistance / m_fExtent) * XMConvertToRadians(90.f)) * m_fHeight;

			if (fHeightDelta > 0) {
				pHeightMapData[index] = fHeightDelta;
			}
		}
	}
}

void CStempManager::SetCurStempIndex(int index){
	if (index >= m_vStemp.size()) return;
	m_nCurStemp = index;
}

void CStempManager::CreateStemp(wstring name){
	CStemp* pStemp = CStemp::CreateStemp(name, this);
	m_vStemp.push_back(pStemp);
}

CStempManager * CStempManager::CreateStempManager(float fSpaceSize, CTerrainContainer* pTerrainContainer){
	CStempManager* pStempManager = new CStempManager;
	pStempManager->SetSpaceSize(fSpaceSize);
	pStempManager->SetTerrainContainer(pTerrainContainer);
	pStempManager->Begin();
	return pStempManager;
}

CStempManager::CStempManager() : DXObject("stempmanager"){

}

CStempManager::~CStempManager(){

}
