#include <ANSIColors.hpp>
#include <LocationFinder.hpp>
#include <LocationTracker.hpp>
#include <PerfTimer.hpp>
#include <QRCryptoManager.hpp>
#include <ThreadedLogger.hpp>
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
    // Create a logger, and wait for Baremetal to initialize it.
    ThreadedLogger logger = Logger{{
        {"239.0.0.2", 5003},      // Multicast
        {"192.168.4.100", 5001},  // Unicast
    }};
    cout << "Waiting for Baremetal to be initialized ..." << endl;
    while (!logger.isInitialized())
        this_thread::sleep_for(10ms);
    cout << ANSIColors::greenb << "Baremetal initialization done!"
         << ANSIColors::reset << endl;
    // Spawns another thread, is cleaned up when destructed. Update every 5ms.
    logger.begin(500ms);

    cout << "Press any key to restart the logger" << endl;
    cin.ignore();  // Wait for user input
}