#pragma once
//#include "Camera.h"
#include "SingleTon.h"
#include "FileImporter.h"
#include "BinaryImporter.h"
#include "TxtImporter.h"
#include <fstream>

class CImporter : public CSingleTonBase<CImporter> {
public:
	bool Begin(string inputPath);//file open하구
	virtual bool End();//file close

	//const WCHAR* ReadWCHAR();
	//const char* ReadCHAR();
	const wstring ReadWstring();
	const string Readstring();
	UINT ReadUINT();
	int ReadInt();
	float ReadFloat();
	XMFLOAT4X4 ReadFloat4x4();
	XMMATRIX ReadXMMatrix();
	bool ReadBool();
	//txt file 사용할 경우 각 인자의 경계
	void ReadSpace();
	void ReadEnter();

	Pixel24* ReadBitmap24(const WCHAR* fileName, int& width, int& height);
	Pixel24* ReadBitmap24(const char* fileName, int& width, int& height);
	float* ReadBitmap24float(const WCHAR* fileName, int& width, int& height);
	float* ReadBitmap24float(const char* fileName, int& width, int& height);
	Pixel32* ReadBitmap32(const WCHAR* fileName, int& width, int& height);
private:
	
	
	CFileImporter* m_pFileImporter{ nullptr };
public:
	CImporter() : CSingleTonBase<CImporter>("importersingleton") {}

};
