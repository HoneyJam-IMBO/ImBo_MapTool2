#include "stdafx.h"
#include "SpotLight.h"

bool CSpotLight::Begin() {
	m_lightid = light_id::LIGHT_SPOT;
//	m_objectID = object_id::OBJECT_SPOT_LIGHT;

	//미리 계산 가능한 것은 계산 해 놓는다.
	float fCosInnerCone = cosf(XM_PI * m_SpotData.fInnerAngle / 180.0f);
	float fCosOuterCone = cosf(XM_PI * m_SpotData.fOuterAngle / 180.0f);
	float fSinOuterCone = sinf(XM_PI * m_SpotData.fOuterAngle / 180.0f);

	m_fSpotLightRangeRcp = 1.0f / m_SpotData.SpotLightRange;
	m_fSpotCosOuterCone = fCosOuterCone;
	m_fSpotSinOuterCone = fSinOuterCone;
	m_fSpotCosConeAttRcp = 1.0f / (fCosInnerCone - fCosOuterCone);

	float SpotLightRange = m_SpotData.SpotLightRange;
	m_xmmtxScale = XMMatrixScalingFromVector(XMVECTOR(XMVectorSet(SpotLightRange, SpotLightRange, SpotLightRange, 1.0f)));


	////새로운 객체가 생겨나면 Begin에서 Component들을 set해준다. 
	//CComponent* pComponent = new CRotateComponent;
	//pComponent->Begin();
	//SetComponent(pComponent);

	return CLight::Begin();
}
bool CSpotLight::End() {

	return CLight::End();
}

//instance buffer controll base
void CSpotLight::SetBufferInfo(void** ppMappedResources, int& nInstance,  CCamera* pCamera) {

	//형변환
	SPOT_LIGHT_DS_CB *pDS_InstanceData = (SPOT_LIGHT_DS_CB*)ppMappedResources[0];
	SPOT_LIGHT_PS_CB *pPS_InstanceData = (SPOT_LIGHT_PS_CB*)ppMappedResources[1];
	//transpose 이후 정보 주입

	//light projection 이 작업 gpu로 옮겨야함 later
	XMMATRIX xmmtxWorld = GetWorldMtx();
	XMMATRIX xmmtxView = pCamera->GetViewMtx();
	XMMATRIX xmmtxProjection = pCamera->GetProjectionMtx();

	XMMATRIX xmmtxLight = m_xmmtxScale * xmmtxWorld * xmmtxView * xmmtxProjection;

	pDS_InstanceData[nInstance].LightProjection = XMMatrixTranspose(xmmtxLight);
	pDS_InstanceData[nInstance].SpotCosOuterCone = m_fSpotCosOuterCone;
	pDS_InstanceData[nInstance].SpotSinOuterCone = m_fSpotSinOuterCone;

	//light projection
	XMStoreFloat3(&pPS_InstanceData[nInstance].SpotLightPos, GetPosition());
	pPS_InstanceData[nInstance].SpotLightRangeRcp = m_fSpotLightRangeRcp;
	pPS_InstanceData[nInstance].SpotLightColor = m_SpotData.SpotLightColor;
	pPS_InstanceData[nInstance].SpotCosOuterCone = m_fSpotCosOuterCone;
	pPS_InstanceData[nInstance].SpotCosConeAttRcp = m_fSpotCosConeAttRcp;
	XMStoreFloat3(&pPS_InstanceData[nInstance].SpotLightDir, -GetLook());

}

void CSpotLight::SetLength(float len) {
	m_SpotData.SpotLightRange = len;

	float SpotLightRange = m_SpotData.SpotLightRange;
	m_xmmtxScale = XMMatrixScalingFromVector(XMVECTOR(XMVectorSet(SpotLightRange, SpotLightRange, SpotLightRange, 1.0f)));
}

void CSpotLight::SetRange(float outer, float inner) {

	m_SpotData.fInnerAngle = MIN(outer, inner);
	m_SpotData.fOuterAngle = outer;

	//미리 계산 가능한 것은 계산 해 놓는다.
	float fCosInnerCone = cosf(XM_PI * m_SpotData.fInnerAngle / 180.0f);
	float fCosOuterCone = cosf(XM_PI * m_SpotData.fOuterAngle / 180.0f);
	float fSinOuterCone = sinf(XM_PI * m_SpotData.fOuterAngle / 180.0f);

	m_fSpotLightRangeRcp = 1.0f / m_SpotData.SpotLightRange;
	m_fSpotCosOuterCone = fCosOuterCone;
	m_fSpotSinOuterCone = fSinOuterCone;
	m_fSpotCosConeAttRcp = 1.0f / (fCosInnerCone - fCosOuterCone);

}

void CSpotLight::SetInner(float inner){
	m_SpotData.fInnerAngle = inner;

	float fCosInnerCone = cosf(XM_PI * m_SpotData.fInnerAngle / 180.0f);
	float fCosOuterCone = cosf(XM_PI * m_SpotData.fOuterAngle / 180.0f);
	float fSinOuterCone = sinf(XM_PI * m_SpotData.fOuterAngle / 180.0f);

	m_fSpotLightRangeRcp = 1.0f / m_SpotData.SpotLightRange;
	m_fSpotCosOuterCone = fCosOuterCone;
	m_fSpotSinOuterCone = fSinOuterCone;
	m_fSpotCosConeAttRcp = 1.0f / (fCosInnerCone - fCosOuterCone);

}

void CSpotLight::SetOuter(float outer){
	m_SpotData.fOuterAngle = outer;
	float fCosInnerCone = cosf(XM_PI * m_SpotData.fInnerAngle / 180.0f);
	float fCosOuterCone = cosf(XM_PI * m_SpotData.fOuterAngle / 180.0f);
	float fSinOuterCone = sinf(XM_PI * m_SpotData.fOuterAngle / 180.0f);

	m_fSpotLightRangeRcp = 1.0f / m_SpotData.SpotLightRange;
	m_fSpotCosOuterCone = fCosOuterCone;
	m_fSpotSinOuterCone = fSinOuterCone;
	m_fSpotCosConeAttRcp = 1.0f / (fCosInnerCone - fCosOuterCone);

}

void CSpotLight::SetColor(float r, float g, float b) {
	m_SpotData.SpotLightColor = XMFLOAT3(r, g, b);
}

XMFLOAT3 CSpotLight::GetColor()
{
	return m_SpotData.SpotLightColor;
}

CSpotLight* CSpotLight::CreateSpotLight(float SpotLightRange, XMFLOAT3 SpotLightColor, float fOuterAngle, float fInnerAngle){
	CSpotLight* pLight = new CSpotLight;
	SPOT_LIGHT data;
	data.fInnerAngle = fInnerAngle;
	data.fOuterAngle = fOuterAngle;
	data.SpotLightColor = SpotLightColor;
	data.SpotLightRange = SpotLightRange;
	pLight->SetSpotLightData(data);
	pLight->Begin();
	return pLight;
}
void TW_CALL SetSpotLightLength(void * value, void * clientData) {
	if (nullptr == clientData) return;
	CSpotLight* plight = reinterpret_cast<CSpotLight*>(clientData);
	*static_cast<float *>(value) = plight->GetLength();
}

void TW_CALL GetSpotLightLength(const void * value, void * clientData) {
	if (nullptr == clientData) return;
	CSpotLight* plight = reinterpret_cast<CSpotLight*>(clientData);
	float* data = (float*)value;
	plight->SetLength(*data);
}
//inner
void TW_CALL SetSpotLightInner(void * value, void * clientData) {
	if (nullptr == clientData) return;
	CSpotLight* plight = reinterpret_cast<CSpotLight*>(clientData);
	*static_cast<float *>(value) = plight->GetInner();
}
void TW_CALL GetSpotLightInner(const void * value, void * clientData) {
	if (nullptr == clientData) return;
	CSpotLight* plight = reinterpret_cast<CSpotLight*>(clientData);
	float* data = (float*)value;
	plight->SetInner(*data);
}
//inner
//outter
void TW_CALL SetSpotLightOuter(void * value, void * clientData) {
	if (nullptr == clientData) return;
	CSpotLight* plight = reinterpret_cast<CSpotLight*>(clientData);
	*static_cast<float *>(value) = plight->GetOuter();
}
void TW_CALL GetSpotLightOuter(const void * value, void * clientData) {
	if (nullptr == clientData) return;
	CSpotLight* plight = reinterpret_cast<CSpotLight*>(clientData);
	float* data = (float*)value;
	plight->SetOuter(*data);
}
//outter
void CSpotLight::PickingProc(){
	CGameObject::PickingProc();

	//color
	TWBARMGR->AddColorBar3F("PickingBar", "Light", "Color", &m_SpotData.SpotLightColor);
	//length
	TWBARMGR->AddMinMaxBarCB("PickingBar", "Light", "Length", GetSpotLightLength, SetSpotLightLength, this,
		1.0f, 1000.f, 0.1f);
	//range inner, outer
	TWBARMGR->AddMinMaxBarCB("PickingBar", "Light", "inner", GetSpotLightInner, SetSpotLightInner, this,
		1.0f, 1000.f, 0.1f);
	TWBARMGR->AddMinMaxBarCB("PickingBar", "Light", "outter", GetSpotLightOuter, SetSpotLightOuter, this,
		1.0f, 1000.f, 0.1f);

}

void CSpotLight::SaveInfo(){
	CGameObject::SaveInfo();
	EXPORTER->WriteFloat(m_SpotData.fInnerAngle);EXPORTER->WriteSpace();
	EXPORTER->WriteFloat(m_SpotData.fOuterAngle);
	EXPORTER->WriteEnter();

	EXPORTER->WriteFloat(m_SpotData.SpotLightColor.x); EXPORTER->WriteSpace();
	EXPORTER->WriteFloat(m_SpotData.SpotLightColor.y); EXPORTER->WriteSpace();
	EXPORTER->WriteFloat(m_SpotData.SpotLightColor.z); EXPORTER->WriteSpace();
	EXPORTER->WriteEnter();

	EXPORTER->WriteFloat(m_SpotData.SpotLightRange);
	EXPORTER->WriteEnter();
}

void CSpotLight::LoadInfo(){

}

CSpotLight::CSpotLight() : CLight("spotlight") {

}
CSpotLight::~CSpotLight() {

}