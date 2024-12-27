import time
from socket import *

MESSAGE = b"Hello from python script on Linux.\0"

pings = 1

#Send ping 10 times 
while pings < 11:  

    #Create a UDP socket
    clientSocket = socket(AF_INET, SOCK_DGRAM)

    #Set a timeout value of n seconds
    clientSocket.settimeout(5)

    addr = ("192.168.1.3", 11888)

    #Send ping
    start = time.time()
    clientSocket.sendto(MESSAGE, addr)

    #If data is received back from server, print 
    try:
        data, server = clientSocket.recvfrom(1024)
        end = time.time()
        elapsed = end - start
        #print (data + " " + pings + " " + elapsed)        
        print (data)

    #If data is not received back from server, print it has timed out  
    except timeout:
        print ('REQUEST TIMED OUT')

    pings = pings - 1
