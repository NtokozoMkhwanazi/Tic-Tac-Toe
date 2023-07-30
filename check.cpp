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
