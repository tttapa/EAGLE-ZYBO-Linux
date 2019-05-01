#pragma once

#include <SharedMem.hpp>
#include <future>
#include <opencv2/core/mat.hpp>
#include <string>

class QRCryptoManager {
  public:
    QRCryptoManager(BaremetalShared<BaremetalCommStruct> &baremetal)
        : baremetal{baremetal} {}
    void update(const cv::Mat &image);
    void decodeCrypto(const std::string &QRdata);
    bool isQRReady() const;

  private:
    /// Result of the QR decoder (asynchronous)
    std::future<std::string> willBeDecoded;
    BaremetalShared<BaremetalCommStruct> &baremetal;
};
