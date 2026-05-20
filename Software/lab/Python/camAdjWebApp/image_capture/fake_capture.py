import cv2
from threading import Thread
import time
from stream import Stream
from image_capture.image_capture import ImageCapture


class FakeCapture(ImageCapture):
    def __init__(self, input_stream: Stream=None, output_stream: Stream=None, capture_interval=1.0):
        self.input_stream = input_stream
        self.output_stream = output_stream
        self.capture_interval = capture_interval
        self.current_frame_seq_no: int = 0
        self.running = False

    def start(self):
        self.running = True
        self.thread = Thread(target=self._capture_loop, daemon=True)
        self.thread.start()

    def stop(self):
        self.running = False

    def _capture_loop(self):

        print("Fake initialized in thread")

        while self.running:
            frame = cv2.imread("test_data/img1.jpg")
            if frame is not None:
                  self.current_frame_seq_no += 1
                  self.output_stream.append(frame_seq_no=self.current_frame_seq_no, frame=frame)

            time.sleep(self.capture_interval)

    def get_intersections(self):
        return [{"x": float(1), "y": float(2)}]
