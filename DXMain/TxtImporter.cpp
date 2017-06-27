#include "stdafx.h"
#include "TxtImporter.h"


#define TEXT_BUFF_SIZE 256
string CTxtImporter::Readstring() {
	UINT size;
	m_in >> size;
	if (size > TEXT_BUFF_SIZE) return string("size overflow");
	char data[TEXT_BUFF_SIZE];
	m_in >> data;
	string ws{ data };
	return ws;
}

UINT CTxtImporter::ReadUINT() {
	UINT data;
	m_in >> data;
	return data;
}

int CTxtImporter::ReadInt() {
	int data;
	m_in >> data;
	return data;
}

float CTxtImporter::ReadFloat() {
	float data;
	m_in >> data;
	return data;
}

bool CTxtImporter::ReadBool(){
	bool data;
	m_in >> data;
	return data;
}

void CTxtImporter::ReadSpace() {
	//in >> L" ";
}

void CTxtImporter::ReadEnter() {
	//in >> L"\n";
}

CTxtImporter::CTxtImporter(string inputPath) : CFileImporter(inputPath) {
	m_in = ifstream(m_inputPath, ios::in);
}

CTxtImporter::~CTxtImporter() {

}
