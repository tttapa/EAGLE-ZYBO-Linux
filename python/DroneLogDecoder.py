from DroneLogger import LogEntry
import os

logfile = "eagle-2019-05-19.23.39.23.dat"
with open(os.path.join(os.getenv("HOME"), "dronelogs", logfile), 'rb') as f:
    logentries = f.read()
    size = len(bytes(LogEntry()))
    if len(logentries) % size != 0:
        raise RuntimeError(
            "Error: file size is not a multiple of the log entry size")
    for i in range(len(logentries) // size):
        logentry = LogEntry(logentries[size * i:size * (i+1)])
        print(logentry.millis)
