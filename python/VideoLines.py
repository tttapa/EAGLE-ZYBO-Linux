#!/usr/bin/env python3

import cv2
import numpy as np
import PyGridFinder as gr
from math import cos, sin
from timeit import default_timer as timer
import os
import matplotlib.pyplot as plt

scriptfolder = os.path.dirname(os.path.realpath(__file__))
outfolder = os.path.join(scriptfolder, '../Video/test-sequences/out')


def main(outname, filename, bgr=False):
    video = cv2.VideoCapture(filename)
    if not video.isOpened():
        raise RuntimeError("Unable to open video file " + filename)
    frame_width = int(video.get(3))
    frame_height = int(video.get(4))
    fps = video.get(cv2.CAP_PROP_FPS) / 4
    number_frames = int(video.get(cv2.CAP_PROP_FRAME_COUNT))

    if not os.path.exists(outfolder):
        os.makedirs(outfolder)

    out = cv2.VideoWriter(os.path.join(outfolder, outname + '.out.avi'),
                          cv2.VideoWriter_fourcc('M', 'J', 'P', 'G'), fps,
                          (frame_width * 2, frame_height))

    gr_time = 0
    gr_times = np.zeros((number_frames, ))
    mask_time = 0
    framectr = 0
    locationTracker = gr.LocationTracker()
    locations = np.zeros((number_frames, 3))
    index = 0

    result, image = video.read()
    while result and video.isOpened():
        if not bgr:
            image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
        mask, mask_duration = redmask(image)
        mask_time += mask_duration

        processed, square, time = processFrame(image, mask)
        location = locationTracker.getLocation(
            square, (frame_width / 2, frame_height / 2))
        showAngle(locationTracker.getAngle(),
                  locationTracker.getSquareCenter(), processed)
        cv2.circle(processed, (frame_width // 2, frame_height // 2), 3,
                   (0, 200, 160), -1)
        angle = locationTracker.getAngle()
        angle = angle if angle <= np.pi else angle - 2 * np.pi
        locations[index] = (location.x, 1 - location.y, angle)

        gr_time += time
        gr_times[framectr] = time
        result, image = video.read()
        mask = cv2.cvtColor(np.array(mask), cv2.COLOR_GRAY2RGB)
        font = cv2.FONT_HERSHEY_SIMPLEX
        framectr += 1
        cv2.putText(mask, str(framectr), (16, frame_height - 16), font, 1,
                    (0, 100, 255), 2, cv2.LINE_AA)
        outimg = np.concatenate((processed, mask), axis=1)
        out.write(cv2.cvtColor(outimg, cv2.COLOR_RGB2BGR))

        index += 1

    print(locations)

    total_time = mask_time + gr_time
    med_gr_time = np.median(gr_times)
    print("  Mask             : {:5d} fps".format(
        int(round(framectr / mask_time))))
    print("  GridFinder (avg) : {:5d} fps".format(
        int(round(framectr / gr_time))))
    print("  GridFinder (med) : {:5d} fps".format(int(round(1. /
                                                            med_gr_time))))
    print("+ ──────────────────────────")
    print("  Combined         : {:5d} fps".format(
        int(round(framectr / total_time))))

    np.savetxt(os.path.join(outfolder, outname + '.csv'),
               locations,
               delimiter=",",
               header='x,y,yaw',
               comments='')
    plt.plot(locations[:,0:2])
    plt.savefig(os.path.join(outfolder, outname + '.png'))
    plt.close()

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


def showAngle(angle, location, image: np.array):
    h, w, _ = image.shape
    p1 = (int(round(location.x)), int(round(location.y)))
    p2 = (int(round(location.x + 2 * w * cos(angle))),
          int(round(location.y + 2 * w * sin(angle))))
    cv2.line(image, p1, p2, (100, 100, 100), 2)
    cv2.circle(image, p1, 3, (0, 0, 255), -1)


def processFrame(image, mask):
    start = timer()
    gf = gr.GridFinder(mask)
    square = gf.findSquare()
    lines, points = square.lines, square.points
    end = timer()
    time = end - start

    colors = [(0, 80, 255), (0, 200, 255), (0, 255, 0), (255, 255, 0),
              (255, 150, 0)]

    for i in range(len(lines)):
        if lines[i]:
            showLine(lines[i], colors[i], image)

    for point in points:
        if point:
            point = (round(point.x), round(point.y))
            cv2.circle(image, point, 3, (255, 0, 0), -1)

    return image, square, time


def redmask(image):
    start = timer()
    mask = gr.Mask(image)
    end = timer()
    mask_duration = end - start
    return mask, mask_duration


if __name__ == '__main__':
    # main('DroneCam', '../Videos/DroneCam.mp4')
    # main('easy', '../Video/easy.mp4')
    # main('drone-images', 'drone-images/image%4d.bmp', bgr=True)
    main('small_skew_skewnoise', '../Video/small_skew_skewnoise.avi')
    main('mix_skewnoise', '../Video/test-sequences/mix_skewnoise.avi')
    main('rotate_skewnoise', '../Video/test-sequences/rotate_skewnoise.avi')
    main('small_skew', '../Video/test-sequences/small_skew.avi')
    main('small_skew_skewnoise',
         '../Video/test-sequences/small_skew_skewnoise.avi')
    main('translate_10', '../Video/test-sequences/translate_10.avi')
    main('translate_10_skewnoise',
         '../Video/test-sequences/translate_10_skewnoise.avi')
    main('translate_20', '../Video/test-sequences/translate_20.avi')
    main('translate_20_skewnoise',
         '../Video/test-sequences/translate_20_skewnoise.avi')
    main('translate_30', '../Video/test-sequences/translate_30.avi')
    main('translate_30_skewnoise',
         '../Video/test-sequences/translate_30_skewnoise.avi')
    main('translate_50', '../Video/test-sequences/translate_50.avi')
    main('translate_50_skewnoise',
         '../Video/test-sequences/translate_50_skewnoise.avi')
    main('zoom', '../Video/test-sequences/zoom.avi')
    main('zoom_extreme', '../Video/test-sequences/zoom_extreme.avi')
    main('zoom_extreme_skewnoise',
         '../Video/test-sequences/zoom_extreme_skewnoise.avi')
    main('zoom_skewnoise', '../Video/test-sequences/zoom_skewnoise.avi')
