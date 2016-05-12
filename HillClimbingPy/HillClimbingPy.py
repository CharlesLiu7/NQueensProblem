# from http://letstalkdata.com/2013/12/n-queens-part-1-steepest-hill-climbing/
# form http://letstalkdata.com/2013/12/n-queens-part-2-more-algorithms/
import random
def make_move_steepest_hill(board):
  """
  The hill climbing algorithm gets its name from the metaphor of climbing a hill where the peak is h=0. 
  But there is more than one way to climb a hill. If we always choose the path with the best improvement 
  in heuristic cost then we are using the steepest hill variety.
  """
  moves = {}
  for col in range(len(board)):
    best_move = board[col]
    for row in range(len(board)):
      if board[col] == row:
        #We don't need to evaluate the current
        #position, we already know the h-value
        continue
      board_copy = list(board)
      #Move the queen to the new row
      board_copy[col] = row
      moves[(col,row)] = get_h_cost(board_copy)
  best_moves = []
  h_to_beat = get_h_cost(board)
  for k,v in list(moves.items()):
    if v < h_to_beat:
      h_to_beat = v
  # also include the 'sideway move'
  for k,v in list(moves.items()):
    if v == h_to_beat:
      best_moves.append(k)
  #Pick a random best move
  if len(best_moves) > 0:
    pick = random.randint(0,len(best_moves) - 1)
    col = best_moves[pick][0]
    row = best_moves[pick][1]
    board[col] = row
  return board

def make_move_first_choice(board):
  """
  calculate the heuristic cost easily if we represent a board as an array where the index is the column and the value is the row.
  """
  h_to_beat = get_h_cost(board)
  for col in range(len(board)): 
    for row in range(len(board)):
      if board[col] == row:
        #We don't need to evaluate the current
        #position, we already know the h-value
        continue
       
      board_copy = list(board)
      #Move the queen to the new row
      board_copy[col] = row
      new_h_cost = get_h_cost(board_copy)
       
      #Return the first better (not best!) match you find
      if new_h_cost < h_to_beat:
        board[col] = row
        return board
  return board

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

def hillClimbing(num,randomStarts=100):
    board=[random.randint(0,num-1) for i in range(num)]
    while get_h_cost(board)!=0:
        for k in range(randomStarts):
            board=make_move_steepest_hill(board)
            if get_h_cost(board)==0:
                show(board,len(board))
                return
        board=[random.randint(0,num-1) for i in range(num)]
        

if __name__=='__main__':
    hillClimbing(100,randomStarts=50000)