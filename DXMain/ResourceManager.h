#pragma once

#include "SingleTon.h"
#include "DirectXFramework.h"

#include "Animater.h"//animater!!

#include "DirectionalLightMesh.h"
#include "PointLightMesh.h"
#include "SpotLightMesh.h"
#include "CapsuleLightMesh.h"
#include "TestMesh.h"
#include "PlaneMesh.h"
#include "TestDeferredMesh.h"
#include "SkyBoxMesh.h"//skybox
#include "SpaceMesh.h"//space
#include "TerrainMesh.h"//terrain
#include "BoundingBoxMesh.h"//BoundingBox mesh
#include "DebugTextureMesh.h"//debug texture mesh
#include "CoordinateSysMesh.h"//CoordinateSystem Mesh
#include "FileBasedMesh.h"//fbx mesh
#include "TessPlaneMesh.h"//Tess Plane
#include "NaviMesh.h"//navi mesh
//#include "AnimationMesh.h"//animation mesh
#include "MultiMesh.h"//multiMesh

#include "TestObject.h"
#include "Light.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "CapsuleLight.h"
#include "SkyBox.h"//skybox
#include "Space.h"//space
#include "Terrain.h"//terrain
#include "DebugTexture.h"//debug texture
#include "Material.h"

#include "Texture.h"
#include "Sampler.h"
#include "RenderShader.h"


class CResourceManager : public CSingleTonBase<CResourceManager> {

public:
	bool Begin();
	bool End();

	//create resource 
	//texture array
	 CTexture* CreateTexture(string name, UINT nTextures, _TCHAR(*ppstrFilePaths)[128], UINT Slot = 0, UINT BindFlag = BIND_PS,  CBuffer* pConstantBuffer = nullptr);
	 CTexture* CreateTexture(string name, UINT nTextures, ID3D11Texture2D **ppd3dTextures, UINT Slot, UINT BindFlag,  CBuffer* pConstantBuffer);
	//texture 1개
	 CTexture* CreateTexture(string name, _TCHAR(pstrFilePath)[128], UINT Slot = 0, UINT BindFlag = BIND_PS,  CBuffer* pConstantBuffer = nullptr);
	//완성된 srv set
	 CTexture* CreateTexture(string name, ID3D11ShaderResourceView* pShaderResourceView, UINT Slot = 0, UINT BindFlag = BIND_PS,  CBuffer* pConstantBuffer = nullptr);
	 CSampler* CreateSampler(string name, UINT Slot = 0, UINT BindFlags = BIND_PS, D3D11_TEXTURE_ADDRESS_MODE Mode = D3D11_TEXTURE_ADDRESS_WRAP, D3D11_FILTER Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_COMPARISON_FUNC ComparisionFunc = D3D11_COMPARISON_NEVER, float MinLOD = 0, float MaxLOD = 0, float BorderColor = 0);
	 CRenderShader* CreateRenderShader(string name, LPCTSTR ShaderName, UINT InputElementFlag, UINT BindFlag = BIND_VS | BIND_PS);
	 CBuffer* CreateConstantBuffer(string name, UINT nObject, UINT BufferStride, UINT Slot = 0, UINT BindFlag = BIND_VS, UINT Offset = 0);
	 CBuffer* CreateInstancingBuffer(string name, UINT nObject, UINT BufferStride, UINT Offset = 0);
	 CBuffer* CreateGlobalBuffer(string name, UINT nObject, UINT BufferStride, UINT Slot = 0, UINT BindFlag = BIND_VS, UINT Offset = 0);
	 CMaterial* CreateMaterial(string name, XMFLOAT4 color, float specExp, float specIntensity);
	UINT CreateMultiMesh(string path, string name);
	UINT CreateGJMResource(string path, string name);
	UINT CreateFBXResource(string path, string name);
	void CreateTerrainMesh(float fOneSpaceSize, string name);
	void CreateAnimater(string path, string animaterName);

	//get resource func
	 CTexture* GetTexture(string name) { return m_mTexture[name]; }
	 CSampler* GetSampler(string name) { return m_mSampler[name]; }
	 CRenderShader* GetRenderShader(string name) { return m_mRenderShader[name]; }
	// CMesh> GetMesh(string name, int index = 0) { return m_mvMesh[name][index]; }
	vector< CMesh*> GetvMesh(string name) { return m_mvMesh[name]; }
	 CMesh* GetMesh(string name, int index) { return m_mvMesh[name][index]; }
	map<string, vector< CMesh*>> GetAllStempMesh() { return m_mvStempMesh; }
	vector< CMesh*> GetvStempMesh(string name) { return m_mvStempMesh[name]; }
	 CMesh* GetStempMesh(string name, int index) { return m_mvStempMesh[name][index]; }
	 CBuffer* GetBuffer(string name) { return m_mBuffer[name]; }
	 CBuffer* GetGlobalBuffer(string name) { return m_mGlobalBuffer[name]; }
	 CMaterial* GetMaterial(string name) { return m_mMaterial[name]; }
	 CAnimater* GetAnimater(string name) { return m_mAnimater[name]; }

	void ReleaseAllResource();

	void ReleaseMesh(string name);
	void ReleaseStempMesh(string name);
	void ReleaseAnimater(string name);
private:
	//begin func
	void CreateSamplers();
	void CreateTextures();
	void CreateRenderShaders();
	void CreateMeshs();
	void CreateStempMeshs();
	void CreateBuffers();
	void CreateGlobalBuffers();
	void CreateMaterials();
	void CreateAnimaters();
	void CreateGJMResources();
	void CreateFBXResources();

	//end func
	void ReleaseSamplers();
	void ReleaseTextures();
	void ReleaseRenderShaders();
	void ReleaseMeshs();
	void ReleaseStempMeshs();
	void ReleaseBuffers();
	void ReleaseGlobalBuffers();
	void ReleaseMaterials();
	void ReleaseAnimaters();

	//ID3D11Device* m_pd3dDevice{ nullptr };
	//ID3D11DeviceContext* m_pd3dDeviceContext{ nullptr };

	map<string,  CTexture*> m_mTexture;
	using pairTexture = pair<string,  CTexture*>;
	map<string,  CRenderShader*> m_mRenderShader;
	using pairShader = pair<string,  CRenderShader*>;

	//object 이름 string을 가지고, mesh vector를 넣는다.
	map<string, vector< CMesh*>> m_mvMesh;
	map<string, vector< CMesh*>> m_mvStempMesh;
	using pairMesh = pair<string,  CMesh*>;
	map<string,  CBuffer*> m_mBuffer;
	map<string,  CBuffer*> m_mGlobalBuffer;
	using pairBuffer = pair<string,  CBuffer*>;
	map<string,  CMaterial*> m_mMaterial;
	using pairMaterial = pair<string,  CMaterial*>;
	map<string,  CSampler*> m_mSampler;
	using pairSampler = pair<string,  CSampler*>;
	//animater
	map<string,  CAnimater*> m_mAnimater;
	using pairAnimater = pair<string,  CAnimater*>;
public:
	CResourceManager();
	virtual ~CResourceManager();
};
