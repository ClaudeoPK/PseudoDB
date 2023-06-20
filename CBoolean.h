#pragma once
#include "CElement.h"

class CBoolean : public CElement {
public:
	bool getData() const {
		return m_data;
	}
	void setData(bool a) {
		m_data = a;
	}
	void doImport(PBINARY_EXPORT pOut) {
		m_data = *(bool*)pOut->data;
	}
	BINARY_EXPORT doExport() {
		return BINARY_EXPORT{ sizeof(m_data), &m_data };
	}
public:
	std::string getDataS() {
		return std::to_string((int)m_data);
	}
	void setDataS(std::string& data) {
		m_data = (bool)std::stoi(data);
	}
private:
	bool m_data;
};