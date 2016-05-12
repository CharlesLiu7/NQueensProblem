#include <iostream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <string>
#include <fstream>
#include <random>
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
		//for (int i = 0; i < size; ++i)
		//	for (int j = 0; j < size; ++j)
		//		cout << hits(i, j) << endl;
		//cout << endl;
		//printMatrix();
		//cout << hits(Matrix) << endl;
	}
	/* return the Matrix */
	vector<int> getMatrix() {
		return Matrix;
	}
	/* return the Matrix size */
	int getSize() {
		return size;
	}
	/* execut the simulated annealing algorithm */
	void Annealing() {
		double temp = size*size;
		double annealRate = 0.95;
		int hitsBoard = hits(Matrix);
		int steps = 0; // just count the number of cycle, use it to break loop if necessary
		while (hitsBoard > 0) {
			Matrix = makeAnnealingMove(hitsBoard, temp);
			hitsBoard = hits(Matrix);
			temp = max(temp*annealRate, 0.01);
			steps++;
		}
		TimesEnd = clock(); // end to count time
							// printMatrix();
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
	int hits(vector<int> board) {
		int total = 0;
		const int max_conflict = 65535;
		vector<int>::iterator it = board.begin();
		for (int i = 0; i < size; ++i)
			for (int j = i + 1; j < size; ++j) {
				if (*(it + i) == *(it + j)) {
					if (*(it + i) == obstacle[1] && i < obstacle[0] && obstacle[0] < j)
						continue;
					total++;
				}
				if ((j - i) == abs(*(it + i) - *(it + j))) {
					if ((j - i) == abs(*(it + i) - obstacle[1]) && i < obstacle[0] && obstacle[0] < j
						&& min(*(it + i), *(it + j)) < obstacle[1] && obstacle[1] < max(*(it + i), *(it + j)))
						continue;
					total++;
				}
			}
		return total;
	}
	/* to simulated annealing the next step */
	vector<int> makeAnnealingMove(int hitsBoard, double temp) {
		vector<int> MatrixCopy(Matrix);
		bool foundMove = false;
		while (!foundMove) {
			MatrixCopy = Matrix;
			int new_col = rand() % size, new_row = rand() % size;
			if (new_col == obstacle[0] && new_row == obstacle[1])
				continue;
			*(MatrixCopy.begin() + new_col) = new_row;
			int newHitsBoard = hits(MatrixCopy);
			if (newHitsBoard < hitsBoard)
				foundMove = true;
			else {
				int deltaE = hitsBoard - newHitsBoard;
				double acceptProbabily = min(1.0, exp(deltaE / temp));
				foundMove = (((double)rand() / RAND_MAX) <= acceptProbabily);
			}
		}
		return MatrixCopy;
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
void DumpFile(string filename, NQueens queens) {
	ofstream fm(filename, ios::out);
	fm << queens.getSize() << endl;
	fm << ((double)(TimesEnd - TimesStart) / CLOCKS_PER_SEC) << "ms" << endl;
}
int main()
{
	int size, col, row;
	filesIn("input.txt", size, col, row);
	srand((unsigned)time(NULL));
	NQueens queens(size, col, row);
	TimesStart = clock(); // start to count time
	queens.Annealing();
	//queens.printMatrix();
	DumpFile("output.txt", queens);
	//vector<int> vi = queens.getMatrix();
	//for (vector<int>::iterator it = vi.begin(); it != vi.end(); ++it)
	//	cout << (*it) << endl;
	// getchar();
	return 0;
}