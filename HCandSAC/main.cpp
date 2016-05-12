#include <iostream>
#include <vector>
#include <set>
#include <iterator>
#include <ctime>
#include <algorithm>
#include <random>
#include <numeric>
#include <string>
#include <fstream>
using namespace std;
clock_t TimesStart, TimesEnd; // For measuring the time of the executing of the algorithm
default_random_engine engine = default_random_engine{};

/* http://www.cnblogs.com/goagent/archive/2012/11/20/2767267.html */
size_t latestRandNum = 1;
/*设置随机数种子*/
void srandGLibC(size_t seed) {
	latestRandNum = (seed == 0 ? 1 : seed);
}
/*生成[0, 2147483647]之间的随机数*/
size_t randGLibC(void) {
	size_t randNum = ((latestRandNum * 1103515245) + 12345) & 0x7fffffff;
	latestRandNum = randNum;
	return (latestRandNum);
}

class Board {
public:
	/* constructor : initial the checkerboard with shuffling it */
	Board(size_t size, size_t pos_col, size_t pos_row) :size(size) {
		obstacle[0] = pos_col - 1;
		obstacle[1] = pos_row - 1;
		// temporarily do not consider the obstacle
		for (size_t i = 0; i < size; ++i)
			board.push_back(i);
		// random generate a checkerboard
		shuffle(board.begin(), board.end(), engine);
		// some works about initialization, do this work for fast code
		conflicts = 0;
		queensPerRow = getQueensPerRow();
		for (auto i = queensPerRow.begin(); i != queensPerRow.end(); ++i) {
			auto temp = (*i).size();
			if (temp > 1) {
				conflicts++;
				maybeConflictingQueensSet.insert((*i).begin(), (*i).end());
			}
			numberPerRow.push_back(temp);
		}
		queensPerUpdiag = getQueensPerUpdiag();
		for (auto i = queensPerUpdiag.begin(); i != queensPerUpdiag.end(); ++i) {
			auto temp = (*i).size();
			if (temp > 1) {
				conflicts++;
				maybeConflictingQueensSet.insert((*i).begin(), (*i).end());
			}
			numberPerUpdiag.push_back(temp);
		}
		queensPerDowndiag = getQueensPerDowndiag();
		for (auto i = queensPerDowndiag.begin(); i != queensPerDowndiag.end(); ++i) {
			auto temp = (*i).size();
			if (temp > 1) {
				conflicts++;
				maybeConflictingQueensSet.insert((*i).begin(), (*i).end());
			}
			numberPerDowndiag.push_back(temp);
		}
		// printBoard();
	};
	/* print the checkerboard */
	void printBoard() {
		for (size_t i = 0; i < size; ++i) {
			string row(size, '~');
			for (size_t j = 0; j < size; ++j) {
				if (*(board.begin() + j) == (size - i - 1))
					row[j] = 'Q';
				//if (i == (size - 1 - obstacle[1]))
				//	if (j == obstacle[0])
				//		row[j] = 'X';
			}
			cout << row << endl;
		}
	}
	vector<size_t> getBoard() {
		return board;
	}
	size_t cost() {
		return conflicts;
	}
	bool isSolution() {
		if (conflicts == 0)
			return true;
		else
			return false;
	}
	size_t getSize() {
		return size;
	}
	/* for the current checkerboard, random choose out the queen that has the maxinum conflicts
	return the random column */
	size_t getHighestConflictQueen() {
		vector<size_t> maxList;
		maxList.push_back(0);
		size_t maxConflicts = getNumConflicts(0);
		for (size_t i = 1; i < size; ++i) {
			size_t curConflicts = getNumConflicts(i);
			if (curConflicts > maxConflicts) {
				maxList.clear();
				maxList.push_back(i);
				maxConflicts = curConflicts;
			}
			else if (curConflicts == maxConflicts)
				maxList.push_back(i);
		}
		shuffle(maxList.begin(), maxList.end(), engine);
		return *maxList.begin();
	}
	/* as col=queenNum, random choose the position that has the minimun conflicts */
	size_t getLowestConflictMove(size_t queenNum) {
		vector<size_t> minList;
		minList.push_back(0);
		size_t minConflicts = getNumConflicts(queenNum, 0);
		for (size_t i = 1; i < size; ++i) {
			size_t curConflicts = getNumConflicts(queenNum, i);
			if (curConflicts < minConflicts) {
				minList.clear();
				minList.push_back(i);
				minConflicts = curConflicts;
			}
			else if (curConflicts == minConflicts)
				minList.push_back(i);
		}
		shuffle(minList.begin(), minList.end(), engine);
		return *minList.begin();
	}
	/* move alogrithm */
	void moveQueen(size_t queenNum, size_t queenPos) {
		// remove counts and conflicts due to old position
		size_t oldPos = *(board.begin() + queenNum);
		size_t oldUpdiag = oldPos + queenNum;
		size_t oldDowndiag = oldPos + size - queenNum - 1;
		(*(queensPerRow.begin() + oldPos)).erase(queenNum);
		auto it = numberPerRow.begin() + oldPos;
		(*it)--;
		if ((*it) == 1)
			conflicts--;
		(*(queensPerUpdiag.begin() + oldUpdiag)).erase(queenNum);
		it = numberPerUpdiag.begin() + oldUpdiag;
		(*it)--;
		if ((*it) == 1)
			conflicts--;
		(*(queensPerDowndiag.begin() + oldDowndiag)).erase(queenNum);
		it = numberPerDowndiag.begin() + oldDowndiag;
		(*it)--;
		if ((*it) == 1)
			conflicts--;

		*(board.begin() + queenNum) = queenPos;
		size_t queenUpdiag = queenNum + queenPos;
		size_t queenDowndiag = queenPos + size - queenNum - 1;

		// add counts and conflicts due to the new one
		auto is = queensPerRow.begin() + queenPos;
		(*is).insert(queenNum);
		it = numberPerRow.begin() + queenPos;
		(*it)++;
		if ((*it) == 2) {
			conflicts++;
			maybeConflictingQueensSet.insert((*is).begin(), (*is).end());
		}
		is = queensPerUpdiag.begin() + queenUpdiag;
		(*is).insert(queenNum);
		it = numberPerUpdiag.begin() + queenUpdiag;
		(*it)++;
		if ((*it) == 2) {
			conflicts++;
			maybeConflictingQueensSet.insert((*is).begin(), (*is).end());
		}
		is = queensPerDowndiag.begin() + queenDowndiag;
		(*is).insert(queenNum);
		it = numberPerDowndiag.begin() + queenDowndiag;
		(*it)++;
		if ((*it) == 2) {
			conflicts++;
			maybeConflictingQueensSet.insert((*is).begin(), (*is).end());
		}
	}
	/* exchange alogrithm */
	void exchangeQueen(size_t queenNum1, size_t queenNum2) {
		size_t queenPos1 = *(board.begin() + queenNum1);
		size_t queenPos2 = *(board.begin() + queenNum2);
		// remove counts and conflicts due to old position
		size_t oldUpdiag = queenPos1 + queenNum1;
		size_t oldDowndiag = queenPos1 + size - queenNum1 - 1;
		(*(queensPerUpdiag.begin() + oldUpdiag)).erase(queenNum1);
		auto it = numberPerUpdiag.begin() + oldUpdiag;
		(*it)--;
		if ((*it) == 1)
			conflicts--;
		(*(queensPerDowndiag.begin() + oldDowndiag)).erase(queenNum1);
		it = numberPerDowndiag.begin() + oldDowndiag;
		(*it)--;
		if ((*it) == 1)
			conflicts--;

		*(board.begin() + queenNum1) = queenPos2;
		size_t queenUpdiag = queenNum1 + queenPos2;
		size_t queenDowndiag = queenPos2 + size - queenNum1 - 1;

		// add counts and conflicts due to the new one
		auto is = queensPerUpdiag.begin() + queenUpdiag;
		(*is).insert(queenNum1);
		it = numberPerUpdiag.begin() + queenUpdiag;
		(*it)++;
		if ((*it) == 2) {
			conflicts++;
			maybeConflictingQueensSet.insert((*is).begin(), (*is).end());
		}
		is = queensPerDowndiag.begin() + queenDowndiag;
		(*is).insert(queenNum1);
		it = numberPerDowndiag.begin() + queenDowndiag;
		(*it)++;
		if ((*it) == 2) {
			conflicts++;
			maybeConflictingQueensSet.insert((*is).begin(), (*is).end());
		}
		// remove counts and conflicts due to old position
		oldUpdiag = queenPos2 + queenNum2;
		oldDowndiag = queenPos2 + size - queenNum2 - 1;
		(*(queensPerUpdiag.begin() + oldUpdiag)).erase(queenNum2);
		it = numberPerUpdiag.begin() + oldUpdiag;
		(*it)--;
		if ((*it) == 1)
			conflicts--;
		(*(queensPerDowndiag.begin() + oldDowndiag)).erase(queenNum2);
		it = numberPerDowndiag.begin() + oldDowndiag;
		(*it)--;
		if ((*it) == 1)
			conflicts--;

		*(board.begin() + queenNum2) = queenPos1;
		queenUpdiag = queenNum2 + queenPos1;
		queenDowndiag = queenPos1 + size - queenNum2 - 1;

		// add counts and conflicts due to the new one
		is = queensPerUpdiag.begin() + queenUpdiag;
		(*is).insert(queenNum2);
		it = numberPerUpdiag.begin() + queenUpdiag;
		(*it)++;
		if ((*it) == 2) {
			conflicts++;
			maybeConflictingQueensSet.insert((*is).begin(), (*is).end());
		}
		is = queensPerDowndiag.begin() + queenDowndiag;
		(*is).insert(queenNum2);
		it = numberPerDowndiag.begin() + queenDowndiag;
		(*it)++;
		if ((*it) == 2) {
			conflicts++;
			maybeConflictingQueensSet.insert((*is).begin(), (*is).end());
		}
	}
	/* random choose a queen that has conflcits from the maybeConflictintQueenSet */
	size_t getRandomQueen() {
		while (!maybeConflictingQueensSet.empty()) {
			size_t chooseIndex = randGLibC() % maybeConflictingQueensSet.size();
			auto it = maybeConflictingQueensSet.begin();
			for (size_t i = 0; i < chooseIndex; ++i)
				++it;
			size_t choose = *it;
			if (getNumConflicts(choose) > 0)
				return choose;
			else {
				// this queens has no conflicts, remove it from the possibilities
				maybeConflictingQueensSet.erase(choose);
			}
		}
		cerr << "we have conflicts, but i can't find out the conflicting queen ..." << endl;
		exit(-1);
	}
	size_t getRandomMove(size_t queenNum) {
		while (true) {
			size_t row = randGLibC() % size;
			if (row != (*(board.begin() + queenNum)))
				return row;
		}
	}
	/* the number of conflicts of queen that is in the position of (col, row) = (queenNum, queenPos) */
	size_t getNumConflicts(size_t queenNum) {
		size_t queenPos = *(board.begin() + queenNum);
		size_t cost = 0;
		cost += *(numberPerRow.begin() + queenPos);
		cost += *(numberPerUpdiag.begin() + queenNum + queenPos);
		cost += *(numberPerDowndiag.begin() + queenPos + size - queenNum - 1);
		// this three cost counted queenNum itself
		return cost - 3;
	}
	/* the number of conflicts of queen that is in the position of (col, row) = (queenNum, queenPos) */
	size_t getNumConflicts(size_t queenNum, size_t queenPos) {
		size_t cost = 0;
		cost += *(numberPerRow.begin() + queenPos);
		cost += *(numberPerUpdiag.begin() + queenNum + queenPos);
		cost += *(numberPerDowndiag.begin() + queenPos + size - queenNum - 1);
		if (queenPos == *(board.begin() + queenNum))
			cost -= 3;
		return cost;
	}
	~Board() {};
private:
	size_t size;							// the size of the checkerboard
	vector<size_t> board;					// the layout of the checkerboard
	size_t obstacle[2];						// the (column, row) of obstacle
	vector<set<size_t>> queensPerRow;		// the set() of each row
	vector<size_t> numberPerRow;			// number of previous set
	vector<set<size_t>> queensPerUpdiag;	// the set() of each up diagoanl
	vector<size_t> numberPerUpdiag;			// number of previous set
	vector<set<size_t>> queensPerDowndiag;	// the set() of each down diagonal
	vector<size_t> numberPerDowndiag;		// number of previous set
	size_t conflicts;						// the number of conflicts of each row, updiag, downdiag. max = size + 2*size-1 +2*size -1
	set<size_t> maybeConflictingQueensSet;	// the set of column which maybe have conflict
											/* calculate the set() of each row */
	vector<set<size_t>> getQueensPerRow() {
		vector<set<size_t>> vs(size, set<size_t>());
		for (size_t i = 0; i < size; ++i)
			(*(vs.begin() + *(board.begin() + i))).insert(i);
		return vs;
	}
	/* calculate the set() of each up diagonal */
	vector<set<size_t>> getQueensPerUpdiag() {
		vector<size_t> vi;
		for (size_t i = 0; i < size; ++i) {
			vi.push_back(*(board.begin() + i) + i);
		}
		vector<set<size_t>> vs(size * 2 - 1, set<size_t>());
		for (size_t i = 0; i < size; ++i)
			(*(vs.begin() + *(vi.begin() + i))).insert(i);
		return vs;
	}
	/* calculate the set() of each down diagonal */
	vector<set<size_t>> getQueensPerDowndiag() {
		vector<size_t> vi;
		for (size_t i = 0; i < size; ++i) {
			vi.push_back(*(board.begin() + i) + size - i - 1);
		}
		vector<set<size_t>> vs(size * 2 - 1, set<size_t>());
		for (size_t i = 0; i < size; ++i)
			(*(vs.begin() + *(vi.begin() + i))).insert(i);
		return vs;
	}
};
/* hill climbing algorithm */
void HillClimbingMove(Board&board) {
	size_t steps = 0;
	while (true) {
		size_t badQueen = board.getHighestConflictQueen();
		size_t goodMove = board.getLowestConflictMove(badQueen);
		vector<size_t> iv(board.getBoard());
		if (goodMove == (*(iv.begin() + badQueen)))
			// loacl minimum
			break;
		else
			board.moveQueen(badQueen, goodMove);
		steps++;
		//if (steps % 100 == 0) {
		//cout << steps << ":" << board.cost() << endl;
		//board.printBoard();
		//}
	}
}
/* read data from file */
void filesIn(string filename, int&size, int&obs_col, int&obs_row) {
	ifstream fm(filename, ios::in);
	if (!fm) {
		cerr << "fail to open " << filename << endl;
		exit(-1);
	}
	fm >> size >> obs_col >> obs_row;
	fm.close();
}
/* print the output file. Including the execute time, steps, path */
void DumpFile(string filename, Board checker) {
	ofstream fm(filename, ios::out);
	fm << ((double)(TimesEnd - TimesStart) / CLOCKS_PER_SEC) << "s" << endl;
	vector<size_t> iv = checker.getBoard();
	for (auto it = iv.begin(); it != iv.end(); ++it)
		fm << (*it) << endl;
}
/* hill climbing algorithm */
void HillClimbingExchange(Board&board) {
	size_t steps = 0;
	size_t conflicts;
	size_t size = board.getSize();
	while (true) {
		steps++;
		conflicts = board.cost();
		cout << steps << ":" << board.cost() << endl;
		size_t queenNum1 = randGLibC() % size;
		size_t queenNum2 = randGLibC() % size;
		cout << queenNum1 << " " << queenNum2 << endl;
		if (queenNum1 == queenNum2)
			continue;
		board.exchangeQueen(queenNum1, queenNum2);
		if (board.isSolution()) {
			TimesEnd = clock(); // end to count time
			DumpFile("output.txt", board);
			board.printBoard();
		}
		if (board.cost() > conflicts)
			board.exchangeQueen(queenNum2, queenNum1);
	}
}
/* simulate annealing accept probability */
bool Accept(size_t oldCost, size_t newCost, double time) {
	if (oldCost > newCost)
		return true;
	if (((double)rand() / RAND_MAX) <= exp(((int)oldCost - (int)newCost)*time))
		return true;
	else
		return false;
}
/* simulate annealing algorithm */
void SimulatedAnnealing(Board board) {
	size_t time = 0;
	double dt = 0.0005;
	while (true) {
		size_t queen = board.getRandomQueen();
		size_t currentCost = board.getNumConflicts(queen);
		size_t move = board.getRandomMove(queen);
		size_t newCost = board.getNumConflicts(queen, move);
		if (Accept(currentCost, newCost, time*dt)) {
			board.moveQueen(queen, move);
			if (board.isSolution()) {
				TimesEnd = clock(); // end to count time
				DumpFile("output.txt", board);
				//board.printBoard();
				return;
			}
		}
		time++;
		if (time % 100000 == 0)
			cout << board.cost() << endl;
	}
}
int main()
{
	size_t seed = (unsigned int)time(0) + randGLibC();
	int size, col, row;
	filesIn("input.txt", size, col, row);
	Board board(size, col, row);
	TimesStart = clock(); // start to count time
	SimulatedAnnealing(board);
	//for (size_t i = 0; i < 10000; ++i) {
	//	Board checker(size, col, row);
	//	HillClimbingMove(checker);
	//	if (checker.isSolution()) {
	//		TimesEnd = clock(); // end to count time
	//		DumpFile("output.txt",checker);
	//		//checker.printBoard();
	//		break;
	//	}
	//	cout << endl;
	//}
	//getchar();
	return 0;
}