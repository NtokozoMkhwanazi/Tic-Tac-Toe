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
