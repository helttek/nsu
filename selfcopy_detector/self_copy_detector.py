import socket
import sys
import ipaddress
import struct

class self_copy_detector:
    DEFAULT_PORT = 50000
    DEFAULT_IP_ADDRESS = '224.0.0.1'

    def __init__(self, argv):
        if len(argv) == 1:
            self.ipaddress = self.DEFAULT_IP_ADDRESS
            self.port = self.DEFAULT_PORT
        if len(argv) == 2:
            self.ipaddress = argv[1]
            self.port = self.DEFAULT_PORT
        if len(argv) == 3:
            self.ipaddress = argv[1]
            self.port = argv[2]
        self.__create_socket()

    def start(self):
        while True:
            print("Waiting for connection...")

    def __create_socket(self):
        try:
            ip = ipaddress.ip_address(self.ipaddress)
        except ValueError:
            print("invalid IP address: ", self.ipaddress)
            return False
        if ip.version == 4:
            self.__create_socketIPv4()
        else:
            self.__create_socketIPv6()

    def __create_socketIPv4(self):
        print("ipv4")
        try:
            self.sock = socket.socket(
                socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
            self.sock.setsockopt(
                socket.IPPROTO_IPV4, socket.SO_REUSEADDR, 1)
            # self.sock.bind((self.ipaddress, self.port))
        except OSError as e:
            raise RuntimeError("__create_socketIPv4 failed in self_copy_detector.py: ", e)

    def __create_socketIPv6(self):
        print("ipv6")
        try:
            self.sock = socket.socket(
                socket.AF_INET6, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
            ttl = struct.pack('i', 1)
            self.sock.setsockopt(
                socket.IPPROTO_IPV6, socket.IPV6_MULTICAST_HOPS, ttl)
            # self.sock.bind((self.ipaddress, self.PORT))
        except OSError as e:
            raise RuntimeError("__create_socketIPv6 failed in self_copy_detector.py")
