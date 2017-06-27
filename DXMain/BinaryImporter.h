#pragma once
#include "FileImporter.h"

//File Exporter가 사용하는 객체
class CBinaryImporter :public CFileImporter {
public:
	virtual string Readstring();
	virtual UINT ReadUINT();
	virtual int ReadInt();
	virtual float ReadFloat();
	virtual bool ReadBool();

	//txt file 사용할 경우 각 인자의 경계
	virtual void ReadSpace();
	virtual void ReadEnter();
private:

public:
	CBinaryImporter(string inputPath);
	~CBinaryImporter();
};
