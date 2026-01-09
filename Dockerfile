# syntax=docker/dockerfile:1

FROM ubuntu:24.04

RUN apt-get update && apt-get upgrade -y && apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    ninja-build \
    clang-tools \
    gdb \
    clang \
    netcat-traditional \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace

CMD ["bash"]
