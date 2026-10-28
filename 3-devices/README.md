# Part 3 — Devices in Unix-like kernels (CO-562 outcome 13)

A short investigation and write-up. Run the commands inside the Linux container,
paste the output, and explain it.

## Steps

1. `ls -l /dev` — the first letter of each line is `c` (character) or `b` (block).
   Find examples of each. What do the two numbers before the date mean (major, minor)?
2. `cat /proc/devices` — which drivers own which major numbers?
3. `head -c 16 /dev/urandom | od -x` and `echo hi > /dev/null` — devices are used through
   the same `open()`/`read()`/`write()` calls as files. Why is that useful?
4. Write a 15-line C program that calls `ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws)` and prints
   your terminal's rows and columns (`man 2 ioctl`, `man 4 tty_ioctl`).
   Why can't this be done with `read()`/`write()`?

## Results

Docker Desktop (Linux 6.12, aarch64) on an Apple M1; container run with `--privileged` so the VM's disk devices are visible.

```text
$ ls -l /dev   (selection)
crw-rw-rw- 1 root root   1,   3  null
crw-rw-rw- 1 root root   1,   5  zero
crw-rw-rw- 1 root root   1,   8  random
crw-rw-rw- 1 root root   1,   9  urandom
crw-rw-rw- 1 root root   5,   0  tty
brw-rw-rw- 1 root root   7,   0  loop0
brw-rw-rw- 1 root root 254,   0  vda
brw-rw-rw- 1 root root 254,   1  vda1

$ cat /proc/devices   (selection)
Character devices:
  1 mem
  4 tty
  5 /dev/tty
  5 /dev/console
 10 misc
254 gpiochip
Block devices:
  7 loop
254 virtblk
259 blkext

$ head -c 16 /dev/urandom | od -x
0000000 c97a 3282 bbef 9414 8c93 ff9d 39c6 3cdf

$ echo hi > /dev/null; echo $?
0

$ ls /sys/class/net
eth0  lo  (plus tunnel interfaces)
```

Major 254 appears twice: as a character major it belongs to `gpiochip`, as a block major to `virtblk`. Character and block majors are separate number spaces.

`ioctl()` program ([winsize.c](winsize.c)):

```text
$ ./winsize                  # stdout is a terminal (stty rows 40 cols 120)
Terminal size: 40 rows, 120 columns
$ ./winsize > out.txt        # stdout is a regular file
ioctl(TIOCGWINSZ): Inappropriate ioctl for device
```

## Write-up

### 1. Character devices vs. block devices

Character devices provide a stream of bytes, for example `/dev/null`, `/dev/zero`, `/dev/urandom` and terminal devices. Block devices represent storage that can be accessed in fixed-size blocks and at different positions, such as `/dev/vda` and its partition `/dev/vda1`. Linux exposes both types as device files, but their drivers handle the actual operations differently.

### 2. Major and minor device numbers

The major number identifies the driver (or driver class) that handles a device. The minor number identifies a particular device or subdivision managed by that driver. In the measured output, `/dev/vda` was `254,0` and `/dev/vda1` was `254,1`: both share major 254, but have different minor numbers. Character and block devices have separate numbering spaces, so the same major number can occur in both groups without identifying the same driver.

### 3. Why devices use familiar system calls

Linux gives programs a consistent interface to many devices through `open()`, `read()`, `write()` and `close()`. The kernel directs each operation to the device driver. For example, reading `/dev/urandom` returned random bytes, while writing to `/dev/null` succeeded but discarded the data. Some device-specific operations use `ioctl()` instead of ordinary reads and writes.

### 4. Why network interfaces are different

Network interfaces such as `eth0` and `lo` usually appear under `/sys/class/net`, rather than as ordinary `/dev` files. Programs normally communicate over the network using sockets, such as `socket()`, `send()` and `recv()`. The network interface is managed by the kernel networking stack instead of behaving like a sequential file to read or write.

### Terminal size with ioctl()

The `winsize.c` program calls `ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws)` to ask the terminal driver for the current number of rows and columns. This is a device-specific request rather than a normal byte transfer. It works when standard output is attached to a terminal; it may fail with `ENOTTY` when output is redirected to a file or when the process has no terminal.
