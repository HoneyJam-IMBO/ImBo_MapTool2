#pragma once
//#include "Camera.h"
#include "SingleTon.h"

#include "RenderContainer.h"
#include "DebugRenderContainer.h"

class CRenderContainerSeller : public CSingleTonBase<CRenderContainerSeller>{
public:
	bool Begin();
	bool End();

	mapTagRenderContainer& GetTagRenderContainer() { return m_mTagRenderContainer; }
	mapTagRenderContainer& GetStempRenderContainer() { return m_mStempRenderContainer; }

	CRenderContainer* GetRenderContainer(string name);
	CRenderContainer* GetRenderContainer(tag t, string name);


	void ClearStempRenderContainer();
	void CreateStempRenderContainer();
private:
	//이게 진짜 rendercontainer
	mapTagRenderContainer m_mTagRenderContainer;
	mapTagRenderContainer m_mStempRenderContainer;
public:
	CRenderContainerSeller() : CSingleTonBase<CRenderContainerSeller>("inputmanagersingleton") {}

};
