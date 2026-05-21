import cv2
import numpy as np
from processing.processing import Processing

class HoughLinesFeatureDetector(Processing):
    def __init__(self, id: int, short_name: str, type: str, name: str, kernelSize: int=15):
        self.id = id
        self.short_name = short_name
        self.type = type
        self.name = name
        self.kernelSize = kernelSize
        self.enabled = False

    def run(self, frame):
        if frame is None:
            return None
        
        lines = cv2.HoughLines(frame, 1, np.pi / 180, 100)
        return frame
