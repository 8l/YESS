CC = gcc -g

yess: loader.o tools.o memory.o registers.o decodeStage.o executeStage.o writebackStage.o fetchStage.o memoryStage.o main.o dump.o
	gcc loader.o tools.o memory.o registers.o decodeStage.o executeStage.o writebackStage.o fetchStage.o memoryStage.o main.o dump.o -o yess

main.o: bool.h tools.h memory.h dump.h forwarding.h status.h bubbling.h 

dump.o: bool.h dump.h fetchStage.h decodeStage.h executeStage.h memoryStage.h writebackStage.h registers.h memory.h forwarding.h status.h bubbling.h

memory.o:  bool.h tools.h memory.h registers.h

registers.o: registers.h 

loader.o: bool.h loader.h

tools.o: bool.h tools.h

decodeStage.o: decodeStage.h executeStage.h bool.h tools.h instructions.h registers.h forwarding.h status.h bubbling.h

executeStage.o: executeStage.h memoryStage.h bool.h tools.h instructions.h registers.h forwarding.h status.h bubbling.h

writebackStage.o: writebackStage.h bool.h tools.h instructions.h dump.h registers.h forwarding.h status.h

fetchStage.o: fetchStage.h decodeStage.h bool.h tools.h instructions.h registers.h bubbling.h

memoryStage.o: memoryStage.h writebackStage.h bool.h tools.h instructions.h registers.h forwarding.h status.h bubbling.h

clean:
	rm -f *.o
