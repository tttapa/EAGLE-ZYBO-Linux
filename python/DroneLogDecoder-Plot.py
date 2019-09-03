from DroneLogger import LogEntry
from PyQuaternion import Quaternion
import os

import matplotlib.pyplot as plt
import numpy as np
import scipy as sp
from scipy.signal import lfilter

# logfile = "lambda2.dat"
logfile = "video-friday-nav.dat"
with open(os.path.join(os.getenv("HOME"), "dronelogs", logfile), 'rb') as f:
    logentries = f.read()
    print(len(logentries))
    size = len(bytes(LogEntry()))
    if len(logentries) % size != 0:
        raise RuntimeError(
            "Error: file size is not a multiple of the log entry size")
    num_entries = len(logentries) // size
    start = 0
    end = num_entries
    t = np.zeros((end - start, 1))
    pos = np.zeros((end - start, 6))
    alt = np.zeros((end - start, 5))
    altctrl = np.zeros((end - start, 1))
    hovthr = np.zeros((end - start, 1))
    altref = np.zeros((end - start, 1))
    v = np.zeros((end - start, 4))
    ctrl = np.zeros((end - start, 6))
    modes = np.zeros((end - start, 1))
    for i in range(start, end):
        logentry = LogEntry(logentries[size * i:size * (i+1)])
        # print(logentry.millis)
        t[i - start] = logentry.millis
        # pos[i - start][0] = logentry.positionStateEstimate.q[0]
        # pos[i - start][1] = logentry.positionStateEstimate.q[1]

        pos[i - start][0] = logentry.positionStateEstimate.p[0]
        pos[i - start][1] = logentry.positionStateEstimate.p[1]
        pos[i - start][2] = logentry.positionReference.p[0]
        pos[i - start][3] = logentry.positionReference.p[1]
        pos[i - start][4] = logentry.positionMeasurement.p[0]
        pos[i - start][5] = logentry.positionMeasurement.p[1]

        v[i - start][0] = logentry.positionStateEstimate.v[0]
        v[i - start][1] = logentry.positionStateEstimate.v[1]

        alt[i - start][0] = logentry.altitudeStateEstimate.nt
        alt[i - start][1] = logentry.altitudeStateEstimate.z
        alt[i - start][2] = logentry.altitudeStateEstimate.vz
        alt[i - start][3] = logentry.sensorHeightMeasurement

        altctrl[i-start] = logentry.altitudeControlSignal.ut

        hovthr[i - start] = logentry.thrustBias - (0 if ((i - start) == 0) else hovthr[0])

        altref[i - start] = logentry.altitudeReference.z

        # pos[i - start][0] = logentry.sensorPositionMeasurement[0]
        # pos[i - start][1] = logentry.sensorPositionMeasurement[1]

        # pos[i - start][0] = logentry.attitudeStateEstimate.q[1]
        # pos[i - start][1] = logentry.attitudeStateEstimate.q[2]

        ctrl[i - start][0] = logentry.positionControlSignal.q12[0]
        ctrl[i - start][1] = logentry.positionControlSignal.q12[1]
        #  ctrl[i - start][1] = logentry.rollBias
        ctrl[i - start][2] = logentry.pitchBias

        ctrl[i - start][3] = logentry.rollBias

        p = logentry.rcInput.pitch * 0.1745
        r = logentry.rcInput.roll * 0.1745

        ctrl[i - start][4] = p
        ctrl[i - start][5] = r

        modes[i - start][0] = logentry.tickCount 

        #ctrl[i - start][2] = logentry.attitudeReference.q[1]

    hovthr[0] = 0

    # v = np.diff(pos, n=1, axis=0)
    # a = np.diff(v, n=1, axis=0)
    N = 5
    b = (1,) * N
    a = (N,)

    alpha = 0.2
    b = (0, alpha)
    a = (1, -(1-alpha))

    # pos[:, 4] = lfilter(b, a, pos[:, 0])
    # pos[:, 5] = lfilter(b, a, pos[:, 1])

    # v[:,2] = np.diff(pos[:,0], append=pos[-1,0]) / 0.3 * 100
    # v[:,3] = np.diff(pos[:,1], append=pos[-1,1]) / 0.3 * 100

    alpha = 0.4
    b = (0, alpha)
    a = (1, -(1-alpha))

    # alt[:,4] = sp.interpolate.interp1d(
    #         sp.signal.decimate(t[:,0], 3), 
    #         lfilter(b, a, sp.signal.decimate(alt[:,3], 3)), 
    #         kind='zero',
    #         fill_value="extrapolate")(t[:,0])

    # v[:,2] = lfilter(b, a, v[:,2])
    # v[:,3] = lfilter(b, a, v[:,3])

    plt.subplot(311)
    plt.plot(pos)
    plt.legend(('x', 'y', 'xr', 'yr', 'fx', 'fy'))
    plt.axhline(0, linewidth=1, color='black')
    plt.subplot(312)
    plt.plot(v)
    plt.legend(('vx', 'vy', 'diff vx', 'diff vy'))
    plt.axhline(0, linewidth=1, color='black')
    plt.subplot(313)
    plt.plot(ctrl)
    plt.axhline(0, linewidth=1, color='black')
    plt.tight_layout()
    plt.show()

    # plt.figure()
    # plt.plot(modes)
    # plt.show()

    plt.subplot(211)
    plt.plot(alt, '.-')
    plt.plot(altref)
    plt.legend(('nz', 'z', 'vz', 'zmeas', 'filtered meas', 'zr'))
    plt.grid()
    plt.subplot(212)
    plt.plot(altctrl)
    plt.plot(hovthr)
    plt.tight_layout()
    plt.show()

    print(alt[:,3])
