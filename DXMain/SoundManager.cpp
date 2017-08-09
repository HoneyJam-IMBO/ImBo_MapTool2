#include "stdafx.h"
#include "SoundManager.h"

#pragma comment(lib, "../../Assets/import/FMOD/lib/fmod64_vc.lib")

System* CSoundManager::m_pSystem = nullptr;
map<string, stSoundInfo*> CSoundManager::m_mpSoundInfo;
map<string, stSoundInfo*> CSoundManager::m_mp3DSoundInfo;

void CSoundManager::Begin()
{
	m_pSystem->set3DNumListeners(1);//3d listener num

	FMOD::System_Create(&m_pSystem);
	m_pSystem->init(
		FMOD_MAX_CHANNEL_WIDTH
		, FMOD_INIT_NORMAL
		, nullptr
	);
}

void CSoundManager::End()
{
	for (auto& p : m_mpSoundInfo) {
		p.second->m_pSound->release();
	}

	m_pSystem->release();
	m_pSystem->close();
}

/*
flag exam
FMOD_DEFAULT | FMOD_LOOP_OFF
FMOD_LOOP_NORMAL | FMOD_2D
FMOD_DEFAULT | FMOD_LOOP_OFF
*/
void CSoundManager::Add_sound(string sPath, string sName, UINT flag)
{
	if (m_mpSoundInfo.end() != m_mpSoundInfo.find(sName)) {//있다면 지우고
		m_mpSoundInfo.erase(sName);//지우고
	}
	m_mpSoundInfo[sName] = new stSoundInfo;
	m_pSystem->createSound(
		sPath.c_str()
		, flag
		, nullptr
		, &m_mpSoundInfo[sName]->m_pSound
	);
}

void CSoundManager::Play_bgm(string sName)
{
	m_pSystem->playSound(m_mpSoundInfo[sName]->m_pSound
		, nullptr, false, &m_mpSoundInfo[sName]->m_pChannel);
}

void CSoundManager::Play_effect(string sName)
{
	m_pSystem->update();
	m_pSystem->playSound(m_mpSoundInfo[sName]->m_pSound
		, nullptr, false, nullptr);
}

void CSoundManager::Stop_bgm(string sName)
{
	bool bPlaying;
	m_mpSoundInfo[sName]->m_pChannel->isPlaying(&bPlaying);
	if (bPlaying)
		m_mpSoundInfo[sName]->m_pChannel->stop();
}

void CSoundManager::Add_3Dsound(string sPath, string sName, UINT flag)
{
	if (m_mp3DSoundInfo.end() != m_mp3DSoundInfo.find(sName)) {//있다면 지우고
		m_mp3DSoundInfo.erase(sName);//지우고
	}
	flag = flag | FMOD_3D;//flag에 무조건 3D flag추가
	m_mp3DSoundInfo[sName] = new stSoundInfo;
	m_pSystem->createSound(
		sPath.c_str()
		, flag
		, nullptr
		, &m_mp3DSoundInfo[sName]->m_pSound
	);
}

void CSoundManager::Play_3Dsound(string sName, int nPoints, XMFLOAT3* pPoints, float fVolume, float fMin, float fMax)
{
	FMOD_VECTOR pFmodVecPoints[20];//한번에 20개씩 재생가능
	for (int i = 0; i < nPoints; ++i) {
		pFmodVecPoints[i].x = pPoints[i].x;
		pFmodVecPoints[i].y = pPoints[i].y;
		pFmodVecPoints[i].z = pPoints[i].z;
	}
	//m_mp3DSoundInfo[sName]->m_pSound->set3DCustomRolloff(pFmodVecPoints, nPoints);
	m_mp3DSoundInfo[sName]->m_pSound->set3DMinMaxDistance(fMin, fMax);

	for (int i = 0; i < nPoints; ++i) {
		m_pSystem->update();
		m_pSystem->playSound(m_mp3DSoundInfo[sName]->m_pSound
			, nullptr, false, &m_mp3DSoundInfo[sName]->m_pChannel);
		m_mp3DSoundInfo[sName]->m_pChannel->setVolume(fVolume);
		m_mp3DSoundInfo[sName]->m_pChannel->set3DMinMaxDistance(fMin, fMax);
		m_mp3DSoundInfo[sName]->m_pChannel->set3DAttributes(&pFmodVecPoints[i], nullptr);
	}
	
}

void CSoundManager::Update(){
	FMOD_VECTOR poss[1];
	FMOD_VECTOR forwards[1];
	FMOD_VECTOR ups[1];

	auto pCamera = UPDATER->GetCamera();
	XMFLOAT3 xmf3Pos;
	XMStoreFloat3(&xmf3Pos, pCamera->GetPosition());
	poss[0].x = xmf3Pos.x;
	poss[0].y = xmf3Pos.y;
	poss[0].z = xmf3Pos.z;
	
	XMFLOAT3 xmf3Look;
	XMStoreFloat3(&xmf3Look, pCamera->GetLook());
	forwards[0].x = xmf3Look.x;
	forwards[0].y = xmf3Look.y;
	forwards[0].z = xmf3Look.z;

	XMFLOAT3 xmf3Up;
	XMStoreFloat3(&xmf3Up, pCamera->GetUp());
	ups[0].x = xmf3Up.x;
	ups[0].y = xmf3Up.y;
	ups[0].z = xmf3Up.z;
	
	m_pSystem->set3DListenerAttributes(0, poss, nullptr, forwards, ups);
	m_pSystem->update();
}


CSoundManager::CSoundManager()
{

	//	for (auto& p : m_mpSoundInfo) {
	//		p.second->m_pSound = nullptr;
	//	}
}


CSoundManager::~CSoundManager()
{

}