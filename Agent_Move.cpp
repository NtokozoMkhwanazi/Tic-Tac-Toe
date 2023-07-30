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
