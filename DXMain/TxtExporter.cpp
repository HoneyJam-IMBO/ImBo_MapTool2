#include "stdafx.h"
#include "TxtExporter.h"


void CTxtExporter::WriteWstring(const wstring& data){
	m_out << data.size() << ' ';
	m_out << data.c_str();
}

//void CTxtExporter::WriteWCHAR(wofstream& out, const WCHAR * data, int size) {
//	out << data;
//}

void CTxtExporter::WriteUINT(const UINT& data) {
	m_out << data;
}

void CTxtExporter::WriteInt(const int& data) {
	m_out << data;
}

void CTxtExporter::WriteFloat(const float& data) {
	m_out << data;
}

void CTxtExporter::WriteSpace() {
	m_out << L" ";
}

void CTxtExporter::WriteEnter() {
	m_out << L"\n";
}

void CTxtExporter::WriteBool(bool& b){
	m_out << b;
}

CTxtExporter::CTxtExporter(wstring outputPath) : CFileExporter(outputPath){
	m_out = wofstream(outputPath, ios::out);
}

CTxtExporter::~CTxtExporter(){

}
