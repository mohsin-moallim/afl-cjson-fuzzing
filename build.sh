#!/bin/sh
# Build the cJSON fuzz harness: AFL++ instrumentation + AddressSanitizer.
# Assumes cJSON is cloned into ./cJSON :
#   git clone https://github.com/DaveGamble/cJSON.git
AFL_USE_ASAN=1 afl-clang-fast -g -O1 -I cJSON cJSON/cJSON.c fuzz_cjson.c -o fuzz_cjson
echo "built ./fuzz_cjson"
