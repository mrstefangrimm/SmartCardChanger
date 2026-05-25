import numpy as np
import cv2
from threading import Thread
import time
from stream import Stream
from task.processing_task import ProcessingTask
from processing.filters import EdgeFilter

class CarriageDetection(ProcessingTask):
    def __init__(self, input_stream: Stream, output_stream: Stream, edgeFilter: EdgeFilter, detect_interval=1.0):
        self.input_stream = input_stream
        self.output_stream = output_stream
        self.edgeFilter = edgeFilter
        self.detect_interval = detect_interval
        self.current_frame_seq_no: int = 0
        self.running = False

    def start(self):
        self.running = True
        self.thread = Thread(target=self._detect_loop, daemon=True)
        self.thread.start()

    def stop(self):
        self.running = False

    def _detect_loop(self):

        print("CarriageDetection initialized in thread")

        while self.running:
            frame = self.input_stream.get_first_frame()
            frame = self.edgeFilter.run(frame=frame)
            if frame is not None:
                # Upper left corner
                origin_x = 200
                origin_y = 190
                scan_len = 100
                scanline1 = np.zeros(scan_len)
                for i, n in enumerate(range(origin_y, origin_y + scan_len)):
                    pixelCount = frame[n, origin_x]
                    scanline1[i] = pixelCount
                    frame[n, origin_x] = 255

                if np.sum(scanline1) > 0:
                    center_of_mass = np.average(np.arange(scan_len), weights=scanline1)
                    print(center_of_mass)

                    cv2.rectangle(frame, (origin_x-10, (int)(origin_y+center_of_mass-10)), (origin_x+10, (int)(origin_y+center_of_mass+10)), (0, 0, 255), 3)
                else:
                    cv2.rectangle(frame, (origin_x, origin_y), (origin_x, origin_y+100), (255, 0, 0), 1)

            time.sleep(self.detect_interval)

    def get_intersections(self):
        return [{"x": float(1), "y": float(2)}]
