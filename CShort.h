#pragma once
#include "CElement.h"

class CShort : public CElement {
public:
	unsigned short getData() const {
		return m_data;
	}
	void setData(unsigned short a) {
		m_data = a;
	}
	void doImport(PBINARY_EXPORT pOut) {
		m_data = *(unsigned short*)pOut->data;
	}
	BINARY_EXPORT doExport() {
		return BINARY_EXPORT{ sizeof(m_data), &m_data };
	}
public:
	std::string getDataS() {
		return std::to_string((unsigned long)m_data);
	}
	void setDataS(std::string& data) {
		m_data = (unsigned short)std::stoul(data);
	}
private:
	unsigned short m_data;
};