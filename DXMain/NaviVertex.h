#pragma once

class CNaviObject;

class CNaviVertex {
public:
	void Begin();
	void End();

	void AddNaviObject(CNaviObject* pObj) { m_vpNaviObject.push_back(pObj); }
	int GetNaviObjectCount() { return m_vpNaviObject.size(); }
	static CNaviVertex* CreateNaviVertex(XMFLOAT3 pos);

	void SetPosition(XMFLOAT3& pos) { m_xmf3Position = pos; };
	void SetPositionX(float& p) { m_xmf3Position.x = p; };
	void SetPositionY(float& p) { m_xmf3Position.y = p; };
	void SetPositionZ(float& p) { m_xmf3Position.z = p; };
	void RefreshPosition();

	XMFLOAT3& GetPosition() { return m_xmf3Position; }
	vector<CNaviObject*>& GetNaviObjects() { return m_vpNaviObject; }
	void SetIndex(int index) { m_index = index; }
	int GetIndex() { return m_index; }

	void CreateControlUI();
	void DeleteNaviObject(int index);
private:
	vector<CNaviObject*> m_vpNaviObject;
	XMFLOAT3 m_xmf3Position;

	int m_index{ -1 };

public:
	CNaviVertex() {};
	~CNaviVertex() {};

};