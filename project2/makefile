all:jobExecutor

jobExecutor: jobExecutor.o Arguments.o
	gcc jobExecutor.o Arguments.o -o jobExecutor

jobExecutor.o : jobExecutor.c
		gcc -c -g jobExecutor.c

Arguments.o: Arguments.c
		gcc -c -g Arguments.c


clean:
		rm -f *.o jobExecutor
