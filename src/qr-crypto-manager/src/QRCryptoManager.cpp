#include <QR-Decode.hpp>
#include <QRCryptoManager.hpp>
#include <base64.hpp>
#include <crypto.hpp>

using namespace std;

void QRCryptoManager::update(const cv::Mat &image) {
    switch (qrComm->getQRState()) {
        // QR reading is requested, start reading
        case QRFSMState::QR_READ_REQUEST: {
            cout << "Starting QR decoding" << endl;
            willBeDecoded = async(launch::async, QR::decode, image.clone());
            // Use a clone of the image, because it will be updated by
            // vision, and this will confuse the QR decoder.
            qrComm->setQRStateBusy();
        } break;

        // If the QR reader has been started, check if it's finished yet
        case QRFSMState::QR_READING_BUSY: {
            if (isQRReady()) {
                cout << "QR decoding done" << endl;
                string qrDecoded = willBeDecoded.get();
                decodeCrypto(qrDecoded);
            }
        } break;

        default: {
        }  // Ignore all other QR FSM states, as they are handled by ANC
    }
}

void QRCryptoManager::decodeCrypto(const std::string &QRdata) {
    if (QRdata.empty()) {
        cerr << ANSIColors::magenta << "Could not decode QR code"
             << ANSIColors::reset << endl;
        qrComm->setQRStateError();
        return;
    }
    vector<uint8_t> base64Decoded = Base64::decode(QRdata);
    try {
        for (uint8_t byte : base64Decoded) {
            cout << hex << +byte << ' ';
        }
        cout << dec << endl;
        CryptoInstruction instr = decrypt(base64Decoded, HWImplementation{});
        switch (instr.getInstructionType()) {
            case CryptoInstruction::InstructionType::GOTO: {
                VisionPosition dest = {
                    (float) instr.getXCoordinate(),
                    (float) instr.getYCoordinate(),
                };
                cout << "Crypto goto " << dest << endl;
                VisionPosition currPos = {
                    (float) instr.getCurrentXCoordinate(),
                    (float) instr.getCurrentYCoordinate(),
                };
                cout << "Current position: " << currPos << endl;
                qrComm->setCurrentPosition(currPos);
                qrComm->setTargetPosition(dest);
            } break;
            case CryptoInstruction::InstructionType::LAND: {
                cout << "Crypto land" << endl;
                VisionPosition currPos = {
                    (float) instr.getCurrentXCoordinate(),
                    (float) instr.getCurrentYCoordinate(),
                };
                cout << "Current position: " << currPos << endl;
                qrComm->setCurrentPosition(currPos);
                qrComm->setQRStateLand();
            } break;
            case CryptoInstruction::InstructionType::UNKNOWN: {
                VisionPosition currPos = {
                    (float) instr.getCurrentXCoordinate(),
                    (float) instr.getCurrentYCoordinate(),
                };
                cout << "Current position: " << currPos << endl;
                qrComm->setCurrentPosition(currPos);
                qrComm->setQRStateUnkown();
                auto &vecdata = instr.getUnknownData();
                // TODO: print as hex?
                string data = {vecdata.begin(), vecdata.end()};
                cerr << ANSIColors::yellowb
                     << "Warning: Unknown crypto data: " << data << endl;
            } break;
            default: { qrComm->setQRStateError(); }
        }
    } catch (CryptoException &e) {
        cerr << ANSIColors::redb << e.what() << ANSIColors::reset << endl;
        qrComm->setQRStateError();
    }
}

bool QRCryptoManager::isQRReady() const {
    return willBeDecoded.wait_for(chrono::seconds(0)) == future_status::ready;
}
