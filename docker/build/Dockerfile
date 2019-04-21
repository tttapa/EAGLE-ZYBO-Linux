FROM eagle1/ct-ng:v1

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
RUN make -j$(nproc) && \
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
RUN make -j$(nproc) && \
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
RUN make -j$(nproc) && \
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

# Bind the workspace root
ARG WORKSPACE_ROOT
ENV WORKSPACE_ROOT=${WORKSPACE_ROOT}
VOLUME ${WORKSPACE_ROOT}
WORKDIR ${WORKSPACE_ROOT}/build