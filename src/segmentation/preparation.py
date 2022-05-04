import cv2 as cv
import numpy as np


# import matplotlib.pyplot as plt

cv.namedWindow('mask', cv.WINDOW_NORMAL)

def generate_segmentation(frame):
    # Define new frame
    new_frame = np.zeros((frame.shape[0], frame.shape[1], 1), dtype=np.uint8)

    # Color of interest: RGB = 240,10,70, BGR = 70,10,240
    frame_hsv = cv.cvtColor(frame, cv.COLOR_BGR2HSV)
    # interesting_color = np.array([70, 10, 240]).astype(np.int32)
    # interval = 50
    # color_lower_bound = interesting_color - interval
    # color_upper_bound = interesting_color + interval
    lower = np.array([167, 0, 0])
    upper = np.array([175, 255, 255])
    mask = cv.inRange(frame_hsv, lower, upper)
    # Biggest blob detection
    contours, hierarchy = cv.findContours(mask.astype(np.uint8), cv.RETR_EXTERNAL, cv.CHAIN_APPROX_NONE)
    #cv.drawContours(frame, contours, -1, color=(255, 0, 0), thickness=4)


    # Contour of the biggest blob
    if len(contours) != 0:
        biggest_contour = max(contours, key=cv.contourArea)

        # Fill the contour
        cv.drawContours(new_frame, [biggest_contour], -1, color=(255,), thickness=cv.FILLED)

        kernel = np.ones((15, 15), np.uint8)
        new_frame = cv.morphologyEx(new_frame, cv.MORPH_OPEN, kernel)
        kernel = np.ones((50, 50), np.uint8)
        new_frame = cv.morphologyEx(new_frame, cv.MORPH_CLOSE, kernel)
        cv.imshow('mask', new_frame)
        cv.waitKey(1)
    return new_frame


def generate_video_mask():
    in_video_path = "data/red_car.mkv"
    in_video = cv.VideoCapture(in_video_path)
    width = int(in_video.get(cv.CAP_PROP_FRAME_WIDTH))
    height = int(in_video.get(cv.CAP_PROP_FRAME_HEIGHT))
    in_resolution = (width, height)
    in_fps = int(in_video.get(cv.CAP_PROP_FPS))

    fourcc = cv.VideoWriter_fourcc('F', 'L', 'V', '1')
    out_video = cv.VideoWriter('data/output.avi', fourcc, in_fps, in_resolution, isColor=False)

    frame_num = 0
    # Read frame by frame, generate segmentation mask
    # for each frame and save into a new video.
    while in_video.isOpened():
        ret, frame = in_video.read()
        if not ret:
            print("Can't receive frame. Ending segmentation generation.")
            break
        frame_num += 1
        segmented_frame = generate_segmentation(frame)
        # segmented_frame_color = cv.cvtColor(segmented_frame, cv.COLOR_GRAY2RGB)
        out_video.write(segmented_frame)
    print(F"Processed {frame_num} frames.")
    in_video.release()
    out_video.release()


def load_segmentation_video():
    in_video = cv.VideoCapture('data/red_car_mask.mp4')

    while in_video.isOpened():
        ret, frame = in_video.read()
        pass

generate_video_mask()
