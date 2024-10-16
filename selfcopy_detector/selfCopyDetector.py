import socket
import sys
import ipaddress
import struct

class SelfCopyDetector:
    DEFAULT_PORT = 50000
    DEFAULT_IP_ADDRESS = '224.0.0.1'

    def __init__(self, argv):
        if len(argv) == 1:
            self.ipaddress = self.DEFAULT_IP_ADDRESS
            self.port = self.DEFAULT_PORT
        if len(argv) == 2:
            self.ipaddr = argv[1]
            self.port = self.DEFAULT_PORT
        if len(argv) == 3:
            self.ipaddr = argv[1]
            self.port = argv[2]
        if not self.__createSocket():
            raise Exception("Failed to create socket")

    def start(self):


    def __createSocket(self):
        try:
            ip = ipaddress.ip_address(self.ipaddr)
        except ValueError:
            print("invalid IP address: ", self.ipaddr)
            return False
        if ip.version == 4:
            self.__createSocketIPv4()
            return True
        else:
            self.__createSocketIPv6()
            return True

    def __createSocketIPv4(self):
        print("ipv4")
        try:
            self.sock = socket.socket(
                socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
            ttl = struct.pack('b', 1)
            self.sock.setsockopt(
                socket.IPPROTO_IPV4, socket.IP_MULTICAST_TTL, ttl)
            self.sock.bind((self.ipaddr, self.port))
        except OSError as e:
            print("couldn't create socket: ", e)
            sys.exit(0)

    def __createSocketIPv6(self):
        print("ipv6")
        try:
            self.sock = socket.socket(
                socket.AF_INET6, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
            ttl = struct.pack('i', 1)
            self.sock.setsockopt(
                socket.IPPROTO_IPV6, socket.IPV6_MULTICAST_HOPS, ttl)
            # self.sock.bind((self.ipaddr, self.PORT))
        except OSError as e:
            print("couldn't create socket: ", e)
            sys.exit(0)
