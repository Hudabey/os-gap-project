// Count occurrences of the byte 'a' in a file, using read() or mmap().
// Usage: ./count read|mmap <file>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#define BUF_SIZE 4096

static long count_with_read(int fd) {
    char buf[BUF_SIZE];
    long n = 0;
    ssize_t got;

    while ((got = read(fd, buf, BUF_SIZE)) > 0) {
        for (ssize_t i = 0; i < got; i++) {
            if (buf[i] == 'a') {
                n++;
            }
        }
    }

    if (got < 0) {
        perror("read");
        return -1;
    }
    return n;
}

static long count_with_mmap(int fd) {
    struct stat st;
    if (fstat(fd, &st) == -1) {
        perror("fstat");
        return -1;
    }

    if (st.st_size == 0) {
        return 0;
    }

    size_t size = (size_t)st.st_size;
    char *p = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (p == MAP_FAILED) {
        perror("mmap");
        return -1;
    }

    long n = 0;
    for (size_t i = 0; i < size; i++) {
        if (p[i] == 'a') {
            n++;
        }
    }

    if (munmap(p, size) == -1) {
        perror("munmap");
        return -1;
    }
    return n;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "usage: %s read|mmap <file>\n", argv[0]);
        return 1;
    }
    int fd = open(argv[2], O_RDONLY);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    long n = strcmp(argv[1], "mmap") == 0 ? count_with_mmap(fd) : count_with_read(fd);
    clock_gettime(CLOCK_MONOTONIC, &end);

    if (n < 0) {
        close(fd);
        return 1;
    }

    double secs = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("%s: %ld matches in %.3f s\n", argv[1], n, secs);
    close(fd);
    return 0;
}