source ./build/generators/conan_env.sh
g++ ./src/crypto_guard_ctx.cpp ./tests/test_checksum.cpp ./tests/main.cpp -lssl -lcrypto -lz -lgtest -o test_checksum