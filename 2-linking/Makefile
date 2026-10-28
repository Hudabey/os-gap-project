CC = gcc
CFLAGS = -Wall -Wextra -O2

.PHONY: all static dynamic shared clean
all: static dynamic

mylib.o: mylib.c mylib.h
	$(CC) $(CFLAGS) -c mylib.c -o mylib.o

mylib_pic.o: mylib.c mylib.h
	$(CC) $(CFLAGS) -fPIC -c mylib.c -o mylib_pic.o

libmylib.a: mylib.o
	ar rcs libmylib.a mylib.o

libmylib.so: mylib_pic.o
	$(CC) -shared -o libmylib.so mylib_pic.o

static: libmylib.a main.c mylib.h
	$(CC) $(CFLAGS) -o app_static main.c ./libmylib.a

dynamic: libmylib.so main.c mylib.h
	$(CC) $(CFLAGS) -o app_dynamic main.c -L. -lmylib -Wl,-rpath,'$$ORIGIN'

shared: dynamic

clean:
	rm -f *.o *.a *.so app_static app_dynamic
