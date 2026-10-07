#!/bin/sh
# Run the harness on one crashing input AFL saved, to see the ASan report.
# usage: ./reproduce.sh out/default/crashes/id:000000,...
./fuzz_cjson "$1"
