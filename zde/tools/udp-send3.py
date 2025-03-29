# Credits: https://wiki.python.org/moin/UdpCommunication
# 34884 = 8844

import socket

# Send to the server on localhost.
#UDP_IP = "127.0.0.1"
# Send to Gramado on virtualbox.
#UDP_IP = "192.168.1.255"

UDP_IP = "192.168.1.4"
UDP_PORT = 11888
# Using the g protocol for udp.
MESSAGE = b"g:3\0"

print("UDP target IP:   %s" % UDP_IP)
print("UDP target port: %s" % UDP_PORT)
print("message:         %s" % MESSAGE)

sock = socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
sock.sendto(MESSAGE, (UDP_IP, UDP_PORT))

