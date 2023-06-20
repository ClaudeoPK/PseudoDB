#include "pdStmt.h"
#include "ConditionEval.h"
#include <iostream>
extern CSchema* g_Schema;
constexpr const char* rPatterns[] = { //TODO: 0605 �� Stmt�� ���� �ڵ鷯 ������ ��
	R"(SELECT\s+(.*?)\s+FROM\s+(\w+)\s+WHERE\s+(.*))",
	R"(SELECT\s+(.*?)\s+FROM\s+(\w+))",
	R"(UPDATE\s+(\w+)\s+SET\s+(.+)\s+WHERE\s+(.+))",
	R"(UPDATE\s+(\w+)\s+SET\s+(.+))",
	R"(INSERT\s+INTO\s+(\w+)\s+VALUES\s*\((.+)\))",
	R"(DELETE\s+FROM\s+(\w+)\s+WHERE\s+(.+))",
	R"(CREATE\s+TABLE\s+(\w+)\s*\((.+)\))",
	R"(DROP\s+TABLE\s+(\w+))"
};
std::unordered_map<std::string, int> data_types = {
	{"BOOLEAN", ELEMENT_BOOLEAN},
	{"BYTE", ELEMENT_BYTE},
	{"SHORT", ELEMENT_SHORT},
	{"LONG", ELEMENT_LONG},
	{"LONGLONG", ELEMENT_LONGLONG},
	{"STRING", ELEMENT_STRING}
};
std::unordered_map<int, std::string> error_transition = {
	{STMT_SUCCESS, "STMT_SUCCESS"},
	{STMT_COLUMN_NOT_FOUND, "STMT_COLUMN_NOT_FOUND"},
	{STMT_TABLE_NOT_FOUND, "STMT_TABLE_NOT_FOUND"},
	{STMT_ROW_NOT_FOUND, "STMT_ROW_NOT_FOUND"},
	{STMT_SYNTAX_ERROR, "STMT_SYNTAX_ERROR"},
	{STMT_INVALID_STATEMENT, "STMT_INVALID_STATEMENT"},
	{STMT_INVALID_ELEMENT, "STMT_INVALID_ELEMENT"},
	{STMT_INVALID_DATA_TYPE, "STMT_INVALID_DATA_TYPE"},
	{STMT_DUPLICATED_ELEMENT, "STMT_DUPLICATED_ELEMENT"},
	{STMT_EMPTY_BUCKET, "STMT_EMPTY_BUCKET"},
	{STMT_CORRUPTED_ELEMENT, "STMT_CORRUPTED_ELEMENT"},
	{STMT_INTERNAL_ERROR, "STMT_INTERNAL_ERROR"},
	{STMT_INVALID_VALUE, "STMT_INVALID_VALUE"},
	{STMT_VALUE_MISMATCH, "STMT_VALUE_MISMATCH"},
	{STMT_EMPTY_SCOPE, "STMT_EMPTY_SCOPE"},
	{STMT_INVALID_LENGTH, "STMT_INVALID_LENGTH"}
};
std::string& ltrim(std::string& s, const char* t = " \t\n\r\f\v")
{
	s.erase(0, s.find_first_not_of(t));
	return s;
}
std::string& rtrim(std::string& s, const char* t = " \t\n\r\f\v")
{
	s.erase(s.find_last_not_of(t) + 1);
	return s;
}
std::string& trim(std::string& s, const char* t = " \t\n\r\f\v")
{
	return ltrim(rtrim(s, t), t);
}
std::vector<std::string> getColumnsFromString(std::string str, char Delimiter) {
	std::istringstream iss(str);
	std::string buffer;
	std::vector<std::string> result;
	while (getline(iss, buffer, Delimiter)) {
		result.push_back(trim(buffer));
	}
	return result;
}


bool pdStmt::execute(std::string& statement) {
	if (statement[0] == '-') {
		if (statement == "-print") {
			if (!m_scope.table) {
				setLastError(STMT_EMPTY_SCOPE);
				return false;
			}
			for (auto& col : m_scope.columns) {
				std::cout << col << '\t';
			}
			std::cout << std::endl;
			for (auto& row : m_scope.rows) {
				for (auto& col : m_scope.columns) {
					CElement* pElement = row->getColumn(col);
					if (pElement)
						std::cout << pElement->getDataS() << '\t';
				}
				std::cout << std::endl;
			}
			std::cout << std::endl;
		}
		return true;
	}
	for (int i = 0; i < STMT_DROP_MAX; i++) {
		std::regex regex(rPatterns[i]);
		std::smatch match;
		if (std::regex_match(statement, match, regex)) {
			return _execute_internal_(i, match, statement);
		}
	}
	setLastError(STMT_INVALID_STATEMENT);
	return false;
}
bool pdStmt::_execute_internal_(int i, std::smatch match, std::string& source_stmt) {
	CTable* pTable = NULL;

	switch (i) {
	case STMT_SELECT_WHERE:
	{
		std::string column = match[1];
		std::string tableName = match[2];
		std::string condition = match[3];
		column = trim(column);
		tableName = trim(tableName);
		condition = trim(condition);
		pTable = g_Schema->getTableByName(tableName);
		if (!pTable) {
			setLastError(STMT_TABLE_NOT_FOUND);
			return false;
		}
		if (column == "*")
			m_scope.columns = pTable->getColumnNames();
		else {
			std::vector<std::string> columns = getColumnsFromString(column, ',');
			if (pTable->hasColumn(columns)) {
				m_scope.columns = columns;
			}
			else {
				setLastError(STMT_COLUMN_NOT_FOUND);
				return false;
			}
		}
		std::vector<CRow*> rows = {};
		int err = STMT_SUCCESS;
		bool bSuccess = evaluateCondition(condition, pTable, rows, err);
		if (bSuccess && err == STMT_SUCCESS) {
			m_scope.table = pTable;
			m_scope.rows = rows;
			return true;
		}
		else {
			setLastError(err);
			return false;
		}
		break;
	}
	case STMT_SELECT:
	{
		std::string column = match[1];
		std::string tableName = match[2];
		column = trim(column);
		tableName = trim(tableName);
		pTable = g_Schema->getTableByName(tableName);
		if (!pTable) {
			setLastError(STMT_TABLE_NOT_FOUND);
			return false;
		}
		if (column == "*")
			m_scope.columns = pTable->getColumnNames();
		else {
			std::vector<std::string> columns = getColumnsFromString(column, ',');
			if (pTable->hasColumn(columns)) {
				m_scope.columns = columns;
			}
			else {
				setLastError(STMT_COLUMN_NOT_FOUND);
				return false;
			}
		}
		m_scope.rows = pTable->getRows();
		m_scope.table = pTable;
		return true;
		break;
	}
	case STMT_UPDATE_WHERE:
	{
		std::string tableName = match[1].str();
		pTable = g_Schema->getTableByName(tableName);
		if (!pTable) {
			setLastError(STMT_TABLE_NOT_FOUND);
			return false;
		}
		std::string setValues = match[2].str();
		std::string condition = match[3].str();
		tableName = trim(tableName);
		setValues = trim(setValues);
		condition = trim(condition);

		std::regex setValueRegex(R"(\s*(\w+)\s*=\s*('([^']*)'|\d+)\s*)");
		std::sregex_iterator iter(setValues.begin(), setValues.end(), setValueRegex);
		std::sregex_iterator end;
		std::vector<std::pair<std::string, std::string>> columnsToUpdate = {};
		while (iter != end) {
			std::smatch setValueMatch = *iter;
			std::string column = setValueMatch[1].str();
			std::string value;
			if (setValueMatch[3].matched) {
				value = setValueMatch[3].str();
			}
			else if (setValueMatch[2].matched) {
				value = setValueMatch[2].str();
			}
			columnsToUpdate.push_back({ column, value });
			++iter;
		}
		bool try_update = columnsToUpdate.size() > 0 ? true : false;
		if (!try_update) {
			setLastError(STMT_SYNTAX_ERROR);
			return false;
		}
		for (auto& data : columnsToUpdate) {
			if (!pTable->hasColumn(data.first)) {
				setLastError(STMT_COLUMN_NOT_FOUND);
				return false;
			}
		}
		std::vector<CRow*> rows = {};
		int err = STMT_SUCCESS;
		bool bSuccess = evaluateCondition(condition, pTable, rows, err);
		if (bSuccess && err == STMT_SUCCESS) {
			for (auto& pRow : rows) {
				for (auto& data : columnsToUpdate) {
					if (!pRow->updateRecord(data.first, data.second)) {
						setLastError(STMT_VALUE_MISMATCH);
						return false;
					}
				}
			}
			return true;
		}
		else {
			setLastError(err);
			return false;
		}


		break;
	}
	case STMT_DELETE_WHERE:
	{
		std::string tableName = match[1].str();
		std::string condition = match[2].str();
		tableName = trim(tableName);
		condition = trim(condition);
		pTable = g_Schema->getTableByName(tableName);
		if (!pTable) {
			setLastError(STMT_TABLE_NOT_FOUND);
			return false;
		}
		std::vector<CRow*> rows = {};
		int err = STMT_SUCCESS;
		bool bSuccess = evaluateCondition(condition, pTable, rows, err);
		if (bSuccess && err == STMT_SUCCESS) {
			for (auto& pRow : rows) {
				pTable->deleteRecord(pRow);
			}
			return true;
		}
		setLastError(err);
		return false;
		break;
	}
	case STMT_UPDATE:
	{
		std::string tableName = match[1].str();
		pTable = g_Schema->getTableByName(tableName);
		if (!pTable) {
			setLastError(STMT_TABLE_NOT_FOUND);
			return false;
		}
		std::string setValues = match[2].str();
		tableName = trim(tableName);
		setValues = trim(setValues);

		std::regex setValueRegex(R"(\s*(\w+)\s*=\s*('([^']*)'|\d+)\s*)");
		std::sregex_iterator iter(setValues.begin(), setValues.end(), setValueRegex);
		std::sregex_iterator end;
		std::vector<std::pair<std::string, std::string>> columnsToUpdate = {};
		while (iter != end) {
			std::smatch setValueMatch = *iter;
			std::string column = setValueMatch[1].str();
			std::string value;
			if (setValueMatch[3].matched) {
				value = setValueMatch[3].str();
			}
			else if (setValueMatch[2].matched) {
				value = setValueMatch[2].str();
			}
			columnsToUpdate.push_back({ column, value });
			++iter;
		}
		bool try_update = columnsToUpdate.size() > 0 ? true : false;
		if (!try_update) {
			setLastError(STMT_SYNTAX_ERROR);
			return false;
		}
		for (auto& data : columnsToUpdate) {
			if (!pTable->hasColumn(data.first)) {
				setLastError(STMT_COLUMN_NOT_FOUND);
				return false;
			}
		}
		std::vector<CRow*> rows = pTable->getRows();

		for (auto& pRow : rows) {
			for (auto& data : columnsToUpdate) {
				if (!pRow->updateRecord(data.first, data.second)) {
					setLastError(STMT_VALUE_MISMATCH);
					return false;
				}
			}
		}
		return true;
		break;
	}
	case STMT_CREATE:
	{
		std::string tableName = match[1].str();
		if (g_Schema->getTableByName(tableName)) {
			setLastError(STMT_DUPLICATED_ELEMENT);
			return false;
		}
		if (tableName.size() > 63) {
			setLastError(STMT_INVALID_LENGTH);
			return false;
		}
		std::string columnDefinitions = match[2].str();
		std::regex columnRegex(R"(\s*(\w+)\s+(\w+))");
		std::sregex_iterator iter(columnDefinitions.begin(), columnDefinitions.end(), columnRegex);
		std::sregex_iterator end;
		std::vector<std::pair<std::string, int>> columnDef;
		while (iter != end) {
			std::smatch columnMatch = *iter;
			std::string columnName = columnMatch[1].str();
			for (auto& pa : columnDef) {
				if (columnName == pa.first) {
					setLastError(STMT_DUPLICATED_ELEMENT);
					return false;
				}
			}
			std::string dataType = columnMatch[2].str();
			if (data_types.find(dataType) == data_types.end()) {
				setLastError(STMT_INVALID_DATA_TYPE);
				return false;
			}
			columnDef.push_back({ columnName , data_types.at(dataType) });
			++iter;
		}
		if (columnDef.size() > 0) {
			return g_Schema->createTable(tableName, columnDef);
		}
		setLastError(STMT_EMPTY_BUCKET);
		return false;
		break;
	}
	case STMT_DROP:
	{
		std::string tableName = match[1].str();
		if (!g_Schema->getTableByName(tableName)) {
			setLastError(STMT_TABLE_NOT_FOUND);
			return false;
		}
		return g_Schema->dropTable(tableName);
		break;
	}
	case STMT_INSERT:
	{
		std::string tableName = match[1].str();
		pTable = g_Schema->getTableByName(tableName);
		if (!pTable) {
			setLastError(STMT_TABLE_NOT_FOUND);
			return false;
		}
		std::string values = match[2].str();

		std::regex valueRegex(R"(\s*'([^']*)'\s*|\s*(\d+)\s*)");
		std::sregex_iterator iter(values.begin(), values.end(), valueRegex);
		std::sregex_iterator end;
		std::vector<std::string> toStore;
		while (iter != end) {
			std::smatch valueMatch = *iter;
			std::string value;

			if (valueMatch[1].matched) {
				value = valueMatch[1].str();
			}
			else if (valueMatch[2].matched) {
				value = valueMatch[2].str();
			}
			toStore.push_back(value);
			++iter;
		}
		if (pTable->getColumnCount() == toStore.size()) {
			bool result = pTable->insertRecord(toStore);
			if (!result)
				setLastError(STMT_VALUE_MISMATCH);
			return result;
		}
		setLastError(STMT_VALUE_MISMATCH);
		return false;
	}
	default:
		setLastError(STMT_INTERNAL_ERROR);
		break;
	}
	return false;
}
std::string pdStmt::_error_transition_(int e) {
	return e < STMT_ERROR_MAX ? error_transition.at(e) : "STMT_UNKNOWN";
}