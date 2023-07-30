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

void print(vector<vector<char>> grid) {
    for (int j = 0; j < 3; j++) {
        cout << "  -------" << endl;
        cout << " |";
        for (int i = 0; i < 3; i++) {
            cout << grid[j][i] << "|";
        }
        cout << endl;
    }
    cout << "  -------" << endl;
}

bool checkWin(const vector<vector<char>>& board, char symbol) {
    for (int i = 0; i < 3; i++) {
        if (board[i][0] == symbol && board[i][1] == symbol && board[i][2] == symbol) {
            return true; // Horizontal win
        }
        if (board[0][i] == symbol && board[1][i] == symbol && board[2][i] == symbol) {
            return true; // Vertical win
        }
    }
    if (board[0][0] == symbol && board[1][1] == symbol && board[2][2] == symbol) {
        return true; // Diagonal win (top-left to bottom-right)
    }
    if (board[0][2] == symbol && board[1][1] == symbol && board[2][0] == symbol) {
        return true; // Diagonal win (top-right to bottom-left)
    }
    return false;
}

bool checkDraw(const vector<vector<char>>& board) {
    for (const auto& row : board) {
        for (char cell : row) {
            if (cell == '#') {
                return false; // There is an empty cell, game is not a draw yet
            }
        }
    }
    return true; // All cells are filled, game is a draw
}

int evaluateBoard(const vector<vector<char>>& board, char symbol) {
    // Check if the given symbol has won
    if (checkWin(board, symbol)) {
        return 10; // AI wins
    }

    // Check if the opponent (user) can win in the next move
    char opponentSymbol = (symbol == 'X') ? 'O' : 'X';
    if (checkWin(board, opponentSymbol)) {
        return -10; // Block opponent's win
    }

    // Calculate weights for non-winning moves based on AI symbols in rows, columns, and diagonals
    int weight = 0;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] == symbol) {
                // Increase weight for AI symbols in a row, column, or diagonal
                if (i == j || i + j == 2) {
                    weight += 2; // Diagonal
                } else {
                    weight += 1; // Row or column
                }
            } else if (board[i][j] == opponentSymbol) {
                // Decrease weight for opponent symbols to block their moves
                if (i == j || i + j == 2) {
                    weight -= 2; // Diagonal
                } else {
                    weight -= 1; // Row or column
                }
            }
        }
    }

    return weight;
}

int minimax(State* state, char AgentSymbol, int depth, int alpha, int beta, bool isMaximizing) {
    // Evaluate the board at the terminal nodes (leaf nodes)
    char userSymbol = (AgentSymbol == 'X') ? 'O' : 'X';
    int score = evaluateBoard(state->Board, AgentSymbol);

    // Check if the game is over or maximum depth is reached
    if (score == 10 || score == -10 || depth == 0 || checkDraw(state->Board)) {
        return score;
    }

    if (isMaximizing) {
        int bestScore = INT_MIN;
        for (int i = 0; i < state->Board.size(); i++) {
            for (int j = 0; j < state->Board.size(); j++) {
                if (state->isSafe(j, i) && state->isEmpty(j, i)) {
                    vector<vector<char>> newBoard = state->Board;
                    newBoard[i][j] = AgentSymbol;
                    unique_ptr<State> newState = make_unique<State>(newBoard, state);
                    int score = minimax(newState.get(), AgentSymbol, depth - 1, alpha, beta, false);
                    bestScore = max(bestScore, score);
                    alpha = max(alpha, bestScore);
                    if (beta <= alpha) {
                        // Beta cut-off
                        break;
                    }
                }
            }
        }
        return bestScore;
    } else {
        int bestScore = INT_MAX;
        for (int i = 0; i < state->Board.size(); i++) {
            for (int j = 0; j < state->Board.size(); j++) {
                if (state->isSafe(j, i) && state->isEmpty(j, i)) {
                    vector<vector<char>> newBoard = state->Board;
                    newBoard[i][j] = userSymbol;
                    unique_ptr<State> newState = make_unique<State>(newBoard, state);
                    int score = minimax(newState.get(), AgentSymbol, depth - 1, alpha, beta, true);
                    bestScore = min(bestScore, score);
                    beta = min(beta, bestScore);
                    if (beta <= alpha) {
                        // Alpha cut-off
                        break;
                    }
                }
            }
        }
        return bestScore;
    }
}

unique_ptr<State> AgentMove(State* state, char AgentSymbol) {
    int boardSize = state->Board.size();
    int maxDepth = (boardSize == 3) ? 5 : 3; // Adjust depth based on board size

    int bestScore = INT_MIN;
    unique_ptr<State> bestMove = nullptr;
    char userSymbol = (AgentSymbol == 'X') ? 'O' : 'X';

    for (int i = 0; i < boardSize; i++) {
        for (int j = 0; j < boardSize; j++) {
            if (state->isSafe(j, i) && state->isEmpty(j, i)) {
                vector<vector<char>> newBoard = state->Board;
                newBoard[i][j] = AgentSymbol;
                unique_ptr<State> newState = make_unique<State>(newBoard, state);
                int score = minimax(newState.get(), AgentSymbol, maxDepth, INT_MIN, INT_MAX, false);
                if (score > bestScore) {
                    bestScore = score;
                    bestMove = move(newState);
                }
            }
        }
    }

    return bestMove;
}

vector<vector<char>> userPlay(vector<vector<char>>& Board, int x, int y, char userSymbol) {
    Board[y][x] = userSymbol; // Swap x and y here
    return Board;
}

int main() {
    srand(time(NULL)); // Seed the random number generator

    vector<vector<char>> Board(3, vector<char>(3, '#'));
    unique_ptr<TicTacToeNode> TTT = make_unique<TicTacToeNode>(Board);
    char agentSymbol = 'X'; // Switch the symbols for the user and AI
    char userSymbol = 'O';

    int x, y;

    while (true) {
        print(TTT->Board);

        // User's turn
        cout << "Your turn. Enter row (0-2): ";
        cin >> y;
        cout << "Enter column (0-2): ";
        cin >> x;

        if (x < 0 || x >= 3 || y < 0 || y >= 3 || !TTT->isEmpty(y, x)) {
            cout << "Invalid move. Try again." << endl;
            continue;
        }

        TTT->Board = userPlay(TTT->Board, x, y, userSymbol);

        if (checkWin(TTT->Board, userSymbol)) {
            print(TTT->Board);
            cout << "Congratulations! You win!" << endl;
            break;
        }

        if (checkDraw(TTT->Board)) {
            print(TTT->Board);
            cout << "It's a draw!" << endl;
            break;
        }

        // AI's turn
        State* state = dynamic_cast<State*>(TTT.get());
        if (!state->isBoardFull() && !checkWin(TTT->Board, agentSymbol)) {
            unique_ptr<State> newState = AgentMove(state, agentSymbol);
            if (newState) {
                TTT->Board = newState->Board;
            }
        }
    }

    return 0;
}

