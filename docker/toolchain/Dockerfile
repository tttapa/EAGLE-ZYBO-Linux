# Ubuntu 18.04 at the time of writing (2019-04-19)
FROM ubuntu:latest as ct-ng

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

################################################################################

FROM ct-ng as build

WORKDIR /home/develop

# Download OpenCV
RUN wget -O opencv-4.1.0.tar.gz https://codeload.github.com/opencv/opencv/tar.gz/4.1.0
RUN tar -xf opencv-4.1.0.tar.gz && rm opencv-4.1.0.tar.gz
RUN mv opencv-4.1.0 opencv

# Build OpenCV
WORKDIR /home/develop/opencv-arm-build
USER root
RUN chown -R develop .
USER develop
RUN chmod -R 755 $TOOLCHAIN_PATH/arm-cortexa9_neon-linux-gnueabihf/sysroot
RUN cmake \
        -DCMAKE_TOOLCHAIN_FILE=../opencv/platforms/linux/arm.toolchain.cmake \
        -DENABLE_VFPV3=ON -DENABLE_NEON=ON \
        -DGNU_MACHINE="arm-cortexa9_neon-linux-gnueabi" \
        -DWITH_V4L=ON -DWITH_LIBV4L=ON \
        -DCMAKE_BUILD_TYPE=RelWithDebInfo \
        -DCMAKE_INSTALL_PREFIX=$TOOLCHAIN_PATH/arm-cortexa9_neon-linux-gnueabihf/sysroot \
        ../opencv
RUN make -j16 && \
    make install && \
    cd && rm -rf /home/develop/opencv-arm-build

WORKDIR /home/develop/opencv-x86-build
USER root
RUN chown -R develop .
USER develop
RUN cmake \
        -DWITH_V4L=ON -DWITH_LIBV4L=ON \
        -DCMAKE_BUILD_TYPE=RelWithDebInfo \
        -DCMAKE_INSTALL_PREFIX=/home/develop/.local \
        ../opencv
RUN make -j16 && \
    make install && \
    cd && rm -rf /home/develop/opencv-x86-build

# Download Google Test
WORKDIR /home/develop
RUN git clone https://github.com/google/googletest.git

# Download and build Doxygen
WORKDIR /home/develop
RUN git clone https://github.com/doxygen/doxygen.git
WORKDIR /home/develop/doxygen/build
USER root
RUN chown -R develop .
USER develop
RUN cmake -DCMAKE_INSTALL_PREFIX=/home/develop/.local ..
RUN make -j16 && \
    make install && \
    cd && rm -rf /home/develop/doxygen/build

# Install dot and gdbserver
USER root
RUN apt-get update && \
    apt-get install -y graphviz gdbserver && \
    apt-get clean autoclean && \
    apt-get autoremove -y && \
    rm -rf /var/lib/apt/lists/*

USER develop

# for gdbserver
EXPOSE 2000