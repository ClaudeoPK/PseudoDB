#pragma once
#include "CElement.h"

class CLong : public CElement {
public:
	unsigned long getData() const {
		return m_data;
	}
	void setData(unsigned long a) {
		m_data = a;
	}
	void doImport(PBINARY_EXPORT pOut) {
		m_data = *(unsigned long*)pOut->data;
	}
	BINARY_EXPORT doExport() {
		return BINARY_EXPORT{ sizeof(m_data), &m_data };
	}
public:
	std::string getDataS() {
		return std::to_string(m_data);
	}
	void setDataS(std::string& data) {
		m_data = std::stoul(data);
	}
private:
	unsigned long m_data;
};