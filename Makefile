all:
	$(MAKE) -C 1-mmap
	$(MAKE) -C 2-linking
	$(CC) -O2 -Wall -o 3-devices/winsize 3-devices/winsize.c

clean:
	$(MAKE) -C 1-mmap clean
	$(MAKE) -C 2-linking clean
	rm -f 3-devices/winsize
