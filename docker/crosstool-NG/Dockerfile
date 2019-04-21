# Ubuntu 18.04 at the time of writing (2019-04-19)
FROM ubuntu:latest

# Install some tools and compilers + clean up
RUN apt-get update && \
    apt-get install -y sudo git wget \
                       gcc g++ cmake make autoconf automake \
                       gperf diffutils bzip2 xz-utils \
                       flex gawk help2man libncurses-dev patch bison \
                       python-dev gnupg2 texinfo unzip libtool-bin && \
    apt-get clean autoclean && \
    apt-get autoremove -y && \
    rm -rf /var/lib/apt/lists/*

# Add a user called `develop`
RUN useradd -ms /bin/bash develop
RUN echo "develop   ALL=(ALL:ALL) ALL" >> /etc/sudoers

USER develop
WORKDIR /home/develop

ENV CROSSTOOL_NG_VERSION=1.24.0
RUN git clone https://github.com/crosstool-ng/crosstool-ng.git
WORKDIR /home/develop/crosstool-ng
RUN git checkout crosstool-ng-${CROSSTOOL_NG_VERSION}
RUN ./bootstrap
RUN ./configure --prefix=/home/develop/.local
RUN make -j$(nproc)
RUN make install
ENV PATH=/home/develop/.local/bin:$PATH
RUN ct-ng --version

WORKDIR /home/develop
RUN wget https://cdn.kernel.org/pub/linux/kernel/v4.x/linux-4.4.tar.xz

RUN mkdir /home/develop/ZYBO
WORKDIR /home/develop/ZYBO
COPY --chown=develop:develop config .config
RUN ls -al
RUN ct-ng build && rm -rf .build
ENV TOOLCHAIN_PATH=/home/develop/x-tools/arm-cortexa9_neon-linux-gnueabihf
ENV PATH=${TOOLCHAIN_PATH}/bin:$PATH