# from http://letstalkdata.com/2013/12/n-queens-part-2-more-algorithms/
import math
import random
 
def annealing(board):
  temp = len(board)**2
  anneal_rate = 0.95
  new_h_cost = get_h_cost(board)
  steps = 0
  while new_h_cost > 0:
    board = make_annealing_move(board,new_h_cost,temp)
    new_h_cost = get_h_cost(board)
    #Make sure temp doesn't get impossibly low
    new_temp = max(temp * anneal_rate,0.01)
    temp = new_temp
    steps+=1
    if steps >= 50000:
      #print('limit')
      break
  show(board,len(board))
 
def make_annealing_move(board,h_to_beat,temp):
  board_copy = list(board)
  found_move = False
  while not found_move:
    board_copy = list(board)
    new_row = random.randint(0,len(board)-1)
    new_col = random.randint(0,len(board)-1)
    board_copy[new_col] = new_row
    new_h_cost = get_h_cost(board_copy)
    if new_h_cost < h_to_beat:
      found_move = True
    else:
      #How bad was the choice?
      delta_e = h_to_beat - new_h_cost
      #Probability can never exceed 1
      #print(temp)
      accept_probability = min(1,math.exp(delta_e/temp))
      found_move = random.random() <= accept_probability
  return board_copy

def get_h_cost(board):
  h = 0
  for i in range(len(board)):
    #Check every column we haven't already checked
    for j in range(i + 1,len(board)):
      #Queens are in the same row
      if board[i] == board[j]:
        h += 1
      #Get the difference between the current column
      #and the check column
      offset = j - i
      #To be a diagonal, the check column value has to be 
      #equal to the current column value +/- the offset
      if board[i] == board[j] - offset \
        or board[i] == board[j] + offset:
        h += 1
  return h

def show(soln,nr):
    """
    print the maps
    soln[4] for example
    [2, 0, 3, 1]
    row
    ^
    | ~ ~ Q ~
    | Q ~ ~ ~
    | ~ ~ ~ Q
    | ~ Q ~ ~
    ----------> col
    maps [0, 2] [1, 0] [2, 3] ,[3, 1]
    """
    # print(soln)
    for i in range(nr):
        row=['~']*nr
        for col in range(nr):
            if soln[col]==nr-1-i:
                row[col]='Q'
        print(' '.join(row))

def annealingAlgo(num):
    board=[random.randint(0,num-1) for i in range(num)]
    annealing(board)

if __name__ == '__main__':
    annealingAlgo(20)
