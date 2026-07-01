#!/usr/bin/env python3

import socket

# Broadcast query
MESSAGE = b"g:ip\0"

PORT = 11888
BROADCAST = "255.255.255.255"

# Create UDP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# Allow broadcast
sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)

# Wait up to 5 seconds
sock.settimeout(5)

print("Searching for Gramado...")

try:
    # Send broadcast
    sock.sendto(MESSAGE, (BROADCAST, PORT))

    # Wait for reply
    data, addr = sock.recvfrom(1024)

    print("Response received!")
    print("Host IP :", addr[0])
    print("Host Port:", addr[1])

    try:
        print("Message :", data.decode("ascii", errors="ignore"))
    except:
        print("Raw Data:", data)

except socket.timeout:
    print("No Gramado kernel found.")

finally:
    sock.close()

    