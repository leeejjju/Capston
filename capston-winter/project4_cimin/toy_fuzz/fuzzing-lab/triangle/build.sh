set -x
clang -g -O0 -fsanitize=fuzzer,signed-integer-overflow -fno-sanitize-recover=signed-integer-overflow -o triangle triangle.c fuzz_target.c
