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
import numpy as np
import cv2 as cv
import matplotlib

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
    print_subsample = 10

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
            print('frame time:          ', logentry.framecounter)
            # print('mode:                ', logentry.mode)
            print('reference location:  ', logentry.reference_location)
            print('measurement location:', logentry.measurement_location)
            print('estimated location:  ', logentry.navigation_observer_state[2:4])
            # print('nav ctrl output qr1: ', logentry.reference_orientation[1])
            # print('nav ctrl output qr2: ', logentry.reference_orientation[2])
            # print('observer state:      ', logentry.navigation_observer_state)
            # print('alt control signal:  ', logentry.altitude_control_signal)
            # print('sonar measurement:   ', logentry.measurement_height)
            # print('alt observer state:  ', logentry.altitude_observer_state)
            # print('hov thrust:          ', logentry.hover_thrust)
            # print('common thrust:           ', logentry.common_thrust)
            # print('rc thrust:           ', logentry.rc_throttle)
            # print('reference orientation: ', logentry.reference_orientation)
            # print('measurement orientation: ', logentry.measurement_orientation)
            # print('position control signal  ', logentry.position_control_signal)
            # print('measurement ang vel:     ', logentry.measurement_angular_velocity)
            # print('motor controls:          ', logentry.motor_control_signals)

            LoggingThreadedUDPRequestHandler.ctr = 0


GRIDSIZE = 48
METERS_2_BLOCK = 1 / 0.3
offset = 200


class LoggingThreadedUDPServer(socketserver.ThreadingMixIn, socketserver.UDPServer):
    allow_reuse_address = True

    def __init__(self, filename: str, *args):
        super().__init__(*args)
        self.filename = filename

        self.image = np.zeros((640, 480, 3), np.uint8)

    def __enter__(self):
        print("Opening file: " + self.filename)
        self.file = open(self.filename, mode='wb')
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        print("Closing file: " + self.filename)
        self.file.close()

    def log(self, logentry: LogEntry):
        self.file.write(bytes(logentry))

        try:

            frame_width = np.size(self.image, 1)
            frame_height = np.size(self.image, 0)

            loc = (int(round(float(logentry.measurement_location[0]) * GRIDSIZE * METERS_2_BLOCK + frame_width/2)),
                   int(round(float(logentry.measurement_location[1]) * GRIDSIZE * METERS_2_BLOCK + frame_height/2)))
            cv.circle(self.image, loc, 3, (255, 255, 255), -1)

            dispimage = self.image

            self.image = self.image * 0.95

            color = (128, 255, 255)
            for y in range(frame_height//GRIDSIZE + 1):
                y *= GRIDSIZE
                y += (frame_height//2) % GRIDSIZE
                cv.line(dispimage, (0, y), (frame_width, y), color, 1)
            for x in range(frame_width//GRIDSIZE + 1):
                x *= GRIDSIZE
                x += (frame_width//2) % GRIDSIZE
                cv.line(dispimage, (x, 0), (x, frame_height), color, 1)
            cv.imshow("image", dispimage)
            cv.waitKey(1)

        except:
            pass


if __name__ == "__main__":
    ts = datetime.datetime.now().strftime('%Y-%m-%d.%H.%M.%S')
    # fname = "/home/pieter/GitHub/EAGLE/DroneLogs/eagle-" + ts + ".dat"
    fname = "/tmp/eagle-" + ts + ".dat"
    with LoggingThreadedUDPServer(fname, (MCAST_GRP, MCAST_PORT), LoggingThreadedUDPRequestHandler) as server:
        server_thread = threading.Thread(target=server.serve_forever)
        server_thread.daemon = True
        server.socket.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP,
                                 socket.inet_aton(MCAST_GRP) + socket.inet_aton(str(socket.INADDR_ANY)))
        server_thread.start()
        print("Server started")
        input("Press Enter to stop ...")
