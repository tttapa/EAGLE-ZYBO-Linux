#pragma once

#include <BaremetalCommunicationDef.hpp>
#include <SharedMem.hpp>
#include <future>
#include <opencv2/core/mat.hpp>
#include <string>

class QRCryptoManager {
  public:
    void update(const cv::Mat &image);
    void decodeCrypto(const std::string &QRdata);
    bool isQRReady() const;

  private:
    /// Result of the QR decoder (asynchronous)
    std::future<std::string> willBeDecoded;
    BaremetalShared<QRCommStruct> qrComm;
};
