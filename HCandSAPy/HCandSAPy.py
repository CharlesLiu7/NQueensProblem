# This code solves the n-queens problem up to sizes of roughly 1000000 using
# simulated annealing.
# Copyright 2012 Kathy Ray and Omer Bar-or.
# from http://kathyray.org/million_queens.html

import collections
import math
import random
import sys

class Board(object):
  def __init__(self, n, boardIn=[]):
    """
    board is represented as a list of columns, each containing one queen.
    board[i] gives the row of the queen in column i
    """
    if boardIn == []:
      self.board = [i for i in range(n)]
      random.shuffle(self.board)
    else:
      self.board = boardIn
    self._n = n
    """
    The key to making this code fast enough to solve the million-queens problem
    is to avoid recalculating the number of conflicting queens from scratch each
    time it's needed.
    Instead we maintain the number of conflicts at all times.
    """
    self._queens_per_row = self.get_queens_per_row()
    self._count_per_row = [len(queens) for queens in self._queens_per_row]
    self._queens_per_updiag = self.get_queens_per_updiag()
    self._count_per_updiag = [len(queens) for queens in self._queens_per_updiag]
    self._queens_per_downdiag = self.get_queens_per_downdiag()
    self._count_per_downdiag = [len(queens) for queens in self._queens_per_downdiag]
    # the number of ( row, updiag, downdiag)
    self._conflicts = sum([1 for x in self._count_per_row if x > 1]
                        + [1 for x in self._count_per_updiag if x > 1]
                        + [1 for x in self._count_per_downdiag if x > 1])
    """
    As we get near a solution, there will be very few queens with conflicts. We
    maintain a list of queens that may have conflicts, removing those that are
    found to be conflict-free. This way we have a smaller group of queens to choose
    from.
    We maintain this information in both a list and a set, because need to
    avoid duplicates and we haven't found a good way to choose a random item 
    from a set. 
    """
    # the set of the col which has conflict
    self._maybe_conflicting_queens_set = set()
    for queens in self._queens_per_row:
      if len(queens) > 1:
        self._maybe_conflicting_queens_set |= queens
    for queens in self._queens_per_updiag:
      if len(queens) > 1:
        self._maybe_conflicting_queens_set |= queens
    for queens in self._queens_per_downdiag:
      if len(queens) > 1:
        self._maybe_conflicting_queens_set |= queens
    # the list of the col which has conflict
    self._maybe_conflicting_queens_list = list(self._maybe_conflicting_queens_set)

  def get_queens_per_row(self):
    queens = [set() for i in range(self._n)]
    for i in range(self._n):
      queens[self.board[i]].add(i)
    return queens

  def get_queens_per_updiag(self):
    upward_diags = [self.board[i] + i for i in range(self._n)]
    queens = [set() for i in range(2 * self._n - 1)]
    for i in range(self._n):
      queens[upward_diags[i]].add(i)
    return queens

  def get_queens_per_downdiag(self):
    downward_diags = [self.board[i] + (self._n - i - 1) for i in range(self._n)]
    queens = [set() for i in range(2 * self._n - 1)]
    for i in range(self._n):
      queens[downward_diags[i]].add(i)
    return queens

  def num_conflicts(self, queenNum, queenPos=None):
    """ 
    (col, row) = (queenNum, queenPos) the number of the conflict queens
    """ 
    if queenPos == None:
      queenPos = self.board[queenNum]
    rowConflicts = self._count_per_row[queenPos] 
    updiagConflicts = self._count_per_updiag[queenPos + queenNum]
    downdiagConflicts = self._count_per_downdiag[queenPos + 
        (self._n - queenNum - 1)]
    cost = rowConflicts + updiagConflicts + downdiagConflicts
    if queenPos == self.board[queenNum]:
      #if queen is actually in queenPos, each of these three costs
      #counted queenNum itself.
      cost -= 3
    return cost

  def get_highest_conflict_queen(self):
    """
    the current compass, random choose out the queen that has the maximum conflicts 
    return the random column
    """
    maxList = [0]
    maxConflicts = self.num_conflicts(0)
    for i in range(1, self._n):
      curConflicts = self.num_conflicts(i)
      if curConflicts > maxConflicts:
        maxList = [i]
        maxConflicts = curConflicts
      elif curConflicts == maxConflicts:
        maxList.append(i)
    return random.choice(maxList)

  def get_lowest_conflict_move(self, queenNum):
    """
    as col=queenNum of the queen, random choose out the column that the queen has the minimum conflicts
    returns column index this queen should move to
    """
    minList = [0]
    minConflicts = self.num_conflicts(queenNum, 0)
    for i in range(1, self._n):
      curConflicts = self.num_conflicts(queenNum, i)
      if curConflicts < minConflicts:
        minList = [i]
        minConflicts = curConflicts
      elif curConflicts == minConflicts:
        minList.append(i)
    return random.choice(minList)

  def get_random_queen(self):
    """
    random choose a queen that has conflicts from the _maybe_conflicting_quuen_set
    """
    while self._maybe_conflicting_queens_list:
      choice_index = random.randint(0, len(self._maybe_conflicting_queens_list) - 1)
      choice = self._maybe_conflicting_queens_list[choice_index]
      if self.num_conflicts(choice) > 0:
        return choice
      else:
        #This queen has no conflicts, so remove it from our list of possibilities
        self._maybe_conflicting_queens_set.remove(choice)
        self._maybe_conflicting_queens_list[choice_index] = (
            self._maybe_conflicting_queens_list[-1])
        self._maybe_conflicting_queens_list.pop()
    raise Exception("We have %d conflicts, but I couldn't find a conflicting queen..." % self._conflicts)

  def get_random_move(self, queenNum):
    while True:
      row = random.randint(0, self._n - 1)
      if row != self.board[queenNum]:
        return row

  def move_queen(self, queenNum, queenPos):
    #remove counts and conflicts due to old position
    oldPos = self.board[queenNum]
    oldUpdiag = oldPos + queenNum
    oldDowndiag = oldPos + (self._n - queenNum - 1)
    self._queens_per_row[oldPos].remove(queenNum)
    self._count_per_row[oldPos] -= 1
    if self._count_per_row[oldPos] == 1:
      self._conflicts -= 1
    self._queens_per_updiag[oldUpdiag].remove(queenNum)
    self._count_per_updiag[oldUpdiag] -= 1
    if self._count_per_updiag[oldUpdiag] == 1:
      self._conflicts -= 1
    self._queens_per_downdiag[oldDowndiag].remove(queenNum)
    self._count_per_downdiag[oldDowndiag] -= 1
    if self._count_per_downdiag[oldDowndiag] == 1:
      self._conflicts -= 1

    self.board[queenNum] = queenPos
    queenUpdiag = queenPos + queenNum
    queenDowndiag = queenPos + (self._n - queenNum - 1)

    #add counts and conflicts due to new position
    self._queens_per_row[queenPos].add(queenNum)
    self._count_per_row[queenPos] += 1
    maybe_new_conflicts = set()
    if self._count_per_row[queenPos] == 2:
      self._conflicts += 1 
      maybe_new_conflicts |= self._queens_per_row[queenPos]
    self._queens_per_updiag[queenUpdiag].add(queenNum)
    self._count_per_updiag[queenUpdiag] += 1
    if self._count_per_updiag[queenUpdiag] == 2:
      self._conflicts += 1
      maybe_new_conflicts |= self._queens_per_updiag[queenUpdiag]
    self._queens_per_downdiag[queenDowndiag].add(queenNum)
    self._count_per_downdiag[queenDowndiag] += 1
    if self._count_per_downdiag[queenDowndiag] == 2:
      self._conflicts += 1
      maybe_new_conflicts |= self._queens_per_downdiag[queenDowndiag]
    if maybe_new_conflicts:
      new_conflicts = maybe_new_conflicts - self._maybe_conflicting_queens_set
      self._maybe_conflicting_queens_set |= new_conflicts
      self._maybe_conflicting_queens_list += list(new_conflicts)

  def __repr__(self):
    return str(self.board)

  def is_solution(self):
    if self._conflicts == 0:
      return True
    else:
      return False

  def cost(self):
    return self._conflicts

  def long_cost(self):
    #Calculate number of conflicts from scratch - too slow for large 
    #problem sizes. Used only for debugging.
    cost = 0
    #there are no columns with more than one queen
    #find number of rows with more than one queen
    count = collections.Counter(self.board)
    repeat_costs = [n_choose_k(count[i], 2) for i in count if count[i] > 1]
    row_cost = sum(repeat_costs)

    #find number of diagonal threats
    upward_diags = [self.board[i] + i for i in range(len(self.board))]
    count = collections.Counter(upward_diags)
    repeat_costs = [n_choose_k(count[i], 2) for i in count if count[i] > 1]
    diag_cost = sum(repeat_costs)

    downward_diags = [self.board[i] + (len(self.board) - i - 1) for i in range(len(self.board))]
    count = collections.Counter(downward_diags)
    repeat_costs = [n_choose_k(count[i], 2) for i in count if count[i] > 1]
    diag_cost += sum(repeat_costs)

    return row_cost + diag_cost

def n_choose_k(n,k):
  return math.factorial(n)//(math.factorial(k)*math.factorial(n-k))

def hill_climb(board):
  steps = 0
  while True:
    badQueen = board.get_highest_conflict_queen()
    goodMove = board.get_lowest_conflict_move(badQueen)
    if goodMove == board.board[badQueen]:
      #have reached local minimum
      break
    else:
      board.move_queen(badQueen, goodMove)
    steps += 1
    if steps % 10 == 0:
      print('steps so far: %d' % steps)
  #print('steps: %d' % steps)

def accept_new_board(oldCost, newCost, time):
  if newCost < oldCost:
    return True
  if random.random() <= math.exp((oldCost-newCost)*time):
    return True
  else:
    return False

def simulated_annealing(board):
  #Using an increasing time instead of a decreasing temperature.
  #This way we don't set a maximum number of iterations before
  #we start - time can increase indefinitely (well, until it overflows)
  #Think of temperature as the inverse of time.
  time = 0
  dt = .0005
  while True:
    queen = board.get_random_queen()
    currentCost = board.num_conflicts(queen)
    move = board.get_random_move(queen)
    newCost = board.num_conflicts(queen, move)
    if accept_new_board(currentCost, newCost, time*dt):
      #print('accepted %d over %d' % (newCost, currentCost))
      board.move_queen(queen, move)
      if board.is_solution():
        return
    #else:
      #print('did not accept %d over %d' % (newCost, currentCost))
    time += 1
    if time % 100 == 0:
      print('time = %d, cost = %d' % (time, board.cost()))
  #print('sim an finished iteration')

_ALGORITHMS = {
    'hill_climb': hill_climb,
    'simulated_annealing': simulated_annealing,
    'hc': hill_climb,
    'sa': simulated_annealing,
    }
def n_queens(n, algorithm_str):
  if algorithm_str not in _ALGORITHMS:
    raise NotImplementedError('Algorithm %s does not exist. Try: %s' % (
      algorithm_str, str(_ALGORITHMS.keys())))
  algorithm = _ALGORITHMS[algorithm_str]

  #randomized restarts
  for i in range(100000):
    b = Board(n)
    #just in case we happened to get a board with 0 conflicts
    #(that's cheating)
    while b.is_solution():
      b = Board(n)
    algorithm(b) 
    if b.is_solution():
      break
  print('is_solution: ' + str(b.is_solution()))
  print('cost: ' + str(b.cost()))
  #print('long_cost: ' + str(b.long_cost()))
  return b 

def main():
  """
  for command line
  """
  args = sys.argv[1:]
  if not args:
    print('usage: N')
    sys.exit(1)
  n = int(args[0])
  if n < 4:
    print('No solutions for N < 4')
    sys.exit(0)
  if len(args) > 1:
    algorithm = args[1]
  else:
    algorithm = 'hill_climb'
  solution_board = n_queens(n, algorithm)
  if solution_board.is_solution():
    f = open('solution.txt', 'w')
    f.write('%s\n' % str(solution_board))
    f.close()
  if n < 15:
    print(solution_board)

if __name__ == '__main__':
  n=100
  algorithm='sa'
  solution_board = n_queens(n, algorithm)
  if solution_board.is_solution():
    f = open('solution.txt', 'w')
    f.write('%s\n' % str(solution_board))
    f.close()
  if n < 15:
    print(solution_board)