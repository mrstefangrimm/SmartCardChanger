from cv2.typing import MatLike

class FrameBufferItem:
    def __init__(self, seqNo: int, frame: MatLike):
        self.seqNo: int = seqNo
        self.frame: MatLike = frame

class FrameBuffer:
    def __init__(self, length: int):
        self.length: int = length
        self.items: FrameBufferItem = [None] * 5 
        self.pushIter: int = 0
        self.popIter:int = 0
        self.overflow:bool = False

    def push(self, seqNo: int, frame: MatLike) -> None:
        self.items[self.pushIter] = FrameBufferItem(seqNo=seqNo, frame=frame)

        currentPushIter = self.pushIter
        self.pushIter = (self.pushIter + 1) % self.length

        self.overflow = currentPushIter > self.pushIter if self.overflow == False else True

        if self.overflow:
            self.popIter = self.pushIter

    def pop(self) -> FrameBufferItem | None:
        item = self.items[self.popIter]

        self.items[self.popIter] = None
        if item == None:
            return None
    
        currentPopIter = self.popIter
        self.popIter = (self.popIter + 1) % self.length

        self.overflow = currentPopIter < self.popIter if self.overflow == True else False

        return item
