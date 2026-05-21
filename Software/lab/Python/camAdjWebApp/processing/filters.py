import cv2
import numpy as np
from processing.processing import Processing

class SizePositionRotateSkewFilter(Processing):
    def __init__(self, id: int, short_name: str, type: str, name: str):
        self.id = id
        self.short_name = short_name
        self.name = name
        self.type = type
        self.x = 0
        self.y = 0
        self.width = 1920
        self.height = 1080
        self.rtn = 0

    def run(self, frame):
        if frame is None:
            return None

        # print(self.x, self.y, self.width, self.height)
        frame = self.rotate_with_cropping(frame)

        # Apply ROI
        frame = frame[self.y : self.height, self.x : self.width]
        return frame

    def rotate_with_cropping(self, frame):
        h, w = frame.shape[:2]
        center = (w // 2, h // 2)

        M = cv2.getRotationMatrix2D(center, self.rtn, 1.0)

        return cv2.warpAffine(frame, M, (w, h))

    def rotate_without_cropping(self, frame, rtn):
        h, w = frame.shape[:2]
        center = (w // 2, h // 2)

        scale = 1.0

        M = cv2.getRotationMatrix2D(center, rtn, scale)

        # Compute new bounding box
        cos = abs(M[0, 0])
        sin = abs(M[0, 1])

        new_w = int((h * sin) + (w * cos))
        new_h = int((h * cos) + (w * sin))

        # Adjust rotation matrix to account for translation
        M[0, 2] += (new_w / 2) - center[0]
        M[1, 2] += (new_h / 2) - center[1]

        return cv2.warpAffine(frame, M, (new_w, new_h))


class EdgeFilter(Processing):
    def __init__(self, id: int, short_name: str, type: str, name: str,
                 blurEnabled: bool=False, gaussianBlurKernelSize: int=15,
                 cannyLowerThreshold: int=50, cannyUpperThreshold: int=150):
        self.id = id
        self.short_name = short_name
        self.type = type
        self.name = name
        self.blurEnabled = blurEnabled
        self.kernelSize = gaussianBlurKernelSize
        self.lowerThreshold = cannyLowerThreshold
        self.upperThreshold = cannyUpperThreshold
        self.enabled = False


    def run(self, frame):
        if frame is None:
            return None
        
        if (self.blurEnabled):
            frame = cv2.blur(frame, (self.kernelSize, self.kernelSize))

        frame = cv2.Canny(frame, self.lowerThreshold, self.upperThreshold)
        return frame
