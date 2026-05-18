class Stream:
    def __init__(self, id: int, name: str, status: str):
        self.id = id
        self.name = name
        self.status = status

    def run(self, frame):
        raise NotImplementedError

    def to_dict(self):
        return {"id": self.id, "name": self.name, "status": self.status}
    