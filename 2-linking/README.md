# Part 2 — Static vs Dynamic Linking

The program prints a greeting and the result of `add(2, 3)` using two functions defined in `mylib.c`.

## Build and test

```sh
make
./app_static
./app_dynamic
ls -lh app_static app_dynamic libmylib.a libmylib.so
ldd ./app_static
ldd ./app_dynamic
nm app_static | grep -E ' (add|greet)$'
nm -D app_dynamic | grep -E ' (add|greet)$'
```

## Explanation

- **Static library linking:** The linker copies the `add()` and `greet()` code from `libmylib.a` into `app_static`. This executable does not need `libmylib.a` at runtime.
- **Dynamic library linking:** `app_dynamic` has a runtime dependency on `libmylib.so`. The dynamic loader must find the library before it can resolve the functions. The Makefile sets the runtime search path to the executable's directory (`$ORIGIN`).
- **Inspection:** `ldd` can show that `app_dynamic` depends on `libmylib.so`, whereas `app_static` does not. Both may still depend on the system's shared C library (`libc`); linking *our library* statically is not the same as a fully statically linked executable.
- **Trade-off:** Static linking avoids a runtime dependency on our custom library but typically embeds more code in each executable. Dynamic linking enables multiple programs to share a library and supports replacing a compatible `.so`, but that file must be present and loadable. In this test the two executables are almost the same size (70,640 vs. 70,552 bytes) because the library is only two tiny functions and the aarch64 linker pads each binary to 64 KB pages; the difference grows with the size of the library.

## Results

Docker Desktop (Linux 6.12, aarch64) on an Apple M1, gcc 14.3.

```text
$ ./app_static
Hello, Hudeifa!
2 + 3 = 5
$ ./app_dynamic
Hello, Hudeifa!
2 + 3 = 5

$ ls -l   (bytes)
70640 app_static
70552 app_dynamic
 1782 libmylib.a
69608 libmylib.so

$ ldd ./app_static
	linux-vdso.so.1
	libc.so.6 => /lib/aarch64-linux-gnu/libc.so.6
	/lib/ld-linux-aarch64.so.1
$ ldd ./app_dynamic
	linux-vdso.so.1
	libmylib.so => /work/2-linking/./libmylib.so
	libc.so.6 => /lib/aarch64-linux-gnu/libc.so.6
	/lib/ld-linux-aarch64.so.1

$ nm app_static | grep -E ' (add|greet)$'
00000000004006e0 T add
00000000004006e8 T greet
$ nm -D app_dynamic | grep -E ' (add|greet)$'
                 U add
                 U greet

$ readelf -d app_dynamic | grep -E 'NEEDED|RUNPATH'
 (NEEDED)   Shared library: [libmylib.so]
 (NEEDED)   Shared library: [libc.so.6]
 (RUNPATH)  Library runpath: [$ORIGIN]
```

**Update test:** changed the message in `greet()`, rebuilt only `libmylib.so`, reran both apps without relinking them:

```text
$ ./app_static
Hello, Hudeifa!
$ ./app_dynamic
Hi again, Hudeifa (new libmylib.so)!
```

Only `app_dynamic` picked up the change.
