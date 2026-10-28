CFLAGS = -O2 -Wall

count: count.c
	$(CC) $(CFLAGS) -o $@ $<

bigfile:
	head -c 1G /dev/urandom > bigfile.bin

clean:
	rm -f count bigfile.bin
