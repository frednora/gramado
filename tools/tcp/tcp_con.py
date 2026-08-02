#!/usr/bin/env python3

from socket import *

TARGET = "192.168.1.4"
PORT = 11888

sock = socket(AF_INET, SOCK_STREAM)
sock.settimeout(5)

try:
    sock.connect((TARGET, PORT))
    print(f"Connected to {TARGET}:{PORT}")
except Exception as e:
    print("Connection failed:", e)
    exit(1)

while True:
    cmd = input("tcp console> ")

    if cmd == "quit":
        break

    try:
        # Send command (with null terminator like your UDP version)
        sock.sendall(cmd.encode() + b'\0')

        # Receive response
        data = sock.recv(1024)
        if not data:
            print("Connection closed by server")
            break

        clean = data.split(b'\0', 1)[0].decode(errors="ignore")
        print("Server:", clean)

    except timeout:
        print("No response")
    except Exception as e:
        print("Error:", e)
        break

sock.close()

