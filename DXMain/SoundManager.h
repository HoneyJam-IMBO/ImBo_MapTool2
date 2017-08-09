#pragma once
#include "../../Assets/import/FMOD/include/fmod.hpp"

using namespace FMOD;

struct stSoundInfo {
	Sound* m_pSound;
	Channel* m_pChannel;
};
class CSoundManager {
public:
	static void Begin();
	static void End();

	static void Add_sound(string sPath, string sName, UINT flag);
	static void Play_bgm(string sName);
	static void Play_effect(string sName);
	static void Stop_bgm(string sName);

	static void Add_3Dsound(string sPath, string sName, UINT flag);
	static void Play_3Dsound(string sName, int nPoints, XMFLOAT3* pPoints, float fVolume = 5.f, float fMin = 10.f, float fMax = 1000.f);
	static void Update();

	//static void SetListenerNum(int nListener) { m_nListener = nListener; }
private:
	static System* m_pSystem;
	static map<string, stSoundInfo*> m_mpSoundInfo;
	static map<string, stSoundInfo*> m_mp3DSoundInfo;

	//static int m_nListener;
public:
	CSoundManager();
	~CSoundManager();

};