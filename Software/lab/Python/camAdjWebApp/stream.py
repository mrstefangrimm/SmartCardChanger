
from itertools import cycle

class StreamItem:
    def __init__(self, frame_seq_no: int, frame):
        self.frame_seq_no = frame_seq_no
        self.frame = frame


class Stream:
    def __init__(self, id: int, name: str, status: str):
        self.id = id
        self.name = name
        self.status = status
        self.items: StreamItem = [None] * 10
        self.current_frame = None

    def append(self, frame_seq_no: int, frame):
        # self.items.append(StreamItem(frame_seq_no=frame_seq_no, frame=frame))
        self.current_frame = StreamItem(frame_seq_no=frame_seq_no, frame=frame)

    def get_first_frame(self):
        return self.current_frame.frame
    