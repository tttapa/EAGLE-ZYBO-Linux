import matplotlib.pyplot as plt
import numpy as np
import time
import math

from DroneLogger import LogEntry

class LivePlotter:

    def __init__(self, buffer_len):
        self.buffer_len = buffer_len
        self.start_time = float('nan')
        self.end_time = float('nan')
        self.sample_counter = 0

        self.data = dict()

        self.data['t'] = np.full(
            (buffer_len,), float('nan'), dtype=np.float)

        self.data['x_measurement'] = np.full(
            (buffer_len,), float('nan'), dtype=np.float)
        self.data['x_reference'] = np.full(
            (buffer_len,), float('nan'), dtype=np.float)
        self.data['y_measurement'] = np.full(
            (buffer_len,), float('nan'), dtype=np.float)
        self.data['y_reference'] = np.full(
            (buffer_len,), float('nan'), dtype=np.float)

        self.create_axes()

    def create_axes(self):
        plt.ion()
        self.fig = plt.figure()
        grid = (2, 1)
        gs = self.fig.add_gridspec(*grid)

        self.axes = dict()

        self.axes['x'] = self.fig.add_subplot(gs[0, 0])
        self.axes['y'] = self.fig.add_subplot(gs[1, 0])

        self.lines = dict()

        self.lines['x_reference'], = self.axes['x'].plot(
            self.data['t'], self.data['x_reference'],
            color='red', label='Reference', marker='.')

        self.lines['x_measurement'], = self.axes['x'].plot(
            self.data['t'], self.data['x_measurement'],
            color='blue', label='Measurement', marker='.')

        self.lines['y_reference'], = self.axes['y'].plot(
            self.data['t'], self.data['y_reference'],
            color='red', label='Reference', marker='.')

        self.lines['y_measurement'], = self.axes['y'].plot(
            self.data['t'], self.data['y_measurement'],
            color='blue', label='Measurement', marker='.')

        # plt.show(self.fig)

    def shift_data(self):
        for key in self.data.keys():
            self.data[key] = np.roll(self.data[key], 1)

    def update_data(self, logentry: LogEntry):
        self.shift_data()
        self.data['t'][0] = logentry.millis / 1000
        self.data['x_reference'][0] = logentry.positionReference.p[0]
        self.data['x_measurement'][0] = logentry.sensorPositionMeasurement[0]
        self.data['y_reference'][0] = logentry.positionReference.p[1]
        self.data['y_measurement'][0] = logentry.sensorPositionMeasurement[1]

        if math.isnan(self.start_time):
            self.start_time = self.data['t'][0]
        elif self.sample_counter > self.buffer_len:
            self.start_time = self.data['t'][-1]
        self.end_time = self.data['t'][0]
        self.sample_counter += 1

    def update_plots(self):
        for key, line in self.lines.items():
            line.set_xdata(self.data['t'])
            line.set_ydata(self.data[key])
        for ax in self.axes.values():
            ax.set_xlim(self.start_time, self.end_time)
            ax.relim()
            ax.autoscale_view()
        self.fig.canvas.draw()

    def update(self, logentry: LogEntry):
        self.update_data(logentry)
        self.update_plots()
