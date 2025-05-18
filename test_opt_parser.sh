source ./build/generators/conan_env.sh
g++ ./src/cmd_options.cpp ./tests/test_opt_parser.cpp ./tests/main.cpp -lboost_program_options -lgtest -fsanitize=address -o test_opt_parser
# ./test_cmd_opt