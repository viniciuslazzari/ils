from datatypes import Point
from utils import euclidian_distance

c = Point(0, 0)

def read_input(instance: str) -> tuple[int, int, dict]:
  """
    Read raw input in file.

    Args:
        instance (str): name of the file.

    Returns:
        int: the number of collectors.
        int: the number of points.
        dict: points informations.
  """
      
  path = "./instances/" + instance

  with open(path) as f:
    lines = f.read().splitlines()

    k = int(lines[0])

    # + 1 to account for the company location
    n = len(lines[1:]) + 1

    points = {}
    points[0] = c;

    for line in lines[1:]:
      point_raw = [int(x) for x in line.split(" ")]
      points[point_raw[0]] = Point(point_raw[1], point_raw[2])

  return k, n, points

def build_distance_matrix(n: int, points: dict) -> list:
  """
    Return the matrix of distances between each point pair.

    Args:
        n (int): number of points.
        points (dict): points informations.

    Returns:
        list: the matrix containing all the distances.
  """

  m = []

  for i in range(n):
    temp = []

    for j in range(n):
      distance = euclidian_distance(points[i], points[j]) if i != 0 and j != 0 else 0
      distance = round(distance, 2)
      temp.append(distance)

    m.append(temp)

  return m