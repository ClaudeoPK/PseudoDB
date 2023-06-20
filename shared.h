#pragma once
#include <vector>
#include <utility>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <regex>
#include <stack>
#include <algorithm>
#include "fsutil.h"
#ifndef _WINDOWS_IMPL_
#include <signal.h>
#include <unistd.h>
#endif
enum ELEMENT_TYPE {
	ELEMENT_BOOLEAN = 0,
	ELEMENT_BYTE,
	ELEMENT_SHORT,
	ELEMENT_LONG,
	ELEMENT_LONGLONG,
	ELEMENT_STRING
};
enum STMT_ERROR {
	STMT_SUCCESS = 0,
	STMT_COLUMN_NOT_FOUND,
	STMT_TABLE_NOT_FOUND,
	STMT_ROW_NOT_FOUND,
	STMT_SYNTAX_ERROR,
	STMT_INVALID_STATEMENT,
	STMT_INVALID_ELEMENT,
	STMT_INVALID_DATA_TYPE,
	STMT_DUPLICATED_ELEMENT,
	STMT_EMPTY_BUCKET,
	STMT_CORRUPTED_ELEMENT,
	STMT_INTERNAL_ERROR,
	STMT_INVALID_VALUE,
	STMT_VALUE_MISMATCH,
	STMT_EMPTY_SCOPE,
	STMT_INVALID_LENGTH,
	STMT_ERROR_MAX
};
typedef struct _BINARY_EXPORT {
	int length;
	void* data;
} BINARY_EXPORT, * PBINARY_EXPORT;

typedef struct _ELEMENT_INFORMATION {
	int type;
} ELEMENT_INFORMATION, *PELEMENT_INFORMATION;

typedef struct _TABLE_INFORMATION {
	char table_name[64];
	int row_count;
	int col_count;
} TABLE_INFORMATION, * PTABLE_INFORMATION;


