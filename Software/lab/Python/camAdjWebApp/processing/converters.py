import cv2
from processing.processing import Processing

class ConvertToJpgProcessing(Processing):
    def run(self, frame: bytes):
        if frame is None:
            return None

        ret, buffer = cv2.imencode(".jpg", frame)
        return buffer.tobytes() if ret else None