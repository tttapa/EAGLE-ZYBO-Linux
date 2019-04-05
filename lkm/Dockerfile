FROM ubuntu:xenial

RUN apt-get update && \
    apt-get install -y sudo git wget gcc make xz-utils \
        bc gcc-4.9-arm-linux-gnueabihf && \
    apt-get clean autoclean && \
    apt-get autoremove -y && \
    rm -rf /var/lib/apt/lists/*
    
RUN useradd -ms /bin/bash develop
RUN echo "develop   ALL=(ALL:ALL) ALL" >> /etc/sudoers

RUN ln -s arm-linux-gnueabihf-gcc-4.9 /usr/bin/arm-linux-gnueabihf-gcc

USER develop
WORKDIR /home/develop
RUN wget https://cdn.kernel.org/pub/linux/kernel/v4.x/linux-4.4.1.tar.xz
RUN tar xf linux-4.4.1.tar.xz && rm linux-4.4.1.tar.xz

ARG WORKSPACE_ROOT
ENV WORKSPACE_ROOT=${WORKSPACE_ROOT}
VOLUME ${WORKSPACE_ROOT}
WORKDIR ${WORKSPACE_ROOT}/build