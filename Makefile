main: main.o raw.o row.o piece.o io.o edit.o 
	gcc -o main main.o piece.o row.o raw.o io.o edit.o
main.o: main.c piece.c edit.c row.c raw.c io.c
	gcc -c main.c -o main.o
piece.o: piece.c piece.h
	gcc -c piece.c -o piece.o
row.o: row.c 
	gcc -c row.c -o row.o
raw.o: raw.c 
	gcc -c raw.c -o raw.o
edit.o: edit.c 
	gcc -c edit.c -o edit.o
io.o: io.c
	gcc -c io.c -o io.o
clean:
	rm *.o
