#!/bin/bash

# Run make run command ten times and append output to monitor.txt
for i in {1..10}; do
    echo "Run $i" >> monitor.txt
    make run >> monitor.txt 2>&1
    echo "----------------------------------------" >> monitor.txt
done

echo "Script completed. Check monitor.txt for details."
