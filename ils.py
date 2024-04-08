import random
from copy import deepcopy
from utils import get_distance

def get_initial_solution(k: int, n: int) -> list:
  """
    Return initial randomic solution for the problem by randomly choosing points and adressing it to routes.

    Args:
        k (int): number of collectors.
        n (int): number of points.

    Returns:
        list: the routes of each collector.
  """
    
  r = []

  temp = [i for i in range(1, n)]

  for i in range(k):
    r.append([])

  for i in range(1, n):
    collector = random.randint(0, k - 1)
    point = random.choice(temp)

    r[collector].append(point)
    temp.remove(point)

  for i in range(k):
    r[i].insert(0, 0)
    r[i].append(0)

  return r

def get_route_distance(a: list, m: list) -> float:
  n = len(a)
  total = 0

  for i in range(n - 1):
    distance = get_distance(a[i], a[i + 1], m)
    total += distance

  return total

def get_max_route_distance(r: list, m: list):
  max = 0

  for route in r:
    route_distance = get_route_distance(route, m)

    if route_distance > max:
      max = route_distance

  return round(max, 2)

def route_is_empty(r: list) -> bool:
  return not any(r)

def choose_two_routes(k: int) -> tuple[int, int, int]:
  return random.sample(range(0, k), 2)

def random_route_point(r: list):
  n = len(r)
  index = random.randint(1, n - 2)

  return r[index], index

def local_search(s: list, k: int, y: int, m: list) -> list:
  print("Local search")

  bks = deepcopy(s)
  ps = deepcopy(s)
  min = get_max_route_distance(s, m)
  first = True

  while min < get_max_route_distance(ps, m) or first:
    ps = deepcopy(bks)
    first = False

    print("Min: %.2f" % min)
    
    for i in range(y):
      ts = deepcopy(ps)

      r1, r2 = choose_two_routes(k)

      p1, i1 = random_route_point(ts[r1])
      p2, i2 = random_route_point(ts[r2])

      ts[r1].remove(p1)
      ts[r2].remove(p2)

      ts[r1].insert(i1, p2)
      ts[r2].insert(i2, p1)

      distance = get_max_route_distance(ts, m)

      if distance < min:
        bks = deepcopy(ts)
        min = distance

  return bks, min

def find_max_gap_route(a: list, m: list) -> tuple[int, int]:
  n = len(a)

  if n < 4:
    return 0

  max_gap = 0
  index = 0

  for i in range(1, n - 1):
    distance = get_distance(a[i], a[i + 1], m)

    if distance > max_gap:
      max_gap = distance
      index = i

  return max_gap, index

def find_max_gap(r: list, m: list):
  max_gap = 0
  route = 0
  index = 0

  for i in range(len(r)):
    gap, p = find_max_gap_route(r[i], m)

    if gap > max_gap:
      max_gap = gap
      index = p
      route = i

  return round(max_gap, 2), route, index

def find_point_min_gap(p: int, m: list) -> int:
  n = len(m[p])
  min_gap = 9999999999999
  index = 0

  for i in range(1, n):
    if i == p:
      continue

    if m[p][i] < min_gap:
      min_gap = m[p][i]
      index = i

  return index, min_gap

def find_point_in_routes(s: list, p: int) -> tuple[int, int]:
  n = len(s)

  for i in range(n):
    m = len(s[i])

    for j in range(m):
      if s[i][j] == p:
        return i, j

  return 0, 0

def perturbe(s: list, k: int, m: list) -> list:
  ns = deepcopy(s)

  gap, dr, di = find_max_gap(ns, m)
  fp, min_gap = find_point_min_gap(ns[dr][di + 1], m)
  fr, fi = find_point_in_routes(ns, fp)

  # On the same route
  if fr == dr:
    dp = ns[dr].pop(di)
    ns[fr].pop(fi - 1)

    ns[fr].insert(fi - 1, dp)
    ns[dr].insert(di, fp)
  else:
    dp = ns[dr].pop(di)
    ns[fr].pop(fi)

    ns[fr].insert(fi, dp)
    ns[dr].insert(di, fp)

  print("Perturbing local minimum")
  print("Changed point %d in route %d -> point %d in route %d, removing gap of %.2f" % (dp, dr, fp, fr, gap))

  return ns

def ils(k: int, n: int, m: list, x: int, y: int) -> list:
  s = get_initial_solution(k, n)
  s, min = local_search(s, k, y, m)
  p = 0

  while p < x:
    ns = perturbe(s, k, m)
    ns, n_min = local_search(ns, k, y, m)

    if n_min < min:
      s = ns
      min = n_min
      p = 0
    else:
      p += 1

  return s, min