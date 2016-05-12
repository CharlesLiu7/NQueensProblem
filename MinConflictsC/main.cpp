#include <iostream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <string>
#include <fstream>
#include <chrono>	// chrono random
#include <numeric>	// accumulate
#include <random>	// default_random_engine
#include <cassert>
#include <ctime>
using namespace std;
clock_t TimesStart, TimesEnd; // For measuring the time of the executing of the algorithm
class NQueens {
public:
	/* constructor */
	NQueens(int size, int pos_col, int pos_row) :size(size) {
		obstacle[0] = pos_col - 1;
		obstacle[1] = pos_row - 1;
		initialMatrix();
		//test for Matrix
		printMatrix();
		for (int i = 0; i < size; ++i)
			for (int j = 0; j < size; ++j)
				cout << hits(i, j) << endl;
		cout << endl;
	}
	/* return the Matrix */
	vector<int> getMatrix() {
		return Matrix;
	}
	vector<int> status;
	void execMinConflict(int itersLimits) {
		for (int k = 0; k < itersLimits; ++k) {
			status = findConflict();
			if (accumulate(status.begin(), status.end(), 0) == 0)
				return;
			int col = randomChoice(status);
			vector<int> colConflicts;
			int tempMin = 65535, temp;
			for (int i = 0; i < size; ++i) {
				temp = hits(col, i);
				if (temp < tempMin)
					tempMin = temp;
				colConflicts.push_back(temp);
			}
			*(Matrix.begin() + col) = randomChoice(colConflicts, tempMin);
			//printMatrix();
			//cout << k << endl << endl;
		}
		assert("k overflow!");
	}
	~NQueens() {
	}
	/* print the checkerboard */
	void printMatrix() {
		for (int i = 0; i < size; ++i) {
			string row(size, '~');
			for (int j = 0; j < size; ++j) {
				if (*(Matrix.begin() + j) == (size - i - 1))
					row[j] = 'Q';
				if (i == (size - 1 - obstacle[1]))
					if (j == obstacle[0])
						row[j] = 'X';
			}
			cout << row << endl;
		}
	}
private:
	vector<int> Matrix;	// the Matrix of checkerboard
	int size;			// the size of Matrix
	int obstacle[2];	// the obstacle chessman
						/* initial Matrix for constructor
						* read the Matrix-size and the point of obstacle from the file
						* initial a diagonal queens of the size
						Matrix[4] for example
						[2, 0, 3, 1]
						row
						^
						| ~ ~ Q ~
						| Q ~ ~ ~
						| ~ ~ ~ Q
						| ~ Q ~ ~
						----------> col
						maps [0, 2] [1, 0] [2, 3] ,[3, 1]*/
	void initialMatrix() {
		int j;
		if (obstacle[1] > obstacle[0])
			j = obstacle[1] - obstacle[0] - 1;
		else
			j = size + obstacle[1] - obstacle[0] - 1;
		for (int i = 0; i < size; ++i) {
			Matrix.push_back(j);
			j++;
			j %= size;
		}
	}
	/* find the number of conflicts of this row */
	int hits(const int col, const int row) {
		int total = 0;
		const int max_conflict = 65535;
		vector<int>::iterator it = Matrix.begin();
		for (int i = 0; i < size; ++i) {
			// it can't exist the conflicts because of same row
			if (i == col) {
				if (col == obstacle[0] && row == obstacle[1])
					return max_conflict;
				continue;
			}
			// the same row
			if ((*(it + i)) == row) {
				if (obstacle[1] == row)
					if (min(i, col) < obstacle[0] && obstacle[0] < max(i, col))
						continue;
				total++;
			}
			// the same diagonal
			if (abs(i - col) == abs((*(it + i)) - row)) {
				if (abs(obstacle[0] - col) == abs(obstacle[1] - row))
					if (min(i, col) < obstacle[0] && obstacle[0] < max(i, col) && min(*(it + i), *(it + col)) < obstacle[1] && obstacle[1] < max(*(it + i), *(it + col)))
						continue;
				total++;
			}
		}
		return total;
	}
	/* random choice a vector in vc with the constraint function fliter() */
	int randomChoice(vector<int> vc) {
		vector<int> temp;
		int i = 0;
		for (vector<int>::iterator it = vc.begin(); it != vc.end(); ++it, ++i)
			if ((*it) > 0)
				temp.push_back(i);
		unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
		shuffle(temp.begin(), temp.end(), default_random_engine(seed));
		return *temp.begin();
	}
	int randomChoice(vector<int> vc, int minNum) {
		vector<int> temp;
		int i = 0;
		for (vector<int>::iterator it = vc.begin(); it != vc.end(); ++it, ++i)
			if ((*it) == minNum)
				temp.push_back(i);
		unsigned seed = (unsigned)std::chrono::system_clock::now().time_since_epoch().count();
		shuffle(temp.begin(), temp.end(), default_random_engine(seed));
		return *temp.begin();
	}
	/* find the conflict of the current checkerboard */
	vector<int> findConflict() {
		vector<int> temp;
		vector<int>::iterator it = Matrix.begin();
		for (int i = 0; i < size; ++i) {
			temp.push_back(hits(i, *(it + i)));
		}
		return temp;
	}
};
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
void DumpFile(string filename) {
	ofstream fm(filename, ios::out);
	fm << ((double)(TimesEnd - TimesStart) / CLOCKS_PER_SEC) << "ms" << endl;
}
int main()
{
	int size, col, row;
	filesIn("input.txt", size, col, row);
	NQueens queens(size, col, row);
	TimesStart = clock(); // start to count time
	queens.execMinConflict(10000);
	TimesEnd = clock(); // end to count time
						//queens.printMatrix();
	DumpFile("output.txt");
	//vector<int> vi = queens.getMatrix();
	//for (vector<int>::iterator it = vi.begin(); it != vi.end(); ++it)
	//	cout << (*it) << endl;
	getchar();
	return 0;
}