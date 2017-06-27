#pragma once
#include "DXObject.h"
#include "GameObject.h"

//dxobject
#include "Mesh.h"
#include "RenderShader.h"
#include "Texture.h"
#include "Buffer.h"
#include "Material.h"
#include "Animater.h"//animater!!
//dxobject

#include "Camera.h"

class CRenderContainer : public DXObject {
public:
	//---------------------------dxobject---------------------------------
	virtual bool Begin();
	virtual bool End();

	virtual void SetShaderState();
	virtual void CleanShaderState();

	virtual void UpdateShaderState( CCamera* pCamera);
	//---------------------------dxobject---------------------------------

	//--------------------------container---------------------------------
	void ClearVolatileResources();
	//picking
	bool CheckPickMesh(XMVECTOR xmvModelCameraStartPos, XMVECTOR xmvModelRayDir, float & distance);
private:
	virtual void RenderExcute();
	void RenderExcuteWithOutObject();
public:
	void Render( CCamera* pCamera);
	void RenderWithOutObject( CCamera* pCamera);

	 CMesh* GetMesh(UINT index = 0) { return m_vpMesh[index]; }
	vector< CMesh*>& GetvMesh() { return m_vpMesh; }
	//set�� �ƴ϶����� add��.
	void ClearMesh();
	void ClearBuffer();
	void ClearAnimater();
	void AddMesh( CMesh* pMesh);
	void SetMesh( CMesh* pMesh);
	void SetShader( CRenderShader* pShader);
	
	//void AddMesh( CMesh* pMesh);
	//void AddShader( CRenderShader* pShader);
	void AddTexture( CTexture* pTexture);
	//�ֹ߼� texture�� add
	void AddVolatileTexture( CTexture* pTexture);
	void AddBuffer( CBuffer* pBuffers);
	//�ֹ߼� buf�� �߰���
	void AddVolatileBuffer( CBuffer* pBuffers);
	void AddInstanceBuffer( CBuffer* pBuffers);
	void AddMaterial( CMaterial* pMaterial);
	
	//animater!
	void SetAnimater( CAnimater* pAnimater) { m_pAnimater = pAnimater; }
	 CAnimater* GetAnimater() { return m_pAnimater; }
	//animater!

	void SetObejcts(int n, CGameObject** ppObjects);
	void AddObject(CGameObject* pObject);
	void RemoveObject(CGameObject* pObject);
	void ClearObjectList() { m_lpObjects.clear(); }//�׸� �� �׸��� clear
	list<CGameObject*>& GetObjectList() { return m_lpObjects; }

	void ClearTextures() { m_vpTexture.clear(); }

	vector< CMaterial*>& GetMaterials() {return m_vpMaterial;	}
	//--------------------------container---------------------------------
protected:
	//--------------------------���� ���------------------------------
	//�����̳��� ��ü���� �������� ���� ���̴�. �׷��Ƿ� ����Ʈ�� �����ϴ� ���� �´�
	list<CGameObject*> m_lpObjects;

	vector< CMesh*> m_vpMesh;
	 CRenderShader* m_pShader;
	vector< CTexture*> m_vpTexture;
	//�ֹ߼� texture �ѹ� �����ϸ� �������. 
	vector< CTexture*> m_vpVolatileTexture;
	vector< CBuffer*> m_vpBuffer;
	//�ֹ߼� buf �ѹ� �����ϸ� �������. 
	vector< CBuffer*> m_vpVolatileBuffer;
	 CAnimater* m_pAnimater{ nullptr };//animater

	int m_nBuffer{ 0 };
	vector< CMaterial*> m_vpMaterial;
	int m_nMaterial{ 0 };
	//--------------------------���� ���------------------------------

	void** m_ppBufferData{ nullptr };
public:
	CRenderContainer();
	virtual ~CRenderContainer();

};

typedef map<string, CRenderContainer*> mapRenderContainer;
typedef map<tag, mapRenderContainer> mapTagRenderContainer;
typedef pair<string, CRenderContainer*> pairRenderContainer;
typedef pair<tag, pairRenderContainer> pairTagRenderContainer;


/*
�����̳� - AnimateContainer, RenderContainer�� �и�
{
���� ���� - 
	1. AnimateContainer�� ��� object Animate
	2. �׸� �׸� object RenderContainer�� AddObject
	3. RenderContainer���� ī�޶� �������� �ø� �� �ν��Ͻ����� ����
���⿡ �������� ����?
CScene�� Space�� �����Ѵ�.
Spcae�� Container�� �����ϴµ� AnimateContainer��.
Animate�� �� �߿��Ѱ� ������ ��� ��� �ٷ� �ٸ� ������ �̽��ϴ� ���� �ƴ϶�
���� �����Ҵٰ� �־� ��� �Ѵ�. pExcludeContainer
���⼭ ExcluderContainer�� ��ü���� ���� ������ �� �� ������

----------------------------------------------------
AnimateContainer/ object_id{enum} �ʿ伺 ���

enum���� 0-nObject ��ü �������� enum�ؼ� �̰� objectid ������ ������ �־
������ �з��� �� �� �ִٸ� AnimateContainer�� object�� �߻�ȭ�� �� ���ִٴ� �����Ͽ�
��ü���� ��� �ǰ� �ϳ��� �������� �ϳ����� ������ �ȴ�.
----------------------------------------------------

----------------------------------------------------
RenderContainer/ object���� �ʿ��ϴ�
RenderContainer�� instancing�ؾ� �ϴϱ� ���� �� ��ü���� �����ؾ� �Ѵ�.
���� RenderContainer�� CScene�� �����ϰ�
Scene�� RenderContainer�� map���� �����Ѵ�. map<object_id, RenderContainer> 

AnimateContainer���� enum�̷� �з��� ������ �ؼ� RenderContainer���� �Ѱ��ָ�
�װ� �޾Ƽ� ó���ϵ��� �Ѵ�.

RenderContainer�� CScene�� �����ϴ°�
}
*/