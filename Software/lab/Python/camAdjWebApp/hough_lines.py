import numpy as np
from itertools import combinations

class LineIntersection:
    """Class for handling Hough line intersections."""
    
    def __init__(self, image_shape=None):
        self.image_shape = image_shape
    
    def get_intersection(self, line1, line2):
        """Find intersection of two lines given in rho-theta format."""
        rho1, theta1 = line1[0]
        rho2, theta2 = line2[0]
        
        a1 = np.cos(theta1)
        b1 = np.sin(theta1)
        c1 = -rho1
        
        a2 = np.cos(theta2)
        b2 = np.sin(theta2)
        c2 = -rho2
        
        determinant = a1 * b2 - a2 * b1
        
        if abs(determinant) < 1e-10:
            return None
        
        x = (b1 * c2 - b2 * c1) / determinant
        y = (a2 * c1 - a1 * c2) / determinant
        
        return (x, y)
    
    def is_valid(self, point):
        """Check if intersection point is within image bounds."""
        if self.image_shape is None:
            return True
        
        x, y = point
        height, width = self.image_shape[:2]
        return 0 <= x < width and 0 <= y < height
    
    def find_all_intersections(self, lines):
        """Find all intersections from a set of Hough lines."""
        if lines is None or len(lines) < 2:
            return []
        
        intersections = []
        for line1, line2 in combinations(lines, 2):
            intersection = self.get_intersection(line1, line2)
            if intersection and self.is_valid(intersection):
                intersections.append({
                    'x': float(intersection[0]),
                    'y': float(intersection[1])
                })
        
        return intersections
