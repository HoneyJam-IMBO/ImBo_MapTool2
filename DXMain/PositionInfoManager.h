#pragma once
//그 position과 quaternion을 관리할 객체이다.
#include "PositionInfoData.h"

static wstring wroot_path = L"../../Assets/SceneResource/ActionPositionInfo/";
static string root_path = "../../Assets/SceneResource/ActionPositionInfo/";

static wstring wextent = L".poi";
static string extent = ".poi";//pos info/ poi


class CCamera;

class CPositionInfoManager {
public:
	static void Begin();
	static void End();

	static void PushPositionInfoData(CPositionInfoData PositionInfoData) { m_vPositionInfoData.push_back(PositionInfoData); }
	
	static void ClearPositionInfoData() { m_vPositionInfoData.clear(); }
	static void DeletePositionInfoData(int index);
	static void SaveData(wstring name);//이름으로 정해진 디렉토리에 저장하고
	static void LoadData(string name);//이름으로 정해진 디렉토리에서 찾아서 쓴다.

	static size_t GetInfoCnt() { return m_vPositionInfoData.size(); }
	static void ProcessInput(CCamera* pCamera);
	static void SetActoionSpeed(float fSpeed) { m_fActionSpeed = fSpeed; }
	//render
	static void RenderAllPositionInfo();
	//getter
	static vector<CPositionInfoData>& GetAllPositionInfo() { return m_vPositionInfoData; };
	static float GetActionSpeed() { return m_fActionSpeed; }
private:
	static vector<CPositionInfoData> m_vPositionInfoData;
	static float m_fActionSpeed;

public:
	CPositionInfoManager() { m_vPositionInfoData.clear(); };
	~CPositionInfoManager() {};

};