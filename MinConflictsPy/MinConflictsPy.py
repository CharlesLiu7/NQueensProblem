"""
code from https://gist.github.com/747203
"""
import sys
import random

def nqueens(nr):
    """
    minimum conflict algorithm
    the initial state is [0, 1, 2, 3, ...]
    """
    show(min_conflicts(list(range(nr)),nr),nr)

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
    print(soln)
    for i in range(nr):
        row=['~']*nr
        for col in range(nr):
            if soln[col]==nr-1-i:
                row[col]='Q'
        print(' '.join(row))

def min_conflicts(soln,nr,iters=1000):
    def random_pos(li,filt):
        """
        choose a position from li[] with the filter function flit()
        """
        return random.choice([i for i in range(nr) if filt(li[i])])

    for k in range(iters):
        confs=find_conflicts(soln,nr)
        if sum(confs)==0:
            return soln
        # print(soln)
        col=random_pos(confs,lambda elt:elt>0)
        vconfs=[hits(soln,nr,col,row) for row in range(nr)]
        soln[col]=random_pos(vconfs,lambda elt: elt==min(vconfs))
    raise Exception("Incomplete solution")

def find_conflicts(soln,nr):
    """
    for the queue soln[nr]
    return the conflicts of every row
    """
    return [hits(soln,nr,col,soln[col]) for col in range(nr)]

def hits(soln,nr,col,row):
    """
    for the soln[nr], col, row
    find the exact conflicts of this row
    """
    total=0
    for i in range(nr):
        if i==col:
            continue
        # it can't exist the conflicts because of same row
        if soln[i]==row or abs(i-col)==abs(soln[i]-row):
            total+=1
    return total

if __name__=='__main__':
    nqueens(100)