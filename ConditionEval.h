#pragma once
#include "shared.h"
class CTable;
class CRow;

bool evaluateCondition(std::string condition, CTable* pTable, std::vector<CRow*>& ret, int& err);
bool evaluateCondition(std::string condition, CRow* pRow, int& err);