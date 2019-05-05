#!/usr/bin/env python3

import socketserver
import socket
import struct
import threading
import time
import datetime
import sys
from DroneLogger import LogEntry
import heapq

MCAST_GRP = '239.0.0.2'
MCAST_PORT = 5003

if len(sys.argv) > 1:
    MCAST_PORT = int(sys.argv[1])

BUFFER_SIZE = 4


class ComparableLogEntry(LogEntry):
    def __lt__(self, other: LogEntry):
        return self.frametime < other.frametime


class LoggingThreadedUDPRequestHandler(socketserver.BaseRequestHandler):
    queue = []
    ctr = 0
    print_subsample = 5

    def handle(self):
        data = self.request[0]
        new_entry = ComparableLogEntry(bytes(data))
        if len(LoggingThreadedUDPRequestHandler.queue) >= BUFFER_SIZE:
            logentry = heapq.heappushpop(
                LoggingThreadedUDPRequestHandler.queue, new_entry)
            self.handle_entry(logentry)
        else:
            heapq.heappush(LoggingThreadedUDPRequestHandler.queue, new_entry)

    def handle_entry(self, logentry: LogEntry):
        self.server.log(logentry)

        LoggingThreadedUDPRequestHandler.ctr += 1
        if LoggingThreadedUDPRequestHandler.ctr == self.print_subsample:
            print('frame time:          ', logentry.frametime)
            print('reference location:  ', logentry.reference_location)
            print('measurement location:', logentry.measurement_location)
            print('nav ctrl output qr1: ', logentry.reference_orientation[1])
            print('nav ctrl output qr2: ', logentry.reference_orientation[2])
            # print('observer state:      ', logentry.navigation_observer_state)
            # print('control signal:      ', logentry.motor_control_signals)
            print('sonar measurement:   ', logentry.measurement_height)
            # print('alt observer state:  ', logentry.altitude_observer_state)
            # print('hov thrust:          ', logentry.hover_thrust)
            # print('rc thrust:           ', logentry.rc_throttle)
            print('measurement orientation: ', logentry.measurement_orientation)
            print()
            
            LoggingThreadedUDPRequestHandler.ctr = 0


class LoggingThreadedUDPServer(socketserver.ThreadingMixIn, socketserver.UDPServer):
    allow_reuse_address = True

    def __init__(self, filename: str, *args):
        super().__init__(*args)
        self.filename = filename

    def __enter__(self):
        print("Opening file: " + self.filename)
        self.file = open(self.filename, mode='wb')
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        print("Closing file: " + self.filename)
        self.file.close()

    def log(self, logentry: LogEntry):
        self.file.write(bytes(logentry))


if __name__ == "__main__":
    ts = datetime.datetime.now().strftime('%Y-%m-%d.%H.%M.%S')
    # fname = "/home/pieter/GitHub/EAGLE/DroneLogs/eagle-" + ts + ".dat"
    fname = "/media/btamm12/GeneralHDD/DroneLogs/eagle-" + ts + ".dat"
    with LoggingThreadedUDPServer(fname, (MCAST_GRP, MCAST_PORT), LoggingThreadedUDPRequestHandler) as server:
        server_thread = threading.Thread(target=server.serve_forever)
        server_thread.daemon = True
        server.socket.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP,
                                 socket.inet_aton(MCAST_GRP) + socket.inet_aton(str(socket.INADDR_ANY)))
        server_thread.start()
        print("Server started")
        input("Press Enter to stop ...")
