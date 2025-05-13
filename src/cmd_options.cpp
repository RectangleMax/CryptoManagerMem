#include "../include/cmd_options.h"

namespace CryptoGuard {


ProgramOptions::ProgramOptions() : desc_("Allowed options") {
    desc_.add_options()
    ("help,h", "Help screen")
    ("command",  po::value<std::string>(), "Commands are available: encrypt, decrypt, checksum.")
    ("input,i",  po::value<std::string>(), "Input full or local file name.") 
    ("output,o",  po::value<std::string>(), "Output local file name (saving into program directory)")
    ("password,p",  po::value<std::string>(), "Password for encryption and decryption.");
}

ProgramOptions::~ProgramOptions() = default;

ProgramOptions::PARSING_ERR ProgramOptions::Parse(int argc, char *argv[]) { 
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc_), vm);
    po::notify(vm);
    std::map<PARSING_ERR, std::string> errmap;

//  Разбор входных аргументов
//  1. Проверка сценария с вызовом help
    if (vm.count("help")  &&  (vm.size() == 1)) {
        PrintHelp();
        return PARSING_ERR::NO_ERROR;
    }

//  2. Разбор других опций
    if (!vm.count("command")) {
        errmap[PARSING_ERR::NO_CMD] = "Необходимо задать значение для опции --command.";
    } else {
        std::string cmdName = vm["command"].as<std::string>();
        auto it_cmd = commandMapping_.find(cmdName);
        if (it_cmd == commandMapping_.end()) {
            errmap[PARSING_ERR::INVALID_CMD] = "Некорректное значение для опции <" + cmdName + ">. Доступные команды: encrypt, decrypt, checksum.";
        } else {
            command_ = it_cmd->second;
            if ((command_ == COMMAND_TYPE::ENCRYPT)  ||  (command_ == COMMAND_TYPE::DECRYPT)) {
//  2.1 Разбор опций для шифрования/расшифрования
//  2.1.1 Проверка наличия пароля
                if (auto it = vm.find("password"); it == vm.end()) {
                    errmap[PARSING_ERR::NO_PASSWORD] = "Для шифрования/расшифрования необходима опция --password(-p).";
                } else {
                    password_ = it->second.as<std::string>();
                }
//  2.1.2 Проверка имени выходного файла
                if (auto it = vm.find("output"); it == vm.end()) {
                    errmap[PARSING_ERR::NO_OUTPUT] = "Для шифрования/расшифрования необходима опция --output(-o).";
                } else {
                    outputFile_ = it->second.as<std::string>();
                }
            }
        }
    }
//  2.2 Проверка наличия имени входного файла
    if (auto it = vm.find("input"); it == vm.end()) {
        errmap[PARSING_ERR::NO_INPUT] = "Для работы программы необхоимо передать имя входного файла с опцией --input.";
    } else {
        inputFile_ = it->second.as<std::string>();
    }

// Выовод сообщений об ошибках в поток std::cerr
    for (const auto& err_ : errmap)
        std::cerr << "Ошибка ввода параметров программы: " << err_.second << std::endl;

    if (errmap.empty()) {
        return PARSING_ERR::NO_ERROR;
    } else {
        return errmap.begin()->first;  
    }
}

void ProgramOptions::PrintHelp() {
    std::cout << desc_ << "\n";
}

}  // namespace CryptoGuard
