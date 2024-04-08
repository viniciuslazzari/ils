import sys
from startup import *
from ils import *

def main(instance: str, x: int, y: int):
  k, n, points = read_input(instance)
  m = build_distance_matrix(n, points)
  
  s, min = ils(k, n, m, x, y)

  print(s)
  print(min)

if __name__ == "__main__":
  instance = sys.argv[1]
  x = int(sys.argv[2])
  y = int(sys.argv[3])

  # instance = "cl00.dat"
  # x = 10
  # y = 10

  main(instance, x, y)