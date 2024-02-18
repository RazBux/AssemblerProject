preAsmbler: preAsmbler.c
	gcc -ansi -Wall -pedantic -g preAsmbler.c -o preAsmbler

run: preAsmbler
	./preAsmbler < m.txt > output.txt
	open output.txt
	