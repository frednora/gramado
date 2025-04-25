# Credits: https://wiki.python.org/moin/UdpCommunication
# 34884 = 8844

import socket

UDP_IP = "127.0.0.1"
UDP_PORT = 11999

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((UDP_IP, UDP_PORT))

while True:
    MyBuffer, addr = sock.recvfrom(1024)  # buffer size is 1024 bytes
    print("received message: %s" % MyBuffer )


