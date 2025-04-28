# Credits: https://wiki.python.org/moin/UdpCommunication
# 34884 = 8844

import socket

# Send to the server on localhost.
#UDP_IP = "127.0.0.1"
# Send to Gramado on virtualbox.
#UDP_IP = "192.168.1.255"

UDP_IP = "192.168.1.6"
UDP_PORT = 11888

MESSAGE = b"Type a message:\n\0"

print("UDP target IP:   %s" % UDP_IP)
print("UDP target port: %s" % UDP_PORT)
print("message:         %s" % MESSAGE)

sock = socket.socket(socket.AF_INET,socket.SOCK_DGRAM)

while True:
    INPUT_MESSAGE = input(MESSAGE).encode()
    sock.sendto(INPUT_MESSAGE, (UDP_IP, UDP_PORT))
   
