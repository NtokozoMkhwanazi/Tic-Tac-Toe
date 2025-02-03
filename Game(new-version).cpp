#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <memory>
#include <cstdlib>
#include <ctime>
#include <climits>

using namespace std;

// Function prototypes
bool checkWin(const vector<vector<char>>& board, char symbol);
bool checkDraw(const vector<vector<char>>& board);
vector<vector<char>> getBoard();
bool isBoardFull(const vector<vector<char>>& board);
vector<vector<char>> userPlay(const vector<vector<char>>& Board, int x, int y, char userSymbol);
class TicTacToeNode;
class State;
int minimax(State* state, char AgentSymbol, int depth, int alpha, int beta, bool isMaximizing);

bool checkWin(const vector<vector<char>>& board, char symbol) {
    for (int i = 0; i < 3; i++) {
        if (board[i][0] == symbol && board[i][1] == symbol && board[i][2] == symbol) {
            return true;
        }
        if (board[0][i] == symbol && board[1][i] == symbol && board[2][i] == symbol) {
            return true;
        }
    }
    if (board[0][0] == symbol && board[1][1] == symbol && board[2][2] == symbol) {
        return true;
    }
    if (board[0][2] == symbol && board[1][1] == symbol && board[2][0] == symbol) {
        return true;
    }
    return false;
}

bool checkDraw(const vector<vector<char>>& board) {
    for (const auto& row : board) {
        for (char cell : row) {
            if (cell == '#') {
                return false;
            }
        }
    }
    return true;
}

vector<vector<char>> getBoard() {
    vector<vector<char>> board(3, vector<char>(3, '#'));
    return board;
}

bool isBoardFull(const vector<vector<char>>& board) {
    for (const auto& row : board) {
        for (char cell : row) {
            if (cell == '#') {
                return false; 
            }
        }
    }
    return true; 
}

vector<vector<char>> userPlay(const vector<vector<char>>& Board, int x, int y, char userSymbol) {
    vector<vector<char>> newBoard = Board;
    newBoard[y][x] = userSymbol;
    return newBoard;
}

// Minimax Evaluation and AI Logic
class TicTacToeNode {
public:
    vector<vector<char>> Board;
    TicTacToeNode* Parent;
    int emptyCells;

public:
    TicTacToeNode(vector<vector<char>> board, TicTacToeNode* parent = nullptr) : Board(board), Parent(parent) {
        updateEmptyCells();
    }

    virtual unique_ptr<TicTacToeNode> makeAIMove(char AgentSymbol) {
        return nullptr;
    }

    virtual ~TicTacToeNode() {}

    bool isSafe(int i, int j) const {
        return i >= 0 && j >= 0 && i < Board.size() && j < Board[0].size() && Board[i][j] == '#';
    }

    void updateEmptyCells() {
        emptyCells = 0;
        for (const auto& row : Board) {
            for (char cell : row) {
                if (cell == '#') {
                    emptyCells++;
                }
            }
        }
    }
};

class State : public TicTacToeNode {
public:
    State(vector<vector<char>> board, TicTacToeNode* parent = nullptr) : TicTacToeNode(board, parent) {}

    unique_ptr<TicTacToeNode> makeAIMove(char AgentSymbol) {
        int maxDepth = (Board.size() == 3) ? 5 : 3;
        int bestScore = INT_MIN;
        unique_ptr<TicTacToeNode> bestMove = nullptr;

        for (int i = 0; i < Board.size(); i++) {
            for (int j = 0; j < Board[i].size(); j++) {
                if (isSafe(i, j) && Board[i][j] == '#') {
                    vector<vector<char>> newBoard = Board;
                    newBoard[i][j] = AgentSymbol;
                    unique_ptr<TicTacToeNode> newState = make_unique<State>(newBoard, this);
                    int score = minimax(static_cast<State*>(newState.get()), AgentSymbol, maxDepth, INT_MIN, INT_MAX, false);
                    if (score > bestScore) {
                        bestScore = score;
                        bestMove = move(newState);
                    }
                }
            }
        }
        return bestMove;
    }

    int evaluateBoard(const vector<vector<char>>& board, char symbol) const {
        if (checkWin(board, symbol)) {
            return 10;
        }
        char opponentSymbol = (symbol == 'X') ? 'O' : 'X';
        if (checkWin(board, opponentSymbol)) {
            return -10;
        }

        int weight = 0;
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (board[i][j] == symbol) {
                    if (i == j || i + j == 2) {
                        weight += 2; 
                    } else {
                        weight += 1; 
                    }
                } else if (board[i][j] == opponentSymbol) {
                    if (i == j || i + j == 2) {
                        weight -= 2; 
                    } else {
                        weight -= 1; 
                    }
                }
            }
        }
        return weight;
    }

    int minimax(State* state, char AgentSymbol, int depth, int alpha, int beta, bool isMaximizing) const {
        char userSymbol = (AgentSymbol == 'X') ? 'O' : 'X';
        int score = evaluateBoard(state->Board, AgentSymbol);

        if (score == 10 || score == -10 || depth == 0 || checkDraw(state->Board)) {
            return score;
        }

        if (isMaximizing) {
            int bestScore = INT_MIN;
            for (int i = 0; i < state->Board.size(); i++) {
                for (int j = 0; j < state->Board[i].size(); j++) {
                    if (state->isSafe(i, j) && state->Board[i][j] == '#') {
                        vector<vector<char>> newBoard = state->Board;
                        newBoard[i][j] = AgentSymbol;
                        unique_ptr<State> newState = make_unique<State>(newBoard, state);
                        int score = minimax(newState.get(), AgentSymbol, depth - 1, alpha, beta, false);
                        bestScore = max(bestScore, score);
                        alpha = max(alpha, bestScore);
                        if (beta <= alpha) {
                            break;
                        }
                    }
                }
            }
            return bestScore;
        } else {
            int bestScore = INT_MAX;
            for (int i = 0; i < state->Board.size(); i++) {
                for (int j = 0; j < state->Board[i].size(); j++) {
                    if (state->isSafe(i, j) && state->Board[i][j] == '#') {
                        vector<vector<char>> newBoard = state->Board;
                        newBoard[i][j] = userSymbol;
                        unique_ptr<State> newState = make_unique<State>(newBoard, state);
                        int score = minimax(newState.get(), AgentSymbol, depth - 1, alpha, beta, true);
                        bestScore = min(bestScore, score);
                        beta = min(beta, bestScore);
                        if (beta <= alpha) {
                            break;
                        }
                    }
                }
            }
            return bestScore;
        }
    }
};

int main() {
    srand(time(NULL));

    sf::Font font;
    if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf")) {
        return 1;
    }

    sf::RenderWindow window(sf::VideoMode(400, 400), "Tic Tac Toe");
    window.setFramerateLimit(60);

    vector<vector<char>> Board(3, vector<char>(3, '#'));
    char userSymbol = 'X';
    char other_userSymbol = 'O';
    const int cellSize = 133;
    bool gameOver = false;
    bool isPlayerOneTurn = true;
    bool modeSelected = false;
    bool mode = false; // False = Player vs Player, True = Player vs AI
    string agent = "";
    
    sf::Text gameText;
    gameText.setFont(font);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (!modeSelected) {
                // Handle menu input
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Num1) {
                        mode = false; // Player vs Player
                        modeSelected = true;
                    } else if (event.key.code == sf::Keyboard::Num2) {
                        mode = true; // Player vs AI
                        modeSelected = true;
                    }
                }
            } else if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    if (!gameOver) {
                        int x = event.mouseButton.x / cellSize;
                        int y = event.mouseButton.y / cellSize;

                        if (Board[y][x] == '#') {
                            
                            Board = userPlay(Board, x, y, isPlayerOneTurn ? userSymbol : other_userSymbol);

                            if (checkWin(Board, userSymbol)) {
                                gameOver = true;
                                agent = "Player X wins!";
                            } else if (checkWin(Board, other_userSymbol)) {
                                gameOver = true;
                                agent = "Player O wins!";
                            } else if (checkDraw(Board)) {
                                gameOver = true;
                                agent = "It's a draw!";
                            }

                            if (!gameOver) {
                                if (mode) {
                                    // AI move
                                    unique_ptr<State> TTT = make_unique<State>(Board);
                                    unique_ptr<TicTacToeNode> move = TTT->makeAIMove(other_userSymbol);
                                    Board = move->Board;
				 
                                    if (checkWin(Board, other_userSymbol)) {
                                        gameOver = true;
                                        agent = "Agent wins!";
                                    
                                    } else if (checkDraw(Board)) {
                                        gameOver = true;
                                        agent = "It's a draw!";
                                    }
                                } else {
                                    isPlayerOneTurn = !isPlayerOneTurn;
                                }
                            }
                        }
                    }
                }
            }
        }

        window.clear();

        if (!modeSelected) {
            // Draw the menu
            sf::Text menuText;
            menuText.setFont(font);
            menuText.setCharacterSize(20);
            menuText.setFillColor(sf::Color::White);
            menuText.setString("Select Mode:\n1. Player vs Player\n2. Player vs AI");
            menuText.setPosition(100.f, 150.f);
            window.draw(menuText);
        } else {
            // Draw the board grid and symbols
            for (int i = 1; i < 3; ++i) {
                sf::Vertex line[] = {
                    sf::Vertex(sf::Vector2f(i * cellSize, 0), sf::Color::White),
                    sf::Vertex(sf::Vector2f(i * cellSize, window.getSize().y), sf::Color::White)
                };
                window.draw(line, 2, sf::Lines);
                sf::Vertex line2[] = {
                    sf::Vertex(sf::Vector2f(0, i * cellSize), sf::Color::White),
                    sf::Vertex(sf::Vector2f(window.getSize().x, i * cellSize), sf::Color::White)
                };
                window.draw(line2, 2, sf::Lines);
            }

            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    if (Board[i][j] != '#') {
                        sf::Text symbolText;
                        symbolText.setFont(font);
                        symbolText.setString(Board[i][j]);
                        symbolText.setCharacterSize(60);
                        symbolText.setFillColor(sf::Color::White);
                        symbolText.setPosition(j * cellSize + 40.f, i * cellSize + 30.f);
                        window.draw(symbolText);
                    }
                }
            }
        }
	
	gameText.setString(agent);
        gameText.setCharacterSize(20);
        gameText.setFillColor(sf::Color::Red);
	window.draw(gameText);
        window.display();
    }

    return 0;
}

