#include "../include/cmd_options.h"
#include "../include/crypto_guard_ctx.h"
// #include <filesystem>

// "prestuplenie-i-nakazanie.txt", "temp_encrypted", "veryGoodPassword");

int main(int argn, char *argv[]) {
    CryptoGuard::ProgramOptions opt;
    CryptoGuard::CryptoGuardCtx cryptoContext;
    
    if (CryptoGuard::ProgramOptions::PARSING_ERR::NO_ERROR != opt.Parse(argn, argv))
        return 1;
    
    std::fstream file_input( opt.GetInputFile(),  std::ios::binary | std::ios::in);
    std::fstream file_output;

    switch (opt.GetCommand()) {
        case CryptoGuard::ProgramOptions::COMMAND_TYPE::ENCRYPT:
            file_output.open(opt.GetOutputFile(), std::ios::binary | std::ios::out | std::ios::trunc);
            cryptoContext.EncryptFile(file_input, file_output, opt.GetPassword());
            break;
        case CryptoGuard::ProgramOptions::COMMAND_TYPE::DECRYPT:
            file_output.open(opt.GetOutputFile(), std::ios::binary | std::ios::out | std::ios::trunc);
            cryptoContext.DecryptFile(file_input, file_output, opt.GetPassword());
            break;
        case CryptoGuard::ProgramOptions::COMMAND_TYPE::CHECKSUM:
            std::cout << cryptoContext.CalculateChecksum(file_input) << std::endl;
            break;    
    }
    
    return 0;
}

