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

