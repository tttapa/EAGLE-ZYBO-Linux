#!/usr/bin/env python3

import socketserver
import socket
import struct
import threading
import time
import datetime
import sys
import SummerLogger

MCAST_GRP = '239.0.0.5'
MCAST_PORT = 5000

if len(sys.argv) > 1:
    MCAST_PORT = int(sys.argv[1])


class LoggingThreadedUDPRequestHandler(socketserver.BaseRequestHandler):
    queue = []
    ctr = 0
    print_subsample = 1

    def handle(self):
        data = self.request[0]
        new_entry = SummerLogger.parse(bytes(data))
        self.handle_entry(new_entry)

    def handle_entry(self, logentry):
        self.server.log(logentry)

        LoggingThreadedUDPRequestHandler.ctr += 1
        if LoggingThreadedUDPRequestHandler.ctr == self.print_subsample:
            print('altitude measurement:  ', logentry["altitude-measurement"])

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

    def log(self, logentry):
        # self.file.write(bytes(logentry))
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
