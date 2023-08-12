#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <memory>
#include <climits>
#include <cstdlib>
#include <ctime>

using namespace std;


bool checkWin(const vector<vector<char>>& board, char symbol){
	       
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
	
bool checkDraw(const vector<vector<char>>& board){
	        
	for (const auto& row : board) {
		for (char cell : row) {
	    	if (cell == '#') {
	        	return false; 
	        }
		}
	}
	        return true; 
}

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

	    bool isEmpty(int i, int j) const {
	        return Board[i][j] == '#';
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
	
	    unique_ptr<TicTacToeNode> makeAIMove(char AgentSymbol) override {
		    
		    int maxDepth = (Board.size() == 3) ? 5 : 3;
	
	        int bestScore = INT_MIN;
	        unique_ptr<TicTacToeNode> bestMove = nullptr;
	
	        for (int i = 0; i < Board.size(); i++) {
	            for (int j = 0; j < Board[i].size(); j++) {
	                if (isSafe(i, j) && isEmpty(i, j)) {
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
	                    if (state->isSafe(i, j) && state->isEmpty(i, j)) {
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
	                    if (state->isSafe(i, j) && state->isEmpty(i, j)) {
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


int main() {
   
    srand(time(NULL));

    sf::Font font;
    if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf")) {
        
        return 1;
    }

    sf::RenderWindow window(sf::VideoMode(400, 400), "Tic Tac Toe");
    window.setFramerateLimit(60);

    vector<vector<char>> Board(3, vector<char>(3, '#'));
    unique_ptr<TicTacToeNode> TTT = make_unique<State>(Board);
    char agentSymbol = 'X';
    char userSymbol = 'O';

    int x, y;
    const int cellSize = 133;

	while (window.isOpen()) {

		sf::Event event;
	    while (window.pollEvent(event)) {
	        if (event.type == sf::Event::Closed) {
	            window.close();
	        }

	        if (event.type == sf::Event::MouseButtonPressed) {
	            if (event.mouseButton.button == sf::Mouse::Left) {
	                int mouseX = event.mouseButton.x;
	                int mouseY = event.mouseButton.y;
	
	                
	                int i = mouseY / cellSize; // Row
	                int j = mouseX / cellSize; // Column
	
	                
	                if (i >= 0 && i < 3 && j >= 0 && j < 3 && TTT->Board[i][j] == '#') {
	                    TTT->Board = userPlay(TTT->Board, j, i, userSymbol);
	
	                    
	                    if (!isBoardFull(TTT->Board) && !checkWin(TTT->Board, userSymbol)) {
	                        TTT = TTT->makeAIMove(agentSymbol);
	                    }
	                }
	            }
	        }
	    }

	        window.clear(sf::Color::White);	
	        
	        for (int i = 1; i < 3; ++i) {
	            sf::Vertex verticalLine[] = {
	                sf::Vertex(sf::Vector2f(i * cellSize, 0), sf::Color::Black),
	                sf::Vertex(sf::Vector2f(i * cellSize, 400), sf::Color::Black)
	            };
	        
	            sf::Vertex horizontalLine[] = {
	                sf::Vertex(sf::Vector2f(0, i * cellSize), sf::Color::Black),
	                sf::Vertex(sf::Vector2f(400, i * cellSize), sf::Color::Black)
	            };
	            window.draw(verticalLine, 2, sf::Lines);
	            window.draw(horizontalLine, 2, sf::Lines);
	        }

        	
	        for (int i = 0; i < TTT->Board.size(); ++i) {
	            for (int j = 0; j < TTT->Board[i].size(); ++j) {
	                
	                if (TTT->Board[i][j] == userSymbol) {
	                
	                    sf::CircleShape oShape(cellSize / 3.0f - 20); // Adjusted size
	                    oShape.setPosition(j * cellSize + cellSize / 6.0f, i * cellSize + cellSize / 6.0f); // Centered
	                    oShape.setFillColor(sf::Color::Transparent);
	                    oShape.setOutlineThickness(10);
	                    oShape.setOutlineColor(sf::Color::Blue);
	                    window.draw(oShape);
	                
	               	} else if (TTT->Board[i][j] == agentSymbol) {
	    			
					    sf::RectangleShape xShape(sf::Vector2f(cellSize / 3.0f, cellSize / 3.0f));
					   	xShape.setPosition(j * cellSize + cellSize / 6.0f, i * cellSize + cellSize / 6.0f);
	 					xShape.setFillColor(sf::Color::Transparent);

    					
	    				xShape.setOutlineThickness(20);
	
	    				sf::Color outlineColor1 = sf::Color::Red;
	    				sf::Color outlineColor2 = sf::Color::Red;
	
	    				sf::Vertex xLine1[] = {
	        				sf::Vertex(sf::Vector2f(j * cellSize + cellSize / 6.0f, i * cellSize + cellSize / 6.0f), outlineColor1),
	        				sf::Vertex(sf::Vector2f((j + 1) * cellSize - cellSize / 6.0f, (i + 1) * cellSize - cellSize / 6.0f), outlineColor1)
	    				};
	    				sf::Vertex xLine2[] = {
	        				sf::Vertex(sf::Vector2f((j + 1) * cellSize - cellSize / 6.0f, i * cellSize + cellSize / 6.0f), outlineColor2),
	        				sf::Vertex(sf::Vector2f(j * cellSize + cellSize / 6.0f, (i + 1) * cellSize - cellSize / 6.0f), outlineColor2)
	    				};
	
					    window.draw(xShape);
	    				window.draw(xLine1, 2, sf::Lines);
	    				window.draw(xLine2, 2, sf::Lines);
					}
				}
			}

            
      
       	sf::Text message;
        message.setFont(font);
        message.setCharacterSize(32);
        message.setFillColor(sf::Color::Black);

        bool gameOver = false;
        if (checkWin(TTT->Board, userSymbol)) {
            message.setString("You win!");
            gameOver = true;
        } else if (checkWin(TTT->Board, agentSymbol)) {
            message.setString("AI wins!");
            gameOver = true;
        } else if (isBoardFull(TTT->Board)) {
            message.setString("It's a draw!");
            gameOver = true;
        }

        
        window.display();

       
        if (gameOver) {
            sf::RenderWindow gameOverWindow(sf::VideoMode(200, 100), "Game Over");
            gameOverWindow.setFramerateLimit(60);

            sf::Text gameOverText;
            gameOverText.setFont(font);
            gameOverText.setCharacterSize(25);
            gameOverText.setFillColor(sf::Color::Black);
            gameOverText.setString(message.getString());

            while (gameOverWindow.isOpen()) {
                sf::Event gameOverEvent;
                while (gameOverWindow.pollEvent(gameOverEvent)) {
                    if (gameOverEvent.type == sf::Event::Closed) {
                        window.close();
                        gameOverWindow.close();
                    }
                }

                gameOverWindow.clear(sf::Color::Blue);
                gameOverWindow.draw(gameOverText);
                gameOverWindow.display();
            }
        }
    }
    

    return 0;
}
