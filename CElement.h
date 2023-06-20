#pragma once
#include "shared.h"
class CElement {
public:
	virtual void doImport(PBINARY_EXPORT pOut) {}
	virtual BINARY_EXPORT doExport() { return {}; }
public:
	virtual std::string getDataS() { return ""; }
	virtual void setDataS(std::string& data) { }
	ELEMENT_TYPE	getType() const { return (ELEMENT_TYPE)m_type; }
	void			setType(int t) { m_type = (ELEMENT_TYPE)t; }
private:
	int	m_type;
};