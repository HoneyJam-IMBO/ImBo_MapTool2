#pragma once
#include "GameObject.h"
#include "Camera.h"

class CSkyBox : public CGameObject {

public:
	//----------------------------dxobject-----------------------------
	bool Begin();
	virtual bool End();
	//----------------------------dxobject-----------------------------

	//object
	virtual void Animate(float fTimeElapsed);

	//skybox
	void SetCamera( CCamera* pCamera);

	virtual bool IsVisible( CCamera* pCamera) { return true; }
private:
	 CCamera* m_pCamera;

public:
	CSkyBox();
	~CSkyBox();
};
