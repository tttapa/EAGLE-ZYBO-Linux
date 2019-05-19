from DroneLogger import *
import os

logfile = "eagle-2019-05-18.18.54.53.dat"
with open(os.path.join(getenv("HOME"), "dronelogs", logfile), 'rb') as f:
    logentries = f.read()
    if len(logentries) % len(bytes(LogEntry())) != 0:
        raise RuntimeError
    