import logging
import os
import subprocess
from typing import List

import cv2 as cv
import matplotlib.pyplot as plt
import numpy as np
from metrics import IOU, Metric

# Unset environment variable QT_QPA_PLATFORM_PLUGIN_PATH
# to remove the conflict between conda's and system's Qt version.
os.environ.pop("QT_QPA_PLATFORM_PLUGIN_PATH")


class SnakeArguments:

    def __init__(self, binary_path: str, video_path: str, save_mask_path: str, points: int):
        self.binary_path = binary_path
        self.input_video_path = video_path
        self.save_mask_path = save_mask_path
        self.points = points
        self.color_of_interest = (240, 10, 70)
        self.dilate = 'true'
        self.erode = 'true'


def start_snake(arguments: SnakeArguments):
    # Define command and run it.
    cwd = os.getcwd()
    cmd = f"{os.path.join(cwd, arguments.binary_path)} " \
          f"--video {os.path.join(cwd, arguments.input_video_path)} " \
          f"--colorOfInterestRGB {arguments.color_of_interest[0]},{arguments.color_of_interest[1]},{arguments.color_of_interest[2]} " \
          f"--morphDilate {arguments.dilate} " \
          f"--morphErode {arguments.erode} " \
          f"--numPoints {arguments.points} " \
          f"--saveMaskPath {os.path.join(cwd, arguments.save_mask_path)}"
    result = subprocess.run(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    logging.debug(f"Snake process return code: {result.returncode}")
    logging.debug(f"Snake process stderr: {result.stderr}")


def evaluate(predicted_mask_video_path: str, reference_mask_video_path: str, metrics: List[Metric]):
    if not os.path.isfile(predicted_mask_video_path):
        raise RuntimeError("Path to predicted mask video does not exist.")
    if not os.path.isfile(reference_mask_video_path):
        raise RuntimeError("Path to reference mask video does not exist.")

    # Load reference mask
    predicted_mask_video = cv.VideoCapture(predicted_mask_video_path)
    reference_mask_video = cv.VideoCapture(reference_mask_video_path)

    predicted_frame_count = int(predicted_mask_video.get(cv.CAP_PROP_FRAME_COUNT))
    reference_frame_count = int(reference_mask_video.get(cv.CAP_PROP_FRAME_COUNT))

    if predicted_frame_count > reference_frame_count:
        raise RuntimeError(f"Predicted frame count should not be greater than the reference: "
                           f"predicted={predicted_frame_count}, reference={reference_frame_count}.")
    common_frames_count = predicted_frame_count
    notcommon_frames_count = reference_frame_count - predicted_frame_count

    # Evaluate on common set of frames
    for i in range(common_frames_count):
        if predicted_mask_video.isOpened() and reference_mask_video.isOpened():
            is_prediction_valid, predicted_mask = predicted_mask_video.read()
            is_reference_valid, reference_mask = reference_mask_video.read()

            if not is_prediction_valid or not is_reference_valid:
                raise RuntimeError("Failed to read prediction or reference mask.")

            for metric in metrics:
                metric.update(predicted_mask, reference_mask)

    # Fill the rest of the frames as zero if the video ended prematurely
    for i in range(notcommon_frames_count):
        if reference_mask_video.isOpened():
            is_reference_valid, reference_mask = reference_mask_video.read()
            empty_frame = np.zeros_like(reference_mask)

            if not is_reference_valid:
                raise RuntimeError("Failed to read reference mask.")

            for metric in metrics:
                metric.update(empty_frame, reference_mask)


def run_experiment_with_different_point_count(metrics: List[Metric], points_arguments: List[int]):
    metrics_scores = []
    for points in points_arguments:
        snake_args = SnakeArguments('build/src/snakes_run',
                                    video_path='data/red_car.mkv',
                                    save_mask_path='mask.avi',
                                    points=points)
        logging.info("Running snake...")
        start_snake(snake_args)

        logging.info("Evaluating generated mask...")
        evaluate(snake_args.save_mask_path, 'data/red_car_mask.avi', metrics)
        for metric in metrics:
            metrics_scores.append(metric.get_values().copy())
            metric.reset()
    num_metrics = len(metrics)
    num_experiments = len(points_arguments)
    scores = np.array(metrics_scores)
    scores = np.reshape(scores, [num_experiments, num_metrics, scores.shape[1]])
    np.save('scores.npy', scores)


def plot_experiment(metrics: List[Metric], points_arguments: List[int]):
    plt.rcParams.update({'font.size': 15})
    scores = np.load('scores.npy')

    for metric_index, metric in enumerate(metrics):
        metric_scores = scores[:, metric_index, :]
        fig, ax = plt.subplots(1, 1, figsize=(8, 5))

        for experiment_idx, points in enumerate(points_arguments):
            experiment_scores = metric_scores[experiment_idx]
            ax.plot(experiment_scores, label=F"{points}")
        num_frames = metric_scores.shape[-1]
        ax.set_xlim([0, num_frames])
        ax.set_ylim([0, 1])
        ax.set_xlabel("Frame")
        ax.set_ylabel(f"{metric.name} score")
        ax.legend()
        fig.tight_layout()
        fig.show()


if __name__ == "__main__":
    # Log to console and log everything
    logging.basicConfig(level=logging.NOTSET)

    metrics = [IOU()]
    points_arguments = range(10, 105, 10)
    run_experiment_with_different_point_count(metrics, points_arguments)
    plot_experiment(metrics, points_arguments)
