FROM ubuntu:latest

RUN apt-get update && apt-get install -y \
    gcc \
    g++ \
    make \
    gdb \
    git \
    bats \
    cloc \
    build-essential \
    libssl-dev \
    zlib1g-dev \
    python3 \
    cloc \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace
