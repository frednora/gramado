# Send UDP message to a given target.
# Credits: https://wiki.python.org/moin/UdpCommunication

import socket

# IP:PORT
TARGET_IP = "192.168.1.10"
TARGET_PORT = 11888
MESSAGE = b"Hello from Python script on Linux.\0"

# Verbose
print("Target socket: %s:%s" % (TARGET_IP,TARGET_PORT))
print("message: %s" % MESSAGE)

# Socket for UDP messages.
sock = socket.socket( socket.AF_INET, socket.SOCK_DGRAM )
# Send
sock.sendto(MESSAGE, (TARGET_IP, TARGET_PORT))

print("Done")

