#include <gtest/gtest.h>
#include "../include/cmd_options.h"
#include <cstring>


std::array<std::string, 20> splitIntoWords(const std::string& str) {
    std::stringstream ss(str);
    std::array<std::string, 20> words;
    std::string word;
    for (int i = 0; std::getline(ss, word, ' '); ++i) {
        words.at(i) = word;
    }
    return words;
}

CryptoGuard::ProgramOptions opt_for_tests;

std::string whatsApp(const std::string& programm_call) {
    auto words = splitIntoWords(programm_call);
    char* argv[words.size()];
    int real_size = 0;
    for (; (real_size < words.size()) && !words[real_size].empty(); ++real_size) {
        argv[real_size] = const_cast<char*>(words[real_size].c_str());
    }
    try {
        opt_for_tests.Parse(real_size, argv);
    } catch (std::runtime_error& err) {
        return err.what();
    }
    return "";
}

TEST(ProgramOptionsParserTest, CorrectCall) {
    EXPECT_TRUE(whatsApp("funName -i input -o output --command encrypt -p -reliablePa$$word").empty());
}

TEST(ProgramOptionsParserTest, NoCommand) {
    EXPECT_STREQ(whatsApp("funName -i input -o output").c_str(), 
                "Необходимо задать значение для опции --command.");
}

TEST(ProgramOptionsParserTest, IncorrectCommand) {
    EXPECT_STREQ(whatsApp("funName -i input -o output --command INVALIDcommand").c_str(),
            "Некорректное значение для опции <command>. Доступные команды: encrypt, decrypt, checksum.");
}

TEST(ProgramOptionsParserTest, NoPassword) {
    EXPECT_STREQ(whatsApp("funName -i input -o output --command encrypt").c_str(),
                 "Для шифрования/расшифрования необходима опция --password(-p).");
}

TEST(ProgramOptionsParserTest, NoOutput) {
    EXPECT_STREQ(whatsApp("funName -i input --command encrypt -p -reliablePa$$word").c_str(), 
                "Для шифрования/расшифрования необходима опция --output(-o).");
}

TEST(ProgramOptionsParserTest, NoInput) {
    EXPECT_STREQ(whatsApp("funName --command checksum").c_str(),
                "Для работы программы необхоимо передать имя входного файла с опцией --input.");
}