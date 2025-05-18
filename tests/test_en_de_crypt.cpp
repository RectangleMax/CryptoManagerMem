#include <gtest/gtest.h>
#include <stdexcept>
#include "../include/crypto_guard_ctx.h"


CryptoGuard::CryptoGuardCtx cryptoContext1;
std::string password = "VeryGoodPa$$word";

std::stringstream EncryptAndDecrypt(std::iostream& originalStream) {
    std::stringstream encryptedStream;
    std::stringstream decryptedStream;
    cryptoContext1.EncryptFile(originalStream,  encryptedStream, password);
    cryptoContext1.DecryptFile(encryptedStream, decryptedStream, password);
    return decryptedStream;
}

TEST(EncryptDecrypt, InputSStream) {
    std::stringstream originalStream{"Text for\nencryption\0 and decryption"};
    EXPECT_EQ(originalStream.str(), EncryptAndDecrypt(originalStream).str());
}

TEST(EncryptDecrypt, InputSStreamEmtpy) {
    std::stringstream originalStream{""};
    EXPECT_EQ(originalStream.str(), EncryptAndDecrypt(originalStream).str());
}

TEST(EncryptDecrypt, InputFile) {
    std::fstream originalStream("prestuplenie-i-nakazanie.txt", std::ios::binary | std::ios::in);
    if (!originalStream.good()) {
        std::runtime_error("При тестировании методов EncryptFile и DecryptFile возникли проблемы с загрузкой файла prestuplenie-i-nakazanie.txt");
    }
    std::stringstream originalStringStream;
    originalStringStream << originalStream.rdbuf();
    originalStream.seekg(0);
    EXPECT_EQ(originalStringStream.str(), EncryptAndDecrypt(originalStream).str());
}

// INSTANTIATE_TEST_SUITE_P(
//     EncryptDecryptTest,
//     EncryptDecryptParam,
//     testing::Values(
//         TestData{"Text for\nencryption\0 and decryption", 1}
//     )
//     // testing::Values(
//     //     TestData{"prestuplenie-i-nakazanie.txt", 0},
//     //     TestData{"Text for\nencryption\0 and decryption", 1},
//     //     TestData{"emptyFile", 0},
//     //     TestData{"", 1}
//     // )
// );

// TEST(EncryptDecryptThrowTest, ThrowNoExistFile) {
//     std::fstream file("noExistFile.txt");
//     std::stringstream encryptedStream;
//     EXPECT_THROW({
//         cryptoContext1.EncryptFile(file, encryptedStream, password);
//     }, std::runtime_error);
// }