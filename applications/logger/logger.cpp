#include <ANSIColors.hpp>
#include <LocationFinder.hpp>
#include <LocationTracker.hpp>
#include <Logger.hpp>
#include <PerfTimer.hpp>
#include <QRCryptoManager.hpp>
#include <chrono>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <unistd.h>  // usleep

using namespace std;
using namespace chrono_literals;

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

void loop() {
    // Initialize communication with Bare-metal
    Logger logger("239.0.0.2", 5003);
    cout << "Waiting for Baremetal to be initialized ..." << endl;
    while (!logger.isInitialized())
        usleep(10'000);
    cout << ANSIColors::greenb << "Baremetal initialization done!"
         << ANSIColors::reset << endl;
    // Initialize the logger
    logger.begin();
    promise<void> loggerStop;
    // And start it in a new thread, updating every 5ms, until loggerStop is set
    thread loggerThread(
        [&logger](auto loggerStop) {
            cout << "Logger thread started" << endl;
            while (loggerStop.wait_for(5ms) != future_status::ready)
                logger.update();
            cout << "Logger thread stopped" << endl;
        },
        move(loggerStop.get_future()));

    cout << "Press any key to restart the logger" << endl;
    cin.ignore();

    loggerStop.set_value();  // Tell the logger thread to stop
    loggerThread.join();     // TODO: is this necessary?
}