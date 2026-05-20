from abc import ABC, abstractmethod

# TODO: This can become ProcessingTask
class ImageCapture(ABC):

    @abstractmethod
    def start(self):
        pass

    @abstractmethod
    def stop(self):
        pass

    @abstractmethod
    def get_intersections(self):
        pass