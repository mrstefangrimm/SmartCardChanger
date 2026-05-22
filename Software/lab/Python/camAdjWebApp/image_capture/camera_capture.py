import cv2
import numpy as np
from hough_lines import LineIntersection
from threading import Thread
import time
from image_capture.image_capture import ImageCapture
from stream import Stream

class CameraCapture(ImageCapture):
    def __init__(self, output_stream: Stream, camera_index=0, capture_interval=1.0):
        self.output_stream=output_stream
        self.camera_index=camera_index
        self.capture_interval=capture_interval
        self.cap=None
        self.current_intersections = [{"x": float(3), "y": float(4)}]
        self.running=False
        self.line_processor=None

    def start(self):
        """Start capturing frames in a separate thread."""
        self.running = True
        self.thread = Thread(target=self._capture_loop, daemon=True)
        self.thread.start()

    def stop(self):
        """Stop capturing frames."""
        self.running = False
        if self.cap:
            self.cap.release()

    def _capture_loop(self):
        """Capture one frame per second and process it."""
        # Initialize camera INSIDE the thread
        self.cap = cv2.VideoCapture(self.camera_index, cv2.CAP_DSHOW)

        # Set camera properties
        self.cap.set(cv2.CAP_PROP_FRAME_WIDTH, 1920)
        self.cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 1080)
        self.cap.set(cv2.CAP_PROP_FPS, 30)
        self.cap.set(cv2.CAP_PROP_BUFFERSIZE, 1)

        if not self.cap.isOpened():
            print("Error: Could not open camera")
            return

        print("Camera initialized in thread")

        while self.running:
            ret, frame = self.cap.read()
            if ret and frame is not None:
                self.current_frame_seq_no += 1
                self.output_stream.append(frame_seq_no=self.current_frame_seq_no, frame=frame)
                # TODO: Remove old, unsused code. self._process_frame(frame)
            else:
                print("Warning: Failed to read frame")

            time.sleep(self.capture_interval)

    def _process_frame(self, frame):
        """Process the frame to find Hough line intersections."""
        gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
        edges = cv2.Canny(gray, 50, 150)
        lines = cv2.HoughLines(edges, 1, np.pi / 180, 100)

        if self.line_processor is None:
            self.line_processor = LineIntersection(image_shape=frame.shape)

        # TODO:  self.current_intersections = self.line_processor.find_all_intersections(lines)

    def get_frame(self):
        if self.current_frame is None:
            return None

        return self.current_frame.copy()

    def get_proc_frame(self):
        """Get the current frame as JPEG bytes for web display."""
        if self.current_frame is None:
            return None

        frame_copy = self.current_frame.copy()

        # frame_copy = cv2.Canny(frame_copy, 50, 150)

        for point in self.current_intersections:
            cv2.circle(
                frame_copy, (int(point["x"]), int(point["y"])), 5, (0, 255, 0), -1
            )

        ret, buffer = cv2.imencode(".jpg", frame_copy)
        return buffer.tobytes() if ret else None

    def get_intersections(self):
        """Get the current list of intersections."""
        return self.current_intersections
