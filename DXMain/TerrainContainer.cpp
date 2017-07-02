#include "stdafx.h"
#include "TerrainContainer.h"
#include "SpaceContainer.h"

void TW_CALL MCTerrainOnOffGetCallback(void * value, void * clientData) {
	if (nullptr == clientData) return;
	CTerrainContainer* pTerrainContainer = reinterpret_cast<CTerrainContainer*>(clientData);
	*static_cast<bool *>(value) = pTerrainContainer->GetActive();
}
void TW_CALL MCTerrainOnOffSetCallback(const void * value, void * clientData) {
	if (nullptr == clientData) return;
	CTerrainContainer* pTerrainContainer = reinterpret_cast<CTerrainContainer*>(clientData);
	pTerrainContainer->SetActive(*static_cast<const bool*>(value));
}
//void TW_CALL MCCreateSplattingButtonCallback(void * clientData) {
//	CTerrainContainer* pData = (CTerrainContainer*)clientData;
//	pData->CreateSplattingInfo();
//	pData->GetSplattingInfoManager()->CreateSplattingListUI();
//}
//
//void TW_CALL SMSetButtonCallback(void * clientData) {
//	CTerrainContainer* pData = (CTerrainContainer*)clientData;
//	pData->SetStempMode(STEMP_MODE_SET);
//}
//void TW_CALL SMIndeButtonCallback(void * clientData) {
//	CTerrainContainer* pData = (CTerrainContainer*)clientData;
//	pData->SetStempMode(STEMP_MODE_INDE);
//}
void TW_CALL SetTerrainOnOff(void* cliendData) {

}
#define TEXTURE_SIZE 257

void CTerrainContainer::Begin() {
	TWBARMGR->AddBoolBarCB("TOOL_MODE", "SceneObject", "TerrainOn/Off", MCTerrainOnOffSetCallback, MCTerrainOnOffGetCallback, this);
	
	//TWBARMGR->DeleteBar("TerrainController");
	//TWBARMGR->AddBar("TerrainController");
	//TWBARMGR->AddButtonCB("TerrainController", "STEMP_MODEVIEW", "SetStemp", SMSetButtonCallback, this);
	//TWBARMGR->AddButtonCB("TerrainController", "STEMP_MODEVIEW", "IndeStemp", SMIndeButtonCallback, this);
	//TWBARMGR->AddMinMaxBarCB("TerrainController", "MainControll", "TerrainHeightScale", MCTerrainHeightScaleSetCallback, MCTerrainHeightScaleGetCallback,
	//	this, 0.1f, 10.f, 0.001f);
	//TWBARMGR->AddButtonCB("TerrainController", "MainControll", "CreateSplatting", MCCreateSplattingButtonCallback, this);

	
	//terrain mesh create
	CreateTerrainMesh(m_pSpaceContainer);
	//terrain mesh create

	m_pSplattingInfoManager = CSplattingInfoManager::CreateSplattingInfoManager(this);
	//m_pSplattingInfoManager->CreateSplattingInfo(L"../slide.bmp", L"../../Assets/Detail_Texture_9.jpg");

	//모든 stemp제작
	m_pStempManager = CStempManager::CreateStempManager(m_pSpaceContainer->GetSpaceSize(), this);

	//global buffer 제작
	m_pGlobalTerrainBuffer = RESOURCEMGR->GetBuffer("TerrainCB");
	
	//global buf set
	m_pGlobalTerrainData = new TERRAIN_GLOBAL_VALUE;
	float fOneSpaceSize = (float)m_pSpaceContainer->GetOneSpaceSize();
	float fOneSideSpaceNum = (float)m_pSpaceContainer->GetOneSideSpaceNum();
	m_pGlobalTerrainData->OneSideSpaceNum = fOneSideSpaceNum;
	m_pGlobalTerrainData->OneSideSpaceNumRcp = 1.0f / fOneSideSpaceNum;
	m_pGlobalTerrainData->OneSpaceSizeRcp = 1.0f / fOneSpaceSize;
	m_pGlobalTerrainData->HeightScale = m_xmf3Scale.y;
	//global buf set

	//터레인 제작
	//te`rrain
	CTerrain* pTerrain = nullptr;
	for (int j = 0; j < m_pSpaceContainer->GetOneSideSpaceNum(); ++j) {
		for (int i = 0; i < m_pSpaceContainer->GetOneSideSpaceNum(); ++i) {
			pTerrain = CTerrain::CreateTerrain(this, i, j);
			m_vpTerrain.push_back(pTerrain);
		}
	}
	//terrain

	
	//RS
	D3D11_RASTERIZER_DESC descRasterizer;
	ZeroMemory(&descRasterizer, sizeof(D3D11_RASTERIZER_DESC));
	//descRasterizer.FillMode = D3D11_FILL_SOLID;
	descRasterizer.FillMode = D3D11_FILL_WIREFRAME;
	descRasterizer.CullMode = D3D11_CULL_NONE;
	GLOBALVALUEMGR->GetDevice()->CreateRasterizerState(&descRasterizer, &m_pd3dSpaceRSState);
	//RS

	//추가
	m_pTessFacterBuffer = RESOURCEMGR->CreateConstantBuffer("TerrainTessFacter", 1, sizeof(TERRAIN_TESS_FACTER), 1, BIND_HS);
	m_pTessFacterData = new TERRAIN_TESS_FACTER;

	TWBARMGR->AddMinMaxBarRW("TOOL_MODE", "TerrainControl", "zn", &m_pTessFacterData->zn, 0, 1000, 1);
	TWBARMGR->AddMinMaxBarRW("TOOL_MODE", "TerrainControl", "zf", &m_pTessFacterData->zf, 0, 1000, 1);
	TWBARMGR->AddMinMaxBarRW("TOOL_MODE", "TerrainControl", "max_facter", &m_pTessFacterData->max_facter_value, 0, 100, 1);
	TWBARMGR->AddMinMaxBarRW("TOOL_MODE", "TerrainControl", "min_facter", &m_pTessFacterData->min_facter_value, 0, 20, 1);

}

bool CTerrainContainer::End() {
	//TWBARMGR->DeleteBar("TerrainController");
	//TWBARMGR->DeleteVar("TOOL_MODE", "SetStemp");
	//TWBARMGR->DeleteVar("TOOL_MODE", "IndeStemp");
	//TWBARMGR->DeleteVar("TOOL_MODE", "TerrainHeightScale");

	if (m_pHeightData) delete[] m_pHeightData;
	m_pHeightData = nullptr;
	if (m_pNormalData) delete[] m_pNormalData;
	m_pNormalData = nullptr;
	if (m_pBaseTexture) {
		m_pBaseTexture->End();
		delete m_pBaseTexture;
	}
	m_pBaseTexture = nullptr;
	if (m_pHeightMapTexture) {
		m_pHeightMapTexture->End();
		delete m_pHeightMapTexture;
	}
	m_pHeightMapTexture = nullptr;
	if (m_pNormalTexture) {
		m_pNormalTexture->End();
		delete m_pNormalTexture;
	}
	m_pNormalTexture = nullptr;
	
	if (m_pGlobalTerrainBuffer) m_pGlobalTerrainBuffer = nullptr;
	if (m_pGlobalTerrainData) delete m_pGlobalTerrainData;
	m_pGlobalTerrainData = nullptr;
	if (m_pStempManager) {
		m_pStempManager->End();
		delete m_pStempManager;
		m_pStempManager = nullptr;
	}

	if (m_pSplattingInfoManager) {
		m_pSplattingInfoManager->End();
		delete m_pSplattingInfoManager;
		m_pSplattingInfoManager = nullptr;
	}

	//terrain object remove
	for (auto pTerrain : m_vpTerrain) {
		m_pSpaceContainer->RemoveObject(pTerrain);
		delete pTerrain;
	}
	m_vpTerrain.clear();

	if(m_pd3dSpaceRSState)m_pd3dSpaceRSState->Release();
	if(m_pd3dTempRSState)m_pd3dTempRSState->Release();

	//추가
	m_pTessFacterBuffer = nullptr;

	if (m_pTessFacterData) delete m_pTessFacterData;
	m_pTessFacterData = nullptr;
	return true;
}

void CTerrainContainer::PrepareRender(){
	if (m_bActive) {

		RENDERER->GetTerrainRenderContainer()->ClearVolatileResources();

		if (GLOBALVALUEMGR->GetToolMode() == TOOL_MODE_TERRAIN && (INPUTMGR->MouseLeftDown() || INPUTMGR->MouseRightDown())) {
			ReadyHeightMap();
		}
		SetBufferInfo();
		RENDERER->GetTerrainRenderContainer()->AddVolatileTexture(m_pBaseTexture);
		
		RENDERER->GetTerrainRenderContainer()->AddVolatileTexture(m_pHeightMapTexture);

		RENDERER->GetTerrainRenderContainer()->AddVolatileTexture(m_pNormalTexture);
		RENDERER->GetTerrainRenderContainer()->AddVolatileBuffer(m_pGlobalTerrainBuffer);
		//추가
		RENDERER->GetTerrainRenderContainer()->AddVolatileBuffer(m_pTessFacterBuffer);

		m_pStempManager->SetShaderState();
		m_pSplattingInfoManager->SetShaderState();//splatting
	}
	//m_pTerrainRenderContainer->ClearObjectList();
}

float CTerrainContainer::GetHeight(XMVECTOR xmvPos, int index){
	XMFLOAT3 xmf3Pos;
	XMStoreFloat3(&xmf3Pos, xmvPos);
	float fx = xmf3Pos.x;
	float fz = xmf3Pos.z;

	return GetHeight(XMFLOAT2(fx, fz), index);
}

float CTerrainContainer::GetHeight(XMFLOAT2 xmf2Pos, int index){
	if (index != -1) {
		return CNaviObjectManager::GetHeight(xmf2Pos, index);
	}
	float fx = m_pSpaceContainer->GetSpaceSize() - xmf2Pos.x;
	float fz = m_pSpaceContainer->GetSpaceSize() - xmf2Pos.y;

	fx = fx / m_xmf3Scale.x;
	fz = fz / m_xmf3Scale.z;
	if ((fx < 0.0f) || (fz < 0.0f) || (fx >= m_nWidth) || (fz >= m_nLength)) return(0.0f);

	int x = (int)fx;
	int z = (int)fz;
	float fxPercent = fx - x;
	float fzPercent = fz - z;

	//float fBottomLeft = (float)m_pHeightMapImage[x + (z*m_nWidth)];
	//float fBottomRight = (float)m_pHeightMapImage[(x + 1) + (z*m_nWidth)];
	//float fTopLeft = (float)m_pHeightMapImage[x + ((z + 1)*m_nWidth)];
	//float fTopRight = (float)m_pHeightMapImage[(x + 1) + ((z + 1)*m_nWidth)];
	float fBottomLeft = (float)m_pHeightData[x + (z*m_nWidth)];
	float fBottomRight = (float)m_pHeightData[(x + 1) + (z*m_nWidth)];
	float fTopLeft = (float)m_pHeightData[x + ((z + 1)*m_nWidth)];
	float fTopRight = (float)m_pHeightData[(x + 1) + ((z + 1)*m_nWidth)];

	if (fzPercent < (1.0f - fxPercent))
		fTopRight = fTopLeft + (fBottomRight - fBottomLeft);
	else
		fBottomLeft = fTopLeft + (fBottomRight - fTopRight);

#ifdef _WITH_APPROXIMATE_OPPOSITE_CORNER
	if (bReverseQuad)
	{
		if (fzPercent >= fxPercent)
			fBottomRight = fBottomLeft + (fTopRight - fTopLeft);
		else
			fTopLeft = fTopRight + (fBottomLeft - fBottomRight);
	}
	else
	{
		if (fzPercent < (1.0f - fxPercent))
			fTopRight = fTopLeft + (fBottomRight - fBottomLeft);
		else
			fBottomLeft = fTopLeft + (fBottomRight - fTopRight);
	}
#endif
	float fTopHeight = fTopLeft * (1 - fxPercent) + fTopRight * fxPercent;
	float fBottomHeight = fBottomLeft * (1 - fxPercent) + fBottomRight * fxPercent;
	float fHeight = fBottomHeight * (1 - fzPercent) + fTopHeight * fzPercent;

	return(fHeight * m_xmf3Scale.y);
}

void CTerrainContainer::SetPicpos(float x, float y){
	m_xmf2CurPickPos.x = x;
	m_xmf2CurPickPos.y = y;
	m_pStempManager->SetPickPos(XMFLOAT2(x / m_pSpaceContainer->GetSpaceSize(), y / m_pSpaceContainer->GetSpaceSize()));
}

void CTerrainContainer::SetRenderRadius(float r){
	m_pStempManager->SetExtent(r / m_pSpaceContainer->GetSpaceSize());
}

void CTerrainContainer::Update( CCamera* pCamera) {
	//추가
	XMStoreFloat3(&m_pTessFacterData->camera_pos, pCamera->GetPosition());

	if (!pCamera) return;
	if (m_bActive) {
		POINT p = INPUTMGR->GetMousePoint();

		//Get screen pos -> Camera pos
		XMFLOAT4X4 xmf4x4Projection;
		XMStoreFloat4x4(&xmf4x4Projection, pCamera->GetProjectionMtx());
		D3D11_VIEWPORT d3dViewport = pCamera->GetViewport();

		//음 이건 화면을 찍은 점의 카메라 좌표계의 녀석이고
		XMFLOAT3 xmf3PickPosition;
		xmf3PickPosition.x = (((2.0f * p.x) / d3dViewport.Width) - 1) / xmf4x4Projection._11;
		xmf3PickPosition.y = -(((2.0f * p.y) / d3dViewport.Height) - 1) / xmf4x4Projection._22;
		xmf3PickPosition.z = 1.0f;

		XMVECTOR xmvPickPosition;
		xmvPickPosition = XMLoadFloat3(&xmf3PickPosition);
		XMMATRIX xmMtxViewInverse;
		xmMtxViewInverse = XMMatrixInverse(nullptr, pCamera->GetViewMtx());
		//picking pos에 camera inverse를 곱했으니 이건 picking pos의 world pos!
		xmvPickPosition = XMVector3TransformCoord(xmvPickPosition, xmMtxViewInverse);
		XMVECTOR xmvRayDir = xmvPickPosition - pCamera->GetPosition();

		CGameObject* pNearestObject = NULL;
		float fHitDistance = FLT_MAX;
		float fNearDistance = FLT_MAX;
		pNearestObject = PickObjects(pCamera->GetPosition(), XMVector3Normalize(xmvRayDir), fHitDistance);
		fNearDistance = fHitDistance;

		int mode = m_StempMode;
		if (mode == STEMP_MODE_SET) {
			if (INPUTMGR->MouseLeftDown() || INPUTMGR->MouseRightDown()) {
				SetPickPosHeight();
			}
		}
		else if (mode == STEMP_MODE_INDE) {
			if (INPUTMGR->MouseLeftDown()) {
				IncreasePickPosHeight();
			}
			else if (INPUTMGR->MouseRightDown()) {
				DecreasePickPosHeight();
			}
		}
		else if (mode == STEMP_MODE_BRUSH) {
			if (INPUTMGR->MouseLeftDown()) {
				IncBrushPickPosHeight();
			}
			else if (INPUTMGR->MouseRightDown()) {
				DecBrushPickPosHeight();
			}
		}

		m_pStempManager->UpdateShaderState();
		m_pSplattingInfoManager->UpdateShaderState();

		//test
		//DEBUGER->AddTexture(XMFLOAT2(500, 150), XMFLOAT2(750, 300), m_pNormalTexture->GetShaderResourceView());
		//DEBUGER->AddTexture(XMFLOAT2(500, 150), XMFLOAT2(750, 300), m_pNormalTexture->GetShaderResourceView());
		if (INPUTMGR->KeyDown(VK_R)) {
			CreateNormalMap();
		}
		
		//registe to renderer
		//RENDERER->SetTerrainContainer(this);
		PrepareRender();

		return;
	}
	
	//registe to renderer
	//RENDERER->SetTerrainContainer(nullptr);
	
}

CGameObject * CTerrainContainer::PickObjects(XMVECTOR xmvWorldCameraStartPos, XMVECTOR xmvRayDir, float & distance){
	float fHitDistance = FLT_MAX;
	distance = fHitDistance;
	float fNearHitDistance = FLT_MAX;
	CGameObject* pObj = nullptr;
	//자신의 모든 객체에 대해서 검사
	for (auto pObject : m_vpTerrain) {
		if (pObject->CheckPickObject(xmvWorldCameraStartPos, xmvRayDir, fHitDistance)) {//ray와 충돌했다면
			if (fNearHitDistance > fHitDistance) {//이전의 가장 가까운 녀석과 비교
				distance = fHitDistance;//더 가까우면 가장 가까운 객체 변경
				return pObject;
			}

		}
	}
	//return pObj;
}

//터레인 추가분
void CTerrainContainer::ReadyHeightMap(){
	if (m_pHeightMapTexture) {
		m_pHeightMapTexture->End();
		delete m_pHeightMapTexture;
	}
	m_pHeightMapTexture = nullptr;

	//height map data init
	WCHAR path[256];

	ID3D11Buffer* pd3dBuffer;
	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.StructureByteStride = sizeof(float);
	d3dBufferDesc.ByteWidth = 512 * 512 * sizeof(float);
	d3dBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	D3D11_SUBRESOURCE_DATA Data;
	ZeroMemory(&Data, sizeof(D3D11_SUBRESOURCE_DATA));
	Data.pSysMem = m_pHeightData;

	GLOBALVALUEMGR->GetDevice()->CreateBuffer(&d3dBufferDesc, &Data, &pd3dBuffer);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = 512 * 512;
	ID3D11ShaderResourceView *pSRV;
	GLOBALVALUEMGR->GetDevice()->CreateShaderResourceView(pd3dBuffer, &srvDesc, &pSRV);
	pd3dBuffer->Release();
	//wsprintf(path, L"../%sHeightMap.bmp", m_wsSceneName.c_str());//name
	//EXPORTER->MakeBitmap24(path, m_pHeightData, m_nWidth, m_nLength);

	m_pHeightMapTexture = CTexture::CreateTexture(pSRV, 1, BIND_DS);
}
//터레인 추가분

void CTerrainContainer::SetBufferInfo(){
	TERRAIN_GLOBAL_VALUE* pData = (TERRAIN_GLOBAL_VALUE*)m_pGlobalTerrainBuffer->Map();
	//pData = m_pGlobalTerrainData;
	pData->OneSideSpaceNum = m_pGlobalTerrainData->OneSideSpaceNum;
	pData->OneSideSpaceNumRcp = m_pGlobalTerrainData->OneSideSpaceNumRcp;
	pData->OneSpaceSizeRcp = m_pGlobalTerrainData->OneSpaceSizeRcp;
	pData->HeightScale = m_pGlobalTerrainData->HeightScale;
	m_pGlobalTerrainBuffer->Unmap();

	//추가
	SetTerrainTessFacter(m_pTessFacterData->zn, m_pTessFacterData->zf, m_pTessFacterData->space_lavel_pow, m_pTessFacterData->max_facter_value, m_pTessFacterData->min_facter_value, m_pTessFacterData->camera_pos);
}

void CTerrainContainer::IncreasePickPosHeight(){
	if (GLOBALVALUEMGR->GetToolMode() == TOOL_MODE_TERRAIN) {
		m_pStempManager->IncreaseTerrainCos(m_pHeightData);
	}
	else if (GLOBALVALUEMGR->GetToolMode() == TOOL_MODE_SPLATTING) {
		if (m_pSplattingInfoManager->GetSplattingInfos().empty()) return;
		m_pStempManager->IncreaseTerrain(m_pSplattingInfoManager->GetCurSplattingInfo()->GetBlendInfo());
	}
}

void CTerrainContainer::DecreasePickPosHeight(){
	if (GLOBALVALUEMGR->GetToolMode() == TOOL_MODE_TERRAIN) {
		m_pStempManager->DecreaseTerrainCos(m_pHeightData);
	}
	else if (GLOBALVALUEMGR->GetToolMode() == TOOL_MODE_SPLATTING) {
		if (m_pSplattingInfoManager->GetSplattingInfos().empty()) return;
		m_pStempManager->DecreaseTerrain(m_pSplattingInfoManager->GetCurSplattingInfo()->GetBlendInfo());
	}
}

void CTerrainContainer::IncBrushPickPosHeight(){
	if (GLOBALVALUEMGR->GetToolMode() == TOOL_MODE_TERRAIN) {
		m_pStempManager->IncBrushTerrainCos(m_pHeightData);
	}
	else if (GLOBALVALUEMGR->GetToolMode() == TOOL_MODE_SPLATTING) {
		if (m_pSplattingInfoManager->GetSplattingInfos().empty()) return;
		m_pStempManager->IncBrushTerrain(m_pSplattingInfoManager->GetCurSplattingInfo()->GetBlendInfo());
	}
}

void CTerrainContainer::DecBrushPickPosHeight(){
	if (GLOBALVALUEMGR->GetToolMode() == TOOL_MODE_TERRAIN) {
		m_pStempManager->DecBrushTerrainCos(m_pHeightData);
	}
	else if (GLOBALVALUEMGR->GetToolMode() == TOOL_MODE_SPLATTING) {
		if (m_pSplattingInfoManager->GetSplattingInfos().empty()) return;
		m_pStempManager->DecBrushTerrain(m_pSplattingInfoManager->GetCurSplattingInfo()->GetBlendInfo());
	}
}

void CTerrainContainer::SetPickPosHeight(){
	if (GLOBALVALUEMGR->GetToolMode() == TOOL_MODE_TERRAIN) {
		m_pStempManager->SetTerrainCos(m_pHeightData);
	}
	else if (GLOBALVALUEMGR->GetToolMode() == TOOL_MODE_SPLATTING) {
		if (m_pSplattingInfoManager->GetSplattingInfos().empty()) return;
		m_pStempManager->SetTerrain(m_pSplattingInfoManager->GetCurSplattingInfo()->GetBlendInfo());
	}
}

void CTerrainContainer::SetBaseTexture(wstring path){
	if (m_pBaseTexture)m_pBaseTexture->End();
	delete m_pBaseTexture;
	m_pBaseTexture = nullptr;

	m_pBaseTexture = CTexture::CreateTexture(path, 0);
}

void CTerrainContainer::SetHeightMapTexture(wstring path){
	if (m_pHeightData) delete[]m_pHeightData;
	m_pHeightData = nullptr;
	if (m_pHeightMapTexture)m_pHeightMapTexture->End();
	m_pHeightMapTexture = nullptr;


}

void CTerrainContainer::CreateSplattingInfo(){
	//m_pSplattingInfoManager->CreateSplattingInfo(L"../outputdata/SplattingBlendInfo/BlendInfo0.bmp", L"../../Assets/Detail_Texture_9.jpg");
	m_pSplattingInfoManager->CreateSplattingInfo(L"../../Assets/Detail_Texture/Detail_Texture_Default.jpg");
	//m_pSplattingInfoManager->CreateSplattingInfo(L"../outputdata/SplattingBlendInfo/BlendInfo0.bmp", L"../../Assets/default.jpg");
}

void CTerrainContainer::CreateNormalMap(){
	//1. bitmap 좌표계의 height다.
	//2. 그래서 world 좌표계 즉 x z가 반대로 되는 세계로 불러들인다.
	//3. height는 그대로이다. 높이 값은 bitmap 세계에 있으니까 그냥 읽어도 됨
	//4. normal을 계산한다.
	//끝
	if (m_pNormalTexture) {
		m_pNormalTexture->End();
		delete m_pNormalTexture;
	}
	m_pNormalTexture = nullptr;

	//calculate normal
	XMVECTOR xmvSumNormal = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	float fScaleX = m_xmf3Scale.x;
	float fScaleZ = m_xmf3Scale.z;
	float fScaleY = m_xmf3Scale.y;
	UINT nNormal{ 0 };
	for (int i = 0; i < m_nWidth; ++i) {
		for (int j = 0; j < m_nLength; ++j) {
			xmvSumNormal = XMVectorSet(0.f, 0.f, 0.f, 0.f);
			int xIndex = i;
			int yIndex = j;
			if ((xIndex - 1) > 0 && (yIndex + 1) < (m_nLength - 1)) {
				XMVECTOR p0 = XMVectorSet(0, m_pHeightData[(xIndex)+(m_nLength * yIndex)] * m_xmf3Scale.y, 0, 0.f);
				XMVECTOR p1 = XMVectorSet(fScaleX, m_pHeightData[(xIndex - 1) + (m_nLength * yIndex)]* m_xmf3Scale.y, 0, 0.f);
				XMVECTOR p2 = XMVectorSet(0, m_pHeightData[(xIndex)+(m_nLength * (yIndex + 1))]* m_xmf3Scale.y, -fScaleZ, 0.f);
				XMVECTOR edge1 = p1 - p0;
				XMVECTOR edge2 = p2 - p0;
				xmvSumNormal += XMVector3Normalize(XMVector3Cross(edge1, edge2));
				nNormal++;
			}
			if ((xIndex + 1) < (m_nWidth - 1) && (yIndex + 1) < (m_nLength - 1)) {
				XMVECTOR p0 = XMVectorSet(0, m_pHeightData[(xIndex)+(m_nLength * yIndex)]* m_xmf3Scale.y, 0, 0.f);
				XMVECTOR p1 = XMVectorSet(0, m_pHeightData[(xIndex)+(m_nLength * (yIndex + 1))]* m_xmf3Scale.y, -fScaleZ, 0.f);
				XMVECTOR p2 = XMVectorSet(-fScaleX, m_pHeightData[(xIndex + 1) + (m_nLength * yIndex)]* m_xmf3Scale.y, 0, 0.f);
				XMVECTOR edge1 = p1 - p0;
				XMVECTOR edge2 = p2 - p0;
				xmvSumNormal += XMVector3Normalize(XMVector3Cross(edge1, edge2));
				nNormal++;
			}
			if (0 < (xIndex - 1) && 0 < (yIndex - 1)) {
				XMVECTOR p0 = XMVectorSet(0, m_pHeightData[(xIndex)+(m_nLength * yIndex)] * m_xmf3Scale.y, 0, 0.f);
				XMVECTOR p1 = XMVectorSet(0, m_pHeightData[(xIndex)+(m_nLength * (yIndex - 1))] * m_xmf3Scale.y, fScaleZ, 0.f);
				XMVECTOR p2 = XMVectorSet(fScaleX, m_pHeightData[(xIndex - 1) + (m_nLength * yIndex)] * m_xmf3Scale.y, 0, 0.f);
				XMVECTOR edge1 = p1 - p0;
				XMVECTOR edge2 = p2 - p0;
				xmvSumNormal += XMVector3Normalize(XMVector3Cross(edge1, edge2));
				nNormal++;
			}
			if ((xIndex + 1) < (m_nWidth - 1) && 0 < (yIndex - 1)) {
				XMVECTOR p0 = XMVectorSet(0, m_pHeightData[(xIndex)+(m_nLength * yIndex)]* m_xmf3Scale.y, 0, 0.f);
				XMVECTOR p1 = XMVectorSet(-fScaleX, m_pHeightData[(xIndex + 1) + (m_nLength * yIndex)]* m_xmf3Scale.y, 0, 0.f);
				XMVECTOR p2 = XMVectorSet(0, m_pHeightData[(xIndex)+(m_nLength * (yIndex - 1))]* m_xmf3Scale.y, fScaleZ, 0.f);
				XMVECTOR edge1 = p1 - p0;
				XMVECTOR edge2 = p2 - p0;
				xmvSumNormal += XMVector3Normalize(XMVector3Cross(edge1, edge2));
				nNormal++;
			}
			XMVECTOR xmvAvgNormal = XMVector3Normalize(xmvSumNormal / nNormal);//-1 1
			xmvAvgNormal += XMVectorSet(1.f, 1.f, 1.f, 0.f);//0 - 2
			xmvAvgNormal /= 2.f;//0-1

			xmvAvgNormal *= 255;//0-255
			XMFLOAT3 xmf3AvgNormal;
			XMStoreFloat3(&xmf3AvgNormal, xmvAvgNormal);
			m_pNormalData[xIndex + (yIndex * m_nLength)].r = xmf3AvgNormal.x;
			m_pNormalData[xIndex + (yIndex * m_nLength)].g = xmf3AvgNormal.y;
			m_pNormalData[xIndex + (yIndex * m_nLength)].b = xmf3AvgNormal.z;
		}
	}
	//height map data init
	WCHAR path[256];
	wsprintf(path, L"../%sNormalMap.bmp", m_wsSceneName.c_str());//name
	EXPORTER->MakeBitmap24(path, m_pNormalData, m_nWidth, m_nLength);
	m_pNormalTexture = CTexture::CreateTexture(path, PS_SLOT_NORMALMAP, BIND_PS);
	
}

CTerrainContainer * CTerrainContainer::CreateTerrainContainer(LPCTSTR pTerrainName, int nWidth, int nLength, float fHeightScale, CSpaceContainer * pSpaceContainer, bool isTool){
	CTerrainContainer* pTerrainContainer = new CTerrainContainer;

	pTerrainContainer->SetSpaceContainer(pSpaceContainer);
	pTerrainContainer->SetTerrainWidth(nWidth);
	pTerrainContainer->SetTerrainLength(nLength);
	pTerrainContainer->SetTerrainScale(XMFLOAT3(static_cast<float>(pSpaceContainer->GetSize() / (nWidth)),
		fHeightScale, static_cast<float>(pSpaceContainer->GetSize() / (nLength))));
	pTerrainContainer->SetSceneName(pTerrainName);
	pTerrainContainer->SetIsTool(isTool);
	
	//terrain texture create
	wstring SceneName{ pTerrainName };
	string name{ "" }; name.assign(SceneName.begin(), SceneName.end());
	if (isTool) {
		//make texture and use
		pTerrainContainer->CreateResetTextures(pTerrainContainer->GetSceneName().c_str());
		RESOURCEMGR->CreateInstancingBuffer(name, 256, sizeof(VS_VB_INSTANCE));
	}
	else {
		//just read texture
		pTerrainContainer->CreateTerrainTextures(pTerrainContainer->GetSceneName().c_str());
		RESOURCEMGR->CreateInstancingBuffer(name, pSpaceContainer->GetSpaceNum(), sizeof(VS_VB_INSTANCE));
	}
	//terrain texture create


	pTerrainContainer->Begin();
	return pTerrainContainer;
}
CTerrainContainer * CTerrainContainer::CreateTerrainContainer(wstring wsOutputPath, wstring wsSceneName, int nWidth, int nLength, CSpaceContainer * pSpaceContainer, bool isTool){
	CTerrainContainer* pTerrainContainer = new CTerrainContainer;
	pTerrainContainer->SetSpaceContainer(pSpaceContainer);
	pTerrainContainer->SetTerrainWidth(256);
	pTerrainContainer->SetTerrainLength(256);
	pTerrainContainer->SetSceneName(wsSceneName);
	pTerrainContainer->SetIsTool(isTool);

	//base texture path
	XMFLOAT3 xmf3Scale;
	xmf3Scale.x = IMPORTER->ReadFloat();
	xmf3Scale.y = IMPORTER->ReadFloat();
	xmf3Scale.z = IMPORTER->ReadFloat();
	pTerrainContainer->SetTerrainScale(xmf3Scale);

	wstring wsBaseTexturePath = IMPORTER->ReadWstring();
	pTerrainContainer->SetBaseTexture(wsBaseTexturePath);
	int width, height = 0;
	////height map texture name
	//wstring wsHeightDataName = wsOutputPath + wsSceneName + L"HeightMap.bmp";
	//pTerrainContainer->SetHeightData(IMPORTER->ReadBitmap24(wsHeightDataName.c_str(), width, height));//heightmap
	//pTerrainContainer->SetHeightMapTexture(CTexture::CreateTexture(wsHeightDataName.c_str(), 1, BIND_DS));

	//normal map texture name
	wstring wsNormalDataName = wsOutputPath + wsSceneName + L"NormalMap.bmp";
	pTerrainContainer->SetNormalData(IMPORTER->ReadBitmap24(wsNormalDataName.c_str(), width, height));//nomalmap
	pTerrainContainer->SetNormalMapTexture(CTexture::CreateTexture(wsNormalDataName.c_str(), PS_SLOT_NORMALMAP, BIND_PS));

	//create splatting info
	int nSplatting = IMPORTER->ReadInt();
	for (int i = 0; i<nSplatting; ++i) {
		//splatting의 detail texture는 path로 받는다.
		wstring wsDetailTexturePath = IMPORTER->ReadWstring();
		
		//splatting의 blending info는 [scene name]이름을 토대로 가공한다.
		//[scene name] + [splatting blending info] + [index]
		WCHAR wcBlendinfoPath[256];
		wsprintf(wcBlendinfoPath, L"%s%sBlendInfo%d.bmp", wsOutputPath.c_str(), wsSceneName.c_str(), i);
		
		pTerrainContainer->GetSplattingInfoManager()->CreateSplattingInfo(wsDetailTexturePath.c_str(), wcBlendinfoPath);
	}
	//create splatting info
	pTerrainContainer->Begin();
	return pTerrainContainer;
}
void CTerrainContainer::SetHeightScale(float scale){
	m_pGlobalTerrainData->HeightScale = m_xmf3Scale.y = scale;
}

void CTerrainContainer::CreateTerrainTextures(LPCTSTR pTerrainName){
	WCHAR path[256];
	//wsprintf(path, L"../%sHeightMap.bmp", pTerrainName);//name
	int width, height = 0;
	//m_pHeightData = IMPORTER->ReadBitmap24(path, width, height);//heightmap
	//m_pHeightMapTexture = CTexture::CreateTexture(path, 1, BIND_DS);
	wsprintf(path, L"../%sNormalMap.bmp", pTerrainName);
	m_pNormalData = IMPORTER->ReadBitmap24(path, width, height);//nomalmap
	m_pNormalTexture = CTexture::CreateTexture(path, PS_SLOT_NORMALMAP, BIND_PS);
	SetBaseTexture(L"../../Assets/default.jpg");
}
void CTerrainContainer::SetHeightData(float * pHeightData){
	if (m_pHeightData) delete[] m_pHeightData;
	m_pHeightData = nullptr;
	m_pHeightData = pHeightData;
}
void CTerrainContainer::SetNormalData(Pixel24 * pNormalData){
	if (m_pNormalData) delete[] m_pNormalData;
	m_pNormalData = nullptr;
	m_pNormalData = pNormalData;
}
void CTerrainContainer::SetHeightMapTexture( CTexture* pHeightMapTexture){
	if (m_pHeightMapTexture) m_pHeightMapTexture->End();
	m_pHeightMapTexture = nullptr;
	m_pHeightMapTexture = pHeightMapTexture;
}
void CTerrainContainer::SetNormalMapTexture( CTexture* pNormalMapTexture){
	if (m_pNormalTexture) m_pNormalTexture->End();
	m_pNormalTexture = nullptr;
	m_pNormalTexture = pNormalMapTexture;
}
void CTerrainContainer::CreateResetTextures(LPCTSTR pTerrainName) {
	//texture 제작
	//height map data init
	WCHAR path[256];
	wsprintf(path, L"../%sHeightMap.bmp", pTerrainName);//name
	m_pHeightData = new float[(m_nWidth) * (m_nLength)];//pixel data
	for (int i = 0; i < m_nWidth*m_nLength; ++i) {
		m_pHeightData[i] = 0.f;
	}

	ReadyHeightMap();
	//m_pHeightMapTexture = CTexture::CreateTexture(path, 1, BIND_DS);
	SetBaseTexture(L"../../Assets/default.jpg");
	//m_pBaseTexture = CTexture::CreateTexture(L"../../Assets/default.jpg", 0);
	////normal
	//wsprintf(path, L"../%sNormalMap.bmp", pTerrainName);
	m_pNormalData = new Pixel24[(m_nWidth) * (m_nLength)];
	ZeroMemory(m_pNormalData, sizeof(Pixel24) * m_nWidth * m_nLength);
	//EXPORTER->MakeBitmap24(path, m_pNormalData, m_nWidth, m_nLength);
	//m_pNormalTexture = CTexture::CreateTexture(path, PS_SLOT_NORMALMAP, BIND_PS);

	CreateNormalMap();
}
void CTerrainContainer::CreateTerrainMesh(CSpaceContainer* pSpaceContainer){
	//convert wstring to string
	string name{ "" }; name.assign(m_wsSceneName.begin(), m_wsSceneName.end());

	//resize instancing buf
	RENDERER->GetTerrainRenderContainer()->ClearMesh();//기존의 mesh clear
	RENDERER->GetTerrainRenderContainer()->ClearBuffer();//기존의 buffer clear
	//resize terrain mesh
	RESOURCEMGR->CreateTerrainMesh(pSpaceContainer->GetOneSpaceSize(), name);//resource 제작은 resource mgr에게
	//RESOURCEMGR->CreateInstancingBuffer(name, pSpaceContainer->GetSpaceNum(), sizeof(VS_VB_INSTANCE));
	//set resource
	RENDERER->GetTerrainRenderContainer()->AddMesh(RESOURCEMGR->GetMesh(name, 0));//만든 mesh사용
	RENDERER->GetTerrainRenderContainer()->AddInstanceBuffer(RESOURCEMGR->GetBuffer(name));

}

void CTerrainContainer::ChangeSpaceData(){
	//1. space안의 모든 terrain제거
	//End();

	//for (auto pTerrain : m_vpTerrain) {
	//	m_pSpaceContainer->RemoveObject(pTerrain);
	//	pTerrain->End();
	//	delete pTerrain;
	//
	//}
	m_vpTerrain.clear();

	//2. mesh/ buffer새로 제작
	//Begin();
	SetTerrainScale(XMFLOAT3(static_cast<float>(m_pSpaceContainer->GetSize() / (m_nWidth)),
		m_xmf3Scale.y, static_cast<float>(m_pSpaceContainer->GetSize() / (m_nLength))));
	CreateTerrainMesh(m_pSpaceContainer);
	
	//3. terrain data 새로 설정
	float fOneSpaceSize = (float)m_pSpaceContainer->GetOneSpaceSize();
	float fOneSideSpaceNum = (float)m_pSpaceContainer->GetOneSideSpaceNum();
	m_pGlobalTerrainData->OneSideSpaceNum = fOneSideSpaceNum;
	m_pGlobalTerrainData->OneSideSpaceNumRcp = 1.0f / fOneSideSpaceNum;
	m_pGlobalTerrainData->OneSpaceSizeRcp = 1.0f / fOneSpaceSize;
	m_pGlobalTerrainData->HeightScale = m_xmf3Scale.y;
	
	//4. terrain 새로 제작
	//터레인 제작
	//terrain
	CTerrain* pTerrain = nullptr;
	for (int j = 0; j < m_pSpaceContainer->GetOneSideSpaceNum(); ++j) {
		for (int i = 0; i < m_pSpaceContainer->GetOneSideSpaceNum(); ++i) {
			pTerrain = CTerrain::CreateTerrain(this, i, j);
			m_vpTerrain.push_back(pTerrain);
		}
	}
	//terrain
	
	//5. stemp의 spacesize 새로 설정
	m_pStempManager->SetSpaceSize(m_pSpaceContainer->GetSpaceSize());

	//추가 
	int space_lavel_pow = 1;
	for (int i = 0; i < m_pSpaceContainer->GetSpaceLevel(); ++i) {
		space_lavel_pow *= 2;
	}
	SetTessFacterSLP(space_lavel_pow);

}
void CTerrainContainer::SetActive(bool b){
	for (auto pTerrain : m_vpTerrain) {
		pTerrain->SetActive(b);
	}
	m_bActive = b;
}
//추가 
void CTerrainContainer::SetTerrainTessFacter(float zn, float zf, float space_level_pow, float max_facter_value, float min_facter_value, XMFLOAT3 xmf3CameraPos){
	TERRAIN_TESS_FACTER* pData = (TERRAIN_TESS_FACTER*)m_pTessFacterBuffer->Map();
	pData->camera_pos = xmf3CameraPos;
	pData->zn = zn;
	pData->zf = zf;
	pData->space_lavel_pow = space_level_pow;
	pData->max_facter_value = max_facter_value;
	pData->min_facter_value = min_facter_value;
	m_pTessFacterBuffer->Unmap();
}
void CTerrainContainer::SaveHeightMap(){

}
CTerrainContainer::CTerrainContainer() : CObject("terraincontainer") {
}

CTerrainContainer::~CTerrainContainer()
{
}

