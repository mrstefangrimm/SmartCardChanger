from abc import ABC, abstractmethod

class Processing(ABC):
    def __init__(self, id: str, type: str, name: str):
        self.id = id
        self.type = type
        self.name = name
        self.enabled = True

    @abstractmethod
    def run(self, frame):
        pass

    def to_dict(self):
        return {"id": self.id, "name": self.name}
