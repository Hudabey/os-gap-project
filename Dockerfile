FROM gcc:14-bookworm
RUN apt-get update && apt-get install -y --no-install-recommends file time strace && rm -rf /var/lib/apt/lists/*
WORKDIR /work
CMD ["bash"]
