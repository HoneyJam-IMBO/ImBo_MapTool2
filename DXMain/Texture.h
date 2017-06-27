#pragma once
#include "DXObject.h"
#include "Buffer.h"

class CTexture : public DXObject {
public:
	//texture array
	bool Begin();
	virtual bool End();

	virtual void SetShaderState();
	virtual void CleanShaderState();

	virtual void UpdateShaderState();

	//setter
	void SetsPath(string path) { m_sPath = path; }
	void SetConstantBuffer( CBuffer* pConstantBuffer);
	void SetpTextureSRV(ID3D11ShaderResourceView* pd3dsrvTexture) { m_pd3dsrvTexture = pd3dsrvTexture; }
	void SetTextureSlot(UINT TextureStartSlot) { m_TextureStartSlot = TextureStartSlot; }
	void SetBindFlag(UINT BindFlag) { m_BindFlag = BindFlag; }
	//getter
	ID3D11ShaderResourceView* GetShaderResourceView() { return m_pd3dsrvTexture; }
	string GetsPath() { return m_sPath; }

	static ID3D11ShaderResourceView *CreateTexture2DArraySRV(_TCHAR(*ppstrFilePaths)[128], UINT nTextures);
	static ID3D11ShaderResourceView *CreateTexture2DArraySRV(ID3D11Texture2D **ppd3dTextures, UINT nTextures);
	static ID3D11ShaderResourceView *CreateTexture2DArraySRV(ID3D11ShaderResourceView **ppd3dSRVs, UINT nTextures);

	//texture array
	static  CTexture* CreateTexture(UINT nTextures, _TCHAR(*ppstrFilePaths)[128], UINT Slot = 0, UINT BindFlag = BIND_PS,  CBuffer* pConstantBuffer = nullptr);
	//static  CTexture> CreateTexture(UINT nTextures, WCHAR(*ppstrFilePaths)[128], UINT Slot = 0, UINT BindFlag = BIND_PS,  CBuffer* pConstantBuffer = nullptr);
	static  CTexture* CreateTexture(UINT nTextures, ID3D11Texture2D **ppd3dTextures, UINT Slot = 0, UINT BindFlag = BIND_PS,  CBuffer* pConstantBuffer = nullptr);
	static  CTexture* CreateTexture(UINT nTextures, ID3D11ShaderResourceView **ppd3dSRVs, UINT Slot = 0, UINT BindFlag = BIND_PS,  CBuffer* pConstantBuffer = nullptr);
	//texture 1개
	static  CTexture* CreateTexture(_TCHAR(pstrFilePath)[128], UINT Slot = 0, UINT BindFlag = BIND_PS,  CBuffer* pConstantBuffer = nullptr);
	static  CTexture* CreateTexture(wstring pstrFilePath, UINT Slot = 0, UINT BindFlag = BIND_PS,  CBuffer* pConstantBuffer = nullptr);
	//완성된 srv set
	static  CTexture* CreateTexture(ID3D11ShaderResourceView* pShaderResourceView, UINT Slot = 0, UINT BindFlag = BIND_PS,  CBuffer* pConstantBuffer = nullptr);

protected:
	//texture 1개
	ID3D11ShaderResourceView*		m_pd3dsrvTexture{ nullptr };
	UINT							m_TextureStartSlot{ 0 };
	UINT							m_BindFlag{ 0 };

	string							m_sPath;
	//이 안에 여러개의 dx객체들이 존재함

	//buffer 여러개
	 CBuffer*	m_pConstantBuffer{ nullptr };

public:
	CTexture();
	virtual ~CTexture();

};