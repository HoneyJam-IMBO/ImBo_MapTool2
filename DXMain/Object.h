#pragma once

class CObject {
public:
	//----------------------------object-----------------------------
	bool Begin();
	virtual bool End();
	//----------------------------object-----------------------------

	tag GetTag() { return m_tag; }
	utag GetUTag() { return m_utag; }
	string GetName() { return m_name; }
	void SetUTag(utag ut) { m_utag = ut; }
	void SetTag(tag t) { m_tag = t; }
	void SetName(string name) { m_name = name; }
protected:
	string m_name;
	tag m_tag{ tag::TAG_DEFAULT };
	utag m_utag{ utag::UTAG_DEFAULT };
public:
	CObject(string name, tag t = tag::TAG_DEFAULT, utag ut = utag::UTAG_DEFAULT);
	virtual ~CObject();
};