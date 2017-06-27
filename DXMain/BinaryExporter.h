#pragma once
#include "FileExporter.h"

//File Exporter가 사용하는 객체
class CBinaryExporter :public CFileExporter {
public:
	void WriteWstring(const wstring &data);
	//void WriteCHAR(wofstream& out, const char* data, int size);
	void WriteUINT(const UINT& data);
	void WriteInt(const int& data);
	void WriteFloat(const float& data);

	//txt file 사용할 경우 각 인자의 경계
	void WriteSpace();
	void WriteEnter();
	void WriteBool(bool& b);
private:

public:
	CBinaryExporter(wstring outputPath);
	virtual ~CBinaryExporter();
};
