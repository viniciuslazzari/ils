import math
from datatypes import Point

def get_distance(a: int, b: int, m: list) -> float:
  """
    Return euclidian distance between two computed points.

    Args:
        a (int): first point.
        b (int): second point.
        m (list): distance matrix.

    Returns:
        float: distance between both points.
  """

  return m[a][b]

def euclidian_distance(a: Point, b: Point) -> float:
  """
    Return euclidian distance between two points.

    Args:
        a (Point): first point.
        b (Point): second point.

    Returns:
        float: distance between both points.
  """
    
  return math.sqrt(math.pow(b.x - a.x, 2) + math.pow(b.y - a.y, 2))

def save_solution(s: list) -> bool:
  with open('result.txt', 'w') as f:
    for route in s:
      for point in route:
        f.write(str(point) + " ")

      f.write("\n")

def distribute(items: int, lists: int) -> list:
  base, extra = divmod(items, lists)
  return [base + (i < extra) for i in range(lists)]