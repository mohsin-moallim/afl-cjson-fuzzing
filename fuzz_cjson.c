/*
 * fuzz_cjson.c -- an AFL++ harness for the cJSON library
 * (https://github.com/DaveGamble/cJSON).
 *
 * A "harness" is the small program that feeds a fuzzer's input into the
 * library you want to test. This one reads a file of bytes and hands them to
 * cJSON_Parse(), then frees whatever it gets back. AFL++ mutates the input
 * trying to make cJSON crash.
 *
 * cJSON is mature and heavily fuzzed, so a crash is unlikely -- a clean run is
 * itself a valid result (it shows the library is robust). If it DOES crash,
 * AddressSanitizer points to the exact line inside cJSON.
 */
#include <stdio.h>
#include <stdlib.h>
#include "cJSON.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "usage: %s <input-file>\n", argv[0]);
        return 1;
    }

    FILE *f = fopen(argv[1], "rb");
    if (!f) { perror("fopen"); return 1; }

    /* read the whole file into a NUL-terminated buffer */
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (size < 0) { fclose(f); return 1; }

    char *buf = (char *)malloc((size_t)size + 1);
    if (!buf) { fclose(f); return 1; }

    size_t got = fread(buf, 1, (size_t)size, f);
    buf[got] = '\0';
    fclose(f);

    /* the one call we are fuzzing */
    cJSON *json = cJSON_Parse(buf);
    if (json) cJSON_Delete(json);

    free(buf);
    return 0;
}
