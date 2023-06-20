
#include <iostream>
#include "CSchema.h"
#include "pdStmt.h"
#define dbFileName "pseudo.db"
CSchema* g_Schema = new CSchema();
void onTerminate(int signo);
void saveSchema() {
    void* f = uvOpen(dbFileName, false);
    if (f != NULL) {
        g_Schema->doExport(f);
        uvClose(f, false);
    }
}
void readSchema() {
    void* f = uvOpen(dbFileName, true);
    if (f != NULL) {
        g_Schema->doImport(f);
        uvClose(f, true);
    }
}
int main()
{
    
    readSchema();
#ifndef _WINDOWS_IMPL_
    signal(SIGINT, onTerminate);
    signal(SIGSTOP, onTerminate);
    signal(SIGQUIT, onTerminate);
    signal(SIGHUP, onTerminate);
#endif
    while (true) {
        std::string sqlQuery;
        std::cout << "Enter SQL query: ";
        std::getline(std::cin, sqlQuery);
        pdStmt* stmt = new pdStmt();
        if (sqlQuery == "-save") {
            saveSchema();
        }
        else if (sqlQuery == "-load") {
            readSchema();
        }
        else {
            stmt->execute(sqlQuery);
            if (stmt->getLastError() != STMT_SUCCESS)
                std::cout << "ErrorCode:" << stmt->getLastError() << "(" << stmt->getLastErrorMessage() << ")" << std::endl;
            if (stmt->hasScope()) {
                    std::string command;
                    std::cout << "  >";
                    std::getline(std::cin, command);
                    stmt->execute(command);
                    if (stmt->getLastError() != STMT_SUCCESS)
                        std::cout << "ErrorCode:" << stmt->getLastError() << "(" << stmt->getLastErrorMessage() << ")" << std::endl;
            }
        }
        delete stmt;
    }
}

void onTerminate(int signo) {
    saveSchema();
	exit(1);
}