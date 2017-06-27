#include "stdafx.h"
#include "BinaryImporter.h"


#define TEXT_BUFF_SIZE 256
string CBinaryImporter::Readstring() {

	int size;
	m_in.read((char*)&size, sizeof(int));
	if (size > TEXT_BUFF_SIZE) return string("size overflow");
	char data[TEXT_BUFF_SIZE];
	m_in.read((char*)&data, size);
	string ws{ data };
	return ws;
}

UINT CBinaryImporter::ReadUINT() {
	UINT data;
	m_in.read((char*)&data, sizeof(UINT));
	return data;
}

int CBinaryImporter::ReadInt() {
	int data;
	m_in.read((char*)&data, sizeof(int));
	return data;
}

float CBinaryImporter::ReadFloat() {
	float data;
	m_in.read((char*)&data, sizeof(float));
	return data;
}

bool CBinaryImporter::ReadBool() {
	bool data;
	m_in.read((char*)&data, sizeof(bool));
	return data;
}

void CBinaryImporter::ReadSpace() {
	//in >> L" ";
}

void CBinaryImporter::ReadEnter() {
	//in >> L"\n";
}

CBinaryImporter::CBinaryImporter(string inputPath) : CFileImporter(inputPath) {
	m_in = ifstream(m_inputPath, ios::in | ios::binary);
}

CBinaryImporter::~CBinaryImporter() {

}
