from DroneLogger import LogEntry, MotorSignals
from time import sleep, time
import socket
from PyQuaternion import *

UCAST_ADDR = '127.0.0.1'
UCAST_PORT = 5001

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)

T_s = 5.0 / 238.0
ctr = 0

try:
    print('Running ...')
    while True:
        log_entry = LogEntry()
        log_entry.millis = int(round(time() * 1000))
        log_entry.motorSignals = MotorSignals(0.5, 0.5, 0.5, 0.5)
        log_entry.attitudeReference.q = EulerAngles.eul2quat(EulerAngles(0, 0.1, 0.2))
        sock.sendto(bytes(log_entry), (UCAST_ADDR, UCAST_PORT))
        ctr += 1
        sleep(T_s)
except KeyboardInterrupt:
    print('\rStopped')