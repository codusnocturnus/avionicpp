FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

# Install essential setup tools
RUN apt-get update && apt-get install -y \
    wget \
    curl \
    git \
    gnupg \
    lsb-release \
    software-properties-common \
    ninja-build \
    build-essential \
    && rm -rf /var/lib/apt/lists/*

# NOT WORKING...
# node/npm/chromium + dependencies required for Cline web_fetch
# RUN apt -y install \
#     nodejs \
#     npm \
#     chromium-browser \
#     libx11-xcb1 \
#     libxcomposite1 \
#     liboss4-salsa-asound2 \
#     libatk1.0-0 \
#     libatk-bridge2.0-0 \
#     libcairo2 \
#     libcups2 \
#     libdbus-1-3 \
#     libexpat1 \
#     libfontconfig1 \
#     libgbm1 \
#     libgcc1 \
#     libglib2.0-0 \
#     libgtk-3-0 \
#     libnspr4 \
#     libpango-1.0-0 \
#     libpangocairo-1.0-0 \
#     libstdc++6 \
#     libx11-6 \
#     libx11-xcb1 \
#     libxcb1 \
#     libxcomposite1 \
#     libxcursor1 \
#     libxdamage1 \
#     libxext6 \
#     libxfixes3 \
#     libxi6 \
#     libxrandr2 \
#     libxrender1 \
#     libxss1 \
#     libxtst6 \
#     && npm install -g puppeteer

# Add Kitware APT Repository for latest CMake
RUN wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | gpg --dearmor - | tee /usr/share/keyrings/kitware-archive-keyring.gpg >/dev/null && \
    echo 'deb [signed-by=/usr/share/keyrings/kitware-archive-keyring.gpg] https://apt.kitware.com/ubuntu/ noble main' | tee /etc/apt/sources.list.d/kitware.list >/dev/null && \
    apt-get update && \
    apt-get install -y cmake kitware-archive-keyring && \
    rm -rf /var/lib/apt/lists/*

# Add Ubuntu Toolchain PPA (for latest GCC)
RUN add-apt-repository -y ppa:ubuntu-toolchain-r/test

# Install GCC 14
RUN apt-get update && apt-get install -y \
    gcc-14 \
    g++-14 \
    gdb \
    && update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-14 100 \
    && update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-14 100 \
    && update-alternatives --install /usr/bin/gcov gcov /usr/bin/gcov-14 100

# Install latest LLVM/Clang using the official script
# We target version 21
RUN wget https://apt.llvm.org/llvm.sh && \
    chmod +x llvm.sh && \
    ./llvm.sh 21 && \
    rm llvm.sh && \
    apt-get update && apt-get install -y \
    clang-format-21 \
    clang-tidy-21 \
    lldb-21 \
    lld-21 \
    llvm-21-tools \
    && rm -rf /var/lib/apt/lists/*

# Set up Clang alternatives
RUN update-alternatives --install /usr/bin/clang clang /usr/bin/clang-21 100 \
    && update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-21 100 \
    && update-alternatives --install /usr/bin/clang-format clang-format /usr/bin/clang-format-21 100 \
    && update-alternatives --install /usr/bin/clang-tidy clang-tidy /usr/bin/clang-tidy-21 100 \
    && update-alternatives --install /usr/bin/lldb lldb /usr/bin/lldb-21 100 \
    && update-alternatives --install /usr/bin/llvm-cov llvm-cov /usr/bin/llvm-cov-21 100 \
    && update-alternatives --install /usr/bin/llvm-profdata llvm-profdata /usr/bin/llvm-profdata-21 100

# Install Code Coverage and Profiling Tools
RUN apt-get update && apt-get install -y \
    lcov \
    gcovr \
    valgrind \
    linux-tools-generic \
    linux-tools-common \
    graphviz \
    && rm -rf /var/lib/apt/lists/*

RUN git config --global --add safe.directory '*'

ENV SHELL /bin/bash
