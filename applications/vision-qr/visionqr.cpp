#include "base64.hpp"
#include "crypto.hpp"
#include <ANSIColors.hpp>
#include <LocationFinder.hpp>
#include <LocationTracker.hpp>
#include <PerfTimer.hpp>
#include <QR-Decode.hpp>
#include <chrono>
#include <future>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
#include <thread>

using namespace std;

void loop();

int main() {
    while (1) {
        try {
            loop();
        } catch (std::runtime_error &e) {
            cerr << ANSIColors::redb << e.what() << ANSIColors::reset << endl;
            cerr << "Restarting loop ..." << endl;
        }
    }
}

// TODO naar logger verplaatsen!!
// --------------------------------
const int QR_READING = 2, CRYPTO_BUSY = 3, NEW_TARGET = 4, LAND = 5,
          QR_UNKNOWN = 6, ERROR = 7, QR_READING_BUSY = 8;
void setState(int state);
int getState();
// --------------------------------

void loop() {
    LocationFinder lf = cv::VideoCapture(0);
    LocationTracker lt;

    cv::VideoCapture &cap = lf.getCapture();
    int frame_width       = cap.get(cv::CAP_PROP_FRAME_WIDTH);
    int frame_height      = cap.get(cv::CAP_PROP_FRAME_HEIGHT);
    double fps            = cap.get(cv::CAP_PROP_FPS);

    cout << "Frame dimensions: (" << frame_width << "×" << frame_height << ")\n"
         << "Frame rate      : " << fps << " fps" << std::endl;

    cv::Mat locimg;
    future<string> willBeDecoded;

    while (true) {
        PerfTimer pt;
        Point locInSquare = lf.updateLocation();
        Point location    = lt.update(locInSquare);

        if (getState() == QR_READING) {
            willBeDecoded =
                async(launch::async, QR::decode, lf.getImage().clone());
            setState(QR_READING_BUSY);
        } else if (getState() == QR_READING_BUSY) {
            if (willBeDecoded.wait_for(chrono::seconds(0)) ==
                future_status::ready) {
                string qrDecoded = willBeDecoded.get();
                if (qrDecoded.empty()) {
                    setState(ERROR);
                } else {
                    vector<uint8_t> base64Decoded = Base64::decode(qrDecoded);
                    try {
                        CryptoInstruction instr = decrypt(base64Decoded);
                        switch (instr.getInstructionType()) {
                            case CryptoInstruction::InstructionType::GOTO:
                                setState(NEW_TARGET);
                                break;
                            case CryptoInstruction::InstructionType::LAND:
                                setState(LAND);
                                break;
                            case CryptoInstruction::InstructionType::UNKNOWN:
                                setState(QR_UNKNOWN);
                                break;
                            default:
                                setState(ERROR);
                                break;
                        }
                    } catch (CryptoException &e) {
                        setState(ERROR);
                    }
                }
            }
        }

        auto duration = pt.getDuration();
        cout << "Position: " << location << endl;
        cout << "Vision duration: " << duration << " µs → " << 1e6 / duration
             << " fps" << endl;
    }
}