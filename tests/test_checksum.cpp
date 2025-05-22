#include <gtest/gtest.h>
#include "../include/crypto_guard_ctx.h"

std::fstream getFstream1(const std::string& str) {
    return std::fstream(str, std::ios::binary | std::ios::in);
}

std::stringstream getSstream1(const std::string& str) {
    return std::stringstream(str);
}

struct TestData {
    std::string original_stream;
    int streamFlag; // 0 - fileStream, stringStream
    std::string actual_checksum;
};

class InputData : public testing::TestWithParam<TestData> {};

CryptoGuard::CryptoGuardCtx cryptoContext2;

TEST_P(InputData, Set) {
    auto params = GetParam();
    std::unique_ptr<std::iostream> stream_ptr;
    if (params.streamFlag == 0) {
        stream_ptr  = std::make_unique<std::fstream>(getFstream1(params.original_stream));
    } else if (params.streamFlag == 1) {
        stream_ptr  = std::make_unique<std::stringstream>(getSstream1(params.original_stream));
    }   
    EXPECT_STREQ((cryptoContext2.CalculateChecksum(*stream_ptr)).c_str(), params.actual_checksum.c_str());
}

INSTANTIATE_TEST_SUITE_P(
    ChecksumTest,
    InputData,
    testing::Values(
        TestData{"../prestuplenie-i-nakazanie.txt", 0, "ef76f4ddda4f150288f3deea5c5dc1ea10522c0ad8dcfb3f69c96d4183e1d9c0"},
        TestData{"Text for calculation checksum", 1, "1f97968965d166ea5555810581865f3f60c36a0b64228d7c60012f4cf4266e12"}
    )
);

TEST(ChecksumTest, ThrowNoExistInputFile) {
    std::fstream file("noExistFile.txt");
    std::stringstream encryptedStream;
    EXPECT_THROW({
        cryptoContext2.CalculateChecksum(file);
    }, std::runtime_error);
}