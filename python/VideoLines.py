#!/usr/bin/env python3

import cv2
import numpy as np
import py_grid_finder as gr
from math import cos, sin
from timeit import default_timer as timer

def main(outname, filename, bgr = False):
    video = cv2.VideoCapture(filename)
    if not video.isOpened():
        raise RuntimeError("Unable to open video file " + filename)
    frame_width = int(video.get(3)) * 2
    frame_height = int(video.get(4))
    fps = video.get(cv2.CAP_PROP_FPS) / 4
    out = cv2.VideoWriter(outname+'.out.avi', cv2.VideoWriter_fourcc(
        'M', 'J', 'P', 'G'), fps, (frame_width, frame_height))

    gr_time = 0
    gr_times = np.zeros((int(video.get(cv2.CAP_PROP_FRAME_COUNT)),))
    mask_time = 0
    framectr = 0
    angleTracker = gr.AngleTracker()
    result, image = video.read()
    while result and video.isOpened():
        if not bgr:
            image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
        mask, mask_duration = redmask(image)
        mask_time += mask_duration
        processed, angle, time = processFrame(image, mask)
        if angle >= 0:
            angle = angleTracker.update(angle)
            showAngle(angle, processed)
        gr_time += time
        gr_times[framectr] = time
        result, image = video.read()
        mask = cv2.cvtColor(np.array(mask), cv2.COLOR_GRAY2RGB)
        font = cv2.FONT_HERSHEY_SIMPLEX
        framectr += 1
        cv2.putText(mask, str(framectr), (16, frame_height - 16),
                    font, 1, (0, 100, 255), 2, cv2.LINE_AA)
        outimg = np.concatenate((processed, mask), axis=1)
        out.write(cv2.cvtColor(outimg, cv2.COLOR_RGB2BGR))

    total_time = mask_time + gr_time
    med_gr_time = np.median(gr_times)
    print("  Mask             : {:5d} fps".format(int(round(framectr / mask_time))))
    print("  GridFinder (avg) : {:5d} fps".format(int(round(framectr / gr_time))))
    print("  GridFinder (med) : {:5d} fps".format(int(round(1. / med_gr_time))))
    print("+ ──────────────────────────")
    print("  Combined         : {:5d} fps".format(int(round(framectr / total_time))))
    
    video.release()
    out.release()


def showLine(line, color, image):
    # print(line.getLineCenter())
    # print(line.getAngle())
    # print(line.getWidth())
    p1 = (line.getLineCenter().x, line.getLineCenter().y)
    p2 = (line.getLineCenter().x + int(2 * 410 * cos(line.getAngle())),
          line.getLineCenter().y + int(2 * 410 * sin(line.getAngle())))
    cv2.line(image, p1, p2, color, 2)

def showAngle(angle, image: np.array):
    h, w, _ = image.shape
    p1 = (int(w / 2), int(h / 2))
    p2 = (int(w / 2 + 2 * w * cos(angle)),
          int(h / 2 + 2 * w * sin(angle)))
    cv2.line(image, p1, p2, (100, 100, 100), 2)

def processFrame(image, mask):
    start = timer()
    gf = gr.GridFinder(mask)
    square = gf.findSquare()
    lines, points = square.lines, square.points
    angle = -1
    try:
        angle = square.getAngle()
    except:
        pass
    end = timer()
    time = end - start

    colors = [(0, 80, 255), (0, 200, 255), (0, 255, 0),
              (255, 255, 0), (255, 150, 0)]

    for i in range(len(lines)):
        if lines[i]:
            showLine(lines[i], colors[i], image)

    for point in points:
        if point:
            point = (round(point.x), round(point.y))
            cv2.circle(image, point, 3, (255, 0, 0), -1)

    return image, angle, time


def redmask(image):
    start = timer()
    mask = gr.Mask(image)
    end = timer()
    mask_duration = end - start
    return mask, mask_duration


if __name__ == '__main__':
    # main('DroneCam', '../Videos/DroneCam.mp4')
    main('easy', '../Video/easy.mp4')
    main('drone-images', 'drone-images/image%4d.bmp', bgr=True)
