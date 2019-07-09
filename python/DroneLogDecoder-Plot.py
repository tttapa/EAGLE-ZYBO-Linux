from DroneLogger import LogEntry
import os

import matplotlib.pyplot as plt
import numpy as np

logfile = "eagle-2019-05-20.04.29.34.dat"
with open(os.path.join(os.getenv("HOME"), "dronelogs", logfile), 'rb') as f:
    logentries = f.read()
    size = len(bytes(LogEntry()))
    if len(logentries) % size != 0:
        raise RuntimeError(
            "Error: file size is not a multiple of the log entry size")
    num_entries = len(logentries) // size
    start = 0
    end = num_entries
    pos = np.zeros((end - start, 4))
    v = np.zeros((end - start, 2))
    ctrl = np.zeros((end - start, 4))
    for i in range(start, end):
        logentry = LogEntry(logentries[size * i:size * (i+1)])
        # print(logentry.millis)
        # pos[i - start][0] = logentry.positionStateEstimate.q[0]
        # pos[i - start][1] = logentry.positionStateEstimate.q[1]

        pos[i - start][0] = logentry.positionStateEstimate.p[0]
        pos[i - start][1] = logentry.positionStateEstimate.p[1]
        pos[i - start][2] = logentry.positionReference.p[0]
        pos[i - start][3] = logentry.positionReference.p[1]
        
        v[i - start][0] = logentry.positionStateEstimate.v[0]
        v[i - start][1] = logentry.positionStateEstimate.v[1]
        
        # pos[i - start][0] = logentry.sensorPositionMeasurement[0]
        # pos[i - start][1] = logentry.sensorPositionMeasurement[1]

        # pos[i - start][0] = logentry.attitudeStateEstimate.q[1]
        # pos[i - start][1] = logentry.attitudeStateEstimate.q[2]

        ctrl[i - start][0] = logentry.positionControlSignal.q12[0]
        ctrl[i - start][1] = logentry.positionControlSignal.q12[1]
        #  ctrl[i - start][1] = logentry.rollBias
        ctrl[i - start][2] = logentry.pitchBias
        
        ctrl[i - start][3] = logentry.rollBias

        #ctrl[i - start][2] = logentry.attitudeReference.q[1]

    # v = np.diff(pos, n=1, axis=0)
    # a = np.diff(v, n=1, axis=0)

    plt.subplot(311)
    plt.plot(pos)
    plt.legend(('x', 'y', 'xr', 'yr'))
    plt.subplot(312)
    plt.plot(v)
    plt.subplot(313)
    plt.plot(ctrl)
    plt.show()
