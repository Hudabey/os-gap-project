# OS Gap Project

Hudeifa Ali Hassan · Matriculation No. 100002025

Three short Linux exercises covering the CO-562 Operating Systems learning outcomes
that SRH BST-BCS-18 (Operating Systems / Web Computing) did not cover.

| CO-562 outcome | Part | What it shows |
| --- | --- | --- |
| 11. Explain how memory mapping can improve I/O performance | [1-mmap](1-mmap/) | `read()` vs. `mmap()` on a large file, timed |
| 12. Restate the functionality of a linker; static vs. dynamic linking | [2-linking](2-linking/) | One library built both ways, inspected with `nm`, `ldd`, `readelf` |
| 13. Outline how different device types are supported by Unix-like kernels | [3-devices](3-devices/) | Character vs. block devices, `/dev`, major/minor numbers, `ioctl()` |

Outcomes 1–10 were covered in BST-BCS-18; sockets (8) and I/O multiplexing (9) also in
Computer Networks coursework; virtualization (14) in a course presentation.

## Run

Everything runs on Linux inside Docker:

```bash
docker build -t os-gap .
docker run --rm -it -v "$PWD":/work os-gap
# inside the container:
make            # build all parts
```

## Results

Each part's README holds its results and explanation.
