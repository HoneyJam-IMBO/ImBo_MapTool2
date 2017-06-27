#pragma once
#include "Mesh.h"

class CTessPlaneMesh : public CMesh {

public:
	//----------------------------dxobject-----------------------------
	//end는 Cmesh에서 진행한다.
	//----------------------------dxobject-----------------------------

	//---------------------------mesh----------------------------------
	//begin func
	virtual bool CreateVertexBuffer();
	virtual bool CreateIndexBuffer();
	//begin func
	//---------------------------mesh----------------------------------

	static  CTessPlaneMesh* CreateTessPlaneMesh();
private:
	//----------------------vertex buffers---------------------------
	ID3D11Buffer* m_pd3dPositionBuffer{ nullptr };
	ID3D11Buffer* m_pd3dNormalBuffer{ nullptr };
	
	//----------------------vertex buffers---------------------------

public:
	CTessPlaneMesh();
	~CTessPlaneMesh();
};