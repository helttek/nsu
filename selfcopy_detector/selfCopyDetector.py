import socket
import sys
import ipaddress
import struct
from time import sleep


class SelfCopyDetector:
    PORT = 50000

    def __init__(self, address):
        self.ipaddr = address

    def Start(self):
        self.__CheckIPAddress()

    def __CheckIPAddress(self):
        try:
            ip = ipaddress.ip_address(self.ipaddr)
        except ValueError:
            print("invalid IP address: ", self.ipaddr)
            sys.exit(0)
        if (ip.version == 4):
            self.__CreateSocketIPv4()
        else:
            self.__CreateSocketIPv6()

    def __CreateSocketIPv4(self):
        print("ipv4")
        try:
            self.sock = socket.socket(
                socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
            ttl = struct.pack('b', 1)
            self.sock.setsockopt(
                socket.IPPROTO_IPV4, socket.IP_MULTICAST_TTL, ttl)
            self.sock.bind((self.ipaddr, self.PORT))
        except OSError as e:
            print("couldn't create socket: ", e)
            sys.exit(0)

    def __CreateSocketIPv6(self):
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
