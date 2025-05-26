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

bool ProgramOptions::Parse(int argc, char *argv[]) { 
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc_), vm);
    po::notify(vm);

//  Разбор входных аргументов
//  1. Проверка сценария с вызовом help
    if (vm.count("help")  &&  (vm.size() == 1)) {
        PrintHelp();
        return true;
    }

//  2. Разбор других опций
    if (!vm.count("command")) {
        throw std::runtime_error("Необходимо задать значение для опции --command.");
    }

    std::string cmdName = vm["command"].as<std::string>();
    auto it_cmd = commandMapping_.find(cmdName);
    if (it_cmd == commandMapping_.end()) {
        throw std::runtime_error("Некорректное значение для опции <command>. Доступные команды: encrypt, decrypt, checksum.");
    }

    command_ = it_cmd->second;
    if ((command_ == COMMAND_TYPE::ENCRYPT)  ||  (command_ == COMMAND_TYPE::DECRYPT)) {
//  2.1 Разбор опций для шифрования/расшифрования
//  2.1.1 Проверка наличия пароля
        auto it_vm = vm.find("password");
        if (it_vm == vm.end()) {
            throw std::runtime_error("Для шифрования/расшифрования необходима опция --password(-p).");
        } 
        password_ = it_vm->second.as<std::string>();
//  2.1.2 Проверка имени выходного файла
        it_vm = vm.find("output");
        if (it_vm == vm.end()) {
            throw std::runtime_error("Для шифрования/расшифрования необходима опция --output(-o).");
        }
        outputFile_ = it_vm->second.as<std::string>();
    }
        
//  2.2 Проверка наличия имени входного файла
    auto it_vm = vm.find("input");
    if (it_vm == vm.end()) {
        throw std::runtime_error("Для работы программы необхоимо передать имя входного файла с опцией --input.");
    }
    inputFile_ = it_vm->second.as<std::string>();

    return true;
}

void ProgramOptions::PrintHelp() {
    std::cout << desc_ << std::endl;
}

}  // namespace CryptoGuard
