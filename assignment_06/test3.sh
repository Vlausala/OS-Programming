#!/bin/bash

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
NC='\033[0m'

gcc task3.c -o task3

rm -r task3-run-output
mkdir -p task3-run-output

passed_tests=0
all_tests=0

for input_file in tests3/*
do
    all_tests=$((all_tests + 1))
    filename=$(basename "$input_file")
    index=$(echo "$filename" | sed 's/[^0-9]//g')
    output_file="task3-run-output/output$index.txt"
    ./task3 < $input_file > $output_file

    diff_output=$(diff $output_file sample-outputs3/output$index.txt)

    if [ -n "$diff_output" ]
    then
        echo -e "${RED}Test $index failed. Differences:${NC}"
        echo "$diff_output"
    else
        echo -e "${GREEN}Test $index passed successfully.${NC}"
        passed_tests=$((passed_tests + 1))
    fi
done

echo -e "${YELLOW}Number of passed tests: $passed_tests${NC}"
