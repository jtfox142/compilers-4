CC = g++

CFLAGS = -Wall -std=c++11

compfs: p4.o scanner.o parser.o tree.o statSem.o symbolTable.o codeGeneration.o
	$(CC) $(CFLAGS) -o statSem p4.o scanner.o parser.o tree.o statSem.o symbolTable.o codeGeneration.o

p4.o: p4.cpp parser.cpp parser.hpp scanner.cpp scanner.hpp tree.hpp token.hpp statSem.cpp statSem.hpp
	$(CC) $(CFLAGS) -c p4.cpp

parser.o: parser.cpp parser.hpp tree.cpp tree.hpp node.hpp scanner.cpp scanner.hpp token.hpp
	$(CC) $(CFLAGS) -c parser.cpp

statSem.o: parser.cpp parser.hpp statSem.cpp statSem.hpp symbolTable.cpp symbolTable.hpp token.hpp codeGeneration.cpp codeGeneration.hpp
	$(CC) $(CFLAGS) -c statSem.cpp

symbolTable.o: symbolTable.cpp symbolTable.hpp token.hpp
	$(CC) $(CFLAGS) -c symbolTable.cpp

tree.o: tree.cpp tree.hpp node.hpp
	$(CC) $(CFLAGS) -c tree.cpp

scanner.o: scanner.cpp scanner.hpp token.hpp
	$(CC) $(CFLAGS) -c scanner.cpp

codeGeneration.o: codeGeneration.cpp codeGeneration.hpp node.hpp token.hpp
	$(CC) $(CFLAGS) -c codeGeneration.cpp

clean: 
	rm *.o statSem