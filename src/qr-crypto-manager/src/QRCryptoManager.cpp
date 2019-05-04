#include <QR-Decode.hpp>
#include <QRCryptoManager.hpp>
#include <base64.hpp>
#include <crypto.hpp>

using namespace std;

void QRCryptoManager::update(const cv::Mat &image) {
    switch (baremetal->getQRState()) {
        // QR reading is requested, start reading
        case QRFSMState::QR_READ_REQUEST: {
            willBeDecoded = async(launch::async, QR::decode, image.clone());
            // Use a clone of the image, because it will be updated by
            // vision, and this will confuse the QR decoder.
            baremetal->setQRStateBusy();
        } break;

        // If the QR reader has been started, check if it's finished yet
        case QRFSMState::QR_READING_BUSY: {
            if (isQRReady()) {
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
        baremetal->setQRStateError();
        return;
    }
    vector<uint8_t> base64Decoded = Base64::decode(QRdata);
    try {
        CryptoInstruction instr = decrypt(base64Decoded);
        switch (instr.getInstructionType()) {
            case CryptoInstruction::InstructionType::GOTO: {
                baremetal->setTargetPosition(instr.getXCoordinate(),
                                             instr.getYCoordinate());
            } break;
            case CryptoInstruction::InstructionType::LAND: {
                baremetal->setQRStateLand();
            } break;
            case CryptoInstruction::InstructionType::UNKNOWN: {
                baremetal->setQRStateUnkown();
                auto &vecdata = instr.getUnknownData();
                // TODO: print as hex?
                string data = {vecdata.begin(), vecdata.end()};
                cerr << ANSIColors::yellowb
                     << "Warning: Unknown crypto data: " << data << endl;
            } break;
            default: { baremetal->setQRStateError(); }
        }
    } catch (CryptoException &e) {
        cerr << ANSIColors::redb << e.what() << ANSIColors::reset << endl;
        baremetal->setQRStateError();
    }
}

bool QRCryptoManager::isQRReady() const {
    return willBeDecoded.wait_for(chrono::seconds(0)) == future_status::ready;
}
