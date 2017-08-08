#include "stdafx.h"
#include "Camera.h"

void CCamera::ActionCamStart()
{
	//CPositionInfoManager::LoadData(name);
	m_bActionCam = true;
	m_CurPositionInfoIndex = 0;
	m_fProgress = 0.f;
}

void CCamera::ActionCamEnd()
{
	//CPositionInfoManager::End();
	m_bActionCam = false;
	m_CurPositionInfoIndex = 0;
	m_fProgress = 0.f;
}

void CCamera::ActionCamProc(){
	if (m_bActionCam) {
		//action cam이 true라면 PositionInfo에서 
		int max_index = CPositionInfoManager::GetInfoCnt() - 1;
		XMVECTOR xmvPos;
		XMVECTOR xmvQua;
		if (m_CurPositionInfoIndex != max_index && max_index > 0) {

			XMVECTOR xmvPos1 = CPositionInfoManager::GetAllPositionInfo()[m_CurPositionInfoIndex].GetPosition();
			XMVECTOR xmvPos2 = CPositionInfoManager::GetAllPositionInfo()[m_CurPositionInfoIndex + 1].GetPosition();
			xmvPos = XMVectorLerp(xmvPos1, xmvPos2, m_fProgress);
			XMVECTOR xmvQua1 = CPositionInfoManager::GetAllPositionInfo()[m_CurPositionInfoIndex].GetQuaternion();
			XMVECTOR xmvQua2 = CPositionInfoManager::GetAllPositionInfo()[m_CurPositionInfoIndex + 1].GetQuaternion();
			xmvQua = XMQuaternionSlerp(xmvQua1, xmvQua2, m_fProgress);
			m_fProgress += TIMEMGR->GetTimeElapsed() * CPositionInfoManager::GetActionSpeed();
			if (m_fProgress > 1.f) {
				m_fProgress = 0.f;
				m_CurPositionInfoIndex++;
			}
		}
		else {
			if (max_index > 0) {
				xmvPos = CPositionInfoManager::GetAllPositionInfo()[m_CurPositionInfoIndex].GetPosition();
				xmvQua = CPositionInfoManager::GetAllPositionInfo()[m_CurPositionInfoIndex].GetQuaternion();
				ActionCamEnd();
			}
			else {
				ActionCamEnd();
				return;
			}
		}

		//world mtx
		XMMATRIX xmMtx = XMMatrixAffineTransformation(XMVectorSet(1, 1, 1, 1), XMQuaternionIdentity(), xmvQua, xmvPos);
		XMFLOAT4X4 xmf4x4;
		XMStoreFloat4x4(&xmf4x4, xmMtx);
		XMFLOAT3 xmf3Look = XMFLOAT3(xmf4x4._31, xmf4x4._32, xmf4x4._33);
		XMVECTOR xmvLook = XMVectorSet(xmf4x4._31, xmf4x4._32, xmf4x4._33, 1.f);
		//XMVECTOR xmvLookAt = xmvPos + xmvLook;
		XMStoreFloat3(&m_xmf3Pos, xmvPos);
		XMStoreFloat3(&m_xmf3At, xmvLook);
		//XMStoreFloat3(&m_xmf3At, xmvLookAt);
	}
}

//DI
bool CCamera::Begin() {

	//변수 초기화
	XMStoreFloat4x4(&m_xmf4x4View, XMMatrixIdentity());
	XMStoreFloat4x4(&m_xmf4x4ReflectionView, XMMatrixIdentity());

	////ProjectionMtx 제작
	//GenerateProjectionMatrix(fFov, fWidth / fHeight, fNear, fFar);

	m_xmf3Right = XMFLOAT3{ 1.0f, 0.0f, 0.0f };
	m_xmf3Up = XMFLOAT3{ 0.0f, 1.0f, 0.0f };
	m_xmf3Look = XMFLOAT3{ 0.0f, 0.0f, 1.0f };
	m_xmf3Pos = XMFLOAT3{ 0.0f, 0.0f, 0.0f };

	//상수버퍼 제작
	D3D11_BUFFER_DESC CameraBufferDesc;
	ZeroMemory(&CameraBufferDesc, sizeof(CameraBufferDesc));
	CameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	CameraBufferDesc.ByteWidth = sizeof(stCameraBuffer);
	CameraBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	CameraBufferDesc.CPUAccessFlags = 0;
	GLOBALVALUEMGR->GetDevice()->CreateBuffer(&CameraBufferDesc, nullptr, &m_pViewProjectionBuffer);

	return true;
}
bool CCamera::End() {
	if (m_pViewProjectionBuffer) m_pViewProjectionBuffer->Release();

	return true;
}
void CCamera::SetPosition(XMVECTOR pos) {

	//바뀐 벡터 설정 후 
	XMStoreFloat3(&m_xmf3Pos, pos);


	//viewMtx 업데이트
	UpdateViewMtx();
}
XMMATRIX CCamera::GetWorldMtx() {
	XMFLOAT4X4 xmf4x4World{
		m_xmf4x4View._11, m_xmf4x4View._21, m_xmf4x4View._31, 0,
		m_xmf4x4View._12, m_xmf4x4View._22, m_xmf4x4View._32, 0,
		m_xmf4x4View._13, m_xmf4x4View._23, m_xmf4x4View._33, 0,
		m_xmf3Pos.x, m_xmf3Pos.y, m_xmf3Pos.z, 1
	};
	return XMLoadFloat4x4(&xmf4x4World);
}


void CCamera::GenerateOrthoMatrix(float fWidth, float fHeight, float fNear, float fFar){

	XMStoreFloat4x4(&m_xmf4x4Projection, XMMatrixOrthographicLH(fWidth, fHeight, fNear, fFar));
	m_fFarClip = fFar;
	m_fNearClip = fNear;
	//create oriented frustum
	BoundingFrustum::CreateFromMatrix(m_OriBoundingFrustum, XMLoadFloat4x4(&m_xmf4x4Projection));
}

//projection Mtx 제작
void CCamera::GenerateProjectionMatrix(float fFov, float fRatio, float fNear, float fFar) {
	XMStoreFloat4x4(&m_xmf4x4Projection, XMMatrixPerspectiveFovLH(fFov, fRatio, fNear, fFar));

	m_fFarClip = fFar;
	m_fNearClip = fNear;
	//create oriented frustum
	BoundingFrustum::CreateFromMatrix(m_OriBoundingFrustum, XMLoadFloat4x4(&m_xmf4x4Projection));
}
//viewmtx 갱신

void CCamera::UpdateViewMtx() {
	
	ActionCamProc();

	XMStoreFloat4x4(&m_xmf4x4View,
		XMMatrixLookAtLH(XMLoadFloat3(&m_xmf3Pos),
			XMVectorAdd(XMLoadFloat3(&m_xmf3Pos), XMLoadFloat3(&m_xmf3At)),
			XMLoadFloat3(&m_xmf3UpDefault)));

	//update frustum
	m_OriBoundingFrustum.Transform(m_BoundingFrustum, GetWorldMtx());

	m_xmf3Right = XMFLOAT3(m_xmf4x4View._11, m_xmf4x4View._21, m_xmf4x4View._31);
	m_xmf3Up = XMFLOAT3(m_xmf4x4View._12, m_xmf4x4View._22, m_xmf4x4View._32);
	m_xmf3Look = XMFLOAT3(m_xmf4x4View._13, m_xmf4x4View._23, m_xmf4x4View._33);
}

void CCamera::UpdateReflectionViewMtx(XMVECTOR xmvReflectePlane)
{
	/*
	ref는 position을 반사시킨 위치로 이동하는 것이다. 
	*/
	XMVECTOR xmvPos = XMLoadFloat3(&m_xmf3Pos);
	XMVECTOR xmvLook = XMLoadFloat3(&m_xmf3At);//카메라 기준 look벡터
	XMVECTOR xmvUp = XMLoadFloat3(&m_xmf3Up);

	XMVECTOR xmvLookAt = xmvPos + xmvLook;//pos + look은 내가 보고있는 방향의 살짝 앞 포지션
	XMVECTOR xmvUpAt = xmvPos + xmvUp;//pos + look은 내가 보고있는 방향의 살짝 앞 포지션


	XMMATRIX xmmtxReflect = XMMatrixReflect(xmvReflectePlane);
	XMMATRIX xmmtxReflectCamera = XMMatrixMultiply(GetWorldMtx(), xmmtxReflect);//카메라와 ref를 곱해서 카메라를 이동시킨다.

	XMVECTOR xmvRefPos = XMVector3Transform(xmvPos, xmmtxReflect);
	XMVECTOR xmvRefLookAt = XMVector3Transform(xmvLookAt, xmmtxReflect);//내가 보고있는 방향의 살짝 앞 포지션
	XMVECTOR xmvRefUpAt = XMVector3Transform(xmvUpAt, xmmtxReflect);

	XMVECTOR xmvRefLook = xmvRefLookAt - xmvRefPos;
	XMVECTOR xmvRefUp = xmvRefUpAt - xmvRefPos;

	SetLookAt(xmvRefPos, xmvRefLook, xmvRefUp);


	UpdateViewMtx();
	UpdateShaderState();
	
	XMStoreFloat4x4(&m_xmf4x4ReflectionView, GetViewMtx());
}

bool CCamera::IsInFrustum(BoundingBox& boundingBox) {
	//if (DISJOINT == m_BoundingFrustum.Contains(boundingBox)) return false;
	//return true;
	bool result = m_BoundingFrustum.Intersects(boundingBox);
	return result;
	//return true;
}

void CCamera::SetLookAt(XMVECTOR xmvPos, XMVECTOR xmvLookAt, XMVECTOR xmvUp)
{
	XMVECTOR vATpos = XMVectorAdd(xmvPos, xmvLookAt);

	XMStoreFloat4x4(&m_xmf4x4View, XMMatrixLookAtLH(xmvPos, vATpos, xmvUp));

	XMStoreFloat3(&m_xmf3Pos, xmvPos);
	XMStoreFloat3(&m_xmf3At, xmvLookAt);
	//XMStoreFloat3(&m_xmf3Pos, xmvPos);

	m_xmf3Right = XMFLOAT3(m_xmf4x4View._11, m_xmf4x4View._21, m_xmf4x4View._31);
	m_xmf3Up = XMFLOAT3(m_xmf4x4View._12, m_xmf4x4View._22, m_xmf4x4View._32);
	m_xmf3Look = XMFLOAT3(m_xmf4x4View._13, m_xmf4x4View._23, m_xmf4x4View._33);
}

void CCamera::SetViewport(DWORD xTopLeft, DWORD yTopLeft, DWORD nWidth, DWORD nHeight, float fMinZ, float fMaxZ) {
	//0,0, r, b
	m_d3dViewport.TopLeftX = float(xTopLeft);
	m_d3dViewport.TopLeftY = float(yTopLeft);
	m_d3dViewport.Width = float(nWidth);
	m_d3dViewport.Height = float(nHeight);
	m_d3dViewport.MinDepth = fMinZ;
	m_d3dViewport.MaxDepth = fMaxZ;
	GLOBALVALUEMGR->GetDeviceContext()->RSSetViewports(1, &m_d3dViewport);

	m_rcClient.bottom = nHeight;
	m_rcClient.right = nWidth;
	m_rcClient.top = 0;
	m_rcClient.left = 0;
}
//상수버퍼 갱신
void CCamera::UpdateShaderState() {
	//정보 갱신
	XMMATRIX xmMtxProjection = (XMLoadFloat4x4(&m_xmf4x4Projection));
	UpdateViewMtx();
	XMMATRIX xmMtxView = XMLoadFloat4x4(&m_xmf4x4View);

	XMStoreFloat4x4(&m_stCameraBufferData.m_xmf4x4ViewProjection, XMMatrixTranspose(XMMatrixMultiply(xmMtxView, xmMtxProjection)));
	//정보 갱신

	//상수버퍼 업데이트
	GLOBALVALUEMGR->GetDeviceContext()->UpdateSubresource(m_pViewProjectionBuffer, 0, NULL, &m_stCameraBufferData, 0, 0);
}

CCamera::CCamera() : DXObject("camera") {
	//카메라 상수버퍼
	m_pViewProjectionBuffer = nullptr;

	XMStoreFloat4x4(&m_xmf4x4View, XMMatrixIdentity());
	XMStoreFloat4x4(&m_xmf4x4Projection, XMMatrixIdentity());
}
CCamera::~CCamera() {

}