#include "stdafx.h"
#include "Importer.h"

bool CImporter::Begin(string outputPath) {
	m_pFileImporter = new CTxtImporter(outputPath);

	return true;
}

bool CImporter::End() {
	delete m_pFileImporter;
	m_pFileImporter = nullptr;

	return true;
}

//const WCHAR* CImporter::ReadWCHAR() {
//	wstring ws = m_pFileImporter->ReadWstring(m_in);
//	return ws.c_str();
//}
//
//const char* CImporter::ReadCHAR() {
//	wstring ws = m_pFileImporter->ReadWstring(m_in);
//	string s;
//	s.assign(ws.begin(), ws.end());
//	return s.c_str();
//}
const wstring CImporter::ReadWstring() {
	string s = m_pFileImporter->Readstring();
	wstring ws{ L"" };
	ws.assign(s.cbegin(), s.cend());
	return ws;
}

const string CImporter::Readstring() {
	string s = m_pFileImporter->Readstring();
	return s;
}
UINT CImporter::ReadUINT() {
	return m_pFileImporter->ReadUINT();
}

int CImporter::ReadInt() {
	return m_pFileImporter->ReadInt();
}

float CImporter::ReadFloat() {
	return m_pFileImporter->ReadFloat();
}

XMFLOAT4X4 CImporter::ReadFloat4x4() {
	XMFLOAT4X4 xmf4x4;
	xmf4x4._11 = ReadFloat(); xmf4x4._12 = ReadFloat(); xmf4x4._13 = ReadFloat(); xmf4x4._14 = ReadFloat();
	xmf4x4._21 = ReadFloat(); xmf4x4._22 = ReadFloat(); xmf4x4._23 = ReadFloat(); xmf4x4._24 = ReadFloat();
	xmf4x4._31 = ReadFloat(); xmf4x4._32 = ReadFloat(); xmf4x4._33 = ReadFloat(); xmf4x4._34 = ReadFloat();
	xmf4x4._41 = ReadFloat(); xmf4x4._42 = ReadFloat(); xmf4x4._43 = ReadFloat(); xmf4x4._44 = ReadFloat();
	return xmf4x4;
}

XMMATRIX CImporter::ReadXMMatrix() {
	return XMLoadFloat4x4(&ReadFloat4x4());
}

bool CImporter::ReadBool(){
	return m_pFileImporter->ReadBool();
}

void CImporter::ReadSpace() {
	m_pFileImporter->ReadSpace();
}

void CImporter::ReadEnter() {
	m_pFileImporter->ReadEnter();
}

Pixel24 * CImporter::ReadBitmap24(const WCHAR * fileName, int& width, int& height){
	BITMAPFILEHEADER    bf;
	BITMAPINFOHEADER    bi;

	ifstream in;
	in.open(fileName, ios_base::in | ios_base::binary);
	in.read((char*)&bf, sizeof(BITMAPFILEHEADER));
	in.read((char*)&bi, sizeof(BITMAPINFOHEADER));
	width = bi.biWidth;
	height = bi.biHeight;
	UINT BUFSIZE = bi.biWidth *	bi.biHeight;
	Pixel24* pPixel = new Pixel24[BUFSIZE];
	in.read((char*)pPixel, BUFSIZE*3);
	Pixel24* rgbTemp = new Pixel24[BUFSIZE];
	for (int i = 0; i < BUFSIZE; ++i) {
		rgbTemp[i].r = pPixel[BUFSIZE - 1 - i].r;
		rgbTemp[i].g = pPixel[BUFSIZE - 1 - i].g;
		rgbTemp[i].b = pPixel[BUFSIZE - 1 - i].b;
	}
	delete pPixel;

	in.close();

	return rgbTemp;
}

Pixel24 * CImporter::ReadBitmap24(const char * fileName, int& width, int& height){
	BITMAPFILEHEADER    bf;
	BITMAPINFOHEADER    bi;

	ifstream in;
	in.open(fileName, ios_base::in | ios_base::binary);
	if (in.fail()) return nullptr;

	in.read((char*)&bf, sizeof(BITMAPFILEHEADER));
	in.read((char*)&bi, sizeof(BITMAPINFOHEADER));
	width = bi.biWidth;
	height = bi.biHeight;
	Pixel24* pPixel = new Pixel24[bi.biWidth*bi.biHeight];
	in.read((char*)pPixel, bi.biSizeImage);
	in.close();

	//UINT BUFSIZE = bi.biWidth*bi.biHeight;
	//Pixel24* rgbTemp = new Pixel24[BUFSIZE];
	//for (int i = 0; i < BUFSIZE; ++i) {
	//	rgbTemp[i].r = pPixel[BUFSIZE - 1 - i].r;
	//	rgbTemp[i].g = pPixel[BUFSIZE - 1 - i].g;
	//	rgbTemp[i].b = pPixel[BUFSIZE - 1 - i].b;
	//}
	//delete[] pPixel;

	return pPixel;
}

float * CImporter::ReadBitmap24float(const WCHAR * fileName, int & width, int & height){
	BITMAPFILEHEADER    bf;
	BITMAPINFOHEADER    bi;

	ifstream in;
	in.open(fileName, ios_base::in | ios_base::binary);
	if (in.fail()) return nullptr;

	in.read((char*)&bf, sizeof(BITMAPFILEHEADER));
	in.read((char*)&bi, sizeof(BITMAPINFOHEADER));
	width = bi.biWidth;
	height = bi.biHeight;
	Pixel24* pPixel = new Pixel24[bi.biWidth*bi.biHeight];
	in.read((char*)pPixel, bi.biSizeImage);
	in.close();

	float* pData = new float[bi.biWidth*bi.biHeight];
	vector<float> vTest;
	for (int i = 0; i < width*height; ++i) {
		float value = pPixel[i].b;
		pData[i] = value / 255.f;
		vTest.push_back(pData[i]);
	}
	//UINT BUFSIZE = bi.biWidth*bi.biHeight;
	//Pixel24* rgbTemp = new Pixel24[BUFSIZE];
	//for (int i = 0; i < BUFSIZE; ++i) {
	//	rgbTemp[i].r = pPixel[BUFSIZE - 1 - i].r;
	//	rgbTemp[i].g = pPixel[BUFSIZE - 1 - i].g;
	//	rgbTemp[i].b = pPixel[BUFSIZE - 1 - i].b;
	//}
	delete[] pPixel;

	return pData;
	return nullptr;
}

float * CImporter::ReadBitmap24float(const char * fileName, int & width, int & height)
{
	return nullptr;
}

Pixel32 * CImporter::ReadBitmap32(const WCHAR * fileName, int& width, int& height){
	BITMAPFILEHEADER    bf;
	BITMAPINFOHEADER    bi;

	ifstream in;
	in.open(fileName, ios_base::in | ios_base::binary);
	in.read((char*)&bf, sizeof(BITMAPFILEHEADER));
	in.read((char*)&bi, sizeof(BITMAPINFOHEADER));
	width = bi.biWidth;
	height = bi.biHeight;
	char* pData = new char[bi.biSizeImage];
	Pixel32* pPixel = new Pixel32[bi.biWidth*bi.biHeight];
	in.read((char*)pPixel, bi.biSizeImage);
	in.close();

	return nullptr;
}
