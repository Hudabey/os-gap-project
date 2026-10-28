# Part 1 — Memory-mapped I/O (CO-562 outcome 11)

Count how many times one byte (e.g. `'a'`) appears in a large file, two ways:

1. **`read()`**: read the file into a buffer chunk by chunk, count in the buffer.
2. **`mmap()`**: map the whole file into memory, count by walking the pointer.

Time both and explain the difference.

## Steps

1. Make a 1 GB test file: `make bigfile`
2. Fill in the two TODOs in `count.c`
3. `make && ./count read bigfile.bin` and `./count mmap bigfile.bin`
4. Run each 3 times; note the times below. Also try a 4 KB vs. 1 MB buffer for `read()`.

Man pages: `man 2 mmap`, `man 2 read`, `man 2 fstat`, `man 2 munmap`.

## Results

| Method | Buffer | Run 1 (s) | Run 2 (s) | Run 3 (s) |
| --- | --- | --- | --- | --- |
| read() | 4 KB | 0.472 | 0.489 | 0.463 |
| read() | 1 MB | 0.414 | 0.420 | 0.781 |
| mmap() | — | 0.869 | 0.453 | 0.421 |

1 GB file of random bytes, warm page cache, Docker Desktop (Linux 6.12 VM) on an Apple M1, gcc 14.3 `-O2`.

System calls (`strace -c`):

| Method | `read` calls | `mmap` calls | Syscall time (s) |
| --- | --- | --- | --- |
| read() 4 KB | 262,146 | 6 | 2.55 (inflated by strace) |
| read() 1 MB | 1,026 | 6 | 0.33 |
| mmap() | 1 | 7 | 0.007 |

Page faults (`/usr/bin/time`): read() 74 minor, mmap() 16,456 minor, 0 major in both.

## Explanation

### 1. What is the difference between read() and mmap()?

`read()` copies data from the file into a buffer, and the program processes that buffer. With `mmap()`, the file is mapped into the process's virtual memory, so the program can access its contents directly through a pointer without explicitly copying each chunk into a separate buffer.

### 2. Which method was faster?

The 4 KB `read()` implementation took about 0.47 seconds, while the 1 MB version took about 0.42 seconds. The `mmap()` version took around 0.42–0.45 seconds. So `mmap()` was slightly faster than the 4 KB version, but increasing the `read()` buffer size produced almost the same performance.

### 3. Why does mmap() use fewer system calls?

With a 4 KB buffer, `read()` needed 262,146 calls to process the 1 GB file. Increasing the buffer to 1 MB reduced this to 1,026 calls. `mmap()` needed only one mapping call because it mapped the entire file at once. This reduces the overhead of repeatedly entering the kernel to request more data.

### 4. What do the page faults tell us?

The `mmap()` version generated 16,456 minor page faults, compared with 74 for `read()`. This happens because the mapped pages must be connected to the process's virtual memory as they are accessed. All the recorded faults were minor, with no major page faults, since the file was already cached in memory. Despite the additional page faults, `mmap()` still performed similarly to `read()`.

### 5. When would read() be the better choice?

`read()` is a better choice for small files and when reading data sequentially, because the overhead of memory mapping may not be worthwhile. It also works with pipes, sockets, and terminals, which generally cannot be accessed using `mmap()` in the same way as regular files. For files that may change size during processing, `read()` can also be easier and safer to manage.
