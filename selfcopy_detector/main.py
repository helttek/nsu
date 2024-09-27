from selfCopyDetector import SelfCopyDetector
import sys
import socket

if (len(sys.argv) == 1):
    print("No args provided")
    sys.exit(0)

scd = SelfCopyDetector(sys.argv[1])
scd.Start()
