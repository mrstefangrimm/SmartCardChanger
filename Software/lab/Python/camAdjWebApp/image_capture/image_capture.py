from abc import ABC, abstractmethod


class ImageCapture(ABC):

    @abstractmethod
    def start(self):
        pass

    @abstractmethod
    def stop(self):
        pass

    @abstractmethod
    def get_frame(self):
        pass

    @abstractmethod
    def get_intersections(self):
        pass