#include "stdafx.h"
#include "DirectionalLight.h"

bool CDirectionalLight::Begin(DIRECTIONAL_AMBIENT_LIGHT& light_info) {
	m_lightid = light_id::LIGHT_DIRECTIONAL;
//	m_objectID = object_id::OBJECT_DIRECTIONAL_LIGHT;

	//SetLook(XMLoadFloat4(&m_Directional_AmbientData.m_DirToLight));

	m_Directional_AmbientData = light_info;

	return CLight::Begin();
}
bool CDirectionalLight::End() {

	return CLight::End();
}
//instance buffer controll base
void CDirectionalLight::SetBufferInfo(void** ppMappedResources, int& nInstance,  CCamera* pCamera) {
	
	//형변환
	DIRECTIONAL_AMBIENT_LIGHT *pnInstances = (DIRECTIONAL_AMBIENT_LIGHT *)ppMappedResources[0];
	//transpose 이후 정보 주입

	//directional
	XMVECTOR vDir = GetLook();
	XMStoreFloat4(&pnInstances[nInstance].m_DirToLight, -vDir);
	pnInstances[nInstance].m_DirLightColor = m_Directional_AmbientData.m_DirLightColor;
	//pnInstances[nInstance].m_DirToLight = m_Directional_AmbientData.m_DirToLight;
	pnInstances[nInstance].m_DirLightPower = m_Directional_AmbientData.m_DirLightPower;

	//ambient
	pnInstances[nInstance].m_AmbientColor = m_Directional_AmbientData.m_AmbientColor;
	pnInstances[nInstance].m_AmbientDown = m_Directional_AmbientData.m_AmbientDown;
	pnInstances[nInstance].m_AmbientRange = m_Directional_AmbientData.m_AmbientRange;
	pnInstances[nInstance].m_AmbientUp = m_Directional_AmbientData.m_AmbientUp;
}

void CDirectionalLight::SetLength(float len){
	m_Directional_AmbientData.m_AmbientUp = XMFLOAT4(len, len, len, 0.f);
	m_Directional_AmbientData.m_AmbientDown = XMFLOAT4(len, len, len, 0.f);
}

void CDirectionalLight::SetRange(float outer, float inner){
	m_Directional_AmbientData.m_AmbientUp = XMFLOAT4(outer, outer, outer, 0.f);
	m_Directional_AmbientData.m_AmbientDown = XMFLOAT4(inner, inner, inner, 0.f);
}

void CDirectionalLight::SetColor(float r, float g, float b){
	m_Directional_AmbientData.m_DirLightColor = XMFLOAT4(r, g, b, 0.f);
}

XMFLOAT3 CDirectionalLight::GetColor()
{
	XMFLOAT3 xmf3Color = XMFLOAT3(m_Directional_AmbientData.m_DirLightColor.x, m_Directional_AmbientData.m_DirLightColor.y, m_Directional_AmbientData.m_DirLightColor.z);
	return xmf3Color;
}

bool CDirectionalLight::IsVisible( CCamera* pCamera){
	//m_BoundingBox = m_OriBoundingBox;
	//m_BoundingBox.Transform(m_BoundingBox, GetWorldMtx());

	return true;
}

void CDirectionalLight::PickingProc(){
	CGameObject::PickingProc();

	//color
	XMFLOAT4 m_AmbientDown;
	XMFLOAT4 m_AmbientRange;
	XMFLOAT4 m_AmbientUp;
	TWBARMGR->AddColorBar4F("PickingBar", "Light", "AmbientColor", &m_Directional_AmbientData.m_AmbientColor);
	TWBARMGR->AddColorBar4F("PickingBar", "Light", "AmbientUp", &m_Directional_AmbientData.m_AmbientUp);
	TWBARMGR->AddColorBar4F("PickingBar", "Light", "AmbientDown", &m_Directional_AmbientData.m_AmbientDown);

	TWBARMGR->AddColorBar4F("PickingBar", "Light", "DirColor", &m_Directional_AmbientData.m_DirLightColor);
	TWBARMGR->AddMinMaxBarRW("PickingBar", "Light", "DirPosOffset", &m_fOffsetLength,
		10.f, 10000.f, 0.1f);

	RENDERER->GetShadow()->CreateShadowControlUI();
	//TWBARMGR->AddColorBar(f"PickingBar", "Light", "DirPower", &m_Directional_AmbientData.m_DirLightPower);
	//TWBARMGR->AddColorBar("PickingBar", "Light", "DirPower", &m_Directional_AmbientData.m_AmbientDown);
	////range
	//TWBARMGR->AddMinMaxBarCB("PickingBar", "Light", "Range", GetPointLightRange, SetPointLightRange, this,
	//	1.0f, 1000.f, 0.1f);

}

void CDirectionalLight::SaveInfo(){
	CGameObject::SaveInfo();

	EXPORTER->WriteFloat(m_Directional_AmbientData.m_AmbientColor.x); EXPORTER->WriteSpace();
	EXPORTER->WriteFloat(m_Directional_AmbientData.m_AmbientColor.y); EXPORTER->WriteSpace();
	EXPORTER->WriteFloat(m_Directional_AmbientData.m_AmbientColor.z); EXPORTER->WriteSpace();
	EXPORTER->WriteFloat(m_Directional_AmbientData.m_AmbientColor.w); EXPORTER->WriteSpace();
	EXPORTER->WriteEnter();

	EXPORTER->WriteFloat(m_Directional_AmbientData.m_AmbientDown.x); EXPORTER->WriteSpace();
	EXPORTER->WriteFloat(m_Directional_AmbientData.m_AmbientDown.y); EXPORTER->WriteSpace();
	EXPORTER->WriteFloat(m_Directional_AmbientData.m_AmbientDown.z); EXPORTER->WriteSpace();
	EXPORTER->WriteFloat(m_Directional_AmbientData.m_AmbientDown.w); EXPORTER->WriteSpace();
	EXPORTER->WriteEnter();

	EXPORTER->WriteFloat(m_Directional_AmbientData.m_AmbientRange.x); EXPORTER->WriteSpace();
	EXPORTER->WriteFloat(m_Directional_AmbientData.m_AmbientRange.y); EXPORTER->WriteSpace();
	EXPORTER->WriteFloat(m_Directional_AmbientData.m_AmbientRange.z); EXPORTER->WriteSpace();
	EXPORTER->WriteFloat(m_Directional_AmbientData.m_AmbientRange.w); EXPORTER->WriteSpace();
	EXPORTER->WriteEnter();

	EXPORTER->WriteFloat(m_Directional_AmbientData.m_AmbientUp.x); EXPORTER->WriteSpace();
	EXPORTER->WriteFloat(m_Directional_AmbientData.m_AmbientUp.y); EXPORTER->WriteSpace();
	EXPORTER->WriteFloat(m_Directional_AmbientData.m_AmbientUp.z); EXPORTER->WriteSpace();
	EXPORTER->WriteFloat(m_Directional_AmbientData.m_AmbientUp.w); EXPORTER->WriteSpace();
	EXPORTER->WriteEnter();

	EXPORTER->WriteFloat(m_Directional_AmbientData.m_DirLightColor.x); EXPORTER->WriteSpace();
	EXPORTER->WriteFloat(m_Directional_AmbientData.m_DirLightColor.y); EXPORTER->WriteSpace();
	EXPORTER->WriteFloat(m_Directional_AmbientData.m_DirLightColor.z); EXPORTER->WriteSpace();
	EXPORTER->WriteFloat(m_Directional_AmbientData.m_DirLightColor.w); EXPORTER->WriteSpace();
	EXPORTER->WriteEnter();

	EXPORTER->WriteFloat(m_Directional_AmbientData.m_DirLightPower.x); EXPORTER->WriteSpace();
	EXPORTER->WriteFloat(m_Directional_AmbientData.m_DirLightPower.y); EXPORTER->WriteSpace();
	EXPORTER->WriteFloat(m_Directional_AmbientData.m_DirLightPower.z); EXPORTER->WriteSpace();
	EXPORTER->WriteFloat(m_Directional_AmbientData.m_DirLightPower.w); EXPORTER->WriteSpace();
	EXPORTER->WriteEnter();

	EXPORTER->WriteFloat(m_Directional_AmbientData.m_DirToLight.x); EXPORTER->WriteSpace();
	EXPORTER->WriteFloat(m_Directional_AmbientData.m_DirToLight.y); EXPORTER->WriteSpace();
	EXPORTER->WriteFloat(m_Directional_AmbientData.m_DirToLight.z); EXPORTER->WriteSpace();
	EXPORTER->WriteFloat(m_Directional_AmbientData.m_DirToLight.w); EXPORTER->WriteSpace();
	EXPORTER->WriteEnter();

	EXPORTER->WriteFloat(m_fOffsetLength);
	EXPORTER->WriteEnter();
}

void CDirectionalLight::LoadInfo(){
	CGameObject::LoadInfo();

	m_Directional_AmbientData.m_AmbientColor.x = IMPORTER->ReadFloat();
	m_Directional_AmbientData.m_AmbientColor.y = IMPORTER->ReadFloat();
	m_Directional_AmbientData.m_AmbientColor.z = IMPORTER->ReadFloat();
	m_Directional_AmbientData.m_AmbientColor.w = IMPORTER->ReadFloat();

	m_Directional_AmbientData.m_AmbientDown.x = IMPORTER->ReadFloat();
	m_Directional_AmbientData.m_AmbientDown.y = IMPORTER->ReadFloat();
	m_Directional_AmbientData.m_AmbientDown.z = IMPORTER->ReadFloat();
	m_Directional_AmbientData.m_AmbientDown.w = IMPORTER->ReadFloat();

	m_Directional_AmbientData.m_AmbientRange.x = IMPORTER->ReadFloat();
	m_Directional_AmbientData.m_AmbientRange.y = IMPORTER->ReadFloat();
	m_Directional_AmbientData.m_AmbientRange.z = IMPORTER->ReadFloat();
	m_Directional_AmbientData.m_AmbientRange.w = IMPORTER->ReadFloat();
	
	m_Directional_AmbientData.m_AmbientUp.x = IMPORTER->ReadFloat();
	m_Directional_AmbientData.m_AmbientUp.y = IMPORTER->ReadFloat();
	m_Directional_AmbientData.m_AmbientUp.z = IMPORTER->ReadFloat();
	m_Directional_AmbientData.m_AmbientUp.w = IMPORTER->ReadFloat();	

	m_Directional_AmbientData.m_DirLightColor.x = IMPORTER->ReadFloat();
	m_Directional_AmbientData.m_DirLightColor.y = IMPORTER->ReadFloat();
	m_Directional_AmbientData.m_DirLightColor.z = IMPORTER->ReadFloat();
	m_Directional_AmbientData.m_DirLightColor.w = IMPORTER->ReadFloat();
	
	m_Directional_AmbientData.m_DirLightPower.x = IMPORTER->ReadFloat();
	m_Directional_AmbientData.m_DirLightPower.y = IMPORTER->ReadFloat();
	m_Directional_AmbientData.m_DirLightPower.z = IMPORTER->ReadFloat();
	m_Directional_AmbientData.m_DirLightPower.w = IMPORTER->ReadFloat();
	
	m_Directional_AmbientData.m_DirToLight.x = IMPORTER->ReadFloat();
	m_Directional_AmbientData.m_DirToLight.y = IMPORTER->ReadFloat();
	m_Directional_AmbientData.m_DirToLight.z = IMPORTER->ReadFloat();
	m_Directional_AmbientData.m_DirToLight.w = IMPORTER->ReadFloat();

	m_fOffsetLength = IMPORTER->ReadFloat();
}

CDirectionalLight::CDirectionalLight() : CLight("directionallight") {
	
}
CDirectionalLight::~CDirectionalLight() {

}