#pragma once
#include "shared.h"
#include "CRow.h"
class CTable {
public:
	~CTable() {
		for (int i = 0; i < m_rows.size(); i++) {
			delete m_rows[i];
		}
	}
	void setName(std::string& name) { m_name = name; }
	std::string getName() const { m_name; }
	int getColumnCount() {
		return m_columnNames.size();
	}
	void executeRoutine(void(*routine)(std::pair<std::string, CElement*>)){
		for (auto& row : m_rows) {
			row->executeRoutine(routine);
		}
	}
	bool hasColumn(std::string& tofind) {
		bool bFound = false;
		for (auto& _element : m_columnNames) {
			if (tofind == _element.first) {
				bFound = true;
				break;
			}
		}
		return bFound;
	}
	bool hasColumn(std::vector<std::string>& tofind) {
		bool result = tofind.size() > 0 ? true : false;
		for (auto& element : tofind) {
			bool bFound = false;
			for (auto& _element : m_columnNames) {
				if (element == _element.first) {
					bFound = true;
					break;
				}
			}
			if (bFound == false) {
				result = false;
				break;
			}
		}
		return result;
	}
	std::vector<CRow*> getRows() {
		return m_rows;
	}
	void setColumns(std::vector<std::pair<std::string, int>>& columnDef) {
		m_columnNames = columnDef;
		return;
	}
	std::vector<std::pair<std::string, int>> getColumns() {
		return m_columnNames;
	}
	std::vector<std::string> getColumnNames() {
		std::vector<std::string> result = {};
		for (auto& a : m_columnNames)
			result.push_back(a.first);
		return result;
	}
	void doImport(void* in) {
		TABLE_INFORMATION table_info;
		uvRead(in,(char*)&table_info, sizeof(TABLE_INFORMATION));
		m_name = table_info.table_name;
		for (int i = 0; i < table_info.col_count; i++) {
			int columnType;
			uvRead(in,(char*)&columnType, sizeof(int));
			int nameLength;
			uvRead(in,(char*)&nameLength, sizeof(int));
			char* nameBuffer = new char[nameLength];
			uvRead(in,(char*)nameBuffer, nameLength);
			m_columnNames.push_back({ std::string(nameBuffer),columnType });
			delete[] nameBuffer;
		}
		for (int i = 0; i < table_info.row_count; i++) {
			CRow* row = new CRow();
			row->doImport(in, m_columnNames);
			m_rows.push_back(row);
		}
	}
	void doExport(void* out) {
		TABLE_INFORMATION table_info;
		strcpy(table_info.table_name, m_name.c_str());
		table_info.col_count = m_columnNames.size();
		table_info.row_count = m_rows.size();
		uvWrite(out,(char*)&table_info, sizeof(TABLE_INFORMATION));
		for (int i = 0; i < table_info.col_count; i++) {
			int columnType = m_columnNames[i].second;
			uvWrite(out,(char*)&columnType, sizeof(int));
			int columnNameLength = m_columnNames[i].first.size() + 1;
			uvWrite(out,(char*)&columnNameLength, sizeof(int));
			uvWrite(out,(char*)m_columnNames[i].first.c_str(), columnNameLength);
		}
		for (int i = 0; i < table_info.row_count; i++) {
			m_rows[i]->doExport(out, m_columnNames);
		}
	}
	bool insertRecord(std::vector<std::string>& values) {
		CRow* pRow = new CRow();
		bool result = true;
		try {
			for (int i = 0; i < m_columnNames.size(); i++) {
				pRow->insertRecord(m_columnNames[i].second, m_columnNames[i].first, values[i]);
			}
		}
		catch (std::invalid_argument&) {
			result = false;
		}
		if (result) {
			m_rows.push_back(pRow);
		}
		else {
			delete pRow;
		}
		return result;
	}
	void deleteRecord(CRow* pRow) {
		m_rows.erase(remove(m_rows.begin(), m_rows.end(), pRow), m_rows.end());
		delete pRow;
	}
private:
	std::string m_name;
	std::vector<std::pair<std::string, int>> m_columnNames;
	std::vector<CRow*> m_rows;
};