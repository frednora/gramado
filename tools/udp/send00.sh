#!/bin/bash

# Sending ...
	echo -n "Hello" | tr -d '\n' | nc -u -w1 192.168.1.10 11888

