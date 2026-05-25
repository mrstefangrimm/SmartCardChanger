from task.processing_task import ProcessingTask
from threading import Thread
from stream import Stream
from processing import *
import time
import cv2

class CamaraAdjustmentOverlay(ProcessingTask):

    def __init__(self, input_stream: Stream, output_stream: Stream, processing_store: Processing, paint_interval=1.0):
        self.input_stream = input_stream
        self.output_stream = output_stream
        self.processing_store = processing_store
        self.capture_interval = paint_interval
        self.video_show_line_profile = False

    def start(self):
        self.running = True
        self.thread = Thread(target=self._paint_loop, daemon=True)
        self.thread.start()

    def stop(self):
        self.running = False

    def _paint_loop(self):

        print("Video initialized in thread")
        edgeFilter = next((p for p in self.processing_store if p.short_name == "EDG"), None)

        while self.running:
            camera_frame = self.input_stream.get_first_frame()
            if self.video_show_line_profile:                
                camera_frame = edgeFilter.run(camera_frame) if edgeFilter else camera_frame
        
                # Convert grayscale canny image back to BGR (3 channels)
                camera_frame = cv2.cvtColor(camera_frame, cv2.COLOR_GRAY2BGR)

            # Apply overlay
            overlay_frame = cv2.rectangle(camera_frame, (110, 190), (210, 290), (0, 255, 0), 3)
            overlay_frame = cv2.rectangle(camera_frame, (110, 750), (210, 850), (0, 255, 0), 3)
            overlay_frame = cv2.rectangle(camera_frame, (460, 190), (560, 290), (0, 255, 0), 3)
            overlay_frame = cv2.rectangle(camera_frame, (460, 750), (560, 850), (0, 255, 0), 3)

            jpgConverter = next((p for p in self.processing_store if p.short_name == "JPG"), None)
            jpgImage = jpgConverter.run(overlay_frame) if jpgConverter else overlay_frame
            if jpgImage:
                self.output_stream.append(frame_seq_no=0, frame=jpgImage)
            
            time.sleep(self.capture_interval)

    def get_intersections(self):
        return [{"x": float(1), "y": float(2)}]

