#include "stdafx.h"
#include "ResourceManager.h"

bool CResourceManager::Begin() {
	CreateSamplers();
	CreateTextures();
	CreateRenderShaders();
	CreateBuffers();
	CreateGlobalBuffers();
	CreateMaterials();
	CreateMeshs();
	CreateAnimaters();
	CreateGJMResources();
	CreateFBXResources();

	return true;
}

bool CResourceManager::End() {
	ReleaseSamplers();
	ReleaseTextures();
	ReleaseRenderShaders();
	ReleaseBuffers();
	ReleaseGlobalBuffers();
	ReleaseMaterials();
	ReleaseMeshs();
	ReleaseStempMeshs();
	ReleaseAnimaters();

	ReleseInstance();
	return true;
}

void CResourceManager::CreateSamplers() {
	//make sampler
	//WRAP
	CreateSampler("WRAP_LINEAR", 0, BIND_DS | BIND_PS, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_FILTER_MIN_MAG_MIP_LINEAR,
		D3D11_COMPARISON_ALWAYS, 0, D3D11_FLOAT32_MAX);
	CreateSampler("WRAP_POINT", 1, BIND_DS | BIND_PS, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_FILTER_MIN_MAG_MIP_POINT,
		D3D11_COMPARISON_ALWAYS, 0, D3D11_FLOAT32_MAX);
	//CLAMP
	CreateSampler("CLAMP_LINEAR", 2, BIND_DS | BIND_PS, D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_FILTER_MIN_MAG_MIP_LINEAR,
		D3D11_COMPARISON_ALWAYS, 0, D3D11_FLOAT32_MAX);
	CreateSampler("CLAMP_POINT", 3, BIND_DS | BIND_PS, D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_FILTER_MIN_MAG_MIP_POINT,
		D3D11_COMPARISON_ALWAYS, 0, D3D11_FLOAT32_MAX);
	CreateSampler("SHADOW", 4, BIND_PS, D3D11_TEXTURE_ADDRESS_BORDER, D3D11_FILTER_MIN_MAG_MIP_LINEAR,
		D3D11_COMPARISON_LESS, 0, D3D11_FLOAT32_MAX, 1);

	for (auto pSampler : m_mSampler) {
		pSampler.second->SetShaderState();
	}
}

void CResourceManager::CreateTextures() {
	//texture
	CreateTexture("DEFAULT", _T("../../Assets/default.jpg"), PS_TEXTURE, BIND_PS);
	CreateTexture("PICPOS", _T("../../Assets/default.jpg"), 2, BIND_PS);

	//terrain heightmap
	CreateTexture("TerrainHeightMap", _T("../../Assets/HeightMap.bmp"), DS_SLOT_HEIGHTMAP, BIND_DS);
	CreateTexture("TerrainBase", _T("../../Assets/Base_Texture.jpg"), PS_SLOT_TERRAIN_BASE, BIND_PS);
	//terrain detail texture
	CreateTexture("TerrainDetail", _T("../../Assets/Detail_Texture_9.jpg"), PS_SLOT_TERRAIN_DETAIL, BIND_PS);
	//terrain normal map
	CreateTexture("TerrainNormalMap", _T("../../Assets/Base_Texture_Normal.jpg"), PS_SLOT_NORMALMAP, BIND_PS);
	CreateTexture("FBX", _T("../../Assets/Model/Test/03_Monster/tex_Zombunny_diffuse.png"), PS_TEXTURE, BIND_PS);

}

void CResourceManager::CreateRenderShaders() {
	//default
	CreateRenderShader("DEFAULT", TEXT("Default"),
		IE_POSITION | IE_NORMAL | IE_TEXCOORD | IE_TANGENT | IE_BINORMAL |IE_INSWORLDMTX);
	//default

	//animation render shader
	CreateRenderShader("AnimationObject", TEXT("AnimationObject"),
		IE_POSITION | IE_NORMAL | IE_TEXCOORD | IE_TANGENT | IE_BINORMAL | IE_BONEWEIGHT | IE_BONEINDEX | IE_INSWORLDMTX);
	//animation render shader

	//post processing shader
	CreateRenderShader("PostProcessing", L"PostProcessing",
		IE_POSITION | IE_TEXCOORD);
	//post processing shader
	//terrain
	CreateRenderShader("Terrain", L"Terrain",
		//IE_POSITION | IE_INSWORLDMTX| IE_TANGENT | IE_BITANGENT,
		IE_POSITION | IE_INSWORLDMTX,
		BIND_VS | BIND_HS | BIND_DS | BIND_PS);
	//terrain
	//tessplane
	CreateRenderShader("WaterPlane", L"WaterPlane",
		IE_POSITION | IE_INSWORLDMTX,
		BIND_VS | BIND_HS | BIND_DS | BIND_PS);

	CreateRenderShader("Water", L"TessPlane", 
		IE_POSITION | IE_INSWORLDMTX,
		BIND_VS | BIND_HS | BIND_DS | BIND_PS);

	CreateRenderShader("SSRF", L"SSRF",
		0, BIND_VS | BIND_PS);
	//tessplane
	//BoundingBox
	CreateRenderShader("BoundingBox", L"BoundingBox",
		IE_INSPOS | IE_INSFLOAT_D_A | IE_INSQUATERNION,
		BIND_VS | BIND_GS | BIND_PS);
	//BoundingBox

	//debug textue
	CreateRenderShader("DebugTexture", TEXT("DebugTexture"),
		IE_INSFLOAT_B_A | IE_INSFLOAT_B_B,
		BIND_VS | BIND_GS | BIND_PS);
	CreateRenderShader("DebugDepthTexture", TEXT("DebugDepthTexture"),
		IE_INSFLOAT_B_A | IE_INSFLOAT_B_B,
		BIND_VS | BIND_GS | BIND_PS);
	//shader
	CreateRenderShader("SkyBox", TEXT("SkyBox"),
		IE_POSITION | IE_TEXCOORD | IE_INSWORLDMTX);


	//coordinatesys render shader
	CreateRenderShader("CoordinateSys", L"CoordinateSys",
		IE_POSITION | IE_INSWORLDMTX);
	//coordinatesys render shader

	//light shader
	 CRenderShader* pShader = CRenderShader::CreateRenderShader(L"DirectionalLight");
	m_mRenderShader.insert(pairShader("DirectionalLight", pShader));

	pShader = CRenderShader::CreateRenderShader(L"PointLight", 0,
		BIND_VS | BIND_HS | BIND_DS | BIND_PS);
	m_mRenderShader.insert(pairShader("PointLight", pShader));

	LPCTSTR pDebugPointLightShaderName[] = { TEXT("VSPointLight.cso") , TEXT("HSPointLight.cso"), TEXT("DSPointLight.cso"), TEXT("PSDebugLight.cso") };
	pShader = CRenderShader::CreateRenderShader(pDebugPointLightShaderName, 0,
		BIND_VS | BIND_HS | BIND_DS | BIND_PS);
	m_mRenderShader.insert(pairShader("DebugPointLight", pShader));

	pShader = CRenderShader::CreateRenderShader(L"CapsuleLight", 0,
		BIND_VS | BIND_HS | BIND_DS | BIND_PS);
	m_mRenderShader.insert(pairShader("CapsuleLight", pShader));

	LPCTSTR pDebugCapsuleLightShaderName[] = { TEXT("VSCapsuleLight.cso") , TEXT("HSCapsuleLight.cso"), TEXT("DSCapsuleLight.cso"), TEXT("PSDebugLight.cso") };
	pShader = CRenderShader::CreateRenderShader(pDebugCapsuleLightShaderName, 0,
		BIND_VS | BIND_HS | BIND_DS | BIND_PS);
	m_mRenderShader.insert(pairShader("DebugCapsuleLight", pShader));

	pShader = CRenderShader::CreateRenderShader(L"SpotLight", 0,
		BIND_VS | BIND_HS | BIND_DS | BIND_PS);
	m_mRenderShader.insert(pairShader("SpotLight", pShader));

	LPCTSTR pDebugSpotLightShaderName[] = { TEXT("VSSpotLight.cso") , TEXT("HSSpotLight.cso"), TEXT("DSSpotLight.cso"), TEXT("PSDebugLight.cso") };
	pShader = CRenderShader::CreateRenderShader(pDebugSpotLightShaderName, 0,
		BIND_VS | BIND_HS | BIND_DS | BIND_PS);
	m_mRenderShader.insert(pairShader("DebugSpotLight", pShader));
	//light shader

	//lay trace shader
	pShader = CRenderShader::CreateRenderShader(L"RayTrace");
	m_mRenderShader.insert(pairShader("RayTrace", pShader));
	//combine shader
	pShader = CRenderShader::CreateRenderShader(L"Combine");
	m_mRenderShader.insert(pairShader("Combine", pShader));
}

void CResourceManager::CreateMeshs() {
	//mesh
	 CMesh* pMesh;

	pMesh = new CTestMesh;
	pMesh->Begin();
	m_mvMesh["Rect1"].push_back(pMesh);

	pMesh = new CTestMesh;
	pMesh->Begin();
	m_mvMesh["Rect2"].push_back(pMesh);

	pMesh = new CTestMesh;
	pMesh->Begin();
	m_mvMesh["Rect3"].push_back(pMesh);

	pMesh = new CTestMesh;
	pMesh->Begin();
	m_mvMesh["Rect4"].push_back(pMesh);

	pMesh = new CTestMesh;
	pMesh->Begin();
	m_mvMesh["Rect5"].push_back(pMesh);

	pMesh = new CPlaneMesh;
	pMesh->Begin();
	m_mvMesh["Plane"].push_back(pMesh);

	pMesh = new CDirectionalLightMesh;
	pMesh->Begin();
	m_mvMesh["DirectionalLight"].push_back(pMesh);

	pMesh = new CPointLightMesh;
	pMesh->Begin();
	m_mvMesh["PointLight"].push_back(pMesh);

	pMesh = new CSpotLightMesh;
	pMesh->Begin();
	m_mvMesh["SpotLight"].push_back(pMesh);

	pMesh = new CCapsuleLightMesh;
	pMesh->Begin();
	m_mvMesh["CapsuleLight"].push_back(pMesh);

	pMesh = new CTestDeferredMesh;
	pMesh->Begin();
	m_mvMesh["PostProcessing"].push_back(pMesh);

	pMesh = new CSpaceMesh;
	pMesh->Begin();
	m_mvMesh["Space"].push_back(pMesh);

	pMesh = new CBoundingBoxMesh;
	pMesh->Begin();
	m_mvMesh["BoundingBox"].push_back(pMesh);

	pMesh = new CTessPlaneMesh;
	pMesh->Begin();
	m_mvMesh["Water"].push_back(pMesh);

	//debug
	pMesh = new CDebugTextureMesh;
	pMesh->Begin();
	m_mvMesh["DebugTexture"].push_back(pMesh);

	pMesh = new CCoordinateSysMesh;
	pMesh->Begin();
	m_mvMesh["CoordinateSys"].push_back(pMesh);
	//skybox
	pMesh = new CSkyBoxMesh;
	pMesh->Begin();
	m_mvMesh["SkyBox"].push_back(pMesh);
	
	//navimesh
	pMesh = new CNaviMesh;
	pMesh->Begin();
	m_mvMesh["NaviMesh"].push_back(pMesh);

	//#ifdef USE_ANIM
	//	 CFileBasedMesh* pTestFBXMesh = newCFileBasedMesh;
	//#else
	//	 CUseFBXMesh* pTestFBXMesh = newCUseFBXMesh;
	//#endif


	//ddd
	//CreateMultiMesh("../../Assets/Model/fbx/1-2/Die_85.fbx", "Test");
	//CreateAnimater("../../Assets/Model/fbx/1-1/ATK1_45.fbx");

	//CreateMultiMesh("../../Assets/Model/fbx/Bless_Elf.fbx");
	//CreateMultiMesh("../../Assets/Model/fbx/2-1/ATK1_45.fbx");


	//pTestFBXMesh->Begin("../../Assets/Model/fbx/Bless_Elf.fbx");
	//pTestFBXMesh->Begin("../../Assets/Model/Test/02_Character_Juno/Juno_idle.fbx");
	//pTestFBXMesh->Begin("../../Assets/Model/Test/unit04_attack.fbx");
	//pTestFBXMesh->Begin("../../Assets/Model/Test/assback3_11_attack.fbx");
	//pTestFBXMesh->Begin("../../Assets/Model/Test/character1_move_r.fbx");
	//pTestFBXMesh->Begin("../../Assets/Model/Test/humanoid.fbx");
	//FBXIMPORTER->Begin("../../Assets/Model/Test/03_Monster/Zombunny_running.fbx");
	//pTestFBXMesh->Begin();
	// CAnimater* pAnimater = newCAnimater;
	//pAnimater->Begin();
	//CAnimationInfo* pAnimationInfo = new CAnimationInfo();
	//pAnimationInfo->Begin(pAnimater);
	//m_mAnimater.insert(pairAnimater("BUNNY", pAnimater));
	//FBXIMPORTER->End();
	//
	//m_mMesh.insert(pairMesh("BUNNY", pTestFBXMesh));

	CreateStempMeshs();
	//mesh
}
void CResourceManager::CreateStempMeshs() {

	//RESOURCEMGR->CreateMultiMesh("../../Assets/Model/TestPlayer/test_player.gjm", "test_player");
	//vector<wstring> vFile;
	//DIRECTORYFINDER->GetFiles(vFile, L"../../Assets/SceneResource", true, true, L".gjm");
	//DIRECTORYFINDER->GetFiles(vFile, L"../../Assets/SceneResource", true, true, L".GJM");
	//
	//int i{ 0 };
	//string name;
	//for (auto fileName : vFile) {
	//	string sPath{ "" };
	//	sPath.assign(fileName.begin(), fileName.end());
	//	/*여기서 file 이름을 가지고 name을 정하도록 한다. 
	//	file 명에서 확장자를 제거하면 될 것이다.
	//	sprintf(name, "파 일 명", i);*/
	//	name = GetFileName(sPath);
	//	CreateMultiMesh(sPath, name);
	//	//sprintf(name, "StempMesh%d", i++);
	//}
}
void CResourceManager::CreateBuffers() {
	//instance buffer
	 CBuffer* pConstantBuffer;

	CreateInstancingBuffer("DEFAULTIB", 1000, sizeof(VS_VB_INSTANCE));

	CreateInstancingBuffer("50000IB", 50000, sizeof(VS_VB_INSTANCE));

	CreateInstancingBuffer("ONEIB", 1, sizeof(VS_VB_INSTANCE));

	//CreateInstancingBuffer("SpaceIB", (UINT)SPACE_NUM, sizeof(VS_VB_INSTANCE));
	//CreateInstancingBuffer("TerrainIB", (UINT)SPACE_NUM, sizeof(VS_VB_INSTANCE));

	CreateInstancingBuffer("BoundingBoxIB", BOUNDINGBOX_NUM, sizeof(VS_VB_BOUNDINGBOX_INSTANCE));
	CreateInstancingBuffer("CoordinateSysIB", COORD_NUM, sizeof(VS_VB_INSTANCE));
	//debug texture
	CreateInstancingBuffer("DebugTextureIB", 1, sizeof(VS_VB_DEBUG_TEXTURE_INSTANCE));
	//instance buffer
	//test fbx
	CreateConstantBuffer("FBX", 1, sizeof(VS_VB_INSTANCE), VS_CB_MODEL, BIND_VS);
	//light buffer
	CreateConstantBuffer("DirectionalLightCB", 1, sizeof(DIRECTIONAL_AMBIENT_LIGHT), PS_OBJECT_BUFFER_SLOT, BIND_PS);
	CreateConstantBuffer("PointLightCB1", 1000, sizeof(POINT_LIGHT_DS_CB), DS_OBJECT_BUFFER_SLOT, BIND_DS);
	CreateConstantBuffer("PointLightCB2", 1000, sizeof(POINT_LIGHT_PS_CB), PS_OBJECT_BUFFER_SLOT, BIND_PS);
	CreateConstantBuffer("SpotLightCB1", 1000, sizeof(SPOT_LIGHT_DS_CB), DS_OBJECT_BUFFER_SLOT, BIND_DS);
	CreateConstantBuffer("SpotLightCB2", 1000, sizeof(SPOT_LIGHT_PS_CB), PS_OBJECT_BUFFER_SLOT, BIND_PS);
	CreateConstantBuffer("CapsuleLightCB1", 1000, sizeof(CAPSULE_LIGHT_DS_CB), DS_OBJECT_BUFFER_SLOT, BIND_DS);
	CreateConstantBuffer("CapsuleLightCB2", 1000, sizeof(CAPSULE_LIGHT_PS_CB), PS_OBJECT_BUFFER_SLOT, BIND_PS);

	CreateConstantBuffer("TerrainCB", 1, sizeof(TERRAIN_GLOBAL_VALUE), VS_GLOBAL_BUFFER_SLOT, BIND_VS | BIND_DS);
//	CreateConstantBuffer("SpaceCB", 1, sizeof(SPACE_GLOBAL_VALUE), 11, BIND_PS);

}

void CResourceManager::CreateGlobalBuffers() {
	
}

void CResourceManager::CreateMaterials() {

	 CMaterial* pMaterial;
	//material
	CreateMaterial("DEFAULT", XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 1.0f, 1.0f);
	CreateMaterial("RED", XMFLOAT4(1.f, 0.f, 0.f, 1.0f), 1.0f, 1.0f);
	CreateMaterial("Core", XMFLOAT4(0.5f, 0.1f, 0.1f, 1.0f), 4.0f, 1.0f);
	CreateMaterial("Boost", XMFLOAT4(0.1f, 0.6f, 0.6f, 1.0f), 4.0f, 1.0f);
	CreateMaterial("Weapon", XMFLOAT4(0.1f, 0.7f, 0.0f, 1.0f), 4.0f, 1.0f);
	CreateMaterial("Adaptor", XMFLOAT4(0.6f, 0.5f, 0.1f, 1.0f), 4.0f, 1.0f);
	CreateMaterial("Plane", XMFLOAT4(0.5f, 0.5f, 0.0f, 1.0f), 8.0f, 2.0f);
	CreateMaterial("Terrain", XMFLOAT4(0.25f, 0.25f, 0.25f, 1.0f), 1.0f, 1.0f);
	CreateMaterial("BoundingBox", XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 8.0f, 2.0f);
	CreateMaterial("FBX", XMFLOAT4(2.0f, 2.0f, 2.0f, 2.0f), 1.f, 1.f);
	CreateMaterial("BUNNY", XMFLOAT4(1.0f, 1.f, 1.f, 1.f), 1.f, 1.f);
	CreateMaterial("SkyBox", XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 1.f, 1.f);
}
void CResourceManager::CreateAnimaters() {

}
void CResourceManager::CreateGJMResources() {
	//WCHAR*pwchar;
	//IMPORTER->ReadWCHAR(pwchar, 0);
}
void CResourceManager::CreateFBXResources() {

}

 CTexture* CResourceManager::CreateTexture(string name, UINT nTextures, _TCHAR(*ppstrFilePaths)[128], UINT Slot, UINT BindFlag,  CBuffer* pConstantBuffer) {
	 CTexture* pTexture = CTexture::CreateTexture(nTextures, ppstrFilePaths, Slot, BindFlag, pConstantBuffer);
	if (m_mTexture.find(name) != m_mTexture.end()) {
		m_mTexture[name]->End();
		delete m_mTexture[name];
		m_mTexture.erase(name);
	}
	m_mTexture.insert(pairTexture(name, pTexture));
	return m_mTexture[name];
}

 CTexture* CResourceManager::CreateTexture(string name, UINT nTextures, ID3D11Texture2D ** ppd3dTextures, UINT Slot, UINT BindFlag,  CBuffer* pConstantBuffer) {
	 CTexture* pTexture = CTexture::CreateTexture(nTextures, ppd3dTextures, Slot, BindFlag, pConstantBuffer);
	if (m_mTexture.find(name) != m_mTexture.end()) {
		m_mTexture[name]->End();
		delete m_mTexture[name];
		m_mTexture.erase(name);
	}
	m_mTexture.insert(pairTexture(name, pTexture));
	return m_mTexture[name];
}

 CTexture* CResourceManager::CreateTexture(string name, _TCHAR(pstrFilePath)[128], UINT Slot, UINT BindFlag,  CBuffer* pConstantBuffer) {
	 CTexture* pTexture = CTexture::CreateTexture(pstrFilePath, Slot, BindFlag, pConstantBuffer);
	if (m_mTexture.find(name) != m_mTexture.end()) {
		m_mTexture[name]->End();
		delete m_mTexture[name];
		m_mTexture.erase(name);
	}
	m_mTexture.insert(pairTexture(name, pTexture));
	return m_mTexture[name];
}

 CTexture* CResourceManager::CreateTexture(string name, ID3D11ShaderResourceView * pShaderResourceView, UINT Slot, UINT BindFlag,  CBuffer* pConstantBuffer) {
	 CTexture* pTexture = CTexture::CreateTexture(pShaderResourceView, Slot, BindFlag, pConstantBuffer);
	if (m_mTexture.find(name) != m_mTexture.end()) {
		m_mTexture[name]->End();
		delete m_mTexture[name];
		m_mTexture.erase(name);
	}
	m_mTexture.insert(pairTexture(name, pTexture));
	return m_mTexture[name];
}

 CSampler* CResourceManager::CreateSampler(string name, UINT Slot, UINT BindFlags, D3D11_TEXTURE_ADDRESS_MODE Mode, D3D11_FILTER Filter, D3D11_COMPARISON_FUNC ComparisionFunc, float MinLOD, float MaxLOD, float BorderColor) {
	 CSampler* pSampler = CSampler::CreateSampler(Slot, BindFlags, Mode, Filter, ComparisionFunc, MinLOD, MaxLOD, BorderColor);
	if (m_mSampler.find(name) != m_mSampler.end()) {
		m_mSampler[name]->End();
		delete m_mTexture[name];
		m_mSampler.erase(name);
	}
	m_mSampler.insert(pairSampler(name, pSampler));
	return m_mSampler[name];
}

 CRenderShader* CResourceManager::CreateRenderShader(string name, LPCTSTR ShaderName, UINT InputElementFlag, UINT BindFlag) {
	 CRenderShader* pShader = CRenderShader::CreateRenderShader(ShaderName, InputElementFlag, BindFlag);

	if (m_mRenderShader.find(name) != m_mRenderShader.end()) {
		m_mRenderShader[name]->End();
		delete m_mRenderShader[name];
		m_mRenderShader.erase(name);
	}
	m_mRenderShader.insert(pairShader(name, pShader));
	return m_mRenderShader[name];
}

 CBuffer* CResourceManager::CreateConstantBuffer(string name, UINT nObject, UINT BufferStride, UINT Slot, UINT BindFlag, UINT Offset) {
	 CBuffer* pBuffer = CBuffer::CreateConstantBuffer(nObject, BufferStride, Slot, BindFlag, Offset);
	if (m_mBuffer.find(name) != m_mBuffer.end()) {//있으면 기존의것을 대체
		m_mBuffer[name]->End();
		delete m_mBuffer[name];
		m_mBuffer.erase(name);
	}

	m_mBuffer.insert(pairBuffer(name, pBuffer));

	return m_mBuffer[name];
}

 CBuffer* CResourceManager::CreateInstancingBuffer(string name, UINT nObject, UINT BufferStride, UINT Offset) {
	 CBuffer* pBuffer = CBuffer::CreateInstancingBuffer(nObject, BufferStride, Offset);
	if (m_mBuffer.find(name) != m_mBuffer.end()) {//있으면 기존의것을 대체
		m_mBuffer[name]->End();
		delete m_mBuffer[name];
		m_mBuffer.erase(name);
	}
	m_mBuffer.insert(pairBuffer(name, pBuffer));
	return m_mBuffer[name];
}

 CBuffer* CResourceManager::CreateGlobalBuffer(string name, UINT nObject, UINT BufferStride, UINT Slot, UINT BindFlag, UINT Offset) {
	 CBuffer* pBuffer = CBuffer::CreateConstantBuffer(nObject, BufferStride, Slot, BindFlag, Offset);
	if (m_mGlobalBuffer.find(name) != m_mGlobalBuffer.end()) {
		m_mGlobalBuffer[name]->End();
		delete m_mGlobalBuffer[name];
		m_mGlobalBuffer.erase(name);
	}
	m_mGlobalBuffer.insert(pairBuffer(name, pBuffer));
	return m_mGlobalBuffer[name];
}

 CMaterial* CResourceManager::CreateMaterial(string name, XMFLOAT4 color, float specExp, float specIntensity) {
	 CMaterial* pMaterial = CMaterial::CreateMaterial(color, specExp, specIntensity);;
	if (m_mMaterial.find(name) != m_mMaterial.end()) {
		m_mMaterial[name]->End();
		delete m_mMaterial[name];
		m_mMaterial.erase(name);
	}
	m_mMaterial.insert(pairMaterial(name, pMaterial));
	return m_mMaterial[name];
}

void CResourceManager::CreateAnimater(string path, string animaterName) {

}

UINT CResourceManager::CreateMultiMesh(string path, string name) {
	if (m_mvStempMesh.find(name) != m_mvStempMesh.end()) {
		for (auto pMesh : m_mvStempMesh[name]) {
			pMesh->End();
			delete pMesh;
		}
		m_mvStempMesh[name].clear();
		m_mvStempMesh.erase(name);
	}

	wstring ws{ L"" };
	ws.assign(path.cbegin(), path.cend());
	wstring extention{ PathFindExtension(ws.c_str()) };

	if (L".FBX" == extention || L".fbx" == extention) {
		return CreateFBXResource(path, name);
	}
	else if (L".GJM" == extention || L".gjm" == extention || L".txt" == extention) {
		return CreateGJMResource(path, name);
	}
}
UINT CResourceManager::CreateGJMResource(string path, string name) {
	IMPORTER->Begin(path);
	char pName[256];

	bool bHasAnimation = IMPORTER->ReadBool();

	 CMesh* pMesh;
	UINT nMeshCnt = IMPORTER->ReadUINT();
	tag t = (tag)IMPORTER->ReadUINT();
	for (UINT i = 0; i < nMeshCnt; ++i) {
		sprintf(pName, "%s", name.c_str());
		pMesh = CFileBasedMesh::CreateMeshFromGJMFile(pName, i, bHasAnimation);
		pMesh->SetTag(t);
		m_mvStempMesh[pName].push_back(pMesh);
	}
	if (false == bHasAnimation) {
		IMPORTER->End();
		return nMeshCnt;
	}

	//animater
	sprintf(pName, "%s", name.c_str());
	 CAnimater* pAnimater = CAnimater::CreateAnimaterFromGJMFile();
	m_mAnimater.insert(pairAnimater(pName, pAnimater));

	//animation info
	int animationCnt = IMPORTER->ReadInt();
	for (int i = 0; i < animationCnt; ++i) {
		CAnimationInfo* pAnimationInfo = CAnimationInfo::CreateAnimationInfoFromGJMFile(pAnimater);
	}//end animation info for

	IMPORTER->End();
	return nMeshCnt;

}
UINT CResourceManager::CreateFBXResource(string path, string name) {
	string sPath{ "" };
	sPath.assign(path.cbegin(), path.cend());

	//multi mesh data load
	FBXIMPORTER->Begin(sPath);
	char pName[20];

	 CFileBasedMesh* pFBXMesh;
	//	int i = FBXIMPORTER->GetMeshCnt();
	if (FBXIMPORTER->GetHasAnimation()) {

		for (UINT i = 0; i < FBXIMPORTER->GetMeshCnt(); ++i) {
			sprintf(pName, "%s", name.c_str());
			pFBXMesh = CFileBasedMesh::CreateMeshFromFBXFile(name, i);
			pFBXMesh->SetTag(TAG_DYNAMIC_OBJECT);
			m_mvStempMesh[pName].push_back(pFBXMesh);
		}

		sprintf(pName, "%s", name.c_str());
		 CAnimater* pAnimater = CAnimater::CreateAnimaterFromFBXFile(true);
		CAnimationInfo* pAnimationInfo = CAnimationInfo::CreateAnimationInfoFromFBXFile(pAnimater);
		m_mAnimater.insert(pairAnimater(pName, pAnimater));
	}
	else {
		for (UINT j = 0; j < FBXIMPORTER->GetMeshCnt(); ++j) {
			sprintf(pName, "%s", name.c_str());
			pFBXMesh = CFileBasedMesh::CreateMeshFromFBXFile(name, j);
			pFBXMesh->SetTag(TAG_STATIC_OBJECT);
			m_mvStempMesh[pName].push_back(pFBXMesh);
		}
	}

	int meshCnt = FBXIMPORTER->GetMeshCnt();
	FBXIMPORTER->End();
	return meshCnt;
}

void CResourceManager::CreateTerrainMesh(float fOneSpaceSize, string name) {
	if (m_mvMesh.find(name) != m_mvMesh.end()) {
		for (auto pMesh : m_mvMesh[name]) {
			pMesh->End();
			delete pMesh;
		}
		m_mvMesh[name].clear();
	}
	m_mvMesh[name].push_back(CTerrainMesh::CreateTerrainMesh(fOneSpaceSize));
}

//Release
void CResourceManager::ReleaseSamplers() {
	for (auto data : m_mSampler) {
		if (data.second) {
			data.second->CleanShaderState();
			data.second->End();
			delete data.second;
		}
	}
	m_mSampler.clear();
}

void CResourceManager::ReleaseTextures() {
	for (auto data : m_mTexture) {
		if (data.second) data.second->End();
		delete data.second;
	}
	m_mTexture.clear();
}

void CResourceManager::ReleaseRenderShaders() {
	for (auto data : m_mRenderShader) {
		if (data.second)data.second->End();
		delete data.second;
	}
	m_mRenderShader.clear();
}

void CResourceManager::ReleaseMeshs() {
	for (auto vMesh : m_mvMesh) {
		for (auto pMesh : vMesh.second) {
			if (pMesh) {
				pMesh->End();
				delete pMesh;
			}
		}
		vMesh.second.clear();
	}
	m_mvMesh.clear();
}

void CResourceManager::ReleaseStempMeshs() {
	for (auto vMesh : m_mvStempMesh) {
		for (auto pMesh : vMesh.second) {
			if (pMesh) {
				pMesh->End();
				delete pMesh;
			}
			
		}
		vMesh.second.clear();
	}
	m_mvStempMesh.clear();
}

void CResourceManager::ReleaseAllResource(){
	ReleaseTextures();
	ReleaseStempMeshs();      //gjm
	ReleaseAnimaters();
}

void CResourceManager::ReleaseMesh(string name) {
	map<string, vector< CMesh*>> ::iterator iter = m_mvMesh.find(name);
	if (iter != m_mvMesh.end()) {
		for (auto pMesh : m_mvMesh[name]) {
			pMesh->End();
			delete pMesh;
		}
		m_mvMesh[name].clear();
	}
	//delete iter->second;
	m_mvMesh.erase(iter);
}

void CResourceManager::ReleaseStempMesh(string name) {
	map<string, vector< CMesh*>> ::iterator iter = m_mvStempMesh.find(name);
	if (iter != m_mvStempMesh.end()) {
		for (auto pMesh : m_mvStempMesh[name]) {
			pMesh->End();
			delete pMesh;
		}
		m_mvStempMesh[name].clear();
	}
	//delete iter->second;
	m_mvMesh.erase(iter);
}

void CResourceManager::ReleaseAnimater(string name) {
	map<string,  CAnimater*> ::iterator iter = m_mAnimater.find(name);
	(iter->second)->End();
	m_mAnimater.erase(iter);
	delete iter->second;
}

void CResourceManager::ReleaseBuffers() {
	for (auto data : m_mBuffer) {
		if (data.second)data.second->End();
		delete data.second;
	}
	m_mBuffer.clear();
}

void CResourceManager::ReleaseGlobalBuffers() {
	for (auto data : m_mGlobalBuffer) {
		if (data.second)data.second->End();
		delete data.second;
	}
	m_mGlobalBuffer.clear();
}

void CResourceManager::ReleaseMaterials() {
	for (auto data : m_mMaterial) {
		if (data.second)data.second->End();
		delete data.second;
	}
	m_mMaterial.clear();
}

void CResourceManager::ReleaseAnimaters() {
	for (auto data : m_mAnimater) {
		if (data.second)data.second->End();
		delete data.second;
	}
	m_mAnimater.clear();
}
//Release
//Release
CResourceManager::CResourceManager() :CSingleTonBase<CResourceManager>("resourcemanager") {

}

CResourceManager::~CResourceManager() {

}
