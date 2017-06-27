#pragma once

class CMesh;;

//이 mesh는 정점 인덱스 전부 아무것도 없다.
class CNaviMesh : public CMesh {
public:
	bool CreateVertexBuffer();
	bool CreateIndexBuffer();

private:

public:
	CNaviMesh();
	~CNaviMesh();

};