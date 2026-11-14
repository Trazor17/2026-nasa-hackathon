FROM mcr.microsoft.com/devcontainers/base:debian-13

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    ninja-build \
    gdb \
    clang \
    clangd \
    clang-format \
    clang-tidy \
    python3 \
    python3-venv \
    git \
    git-lfs \
    curl \
    && rm -rf /var/lib/apt/lists/* \
    | curl -LsSf https://astral.sh/uv/install.sh \
    | env UV_INSTALL_DIR=/usr/local/bin sh

ENV UV_LINK_MODE=copy
