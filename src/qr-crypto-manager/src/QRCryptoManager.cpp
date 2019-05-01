#include <QR-Decode.hpp>
#include <QRCryptoManager.hpp>
#include <base64.hpp>
#include <crypto.hpp>

using namespace std;

void QRCryptoManager::update(const cv::Mat &image) {
    switch (baremetal->qrState) {
        // QR reading is requested, start reading
        case QRFSMState::QR_READ: {
            willBeDecoded = async(launch::async, QR::decode, image.clone());
            // Use a clone of the image, because it will be updated by
            // vision, and this will confuse the QR decoder.
            baremetal->qrState = QRFSMState::QR_READING_BUSY;
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
        baremetal->qrState = QRFSMState::ERROR;
        return;
    }
    vector<uint8_t> base64Decoded = Base64::decode(QRdata);
    try {
        CryptoInstruction instr = decrypt(base64Decoded);
        switch (instr.getInstructionType()) {
            case CryptoInstruction::InstructionType::GOTO:
                baremetal->qrState = QRFSMState::NEW_TARGET;
                // TODO: set target
                break;
            case CryptoInstruction::InstructionType::LAND:
                baremetal->qrState = QRFSMState::LAND;
                break;
            case CryptoInstruction::InstructionType::UNKNOWN:
                baremetal->qrState = QRFSMState::QR_UNKNOWN;
                break;
            default: baremetal->qrState = QRFSMState::ERROR;
        }
    } catch (CryptoException &e) {
        cerr << ANSIColors::redb << e.what() << ANSIColors::reset << endl;
        baremetal->qrState = QRFSMState::ERROR;
    }
}

bool QRCryptoManager::isQRReady() const {
    return willBeDecoded.wait_for(chrono::seconds(0)) == future_status::ready;
}
