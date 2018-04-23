all:jobExecutor

jobExecutor: jobExecutor.o Arguments.o trie.o
	gcc jobExecutor.o Arguments.o trie.o -o jobExecutor

jobExecutor.o : jobExecutor.c
		gcc -c -g jobExecutor.c

Arguments.o: Arguments.c
		gcc -c -g Arguments.c

trie.o: trie.c
		gcc -c -g trie.c

clean:
		rm -f *.o jobExecutor
