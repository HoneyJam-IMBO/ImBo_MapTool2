#include "stdafx.h"
#include "Object.h"


bool CObject::Begin() {
	m_name.clear();
	m_tag = tag::TAG_DEFAULT;
	m_utag = utag::UTAG_DEFAULT;

	return true;
}
bool CObject::End() {
	m_name.clear();
	return true;
}
CObject::CObject(string name, tag t, utag ut) : m_name(name), m_tag(t), m_utag(ut) {

}

CObject::~CObject() {

}