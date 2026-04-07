FROM ubuntu:latest

RUN apt-get update && apt-get install -y \
    gcc \
    g++ \
    make \
    gdb \
    git \
    bats \
    build-essential \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace

