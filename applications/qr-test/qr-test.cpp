#include <BaremetalCommunicationDef.hpp>
#include <SharedMem.hpp>

int main() {
    SharedMemory<QRCommStruct> qrComm;
    qrComm->setQRStateRequest();
}