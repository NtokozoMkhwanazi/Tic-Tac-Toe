#include <iostream>
#include <vector>
#include<climits>

using namespace std;

class TicTacToeNode {
public:
    vector<vector<char>> Board;
    TicTacToeNode* Parent;

public:
    TicTacToeNode(vector<vector<char>> board, TicTacToeNode* parent = nullptr) : Board(board), Parent(parent) {}

    virtual ~TicTacToeNode() {}  // Virtual destructor

    bool isSafe(int i, int j) const {
        return Board[i][j] == '#';
    }

    bool isEmpty(int i, int j) const {
        return Board[i][j] == '#';
    }

    bool symbol_Match(int i, int j, char AgentSymbol) const {
        return Board[i][j] == AgentSymbol;
    }
};


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

vector<vector<char>> getBoard() {
    vector<vector<char>> board(3, vector<char>(3, '#'));
    return board;
}

class State : public TicTacToeNode {
public:
    int status;

    State(vector<vector<char>> board, TicTacToeNode* parent = nullptr) : TicTacToeNode(board, parent), status(0) {
        for (int i = 0; i < board.size(); i++) {
            for (int j = 0; j < board.size(); j++) {
                if (board[i][j] != '#') {
                    status++;
                }
            }
        }
    }
};
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
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] == '#') {
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
        return 9; // Block opponent's win
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
            }
        }
    }

    return weight;
}






State* AgentMove(State* state, int i, int j, char AgentSymbol) {
    
    if (state->isSafe(j + 1, i) && state->isEmpty(j + 1, i)) {
        
        vector<vector<char>> newBoard = state->Board;
        newBoard[j + 1][i] = AgentSymbol;
        
        State* newState = new State(newBoard, state);
        
        int moveWeight = evaluateBoard(newState->Board, AgentSymbol);
    	// Add randomness to non-winning moves
    	if (moveWeight < 10) {
    	    moveWeight += rand() % 3; // Random value between 0 and 2
    	}
    	newState->Parent = state;
    
    	return newState;
    
    } else if (state->isSafe(j - 1, i) && state->isEmpty(j - 1, i)) {
    
        vector<vector<char>> newBoard = state->Board;
        newBoard[j - 1][i] = AgentSymbol;
        
        State* newState = new State(newBoard, state);
       	
       	int moveWeight = evaluateBoard(newState->Board, AgentSymbol);
    	// Add randomness to non-winning moves
    	if (moveWeight < 10) {
        	moveWeight += rand() % 3; // Random value between 0 and 2
    	}
    	newState->Parent = state;
    
    	return newState;
    
    } else if (state->isSafe(j, i + 1) && state->isEmpty(j, i + 1)) {
    
        vector<vector<char>> newBoard = state->Board;
        newBoard[j][i + 1] = AgentSymbol;
    
        State* newState = new State(newBoard, state);
        
        int moveWeight = evaluateBoard(newState->Board, AgentSymbol);
    	// Add randomness to non-winning moves
    	if (moveWeight < 10) {
        	moveWeight += rand() % 3; // Random value between 0 and 2
    	}
    	newState->Parent = state;
    
    	return newState;
    
    } else if (state->isSafe(j, i - 1) && state->isEmpty(j, i - 1)) {
    
        vector<vector<char>> newBoard = state->Board;
        newBoard[j][i - 1] = AgentSymbol;
    
        State* newState = new State(newBoard, state);
        int moveWeight = evaluateBoard(newState->Board, AgentSymbol);
    	// Add randomness to non-winning moves  	
    	if (moveWeight < 10) {
        	moveWeight += rand() % 3; // Random value between 0 and 2
    	}
    	newState->Parent = state;
    
    	return newState;
    }   
    else if (state->isSafe(i, j) && state->isEmpty(i, j)) {
    
        vector<vector<char>> newBoard = state->Board;
        newBoard[i][j] = AgentSymbol;
        State* newState = new State(newBoard, state);
    
        int moveWeight = evaluateBoard(newState->Board, AgentSymbol);
        // Add randomness to non-winning moves
        if (moveWeight < 10) {
            moveWeight += rand() % 3; // Random value between 0 and 2
        }
        newState->Parent = state;
        return newState;
    }
   
   return nullptr; 
}

// ... (previous code)

// Minimax function with alpha-beta pruning
int minimax(State* state, char AgentSymbol, int depth, int alpha, int beta, bool isMaximizing) {
    // Evaluate the board at the terminal nodes (leaf nodes)
    char userSymbol = (AgentSymbol == 'X') ? 'O' : 'X';
    int score = evaluateBoard(state->Board, AgentSymbol);

    // Check if the game is over or maximum depth is reached
    if (score == 10 || score == 9 || depth == 0 || checkDraw(state->Board)) {
        return score;
    }

    if (isMaximizing) {
        int bestScore = INT_MIN;
        for (int i = 0; i < state->Board.size(); i++) {
            for (int j = 0; j < state->Board.size(); j++) {
                if (state->isSafe(j, i) && state->isEmpty(j, i)) {
                    vector<vector<char>> newBoard = state->Board;
                    newBoard[j][i] = AgentSymbol;
                    State* newState = new State(newBoard, state);
                    int score = minimax(newState, AgentSymbol, depth - 1, alpha, beta, false);
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
                    newBoard[j][i] = userSymbol;
                    State* newState = new State(newBoard, state);
                    int score = minimax(newState, AgentSymbol, depth - 1, alpha, beta, true);
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

State* AgentMove(State* state, char AgentSymbol) {
    int bestScore = INT_MIN;
    State* bestMove = nullptr;
    for (int i = 0; i < state->Board.size(); i++) {
        for (int j = 0; j < state->Board.size(); j++) {
            if (state->isSafe(j, i) && state->isEmpty(j, i)) {
                vector<vector<char>> newBoard = state->Board;
                newBoard[j][i] = AgentSymbol;
                State* newState = new State(newBoard, state);
                int score = minimax(newState, AgentSymbol, 5, INT_MIN, INT_MAX, false);
                if (score > bestScore) {
                    bestScore = score;
                    bestMove = newState;
                }
            }
        }
    }
    return bestMove;
}

vector<vector<char>> userPlay(vector<vector<char>>& Board, int x, int y, char userSymbol) {
    Board[x][y] = userSymbol;
    return Board;
}


int main() {
    vector<vector<char>> Board;
    Board = getBoard();
    char agentSymbol = 'X'; // Switch the symbols for the user and AI
    char userSymbol = 'O'; 

    TicTacToeNode* TTT = new TicTacToeNode(Board);

    int x, y;

    while (true) {
        print(TTT->Board);

        // User's turn
        cout << "Your turn. Enter row (0-2): ";
        cin >> x;
        cout << "Enter column (0-2): ";
        cin >> y;

        if (x < 0 || x > 2 || y < 0 || y > 2 || !TTT->isEmpty(x, y)) {
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
    
		State* state = dynamic_cast<State*>(TTT);
		State* newState = AgentMove(state, agentSymbol);
		if (newState) {
    		TTT->Board = newState->Board;
    		delete newState;
		}

	}
	
    return 0;
}


