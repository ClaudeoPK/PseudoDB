#pragma once
#include "shared.h"
#include "CSchema.h"
#include <regex>
#include <stack>

typedef struct _SCOPE { //TODO: <OPTIONAL> 0605 shared_ptr 사용할 것.
	CTable* table;
	std::vector<CRow*> rows;
	std::vector<std::string> columns;
} SCOPE, *PSCOPE;

enum STMT_SELECT_TYPE {
	STMT_SELECT_WHERE = 0,
	STMT_SELECT,
	STMT_SELECT_MAX,
};
enum STMT_UPDATE_TYPE {
	STMT_UPDATE_WHERE = STMT_SELECT_MAX,
	STMT_UPDATE,
	STMT_UPDATE_MAX,
};
enum STMT_INSERT_TYPE {
	STMT_INSERT = STMT_UPDATE_MAX,
	STMT_INSERT_MAX
};
enum STMT_DELETE_TYPE {
	STMT_DELETE_WHERE = STMT_INSERT_MAX,
	STMT_DELETE_MAX
};
enum STMT_CREATE_TYPE {
	STMT_CREATE = STMT_DELETE_MAX,
	STMT_CREATE_MAX
};
enum STMT_DROP_TYPE {
	STMT_DROP = STMT_CREATE_MAX,
	STMT_DROP_MAX
};

class pdStmt {
public:
	pdStmt() {
		initialize();
	}
	bool execute(std::string& statement);
	bool hasScope() {
		return m_scope.table != NULL;
	}
	int getLastError() {
		return m_lastError;
	}
	std::string getLastErrorMessage() {
		return _error_transition_(m_lastError);
	}
private:
	bool _execute_internal_(int i, std::smatch match, std::string& source_stmt);
	void initialize() {
		m_scope = { NULL, {},{} };
		m_lastError = STMT_SUCCESS;
	}
	void setLastError(int e) {
		m_lastError = e;
	}
	std::string _error_transition_(int e);
private:
	SCOPE m_scope;
	int m_lastError;
};
