#!/usr/bin/env python3

import cv2
import numpy as np
import py_grid_finder as gr
from math import cos, sin
from timeit import default_timer as timer

GRIDSIZE = 24

def main(outname, foldername, bgr = False):
    videofiles = foldername + "/image%04d.bmp"
    video = cv2.VideoCapture(videofiles)
    if not video.isOpened():
        raise RuntimeError("Unable to open video file " + videofiles)
    maskfiles = foldername + "/mask%04d.bmp"
    masks = cv2.VideoCapture(maskfiles)
    if not masks.isOpened():
        raise RuntimeError("Unable to open video file " + maskfiles)
    frame_width = int(video.get(cv2.CAP_PROP_FRAME_WIDTH))
    frame_height = int(video.get(cv2.CAP_PROP_FRAME_HEIGHT))
    fps = video.get(cv2.CAP_PROP_FPS) / 4
    out = cv2.VideoWriter(outname+'.out.mp4', cv2.VideoWriter_fourcc(
        'M', 'P', '4', 'V'), fps, (frame_width * 3, frame_height))

    with open(foldername + "/squares.csv") as f:
        squares = f.read().split("\n")
    with open(foldername + "/locs.csv") as f:
        locs = f.read().split("\n")

    i = 0
    posimage = np.zeros((frame_height, frame_width, 3), np.uint8)
    videoresult, image = video.read()
    maskresult, mask = masks.read()
    while maskresult and videoresult:
        if bgr:
            image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
        t = squares[i].split(',')
        points = []
        for p in range((len(t)-2)//2):
            point = (int(round(float(t[2*p + 1]))), 
                     int(round(float(t[2*p + 2]))))
            points += point
            print(point)
            cv2.circle(image, point, 4, (255, 0, 0), -1)

        t = locs[i].split(',')
        loc = (int(round(float(t[1]) * GRIDSIZE + frame_width/2)), 
               int(round(float(t[2]) * GRIDSIZE + frame_height/2)))
        cv2.circle(posimage, loc, 3, (255, 255, 255), -1)

        color = (128, 255, 255)
        for y in range(frame_height//GRIDSIZE + 1):
            y *= GRIDSIZE
            y += (frame_height//2) % GRIDSIZE
            cv2.line(posimage, (0, y), (frame_width, y), color, 1)
        for x in range(frame_width//GRIDSIZE + 1):
            x *= GRIDSIZE
            x += (frame_width//2) % GRIDSIZE
            cv2.line(posimage, (x, 0), (x, frame_height), color, 1)

        font = cv2.FONT_HERSHEY_SIMPLEX
        cv2.putText(image, str(i), (16, frame_height - 16),
                    font, 1, (0, 100, 255), 2, cv2.LINE_AA)

        outimg = np.concatenate((image, mask, posimage), axis=1)
        out.write(outimg)
        videoresult, image = video.read()
        maskresult, mask = masks.read()
        i += 1

    video.release()
    out.release()

main('drone-images-sunlight+mask', 'drone-images-sunlight', bgr=True)
