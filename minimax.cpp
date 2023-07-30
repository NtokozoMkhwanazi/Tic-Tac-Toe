
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
