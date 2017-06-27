#pragma once
#include <fstream>

//File Exporter가 사용하는 객체
class CFileExporter {
public:
	virtual void WriteWstring(const wstring& data) = 0;
	virtual void WriteUINT(const UINT& data) = 0;
	virtual void WriteInt(const int& data) = 0;
	virtual void WriteFloat(const float& data) = 0;
	virtual void WriteBool(bool& b) = 0;

	//txt file 사용할 경우 각 인자의 경계
	virtual void WriteSpace() = 0;
	virtual void WriteEnter() = 0;
private:

protected:
	wstring m_outputPath;
	wofstream m_out;
	ofstream m_bitOut;

public:
	CFileExporter(wstring outputPath) { m_outputPath = outputPath; };
	virtual ~CFileExporter() { m_out.close(); m_bitOut.close(); };
};

