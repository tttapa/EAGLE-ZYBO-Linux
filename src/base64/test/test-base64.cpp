#include <base64.hpp>
#include <gtest/gtest.h>
#include <sstream>

using namespace std;
using namespace Base64;

TEST(base64, base64) {
    string b64str           = "dGhpcyBpcyBhIHRlc3QK";
    vector<uint8_t> vresult = decode(b64str);
    string decode_result    = {vresult.begin(), vresult.end()};
    string decode_expected  = "this is a test\n";
    string encode_result    = encode(vresult);
    ASSERT_EQ(decode_result, decode_expected);
    ASSERT_EQ(encode_result, b64str);
}