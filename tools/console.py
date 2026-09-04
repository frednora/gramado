#!/usr/bin/env python3

from socket import *

TARGET = "192.168.1.5"
PORT = 11888

sock = socket(AF_INET, SOCK_DGRAM)
sock.settimeout(5)

while True:
    cmd = input("gprot> ")

    if cmd == "quit":
        break

    sock.sendto(cmd.encode() + b'\0', (TARGET, PORT))

    try:
        data, addr = sock.recvfrom(1024)
        # print(addr, data.decode(errors="ignore"))
        # print (data)
        clean = data.split(b'\0', 1)[0].decode(errors="ignore")
        print(addr, clean)
    except timeout:
        print("No response")
