app: app.o piece.o
	gcc -o app app.o piece.o
app.o: app.c piece.c piece.h
	gcc -c app.c -o app.o
piece.o: piece.c piece.h
	gcc -c piece.c -o piece.o
clean:
	rm *.o
