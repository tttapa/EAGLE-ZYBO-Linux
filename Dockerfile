FROM eagle1/ct-ng/develop:latest

# for gdbserver
EXPOSE 2000

WORKDIR /home/develop
RUN git clone https://github.com/google/googletest.git
RUN wget -O opencv-4.1.0.tar.gz https://codeload.github.com/opencv/opencv/tar.gz/4.1.0
RUN tar -xf opencv-4.1.0.tar.gz && rm opencv-4.1.0.tar.gz
RUN mv opencv-4.1.0 opencv
WORKDIR /home/develop/opencv-build
USER root
RUN chown -R develop .
USER develop
RUN mkdir -p /home/develop/opt/opencv-arm
RUN cmake -DCMAKE_TOOLCHAIN_FILE=../opencv/platforms/linux/arm.toolchain.cmake \
        -DENABLE_VFPV3=ON -DENABLE_NEON=ON \
        -DGNU_MACHINE="arm-cortexa9_neon-linux-gnueabi" \
        -DWITH_V4L=ON -DWITH_LIBV4L=ON \
        -DCMAKE_BUILD_TYPE=RelWithDebInfo \
        -DCMAKE_INSTALL_PREFIX=/home/develop/opt/opencv-arm \
        ../opencv
RUN make -j$(nproc)
RUN make install

USER develop

ARG WORKSPACE_ROOT
ENV WORKSPACE_ROOT=${WORKSPACE_ROOT}
VOLUME ${WORKSPACE_ROOT}
WORKDIR ${WORKSPACE_ROOT}/build