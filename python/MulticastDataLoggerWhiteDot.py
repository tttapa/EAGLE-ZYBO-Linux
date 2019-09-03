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
from PyQuaternion import EulerAngles

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
        return "(%.2f, %.2f)" % (self.x, self.y)


class Vec3:
    def __init__(self, vec: np.array):
        self.x = vec[0][0]
        self.y = vec[1][0]
        self.z = vec[2][0]

    def __str__(self):
        return "(%.2f, %.2f, %.2f)" % (self.x, self.y, self.z)


class LoggingThreadedUDPRequestHandler(socketserver.BaseRequestHandler):
    queue = []
    ctr = 0
    print_subsample = 12

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
            print('frame time:           ', logentry.millis / 1000)
            # print('tick count:          ', logentry.tickCount)
            # print('wpt mode:            ', logentry.wptMode)
            # print('flight mode:         ', logentry.flightMode)
            # print('throttle:            ', logentry.rcInput.throttle)
            print('reference location:   ', Pos(logentry.positionReference.p))
            print('estimated location:   ', Pos(
                logentry.positionStateEstimate.p))
            print('pos control signal:   ', Pos(
                logentry.positionControlSignal.q12))
            # print('attitude reference:   ', repr(logentry.attitudeReference.q))
            # print('attitude reference:   ', logentry.attitudeReference.q)
            # print('bias pitch:           ', logentry.pitchBias)
            # print('rc pitch:             ', logentry.rcInput.pitch)
            # print('bias roll:            ', logentry.rollBias)
            # print('rc roll:              ', logentry.rcInput.roll)
            # print('attitude reference:   ', logentry.attitudeReference.q)
            # print()
            print('altitude state:       ', logentry.altitudeStateEstimate.z)
            print()
            # print('nav ctrl output:     ', Pos(logentry.positionControlSignal.q12))
            # print('attitude state:       ', repr(
            #     logentry.attitudeStateEstimate.q))
            # print('AHRS measurement:     ', repr(
            #     logentry.attitudeMeasurement.q))
            # print('AHRS measurement:     ', logentry.attitudeMeasurement.q)
            # print('Accel measurement:     ', Vec3(logentry.imuMeasurement.accel.a))
            # print('Gyro measurement:     ', Vec3(logentry.imuMeasurement.gyro.g))

            LoggingThreadedUDPRequestHandler.ctr = 0


GRIDSIZE = 48
METERS_2_BLOCK = 1 / 0.3
offset = 200


class LoggingThreadedUDPServer(socketserver.ThreadingMixIn, socketserver.UDPServer):
    allow_reuse_address = True

    def __init__(self, filename: str, *args):
        super().__init__(*args)
        self.filename = filename

        self.image = np.zeros((480, 480, 3), np.uint8)

    def __enter__(self):
        print("Opening file: " + self.filename)
        self.file = open(self.filename+".dat", mode='wb')
        # fps = 30
        # self.video_out = cv.VideoWriter(self.filename+'.avi', cv.VideoWriter_fourcc(
        #     'M', 'J', 'P', 'G'), fps, (480, 480 + 360))
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        print("Closing file: " + self.filename)
        self.file.close()
        # self.video_out.release()

    def log(self, logentry: LogEntry):
        self.file.write(bytes(logentry))

        try:
            # raise RuntimeError()

            frame_width = np.size(self.image, 1)
            frame_height = np.size(self.image, 0)

            loc = (int(round(float(logentry.positionMeasurement.p[0]) * GRIDSIZE * METERS_2_BLOCK + frame_width/2)),
                   int(round(float(-logentry.positionMeasurement.p[1]) * GRIDSIZE * METERS_2_BLOCK + frame_height/2)))
            cv.circle(self.image, loc, 3, (255, 255, 255), -1)

            dispimage = self.image

            self.image -= self.image // 32

            color = (128, 255, 255)
            for y in range(frame_height//GRIDSIZE + 1):
                y *= GRIDSIZE
                y += (frame_height//2) % GRIDSIZE
                cv.line(dispimage, (0, y), (frame_width, y), color, 1)
            for x in range(frame_width//GRIDSIZE + 1):
                x *= GRIDSIZE
                x += (frame_width//2) % GRIDSIZE
                cv.line(dispimage, (x, 0), (x, frame_height), color, 1)

            max_ctrl = 0.0262
            x_ctrl = int(
                round(logentry.positionControlSignal.q12[1][0]*150/max_ctrl))+240
            y_ctrl = int(
                round(logentry.positionControlSignal.q12[0][0]*150/max_ctrl))+180

            ctrl_image = np.zeros((360, 480, 3), np.uint8)
            cv.rectangle(ctrl_image, (60+30, 30),
                         (480-(60+30), 360-30), (255, 255, 255))
            cv.line(ctrl_image, (x_ctrl, 30),
                    (x_ctrl, 360-30), (0, 255, 0), thickness=2)
            cv.line(ctrl_image, (60+30, y_ctrl),
                    (480-(60+30), y_ctrl), (255, 0, 0), thickness=2)
            cv.line(ctrl_image, (240, 0), (240, 360), (1, 1, 1), thickness=1)
            cv.line(ctrl_image, (0, 180), (480, 180), (1, 1, 1), thickness=1)
            cv.circle(ctrl_image, (x_ctrl, y_ctrl),
                      9, (0, 0, 255), thickness=-1)

            dispimage_with_ctrl = np.concatenate((dispimage, ctrl_image))

            # self.video_out.write(dispimage_with_ctrl)

            cv.imshow("image", dispimage_with_ctrl)
            cv.waitKey(1)

        except Exception as e:
            print(e)


if __name__ == "__main__":
    ts = datetime.datetime.now().strftime('%Y-%m-%d.%H.%M.%S')
    # fname = "/home/pieter/GitHub/EAGLE/DroneLogs/eagle-" + ts + ".dat"
    fname = "/tmp/eagle-" + ts
    with LoggingThreadedUDPServer(fname, (MCAST_GRP, MCAST_PORT), LoggingThreadedUDPRequestHandler) as server:
        server_thread = threading.Thread(target=server.serve_forever)
        server_thread.daemon = True
        server.socket.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP,
                                 socket.inet_aton(MCAST_GRP) + socket.inet_aton(str(socket.INADDR_ANY)))
        server_thread.start()
        print("Server started")
        input("Press Enter to stop ...")

