#!/bin/bash

for i in {1..1000}
do
    echo "$i Sending UDP"
    python3 udp-send.py 
done

