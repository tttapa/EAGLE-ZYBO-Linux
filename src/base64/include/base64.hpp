#include <cstdint>
#include <string>
#include <vector>

namespace Base64 {

/**
 * @brief   Base64-encode the given data.
 * 
 * @param   buf 
 *          A pointer to the binary data to encode. 
 * @param   bufLen 
 *          The length of the data.
 * @return  The base64-encoded version of the data.
 */
std::string encode(uint8_t const *buf, size_t bufLen);

/**
 * @brief   Base64-encode the given data.
 * 
 * @param   data 
 *          The binary data to encode.
 * @return  The base64-encoded version of the data.
 */
std::string encode(const std::vector<uint8_t> &data);

/**
 * @brief   Decode the given base64-encoded string.
 * 
 * @param   b64str
 *          The base64-encoded string to decode. 
 * @return  The decoded binary data.
 */
std::vector<uint8_t> decode(std::string const &b64str);

}  // namespace Base64