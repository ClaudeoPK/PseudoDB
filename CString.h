#pragma once
#include "CElement.h"

class CString : public CElement {
public:
	std::string getData() const {
		return m_data;
	}
	void setData(std::string& a) {
		m_data = a;
	}
	void doImport(PBINARY_EXPORT pOut) {
		char* tmp = (char*)pOut->data;
		m_data = tmp;
	}
	BINARY_EXPORT doExport() {
		return BINARY_EXPORT{ (int)m_data.size() + 1 , (void*)m_data.c_str() };
	}
public:
	std::string getDataS() {
		return m_data;
	}
	void setDataS(std::string& data) {
		m_data = data;
	}
private:
	std::string m_data;
};