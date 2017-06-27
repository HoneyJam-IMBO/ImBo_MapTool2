#include "stdafx.h"
#include "TessPlaneMesh.h"

#define SIZE 100

bool CTessPlaneMesh::CreateVertexBuffer() {
	m_nVertices = 4;
	m_d3dPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST;

	m_pVertices = new XMFLOAT3[4];
	int i = 0;

	XMFLOAT3 pNormal[4];

	pNormal[i] = XMFLOAT3(0, 1, 0);
	m_pVertices[i++] = XMFLOAT3(0.0f, 0.0f, 0.0f);
	pNormal[i] = XMFLOAT3(0, 1, 0);
	m_pVertices[i++] = XMFLOAT3(SIZE, 0.0f, 0.0f);
	pNormal[i] = XMFLOAT3(0, 1, 0);
	m_pVertices[i++] = XMFLOAT3(0.0f, 0.0f, SIZE);
	pNormal[i] = XMFLOAT3(0, 1, 0);
	m_pVertices[i++] = XMFLOAT3(SIZE, 0.0f, SIZE);

	//-----------------------------------버퍼 조립-----------------------------------
	m_pd3dPositionBuffer = CreateBuffer(sizeof(XMFLOAT3), m_nVertices, m_pVertices, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DEFAULT, 0);
	//m_pd3dNormalBuffer = CreateBuffer(sizeof(XMFLOAT3), m_nVertices, pNormal, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DEFAULT, 0);
	ID3D11Buffer *pd3dBuffers[1] = { m_pd3dPositionBuffer };
	UINT pnBufferStrides[1] = { sizeof(XMFLOAT3) };
	UINT pnBufferOffsets[1] = { 0 };
	AssembleToVertexBuffer(1, pd3dBuffers, pnBufferStrides, pnBufferOffsets);
	//-----------------------------------버퍼 조립-----------------------------------

	//create space mesh aabb
	BoundingBox boundingBox;
	BoundingBox::CreateFromPoints(boundingBox, XMVectorSet(0.f, 0.f, 0.f, 0.f), XMVectorSet(SIZE, 5, SIZE, 0.f));
	m_AABB.SetBoundingBoxInfo(boundingBox);

	if (m_ppd3dVertexBuffers) return true;
	
	return false;
}
bool CTessPlaneMesh::CreateIndexBuffer() {

	return true;
}
 CTessPlaneMesh* CTessPlaneMesh::CreateTessPlaneMesh() {
	 CTessPlaneMesh* pMesh = new CTessPlaneMesh;
	pMesh->Begin();
	return pMesh;
}
CTessPlaneMesh::CTessPlaneMesh() : CMesh() {

}
CTessPlaneMesh::~CTessPlaneMesh() {

}