# Fuzzing cJSON (v1.4.7) with AFL++

My second fuzzing project. I wrote an AFL++ harness for **cJSON**, a widely-used
open-source C JSON library, and used it to trigger a **stack-overflow** bug in an
older release (v1.4.7).

## The bug — uncontrolled recursion (CWE-674)
cJSON's parser is recursive: parsing an array calls the value parser for each
element, and a nested array calls the array parser again. Before **v1.5.0** there was
no limit on how deep this could go, so deeply nested input like `[[[[[[...` makes the
parser recurse once per level until it exhausts the stack and crashes. cJSON fixed
this in v1.5.0 by adding a maximum nesting depth (`CJSON_NESTING_LIMIT`).

## Target
[cJSON](https://github.com/DaveGamble/cJSON) **v1.4.7** (Apr 2017).

## Harness
`fuzz_cjson.c` reads a file of bytes and passes them to `cJSON_Parse()`. Built with
AFL++ (`afl-clang-fast`) + AddressSanitizer.

## Build & reproduce
```
git clone https://github.com/DaveGamble/cJSON.git
cd cJSON && git checkout v1.4.7 && cd ..
./build.sh
python3 -c "print('['*100000)" > deep.txt   # deeply nested input
./fuzz_cjson deep.txt
```

## The crash
AddressSanitizer reports a stack overflow, and the backtrace shows the parser
recursing — `parse_array` → `parse_value` → `parse_array` → … once per nesting level:
```
==ERROR: AddressSanitizer: stack-overflow
    #0  in strncmp
    #1  in parse_value   cJSON/cJSON.c:1062
    #2  in parse_array   cJSON/cJSON.c:1232
    #3  in parse_value   cJSON/cJSON.c:1093
    #4  in parse_array   cJSON/cJSON.c:1232
    #5  in parse_value   cJSON/cJSON.c:1093
    ...  (this pair repeats thousands of times)
```

## A note on fuzzer behaviour
I also ran AFL++ as a coverage-guided campaign, but it did **not** save this crash on
its own — which is expected. Deeper nesting doesn't reach new code (it's the same
recursive edge repeated), so AFL gets no coverage reward for going deeper, and its
input-*trimming* stage shrinks the nesting back down. Deep-recursion bugs are a known
blind spot for mutation-based fuzzers. So the bug is demonstrated here with a
directly-constructed deeply-nested input rather than left to AFL's mutation engine.

## Root cause & fix
Unbounded recursion while parsing deeply nested JSON (CWE-674). Fixed upstream in
cJSON **v1.5.0** by adding a nesting-depth limit.

## What I learned
Fuzzers have blind spots — they can't easily find deep-recursion bugs, because going
deeper doesn't reach new code, so a coverage-guided fuzzer isn't pushed toward it.

## Files
- `fuzz_cjson.c` — the harness
- `build.sh`, `run.sh`, `reproduce.sh`
- `seeds/` — starting inputs (including a deeply-nested one)
- cJSON itself is cloned separately (see Build & reproduce) and is not committed here
