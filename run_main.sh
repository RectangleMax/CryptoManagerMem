source build/generators/conan_env.sh
g++ ./src/cmd_options.cpp ./src/crypto_guard_ctx.cpp ./src/mainCryptoGuard.cpp -lssl -lcrypto -lz -lboost_program_options -o CryptoGuard

# ./CryptoGuard -i "prestuplenie-i-nakazanie.txt"      -o "temp_encrypted" -p "VeryReliablePassword" --command encrypt
# ./CryptoGuard -o "prestuplenie-i-nakazanie-copy.txt" -i "temp_encrypted" -p "VeryReliablePassword" --command decrypt
# diff prestuplenie-i-nakazanie.txt prestuplenie-i-nakazanie-copy.txt