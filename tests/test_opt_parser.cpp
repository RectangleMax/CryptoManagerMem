#include <gtest/gtest.h>
#include "../include/cmd_options.h"
#include <cstring>


struct Deleter {
    size_t size_array;
    void operator()(char* charPtrArray[]) { 
        for (size_t i = 0; i < size_array; ++i) {
            delete[] charPtrArray[i];
        }
        delete[] charPtrArray; 
    }
};

std::unique_ptr<char*[], Deleter> convertIntoCharPtrArray(const std::stringstream& ss, int& array_size) {
    std::vector<std::string> vec;
    for (std::string line; std::getline(const_cast<std::stringstream&>(ss), line, ' '); ) {
        vec.push_back(line);
    }
    
    auto argv = std::unique_ptr<char*[], Deleter>(new char*[vec.size()], Deleter{vec.size()});
    for (size_t i = 0; i < vec.size(); ++i) {
        char* str_copy = new char[vec[i].size() + 1];  // +1 для '\0'
        std::strcpy(str_copy, vec[i].c_str());
        argv[i] = str_copy;
    }
    array_size = vec.size();
    return argv;
}

using ERR_CODE = CryptoGuard::ProgramOptions::PARSING_ERR;
std::stringstream getTestData(ERR_CODE err_code) {
    const char* cmd;
    switch(err_code) {
        case ERR_CODE::NO_ERROR:
            cmd = "funName -i input -o output --command encrypt -p -goodPa$$word";
            break;
        case ERR_CODE::NO_CMD: 
            cmd = "funName -i input -o output";
            break;
        case ERR_CODE::INVALID_CMD: 
            cmd = "funName -i input -o output --command INVALIDcommand";
            break;
        case ERR_CODE::NO_INPUT:
            cmd = "funName --command checksum";
            break;
        case ERR_CODE::NO_OUTPUT:
            cmd = "funName -i input --command encrypt";
            break;
        case ERR_CODE::NO_PASSWORD:
            cmd = "funName -i input -o output --command encrypt";
            break;
    }
    return static_cast<std::stringstream>(cmd);
}

CryptoGuard::ProgramOptions opt;

TEST(ProgramOptionsParserTest, CorrectOptions) {
    ERR_CODE code = ERR_CODE::NO_ERROR;
    int argc;
    auto argv = convertIntoCharPtrArray(getTestData(code), argc);
    EXPECT_EQ(code, opt.Parse(argc, argv.get()));
}

TEST(ProgramOptionsParserTest, ForgotCommand) {
    ERR_CODE code = ERR_CODE::NO_CMD;
    int argc;
    auto argv = convertIntoCharPtrArray(getTestData(code), argc);
    EXPECT_EQ(code, opt.Parse(argc, argv.get()));
}

TEST(ProgramOptionsParserTest, IncorrectCommand) {
    ERR_CODE code = ERR_CODE::INVALID_CMD;
    int argc;
    auto argv = convertIntoCharPtrArray(getTestData(code), argc);
    EXPECT_EQ(code, opt.Parse(argc, argv.get()));
}

TEST(ProgramOptionsParserTest, ForgotIniputOption) {
    ERR_CODE code = ERR_CODE::NO_INPUT;
    int argc;
    auto argv = convertIntoCharPtrArray(getTestData(code), argc);
    EXPECT_EQ(code, opt.Parse(argc, argv.get()));
}

TEST(ProgramOptionsParserTest, ForgotOutputOption) {
    ERR_CODE code = ERR_CODE::NO_OUTPUT;
    int argc;
    auto argv = convertIntoCharPtrArray(getTestData(code), argc);
    EXPECT_EQ(code, opt.Parse(argc, argv.get()));
}

TEST(ProgramOptionsParserTest, ForgotPassowrd) {
    ERR_CODE code = ERR_CODE::NO_PASSWORD;
    int argc;
    auto argv = convertIntoCharPtrArray(getTestData(code), argc);
    EXPECT_EQ(code, opt.Parse(argc, argv.get()));
}


// int num_test_cases = 6;

// struct TestData { int num_case; };
// class TestParam : public testing::TestWithParam<TestData> {};

// TEST_P(TestParam, SetInput) {
//     auto err_code = static_cast<ERR_CODE>(GetParam().num_case);
//     int argc;
//     auto argv = convertIntoCharPtrArray(getTestData(err_code), argc);
//     EXPECT_EQ(err_code, opt.Parse(argc, argv.get()));
// }

// // 4. Функция генерации тестовых данных
// std::vector<TestData> GenerateTestData() {
//     std::vector<TestData> data;
//     for (int i = 0; i < num_test_cases; ++i) {
//         data.push_back({i});
//     }
//     return data;
// }

// // 5. Инстанцирование тестов с данными из вектора
// INSTANTIATE_TEST_SUITE_P(OptParser, TestParam,
//     testing::ValuesIn(GenerateTestData())  // Данные из вектора
// );
