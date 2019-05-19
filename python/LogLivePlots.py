#!/usr/bin/env python3

from DroneLogger import *

from LivePlotter import LivePlotter

import socketserver
import socket
import struct
import threading
import time
import datetime
import sys
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
        return self.millis < other.millis


class Pos:
    def __init__(self, vec: np.array):
        self.x = vec[0][0]
        self.y = vec[1][0]

    def __str__(self):
        return "({x}, {y})".format(x=self.x, y=self.y)



print_subsample = 12

serversocket = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)
serversocket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
serversocket.bind((MCAST_GRP, MCAST_PORT))
serversocket.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP,
                        socket.inet_aton(MCAST_GRP)
                        + socket.inet_aton(str(socket.INADDR_ANY)))

queue = []
ctr = 0
plotter = LivePlotter(64)

def handle_entry(logentry: LogEntry):
    global ctr
    ctr += 1
    if ctr == print_subsample:
        print('frame time:    ', logentry.millis / 1000)
        plotter.update(logentry)
        ctr = 0

while True:
    data, addr = serversocket.recvfrom(len(bytes(LogEntry())) + 1)
    new_entry = ComparableLogEntry(bytes(data))

    if len(queue) >= BUFFER_SIZE:
        logentry = heapq.heappushpop(queue, new_entry)
        handle_entry(logentry)
    else:
        heapq.heappush(queue, new_entry)

"""
BUFFER_LEN = 32

p = LivePlotter(BUFFER_LEN)
l = LogEntry()
l.positionReference.p = (1, 0)
fps = 15
for t in range(0, 360 * fps):
    l.millis = int(round(1000 * t / fps))
    l.positionReference.p -= ((0.01,),
                              (0,))
    l.sensorPositionMeasurement += ((0.01,),
                                    (0,))
    p.update(l)
    time.sleep(1.0 / fps)
"""
