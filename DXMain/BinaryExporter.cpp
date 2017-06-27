#include "stdafx.h"
#include "BinaryExporter.h"


void CBinaryExporter::WriteWstring(const wstring& data) {
	string tmp;
	tmp.assign(data.begin(), data.end());

	m_bitOut.write(reinterpret_cast<const char*>(tmp.c_str()), sizeof(char) * data.size() + 1);
}

//void CTxtExporter::WriteWCHAR(wofstream& out, const WCHAR * data, int size) {
//	out << data;
//}

void CBinaryExporter::WriteUINT(const UINT& data) {
	m_bitOut.write(reinterpret_cast<const char*>(&data), sizeof(UINT));
}

void CBinaryExporter::WriteInt(const int& data) {
	m_bitOut.write(reinterpret_cast<const char*>(&data), sizeof(int));
}

void CBinaryExporter::WriteFloat(const float& data) {
	m_bitOut.write(reinterpret_cast<const char*>(&data), sizeof(float));
}

void CBinaryExporter::WriteSpace() {
	
}

void CBinaryExporter::WriteEnter() {
	
}

void CBinaryExporter::WriteBool(bool& b) {
	m_bitOut.write(reinterpret_cast<char*>(&b), sizeof(bool));
}

CBinaryExporter::CBinaryExporter(wstring outputPath) : CFileExporter(outputPath) {
	m_out = wofstream(outputPath, ios::binary);
	m_bitOut = ofstream(outputPath, ios::binary);
}

CBinaryExporter::~CBinaryExporter() {

}
