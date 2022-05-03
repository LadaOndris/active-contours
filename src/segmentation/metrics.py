from abc import abstractmethod, ABC
import numpy as np

class Metric(ABC):

    def __init__(self, name: str):
        self.name = name
        self.values = []

    @abstractmethod
    def update(self, reference, predicted):
        pass

    def get_values(self):
        return self.values

    def reset(self):
        self.values.clear()

class IOU(Metric):

    def __init__(self):
        super().__init__("IoU")

    def update(self, reference, predicted):
        intersection = np.logical_and(reference, predicted)
        union = np.logical_or(reference, predicted)
        if np.sum(union) == 0:
            iou_score = 0
        else:
            iou_score = np.sum(intersection) / np.sum(union)
        self.values.append(iou_score)
