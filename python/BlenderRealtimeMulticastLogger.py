import bpy
import os
import sys
import struct
import socketserver, socket, threading, time
from mathutils import Quaternion, Euler
import heapq
import math

print(sys.version_info)

from DroneLogger import LogEntry

MCAST_GRP = '239.0.0.2'
MCAST_PORT = 5003

SQUARE_SIZE = 0.30

drone = bpy.data.objects["drone"]
motor1 = bpy.data.objects["motor1"]
motor2 = bpy.data.objects["motor2"]
motor3 = bpy.data.objects["motor3"]
motor4 = bpy.data.objects["motor4"]

BUFFER_SIZE = 5


class ComparableLogEntry(LogEntry):
    def __lt__(self, other: LogEntry):
        return self.millis < other.millis

class ThreadedUDPRequestHandler(socketserver.BaseRequestHandler):
    queue = []
    ctr = 0
    print_subsample = 2

    def handle(self):
        data = self.request[0]
        logentry = ComparableLogEntry(bytes(data))
        if len(ThreadedUDPRequestHandler.queue) >= BUFFER_SIZE:
            logentry = heapq.heappushpop(
                ThreadedUDPRequestHandler.queue, logentry)
            self.handle_entry(logentry)
        else:
            heapq.heappush(ThreadedUDPRequestHandler.queue, logentry)

    def handle_entry(self, logentry: LogEntry):
        ThreadedUDPRequestHandler.ctr += 1
        if ThreadedUDPRequestHandler.ctr == self.print_subsample:
            print('time:                  ', logentry.millis)
            # print('reference location:  ', logentry.reference_location)
            # print('measurement location:', logentry.measurement_location)
            # print('nav ctrl output qr1: ', logentry.reference_orientation[1])
            # print('nav ctrl output qr2: ', logentry.reference_orientation[2])
            # print('observer state:      ', logentry.navigation_observer_state)
            # print('control signal:      ', logentry.motor_control_signals)
            # print('sonar measurement:   ', logentry.measurement_height)
            # print('alt observer state:  ', logentry.altitude_observer_state)
            # print('hov thrust:          ', logentry.hover_thrust)
            # print('rc thrust:           ', logentry.rc_throttle)
            # print('measurement orientation: ', logentry.measurement_orientation)
            print()

            q = Quaternion(logentry.attitudeStateEstimate.q.asColVector())
            # eul = q.to_euler()
            # eul.z = -eul.z
            # eul.z += logentry.attitude_yaw_offset
            # drone.rotation_quaternion = eul.to_quaternion()
            drone.rotation_quaternion = q
            fac = 0.2
            motorthrust = logentry.motorSignals
            motor1.dimensions[2] = motorthrust.v0 * fac
            motor2.dimensions[2] = motorthrust.v1 * fac
            motor3.dimensions[2] = motorthrust.v2 * fac
            motor4.dimensions[2] = motorthrust.v3 * fac
            x = logentry.sensorPositionMeasurement[0]
            y = logentry.sensorPositionMeasurement[1]
            if not math.isnan(x) and not math.isnan(y):
                drone.location[0] = x
                drone.location[1] = y
            
            ThreadedUDPRequestHandler.ctr = 0


class ThreadedUDPServer(socketserver.ThreadingMixIn, socketserver.UDPServer):
    allow_reuse_address = True

if __name__ == "__main__":
    server = ThreadedUDPServer((MCAST_GRP, MCAST_PORT), ThreadedUDPRequestHandler)
    server_thread = threading.Thread(target=server.serve_forever)
    server_thread.daemon = True
    server.socket.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP,
                             socket.inet_aton(MCAST_GRP) + 
                             socket.inet_aton(str(socket.INADDR_ANY)))
    server_thread.start()
    print("Server started")
