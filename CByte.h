#pragma once
#include "CElement.h"

class CByte : public CElement {
public:
	unsigned char getData() const {
		return m_data;
	}
	void setData(unsigned char a) {
		m_data = a;
	}
	void doImport(PBINARY_EXPORT pOut) {
		m_data = *(unsigned char*)pOut->data;
	}
	BINARY_EXPORT doExport() {
		return BINARY_EXPORT{ sizeof(m_data), &m_data };
	}
public:
	std::string getDataS() {
		return std::to_string((unsigned long)m_data);
	}
	void setDataS(std::string& data) {
		m_data = (unsigned char)std::stoul(data);
	}
private:
	unsigned char m_data;
};