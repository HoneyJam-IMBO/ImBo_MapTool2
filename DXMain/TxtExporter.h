#pragma once
#include "FileExporter.h"

//File Exporter가 사용하는 객체
class CTxtExporter :public CFileExporter{
public:
	virtual void WriteWstring(const wstring& data);
	virtual void WriteUINT(const UINT& data);
	virtual void WriteInt(const int& data);
	virtual void WriteFloat(const float& data);
	virtual void WriteBool(bool& b);

	//txt file 사용할 경우 각 인자의 경계
	void WriteSpace();
	void WriteEnter();
private:
	
public:
	CTxtExporter(wstring outputPath);
	virtual ~CTxtExporter();
};
