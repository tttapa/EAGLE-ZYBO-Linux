#include <BaremetalCommunicationDef.hpp>
#include <SharedMem.hpp>

#include <chrono>
#include <thread>

using namespace std;
using namespace chrono_literals;

int main() {
    SharedMemory<QRCommStruct> qrComm;
    if (qrComm->getQRState() != QRFSMState::IDLE)
        qrComm->setQRStateIdle();
    qrComm->setQRStateRequest();
    while (qrComm->getQRState() == QRFSMState::QR_READ_REQUEST ||
           qrComm->getQRState() == QRFSMState::QR_READING_BUSY)
        this_thread::sleep_for(10ms);
    switch (qrComm->getQRState()) {
        case QRFSMState::LAND:
            cout << "Land" << endl;
            qrComm->setQRStateIdle();
            break;
        case QRFSMState::NEW_TARGET:
            cout << "New target: " << qrComm->getTargetPosition() << endl;
            break;
        case QRFSMState::QR_UNKNOWN:
            cout << "Unknown" << endl;
            qrComm->setQRStateIdle();
            break;
        case QRFSMState::ERROR:
            cout << "Error" << endl;
            qrComm->setQRStateIdle();
            break;
        default: break;
    }
}