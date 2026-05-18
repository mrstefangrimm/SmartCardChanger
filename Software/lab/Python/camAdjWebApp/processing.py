import cv2
from abc import ABC, abstractmethod

class Processing(ABC):
    def __init__(self, id: int, name: str):
        self.id = id
        self.name = name
        self.enabled = True

    @abstractmethod
    def run(self, frame):
        pass

    def to_dict(self):
        return {"id": self.id, "name": self.name}


class NormalizeDataProcessing(Processing):
    def __init__(self, id: int, name: str):
        self.id = id
        self.name = name
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

class GaussianBlur(Processing):
    def __init__(self, id: int, name: str, kernelSize: int=15):
        self.id = id
        self.name = name
        self.kernelSize = kernelSize
        self.enabled = False

    def run(self, frame):
        if frame is None:
            return None
        
        frame = cv2.blur(frame, (self.kernelSize, self.kernelSize))
        return frame


class LineProfileProcessing(Processing):
    def run(self, frame):
        if frame is None:
            return None

        frame = cv2.Canny(frame, 50, 150)
        return frame


class ConvertToJpgProcessing(Processing):
    def run(self, frame: bytes):
        if frame is None:
            return None

        ret, buffer = cv2.imencode(".jpg", frame)
        return buffer.tobytes() if ret else None