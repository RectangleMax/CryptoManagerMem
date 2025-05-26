source ./build/generators/conan_env.sh
g++ ./src/crypto_guard_ctx.cpp ./tests/test_en_de_crypt.cpp ./tests/main.cpp -lssl -lcrypto -lz -lgtest -fsanitize=address -o test_en_de_crypt