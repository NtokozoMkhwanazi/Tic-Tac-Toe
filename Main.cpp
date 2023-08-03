int main() {
   
    srand(time(NULL));

    sf::Font font;
    if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf")) {
        std::cout << "Error loading font" << std::endl;
        return 1;
    }

    sf::RenderWindow window(sf::VideoMode(400, 500), "Tic Tac Toe");
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
	
	                    
	                    if (!isBoardFull(TTT->Board) && !TTT->checkWin(TTT->Board, userSymbol)) {
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
        if (TTT->checkWin(TTT->Board, userSymbol)) {
            message.setString("You win!");
            gameOver = true;
        } else if (TTT->checkWin(TTT->Board, agentSymbol)) {
            message.setString("AI wins!");
            gameOver = true;
        } else if (isBoardFull(TTT->Board)) {
            message.setString("It's a draw!");
            gameOver = true;
        }

        
        window.draw(message);
        window.display();

        
        if (gameOver) {
            sf::Event gameOverEvent;
            while (window.waitEvent(gameOverEvent)) {
                if (gameOverEvent.type == sf::Event::Closed) {
                    window.close();
                }
                if (gameOverEvent.type == sf::Event::KeyPressed) {
                    break;
                }
            }
        }
    }

    return 0;
}
