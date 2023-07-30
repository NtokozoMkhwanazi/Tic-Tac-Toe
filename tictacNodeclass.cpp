#include <iostream>
#include <vector>
#include <memory>
#include <climits>
#include <cstdlib>
#include <ctime>

using namespace std;

class TicTacToeNode {
public:
    vector<vector<char>> Board;
    TicTacToeNode* Parent;

public:
    TicTacToeNode(vector<vector<char>> board, TicTacToeNode* parent = nullptr) : Board(board), Parent(parent) {
        emptyCells = 9; // Initialize with the total number of cells (3x3)
        for (const auto& row : board) {
            for (char cell : row) {
                if (cell != '#') {
                    emptyCells--;
                }
            }
        }
    }

    virtual ~TicTacToeNode() {} // Virtual destructor

    bool isSafe(int i, int j) const {
        return Board[i][j] == '#';
    }

    bool isEmpty(int i, int j) const {
        return Board[i][j] == '#';
    }

    bool symbol_Match(int i, int j, char symbol) const {
        return Board[i][j] == symbol;
    }

    bool isBoardFull() const {
        return emptyCells == 0;
    }

private:
    int emptyCells;
};

class State : public TicTacToeNode {
public:
    State(vector<vector<char>> board, TicTacToeNode* parent = nullptr) : TicTacToeNode(board, parent) {}
};

vector<vector<char>> getBoard() {
    vector<vector<char>> board(3, vector<char>(3, '#'));
    return board;
}
