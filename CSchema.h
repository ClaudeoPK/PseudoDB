#pragma once
#include "shared.h"
#include "CTable.h"


class CSchema {
public:
	void doImport(void* in) {
		int ntables = 0;
		uvRead(in,(char*)&ntables, sizeof(int));
		for (int i = 0; i < ntables; i++) {
			int tableNameLength = 0;
			uvRead(in,(char*)&tableNameLength, sizeof(int));
			char* tmp = new char[tableNameLength];
			uvRead(in,(char*)tmp, tableNameLength);
			CTable* tmp2 = new CTable();
			tmp2->doImport(in);
			m_tables[tmp] = tmp2;
			delete[] tmp;
		}
	}
	void doExport(void* out) {
		int ntables = m_tables.size();
		uvWrite(out,(char*)&ntables, sizeof(int));
		for (auto& table : m_tables) {
			int tableNameLength = table.first.size() + 1;
			uvWrite(out,(char*)&tableNameLength, sizeof(int));
			uvWrite(out,(char*)table.first.c_str(), tableNameLength);
			table.second->doExport(out);
		}
	}
	CTable* getTableByName(std::string& tableName) const {
		if (m_tables.find(tableName) != m_tables.end()) {
			return m_tables.at(tableName);
		}
		return NULL;
	}
	bool createTable(std::string& tableName, std::vector<std::pair<std::string, int>>& columns) {
		CTable* pTable = new CTable();
		pTable->setName(tableName);
		pTable->setColumns(columns);
		m_tables[tableName] = pTable;
		return true;
	}
	bool dropTable(std::string& tableName) {
		if (m_tables.find(tableName) != m_tables.end()) {
			CTable* pTable = m_tables.at(tableName);
			m_tables.erase(tableName);
			delete pTable;
			return true;
		}
		return true;
	}
private:
	std::unordered_map<std::string, CTable*> m_tables;
};
