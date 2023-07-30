vector<vector<char>> userPlay(vector<vector<char>>& Board, int x, int y, char userSymbol) {
    Board[y][x] = userSymbol; // Swap x and y here
    return Board;
}
