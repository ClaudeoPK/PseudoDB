OBJS	= ConditionEval.o pdStmt.o Pseudo-DB-Local.o fsutil.o
SOURCE	= ConditionEval.cpp pdStmt.cpp Pseudo-DB-Local.cpp fsutil.cpp
HEADER	= CRow.h CTable.h pdStmt.h shared.h CBoolean.h CByte.h CElement.h CLong.h CLongLong.h ConditionEval.h CSchema.h CShort.h CString.h fsutil.h
OUT	= pseudoDB
CC	 = g++
FLAGS	 = -g -c
LFLAGS	 = 

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

ConditionEval.o: ConditionEval.cpp
	$(CC) $(FLAGS) ConditionEval.cpp -std=c++17

pdStmt.o: pdStmt.cpp
	$(CC) $(FLAGS) pdStmt.cpp -std=c++17

Pseudo-DB-Local.o: Pseudo-DB-Local.cpp
	$(CC) $(FLAGS) Pseudo-DB-Local.cpp -std=c++17

fstuil.o: fsutil.cpp
	$(CC) $(FLAGS) fsutil.cpp -std=c++17

clean:
	rm -f $(OBJS) $(OUT)