#pragma once
#include "shared.h"
#include "CElement.h"
#include "CBoolean.h"
#include "CByte.h"
#include "CLong.h"
#include "CLongLong.h"
#include "CShort.h"
#include "CString.h"
class CRow {
public:
	~CRow() {
		for (auto& e : m_elements) {
			delete e.second;
		}
	}
	int getSize() const { return m_elements.size(); }
	bool hasColumn(std::string& name) const { return m_elements.find(name) != m_elements.end(); }
	CElement* getColumn(std::string& name) { return  hasColumn(name) ? m_elements[name] : NULL; }
	std::vector<std::string> getFieldNames() {
		std::vector<std::string> result = {};
		for (auto& element : m_elements) {
			result.push_back(element.first);
		}
		return result;
	}
	void executeRoutine(void(*routine)(std::pair<std::string, CElement*>)) {
		for (auto& element : m_elements) {
			routine(element);
		}
	}
	void doImport(void* in, std::vector<std::pair<std::string, int>>& names) {
		for (int i = 0; i < names.size(); i++) {
			ELEMENT_INFORMATION element_info;
			uvRead(in, (char*)&element_info, sizeof(ELEMENT_INFORMATION));
			CElement* tmp = NULL;
			switch (element_info.type) {
			case ELEMENT_BOOLEAN:
				tmp = new CBoolean();
				break;
			case ELEMENT_BYTE:
				tmp = new CByte();
				break;
			case ELEMENT_SHORT:
				tmp = new CShort();
				break;
			case ELEMENT_LONG:
				tmp = new CLong();
				break;
			case ELEMENT_LONGLONG:
				tmp = new CLongLong();
				break;
			case ELEMENT_STRING:
				tmp = new CString();
				break;
			default:
				break;
			}
			if (!tmp)
				exit(10);
			tmp->setType(element_info.type);

			int dataLength;
			uvRead(in, (char*)&dataLength, sizeof(int));
			unsigned char* dataBuffer = new unsigned char[dataLength];
			uvRead(in, (char*)dataBuffer, dataLength);
			BINARY_EXPORT pack = { dataLength, dataBuffer };
			tmp->doImport(&pack);
			delete[] dataBuffer;
			m_elements[names[i].first] = tmp;
		}
	}
	void doExport(void* out, std::vector<std::pair<std::string, int>>& names) {
		for (int i = 0; i < names.size(); i++) {
			CElement* pElement = m_elements.at(names[i].first);
			ELEMENT_INFORMATION element_info;
			element_info.type = pElement->getType();
			uvWrite(out, (char*)&element_info, sizeof(ELEMENT_INFORMATION));
			BINARY_EXPORT dataInfo = pElement->doExport();
			uvWrite(out, (char*)&dataInfo.length, sizeof(int));
			uvWrite(out, (char*)dataInfo.data, dataInfo.length);
		}
	}
	void insertRecord(int type, std::string& name, std::string& value) {
		CElement* tmp = NULL;
		switch (type) {
		case ELEMENT_BOOLEAN:
			tmp = new CBoolean();
			break;
		case ELEMENT_BYTE:
			tmp = new CByte();
			break;
		case ELEMENT_SHORT:
			tmp = new CShort();
			break;
		case ELEMENT_LONG:
			tmp = new CLong();
			break;
		case ELEMENT_LONGLONG:
			tmp = new CLongLong();
			break;
		case ELEMENT_STRING:
			tmp = new CString();
			break;
		default:

			return;
			break;
		}
		tmp->setType(type);
		tmp->setDataS(value);
		m_elements[name] = tmp;
	}
	bool updateRecord(std::string& name, std::string& value) {
		if (m_elements.find(name) != m_elements.end()) {
			bool result = true;
			CElement* pElement = m_elements.at(name);
			try {
				pElement->setDataS(value);
			}
			catch (std::invalid_argument&) {
				result = false;
			}
			return result;
		}
		return false;
	}
private:
	std::unordered_map<std::string, CElement*> m_elements;
};