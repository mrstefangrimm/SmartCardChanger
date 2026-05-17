import cv2
import numpy as np
from hough_lines import LineIntersection
from threading import Thread
import time
from abc import ABC, abstractmethod
from image_capture.image_capture import ImageCapture


class FakeCapture(ImageCapture):
    def __init__(self):
        self.current_frame = None
        self.running = False

    def start(self):
        self.running = True
        self.current_frame = cv2.imread("test_data/img1.jpg")

    def stop(self):
        self.running = False

    def get_frame(self):
        if self.current_frame is None:
            return None

        if self.running is False:
            return None

        return self.current_frame.copy()

    def get_intersections(self):
        return [{"x": float(1), "y": float(2)}]
