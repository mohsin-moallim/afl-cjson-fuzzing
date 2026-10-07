#!/bin/sh
# -m none: AddressSanitizer needs lots of virtual memory.  @@ = each test input file.
afl-fuzz -i seeds -o out -m none -- ./fuzz_cjson @@
