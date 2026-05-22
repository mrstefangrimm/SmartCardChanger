from abc import ABC, abstractmethod

class ProcessingTask(ABC):

    @abstractmethod
    def start(self):
        pass

    @abstractmethod
    def stop(self):
        pass

    @abstractmethod
    def get_intersections(self):
        pass