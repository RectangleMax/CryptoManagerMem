#include <gtest/gtest.h>
#include "../include/crypto_guard_ctx.h"

std::fstream getFstream(const std::string& str) {
    return std::fstream(str, std::ios::binary | std::ios::in);
}

std::stringstream getSstream(const std::string& str) {
    return std::stringstream(str);
}

bool compareStreams(std::iostream& s1, std::iostream& s2) {
    char byte1;
    char byte2;
    while(s1.get(byte1)) {
        if (!s2.get(byte2))
            return false;
        if (byte1 != byte2)
            return false;
    }
    if (!s2.get(byte2))
        return true;
    return false;
}

struct TestData {
    std::string original_stream;
    int streamFlag; // 0 - fileStream, stringStream
};

class EncryptDecryptParam : public testing::TestWithParam<TestData> {};

CryptoGuard::CryptoGuardCtx cryptoContext1;
std::string password = "VeryGoodPa$$word";

TEST_P(EncryptDecryptParam, SetN) {
    auto params = GetParam();
    std::unique_ptr<std::iostream> originalStream_ptr;
    std::stringstream encryptedStream;
    std::stringstream decryptedStream;
    if (params.streamFlag == 0) {
        originalStream_ptr  = std::make_unique<std::fstream>(getFstream(params.original_stream));
    } else if (params.streamFlag == 1) {
        originalStream_ptr  = std::make_unique<std::stringstream>(getSstream(params.original_stream));
    }

    cryptoContext1.EncryptFile(*originalStream_ptr, encryptedStream, password);
    cryptoContext1.DecryptFile(encryptedStream,     decryptedStream, password);

    originalStream_ptr.get()->clear();
    originalStream_ptr.get()->seekg(0);

    EXPECT_TRUE(compareStreams(*originalStream_ptr, decryptedStream));
}

INSTANTIATE_TEST_SUITE_P(
    EncryptDecryptTest,
    EncryptDecryptParam,
    testing::Values(
        TestData{"prestuplenie-i-nakazanie.txt", 0},
        TestData{"Text for\nencryption\0 and decryption", 1},
        TestData{"emptyFile", 0},
        TestData{"", 1}
    )
);

TEST(EncryptDecryptThrowTest, ThrowNoExistFile) {
    std::fstream file("noExistFile.txt");
    std::stringstream encryptedStream;
    EXPECT_THROW({
        cryptoContext1.EncryptFile(file, encryptedStream, password);
    }, std::runtime_error);
}