#include "ConditionEval.h"
#include "CElement.h"
#include "CSchema.h"
#include <iostream>
extern CSchema* g_Schema;
bool is_alnum_t(char c) {
    bool ret = false;
    switch (c) {
    case '\'':
    case '!':
    case '@':
    case '#':
    case '$':
    case '%':
    case '^':
    case '&':
    case '*':
        ret = true;
        break;
    default:
        ret = std::isalnum(c);
    }
    return ret;
}
std::vector<std::string> tokenize(std::string input) {
    std::vector<std::string> answer;
    std::regex reg(R"(\s+)");
    std::string s = std::regex_replace(input, reg, " ");
    std::string tmp = "";
    bool wasAlnum = is_alnum_t(input[0]);
    tmp += input[0];
    bool force_0 = false;
    for (int i = 1; i < input.size(); i++) {
        if (input[i] == '(' || input[i] == ')') {
            if (tmp.size() > 0)
                answer.push_back(tmp);
            tmp = "";
            tmp += input[i];
            wasAlnum = false;
            continue;
        }
        if (input[i] == '\'') {
            force_0 = !force_0;
        }
        bool isAlnum = force_0 || is_alnum_t(input[i]);
        if (wasAlnum != isAlnum) {
            if (tmp.size() > 0)
                answer.push_back(tmp);
            tmp = "";
            if (input[i] != ' ')
                tmp += input[i];
            else if (force_0) {
                tmp += input[i];
            }
            wasAlnum = isAlnum;
        }
        else {
            if (input[i] != ' ')
                tmp += input[i];
            else if (force_0) {
                tmp += input[i];
            }
        }
    }
    if (tmp.size() > 0)
        answer.push_back(tmp);
    return answer;
}
bool evaluateComparison(std::string& column, std::string& op, std::string& value, CRow* pRow, int& err) {
    if (!pRow->hasColumn(column)) {
        err = STMT_COLUMN_NOT_FOUND;
        return false;
    }
    CElement* pElement = pRow->getColumn(column);
    ELEMENT_TYPE eType = pElement->getType();
    bool isNumeric = eType != ELEMENT_STRING;
    if (isNumeric) {
        unsigned long long columnValue = 0;
        unsigned long long valueToCompare = (unsigned long long)std::stoll(value);
        switch (eType) {
        case ELEMENT_BOOLEAN:
        {
            CBoolean* _pElement = dynamic_cast<CBoolean*>(pElement);
            columnValue = (unsigned long long)_pElement->getData();
            break;
        }

        case ELEMENT_BYTE:
        {
            CByte* _pElement = dynamic_cast<CByte*>(pElement);
            columnValue = (unsigned long long)_pElement->getData();
            break;
        }
        case ELEMENT_SHORT:
        {
            CShort* _pElement = dynamic_cast<CShort*>(pElement);
            columnValue = (unsigned long long)_pElement->getData();
            break;
        }

        case ELEMENT_LONG:
        {
            CLong* _pElement = dynamic_cast<CLong*>(pElement);
            columnValue = (unsigned long long)_pElement->getData();
            break;
        }
        case ELEMENT_LONGLONG:
        {
            CLongLong* _pElement = dynamic_cast<CLongLong*>(pElement);
            columnValue = (unsigned long long)_pElement->getData();
            break;
        }
        default:
        {
            err = STMT_INVALID_ELEMENT;
            return false;
        }
        }
        if (op == "=") {
            return columnValue == valueToCompare;
        }
        else if (op == "<") {
            return columnValue < valueToCompare;
        }
        else if (op == ">") {
            return columnValue > valueToCompare;
        }
        else if (op == "<=") {
            return columnValue <= valueToCompare;
        }
        else if (op == ">=") {
            return columnValue >= valueToCompare;
        }
        else if (op == "<>") {
            return columnValue != valueToCompare;
        }
        else {
            err = STMT_SYNTAX_ERROR;
            return false;
        }
    }
    else {
        if (eType == ELEMENT_STRING) {
            CString* _pElement = dynamic_cast<CString*>(pElement);
            std::string columnValue = _pElement->getData();
            std::string valueToCompare = value;
            valueToCompare.erase(std::remove(valueToCompare.begin(), valueToCompare.end(), '\''), valueToCompare.end());
            if (op == "=") {
                return columnValue == valueToCompare;
            }
            else if (op == "<>") {
                return columnValue != valueToCompare;
            }
            else {
                err = STMT_SYNTAX_ERROR;
                return false;
            }
        }
        else {
            err = STMT_CORRUPTED_ELEMENT;
            return false;
        }
    }

}

bool evaluateCondition(std::vector<std::string> tokens, CRow* pRow, int& err) {
    std::stack<bool> result_stack;
    std::stack<std::pair<std::stack<bool>, std::stack<std::string>>> parenthesesStack;
    std::stack<std::string> operandStack;
    std::stack<std::string> operatorStack;
    
    for (std::string token : tokens) {
        if (token == "(") {
            parenthesesStack.push({result_stack, operatorStack});
            while( !result_stack.empty() )
                result_stack.pop();
            while( !operatorStack.empty() )
                operatorStack.pop();
        }
        else if (token == ")") {
            std::pair<std::stack<bool>, std::stack<std::string>> pastStack = parenthesesStack.top();
            parenthesesStack.pop();
            if (pastStack.first.size() > 0 && pastStack.second.size() > 0) {
                if (pastStack.second.top() == "AND" || pastStack.second.top() == "OR") {
                    if (result_stack.size() > 0) {
                        bool v1 = result_stack.top();
                        result_stack.pop();
                        bool v2 = pastStack.first.top();
                        pastStack.first.pop();
                        if (pastStack.second.top() == "AND")
                            result_stack.push(v1 && v2);
                        else
                            result_stack.push(v1 || v2);
                    } else {
                        err = STMT_SYNTAX_ERROR;
                        return false;
                    }
                } else {
                    err = STMT_SYNTAX_ERROR;
                    return false;
                }
            }
        }
        else {

            if (token == "AND" || token == "OR") {
                operatorStack.push(token);
            }
            else if (token == "=" || token == ">" || token == "<" || token == ">=" || token == "<=" || token == "<>") {
                operatorStack.push(token);
            }
            else {
                operandStack.push(token);
                if (operandStack.size() >= 2) {
                    std::string rightValue = operandStack.top();
                    operandStack.pop();
                    std::string leftValue = operandStack.top();
                    operandStack.pop();
                    if (operatorStack.size() > 0) {
                        std::string op = operatorStack.top();
                        operatorStack.pop();
                        result_stack.push(evaluateComparison(leftValue, op, rightValue, pRow, err));
                        if (result_stack.size() >= 2 && operatorStack.size() > 0) {
                            std::string lop = operatorStack.top();
                            operatorStack.pop();
                            if (lop == "AND" || lop == "OR") {
                                bool v1 = result_stack.top();
                                result_stack.pop();
                                bool v2 = result_stack.top();
                                result_stack.pop();
                                if (lop == "AND")
                                    result_stack.push(v1 && v2);
                                else
                                    result_stack.push(v1 || v2);
                            }
                            else {
                                err = STMT_SYNTAX_ERROR;
                                return false;
                            }

                        }
                    }
                    else {
                        err = STMT_SYNTAX_ERROR;
                        return false;
                    }
                }
            }
        }
    }
    if (result_stack.size() != 1) {
        err = STMT_SYNTAX_ERROR;
        return false;
    }

    bool resultValue = result_stack.top();
    return result_stack.top();
}

bool evaluateCondition(std::string condition, CTable* pTable, std::vector<CRow*>& ret, int& err) {
    if (!pTable) {
        err = STMT_TABLE_NOT_FOUND;
        return false;
    }
    std::vector<std::string> tokens = tokenize(condition);
    if (tokens.size() < 3) {
        err = STMT_SYNTAX_ERROR;
    }
    std::vector<CRow*> rows = pTable->getRows();
    for (auto& row : rows) {
        bool e = evaluateCondition(tokens, row, err);
        if (e)
            ret.push_back(row);
    }
    return err == STMT_SUCCESS;
}
bool evaluateCondition(std::string condition, CRow* pRow, int& err) {
    if (!pRow) {
        err = STMT_ROW_NOT_FOUND;
        return false;
    }
    std::vector<std::string> tokens = tokenize(condition);
    if (tokens.size() < 3) {
        err = STMT_SYNTAX_ERROR;
    }

    bool e = evaluateCondition(tokens, pRow, err);
    return e;
}
//Table�� �μ��� �ް� Condition�� �´� Row ����Ʈ ����
//Row�� �μ��� �ް� Condition�� �´��� bool ����
