ARG BASE_CONTAINER=debian
FROM ${BASE_CONTAINER}
RUN apt update && apt install -y \
    git \
    unzip \
    make \
    cmake \
    curl \
    sudo \
    zlib1g-dev \
    m4 \
    && rm -rf /var/lib/apt/lists/*

ARG SS_RELEASE=3.2.0draft28
ARG SS_JOBS=2

COPY . /GraphBLAS
WORKDIR /GraphBLAS
# RUN git checkout tags/$SS_RELEASE
RUN make clean
RUN make library JOBS=$SS_JOBS
RUN make install
