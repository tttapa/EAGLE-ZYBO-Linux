FROM eagle1/crypto/develop

USER root
RUN apt-get update && \
    apt-get install -y git wget gcc make xz-utils bc && \
    apt-get clean autoclean && \
    apt-get autoremove -y && \
    rm -rf /var/lib/apt/lists/*

USER develop
WORKDIR /home/develop
RUN git clone https://github.com/Xilinx/linux-xlnx
WORKDIR /home/develop/linux-xlnx
RUN git checkout xilinx-v2016.4

ARG WORKSPACE_ROOT
ENV WORKSPACE_ROOT=${WORKSPACE_ROOT}
VOLUME ${WORKSPACE_ROOT}
WORKDIR ${WORKSPACE_ROOT}/build