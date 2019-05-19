from DroneLogger import LogEntry, MotorSignals
from time import sleep, time
import socket
from PyQuaternion import *

MCAST_GRP = '239.0.0.2'
MCAST_PORT = 5003

MULTICAST_TTL = 3

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
sock.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_TTL, MULTICAST_TTL)
sock.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_LOOP, 1)

T_s = 5.0 / 238.0
ctr = 0

try:
    print('Running ...')
    while True:
        log_entry = LogEntry()
        log_entry.millis = int(round(time() * 1000))
        log_entry.motorSignals = MotorSignals(0.5, 0.5, 0.5, 0.5)
        log_entry.attitudeStateEstimate.q = EulerAngles.eul2quat(EulerAngles(0, 0.1, 0.2))
        sock.sendto(bytes(log_entry), (MCAST_GRP, MCAST_PORT))
        ctr += 1
        sleep(T_s)
except KeyboardInterrupt:
    print('\rStopped')