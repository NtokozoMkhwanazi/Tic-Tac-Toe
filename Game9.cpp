// tic_tac_toe_final_sound.cpp
// Compile with SFML (graphics, window, system, audio):
// g++ main.cpp -o tic_tac_toe -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <vector>
#include <memory>
#include <cstdlib>
#include <ctime>
#include <climits>
#include <cmath>
#include <string>
using namespace std;

// ---------- Helper Functions ----------
bool checkWin(const vector<vector<char>>& board, char symbol) {
    for (int i = 0; i < 3; ++i) {
        if (board[i][0] == symbol && board[i][1] == symbol && board[i][2] == symbol) return true;
        if (board[0][i] == symbol && board[1][i] == symbol && board[2][i] == symbol) return true;
    }
    if (board[0][0] == symbol && board[1][1] == symbol && board[2][2] == symbol) return true;
    if (board[0][2] == symbol && board[1][1] == symbol && board[2][0] == symbol) return true;
    return false;
}
bool checkDraw(const vector<vector<char>>& board) {
    for (auto& r : board) for (char c : r) if (c == '#') return false;
    return true;
}
vector<vector<char>> getBoard() { return vector<vector<char>>(3, vector<char>(3, '#')); }

// ---------- Minimax AI ----------
class TicTacToeNode {
public:
    vector<vector<char>> Board;
    TicTacToeNode* Parent;
    int emptyCells, searchDepth;

    TicTacToeNode(vector<vector<char>> b, TicTacToeNode* p=nullptr, int d=6)
        : Board(b), Parent(p), searchDepth(d) { updateEmptyCells(); }

    void updateEmptyCells() {
        emptyCells=0;
        for (auto& r : Board) for (char c : r) if (c=='#') emptyCells++;
    }
    bool isSafe(int i,int j) const { return i>=0&&j>=0&&i<3&&j<3&&Board[i][j]=='#'; }
    virtual unique_ptr<TicTacToeNode> makeAIMove(char AgentSymbol){ return nullptr; }
};

class State : public TicTacToeNode {
public:
    State(vector<vector<char>> b, TicTacToeNode* p=nullptr, int d=6):TicTacToeNode(b,p,d){}
    unique_ptr<TicTacToeNode> makeAIMove(char AgentSymbol) override;
    int evaluate(const vector<vector<char>>& b, char sym) const {
        if (checkWin(b, sym)) return 10;
        char opp = (sym=='X')?'O':'X';
        if (checkWin(b, opp)) return -10;
        return 0;
    }
    int minimax(State* st, char sym, int depth, int alpha, int beta, bool max) const;
};

int State::minimax(State* st, char sym, int depth, int alpha, int beta, bool max) const {
    char opp = (sym=='X')?'O':'X';
    int score = evaluate(st->Board, sym);
    if (score==10 || score==-10 || depth==0 || checkDraw(st->Board)) return score;

    if (max) {
        int best = INT_MIN;
        for(int i=0;i<3;i++) for(int j=0;j<3;j++)
            if(st->isSafe(i,j)){
                auto nb=st->Board; nb[i][j]=sym;
                auto ns=make_unique<State>(nb,st,st->searchDepth);
                int val=minimax(ns.get(),sym,depth-1,alpha,beta,false);
                best=std::max(best,val); alpha=std::max(alpha,best); if(beta<=alpha) goto doneMax;
            }
        doneMax: return best;
    } else {
        int best = INT_MAX;
        for(int i=0;i<3;i++) for(int j=0;j<3;j++)
            if(st->isSafe(i,j)){
                auto nb=st->Board; nb[i][j]=opp;
                auto ns=make_unique<State>(nb,st,st->searchDepth);
                int val=minimax(ns.get(),sym,depth-1,alpha,beta,true);
                best=std::min(best,val); beta=std::min(beta,best); if(beta<=alpha) goto doneMin;
            }
        doneMin: return best;
    }
}
unique_ptr<TicTacToeNode> State::makeAIMove(char sym){
    int bestScore=INT_MIN; unique_ptr<TicTacToeNode> best=nullptr;
    for(int i=0;i<3;i++) for(int j=0;j<3;j++)
        if(isSafe(i,j)){
            auto nb=Board; nb[i][j]=sym;
            auto ns=make_unique<State>(nb,this,searchDepth);
            int s=minimax(ns.get(),sym,searchDepth-1,INT_MIN,INT_MAX,false);
            if(s>bestScore){ bestScore=s; best=move(ns);}
        }
    return best;
}

// ---------- Image Button ----------
struct ImageButton {
    sf::Sprite sprite;
    sf::FloatRect bounds;
    sf::Vector2f desiredSize; // the logical size we want the button to occupy
    float hoverAlpha=210, normalAlpha=255;
    // set texture and initial position/size (pos is top-left of the bounding box)
    void set(const sf::Texture& t, sf::Vector2f pos, sf::Vector2f size){
        sprite.setTexture(t);
        desiredSize = size;
        // compute scale so texture fits into 'size' while preserving aspect ratio
        float s = min(size.x / (float)t.getSize().x, size.y / (float)t.getSize().y);
        sprite.setScale(s, s);
        // place sprite centered inside the provided box
        float texW = t.getSize().x * s;
        float texH = t.getSize().y * s;
        sprite.setPosition(pos.x + (size.x - texW)/2.f, pos.y + (size.y - texH)/2.f);
        bounds = sf::FloatRect(pos.x, pos.y, size.x, size.y);
    }
    bool contains(sf::Vector2i m) const { return bounds.contains((float)m.x, (float)m.y); }
    void updateHover(sf::Vector2i m){ sprite.setColor(sf::Color(255,255,255, contains(m) ? (sf::Uint8)hoverAlpha : (sf::Uint8)normalAlpha)); }
    void draw(sf::RenderWindow& w) const { w.draw(sprite); }
    // set top-left position of the logical bounding box (recomputes sprite position accordingly)
    void setPosition(sf::Vector2f pos) {
        // keep same scale; center sprite inside desiredSize at pos
        float texW = sprite.getTexture()->getSize().x * sprite.getScale().x;
        float texH = sprite.getTexture()->getSize().y * sprite.getScale().y;
        sprite.setPosition(pos.x + (desiredSize.x - texW)/2.f, pos.y + (desiredSize.y - texH)/2.f);
        bounds.left = pos.x;
        bounds.top = pos.y;
        bounds.width = desiredSize.x;
        bounds.height = desiredSize.y;
    }
};

// ---------- Enums ----------
enum class Difficulty { EASY, MEDIUM, HARD };

int main(){
    srand((unsigned)time(nullptr));
    const int WIN_W = 960, WIN_H = 720;
    sf::RenderWindow w(sf::VideoMode(WIN_W, WIN_H), "TicTacToe+", sf::Style::Close);
    w.setFramerateLimit(60);

    //------ Fonts & Titles ----
    sf::Font font;
    if (!font.loadFromFile("assets/montserrat/Montserrat-SemiBold.ttf")) {
        cerr << "Failed to load font at assets/montserrat/Montserrat-SemiBold.ttf\n";
        // try fallback system font (optional) or continue
    }
    sf::Text modeText("", font, 36), difficultyText("", font, 36);
    modeText.setString("SELECT GAME MODE");
    modeText.setFillColor(sf::Color::White);
    modeText.setStyle(sf::Text::Bold);

    difficultyText.setString("SELECT DIFFICULTY LEVEL");
    difficultyText.setFillColor(sf::Color::White);
    difficultyText.setStyle(sf::Text::Bold);

    // ----- Load Textures -----
    sf::Texture xTex,oTex,bgTex,startTex,exitTex,restartTex,pvpTex,aiTex,easyTex,medTex,hardTex,titleTex,winTex,drawTex;
    if (!xTex.loadFromFile("assets/battle-axe.png")) cerr<<"missing battle-axe.png\n";
    if (!oTex.loadFromFile("assets/broadsword.png")) cerr<<"missing broadsword.png\n";
    if (!bgTex.loadFromFile("assets/background.png")) cerr<<"missing background.png\n";
    if (!startTex.loadFromFile("assets/start-button.png")) cerr<<"missing start-button.png\n";
    if (!exitTex.loadFromFile("assets/exit.png")) cerr<<"missing exit.png\n";
    if (!restartTex.loadFromFile("assets/restart.png")) cerr<<"missing restart.png\n";
    if (!pvpTex.loadFromFile("assets/swords.png")) cerr<<"missing swords.png\n";
    if (!aiTex.loadFromFile("assets/versus.png")) cerr<<"missing versus.png\n";
    if (!easyTex.loadFromFile("assets/easy.png")) cerr<<"missing easy.png\n";
    if (!medTex.loadFromFile("assets/medium.png")) cerr<<"missing medium.png\n";
    if (!hardTex.loadFromFile("assets/hard.png")) cerr<<"missing hard.png\n";
    if (!titleTex.loadFromFile("assets/title.png")) cerr<<"missing title.png\n";
    if (!winTex.loadFromFile("assets/win.png")) cerr<<"missing win.png\n";
    if (!drawTex.loadFromFile("assets/draw.png")) cerr<<"missing draw.png\n";

    sf::Sprite title(titleTex), winIcon(winTex), drawIcon(drawTex);
    // Use origin center for easy scaling/positioning
    sf::FloatRect titleRect = title.getLocalBounds();
    title.setOrigin(titleRect.left + titleRect.width/2.f, titleRect.top + titleRect.height/2.f);
    float titleBaseScale = min(750.f / (float)titleRect.width, 150.f / (float)titleRect.height);
    title.setScale(titleBaseScale, titleBaseScale);
    float titleY = 80.f;
    title.setPosition(WIN_W/2.f, titleY);

    winIcon.setPosition((WIN_W - winTex.getSize().x)/2.f, 2);
    drawIcon.setPosition((WIN_W - drawTex.getSize().x)/2.f, 2);

    // ----- Audio -----
    sf::SoundBuffer clickBuf, moveBuf, winBuf;
    sf::Music bgm;
    if (!clickBuf.loadFromFile("assets/click.wav")) cerr<<"missing click.wav\n";
    if (!moveBuf.loadFromFile("assets/move.wav")) cerr<<"missing move.wav\n";
    if (!winBuf.loadFromFile("assets/win.wav")) cerr<<"missing win.wav\n";
    if (!bgm.openFromFile("assets/bgm.ogg")) cerr<<"missing bgm.ogg\n";
    bgm.setLoop(true); bgm.setVolume(40.f); bgm.play();
    sf::Sound click(clickBuf), move(moveBuf), winSnd(winBuf);

    sf::Sprite bg(bgTex);
    bg.setScale((float)WIN_W/bgTex.getSize().x, (float)WIN_H/bgTex.getSize().y);

    // ----- Buttons -----
    ImageButton bStart,bExit,bPvp,bAi,bEasy,bMed,bHard,bRestart,bMenu;
    auto setBtn=[&](ImageButton&b,sf::Texture&t,sf::Vector2f p,sf::Vector2f s){ b.set(t,p,s); };
    setBtn(bStart, startTex, { (WIN_W-360)/2.f, 260 }, { 360, 70 });
    setBtn(bExit, exitTex, { (WIN_W-360)/2.f, 350 }, { 360, 70 });
    setBtn(bPvp, pvpTex, { (WIN_W-400)/2.f, 240 }, { 400, 80 });
    setBtn(bAi, aiTex, { (WIN_W-400)/2.f, 340 }, { 400, 80 });
    setBtn(bEasy, easyTex, { WIN_W/2.f - 260, 300 }, { 220, 70 });
    setBtn(bMed, medTex, { WIN_W/2.f - 110, 300 }, { 220, 70 });
    setBtn(bHard, hardTex, { WIN_W/2.f + 40, 300 }, { 220, 70 });
    setBtn(bRestart, restartTex, { 140, WIN_H - 96 }, { 220, 56 });
    setBtn(bMenu, exitTex, { WIN_W - 360, WIN_H - 96 }, { 220, 56 });

    // Difficulty labels
    sf::Text easyLabel("Easy", font, 22), medLabel("Medium", font, 22), hardLabel("Hard", font, 22);
    easyLabel.setFillColor(sf::Color::White);
    medLabel.setFillColor(sf::Color::White);
    hardLabel.setFillColor(sf::Color::White);

    // initial label positions (they will be adjusted in the render loop when needed)
    easyLabel.setPosition(bEasy.bounds.left + (bEasy.bounds.width - easyLabel.getGlobalBounds().width)/2.f, bEasy.bounds.top - 36);
    medLabel.setPosition(bMed.bounds.left + (bMed.bounds.width - medLabel.getGlobalBounds().width)/2.f, bMed.bounds.top - 36);
    hardLabel.setPosition(bHard.bounds.left + (bHard.bounds.width - hardLabel.getGlobalBounds().width)/2.f, bHard.bounds.top - 36);

    // misc
    sf::RectangleShape fade(sf::Vector2f(WIN_W, WIN_H)); fade.setFillColor(sf::Color(0,0,0,0));
    float transitionAlpha=0, transTimer=0; bool transitioning=false; int phase=0; const float dur=0.3f;
    enum class GState{ MAIN_MENU, MODE_SELECT, DIFFICULTY, PLAYING, GAME_OVER };
    GState state = GState::MAIN_MENU, next = GState::MAIN_MENU;

    vector<vector<char>> Board = getBoard();
    bool vsAI=false, isXturn=true, gameOver=false; string msgStr;
    Difficulty diff = Difficulty::HARD;

    float boardSize = 540, cell = boardSize/3; sf::Vector2f boardPos((WIN_W-boardSize)/2.f, 130.f);
    auto mouseToCell = [&](sf::Vector2i m){ return make_pair((int)((m.y - boardPos.y)/cell), (int)((m.x - boardPos.x)/cell)); };
    auto startTransition = [&](GState to){ transitioning=true; phase=0; transTimer=0; transitionAlpha=0; next=to; };

    auto aiMove = [&](vector<vector<char>>& b, Difficulty d){
        if(d==Difficulty::EASY){
            vector<pair<int,int>> e; for(int r=0;r<3;r++)for(int c=0;c<3;c++) if(b[r][c]=='#') e.push_back({r,c});
            if(!e.empty()){ auto p = e[rand()%e.size()]; b[p.first][p.second] = 'O'; }
            return;
        }
        int depth = (d==Difficulty::MEDIUM) ? 3 : 6;
        State root(b, nullptr, depth); auto mv = root.makeAIMove('O'); if(mv) b = mv->Board;
    };

    sf::Clock clk; float pulseTimer=0;
    while(w.isOpen()){
        float dt = clk.restart().asSeconds(); pulseTimer += dt;
        sf::Event e;
        while(w.pollEvent(e)){
            if(e.type==sf::Event::Closed) w.close();
            sf::Vector2i ms = sf::Mouse::getPosition(w);

            // update hover state
            bStart.updateHover(ms); bExit.updateHover(ms); bPvp.updateHover(ms); bAi.updateHover(ms);
            bEasy.updateHover(ms); bMed.updateHover(ms); bHard.updateHover(ms);
            bRestart.updateHover(ms); bMenu.updateHover(ms);

            if(e.type==sf::Event::MouseButtonPressed && e.mouseButton.button==sf::Mouse::Left && !transitioning){
                click.play();
                if(state==GState::MAIN_MENU){
                    if(bStart.contains(ms)) startTransition(GState::MODE_SELECT);
                    else if(bExit.contains(ms)) w.close();
                } else if(state==GState::MODE_SELECT){
                    
                     if(bRestart.contains(ms)){ startTransition(GState::MAIN_MENU); }
                   // else if(bMenu.contains(ms)){ startTransition(GState::GAME_OVER); }                    

                    if(bPvp.contains(ms)){ vsAI=false; Board=getBoard(); gameOver=false; isXturn=true; msgStr.clear(); startTransition(GState::PLAYING); }
                    else if(bAi.contains(ms)){ vsAI=true; Board=getBoard(); gameOver=false; isXturn=true; msgStr.clear(); startTransition(GState::DIFFICULTY); }
                } else if(state==GState::DIFFICULTY){
                    if(bEasy.contains(ms)){ diff=Difficulty::EASY; startTransition(GState::PLAYING); }
                    else if(bMed.contains(ms)){ diff=Difficulty::MEDIUM; startTransition(GState::PLAYING); }
                    else if(bHard.contains(ms)){ diff=Difficulty::HARD; startTransition(GState::PLAYING); }

                    if(bRestart.contains(ms)){ startTransition(GState::MAIN_MENU); }          

                } else if(state==GState::PLAYING || state==GState::GAME_OVER){
                    if(bRestart.contains(ms)){ Board=getBoard(); gameOver=false; isXturn=true; msgStr.clear(); startTransition(GState::PLAYING); }
                    else if(bMenu.contains(ms)){ startTransition(GState::MAIN_MENU); }
                    else if(!gameOver){
                        auto [r,c] = mouseToCell(ms);
                        if(r>=0&&r<3&&c>=0&&c<3&&Board[r][c]=='#'){
                            move.play();
                            if(!vsAI){
                                char sym = isXturn ? 'X' : 'O';
                                Board[r][c] = sym;
                                if(checkWin(Board, sym)){ msgStr = string(1,sym) + " wins!"; gameOver = true; winSnd.play(); }
                                else if(checkDraw(Board)){ msgStr = "Draw!"; gameOver = true; winSnd.play(); }
                                else isXturn = !isXturn;
                            } else {
                                Board[r][c] = 'X';
                                if(checkWin(Board,'X')){ msgStr = "You win!"; gameOver = true; winSnd.play(); }
                                else if(checkDraw(Board)){ msgStr = "Draw!"; gameOver = true; winSnd.play(); }
                                else { aiMove(Board, diff); move.play(); if(checkWin(Board,'O')){ msgStr = "AI wins!"; gameOver = true; winSnd.play(); } else if(checkDraw(Board)){ msgStr = "Draw!"; gameOver = true; winSnd.play(); } }
                            }
                            if(gameOver) startTransition(GState::GAME_OVER);
                        }
                    }
                }
            }
        }

        // transitions
        if(transitioning){
            transTimer += dt; float t = transTimer / dur;
            if(phase==0){
                if(t>=1){ phase=1; transTimer=0; state=next; }
                else transitionAlpha = t * 255.f;
            } else {
                if(t>=1){ transitioning=false; transitionAlpha=0; }
                else transitionAlpha = (1.f - t) * 255.f;
            }
            fade.setFillColor(sf::Color(0,0,0,(sf::Uint8)transitionAlpha));
        }

        // Title pulsing (scale + alpha + slight bob) only on main menu
        if(state==GState::MAIN_MENU){
            float pulse = 1.f + 0.035f * sin(pulseTimer * 2.8f); // small scale change
            title.setScale(titleBaseScale * pulse, titleBaseScale * pulse);
            float alpha = 210 + 45.f * (sin(pulseTimer * 2.8f) * 0.5f + 0.5f); // 210..255-ish
            sf::Color col = sf::Color(255,255,255,(sf::Uint8)alpha);
            title.setColor(col);
            // subtle vertical bob
            float bobY = titleY + 6.f * sin(pulseTimer * 2.8f);
            title.setPosition(WIN_W/2.f, bobY);
        } else {
            // reset to base so title doesn't stay scaled later
            title.setScale(titleBaseScale, titleBaseScale);
            title.setColor(sf::Color::White);
            title.setPosition(WIN_W/2.f, titleY);
        }

        // ----- Render -----
        w.clear();
        w.draw(bg);

        if(state==GState::MAIN_MENU){
            // draw shadow for title
            sf::Sprite shadow = title;
            shadow.setColor(sf::Color(0,0,0,100));
            shadow.move(4.f, 6.f);
            w.draw(shadow);

            w.draw(title);
            bStart.draw(w);
            bExit.draw(w);
        }
        else if(state==GState::MODE_SELECT){
            // center mode title
            modeText.setPosition((WIN_W - modeText.getGlobalBounds().width) / 2.f, 110.f);
            // small shadow
            sf::Text modeShadow = modeText;
            modeShadow.setFillColor(sf::Color(0,0,0,140));
            modeShadow.move(3.f, 4.f);
            w.draw(modeShadow);
            w.draw(modeText);

            // position buttons vertically centered block
            float btnWidth = 400.f;
            float btnX = (WIN_W - btnWidth) / 2.f;
            float startY = 220.f;
            bPvp.setPosition({btnX, startY});
            bAi.setPosition({btnX, startY + 120.f});
            bPvp.draw(w);
            bAi.draw(w);

            // bottom nav
            bRestart.setPosition({140.f, WIN_H - 96.f});
         //   bMenu.setPosition({WIN_W - 360.f, WIN_H - 96.f});
            bRestart.draw(w); bMenu.draw(w);
        }
        else if(state==GState::DIFFICULTY){
            // center difficulty title
            difficultyText.setPosition((WIN_W - difficultyText.getGlobalBounds().width) / 2.f, 110.f);
            sf::Text diffShadow = difficultyText;
            diffShadow.setFillColor(sf::Color(0,0,0,140));
            diffShadow.move(3.f, 4.f);
            w.draw(diffShadow);
            w.draw(difficultyText);

            // evenly space three difficulty buttons horizontally (centered)
            float totalWidth = bEasy.bounds.width + bMed.bounds.width + bHard.bounds.width + 80.f; // spacing between
            float startX = (WIN_W - totalWidth) / 2.f;
            float y = 260.f;
            bEasy.setPosition({ startX, y });
            bMed.setPosition({ startX + bEasy.bounds.width + 40.f, y });
            bHard.setPosition({ startX + bEasy.bounds.width + 40.f + bMed.bounds.width + 40.f, y });

            bEasy.draw(w);
            bMed.draw(w);
            bHard.draw(w);

            // labels above each button
            easyLabel.setPosition(bEasy.bounds.left + (bEasy.bounds.width - easyLabel.getGlobalBounds().width)/2.f, bEasy.bounds.top - 36.f);
            medLabel.setPosition(bMed.bounds.left + (bMed.bounds.width - medLabel.getGlobalBounds().width)/2.f, bMed.bounds.top - 36.f);
            hardLabel.setPosition(bHard.bounds.left + (bHard.bounds.width - hardLabel.getGlobalBounds().width)/2.f, bHard.bounds.top - 36.f);

            w.draw(easyLabel);
            w.draw(medLabel);
            w.draw(hardLabel);

            // bottom nav
            bRestart.setPosition({140.f, WIN_H - 96.f});
           // bMenu.setPosition({WIN_W - 360.f, WIN_H - 96.f});
            bRestart.draw(w); bMenu.draw(w);
        }
        else if(state==GState::PLAYING || state==GState::GAME_OVER){
    // === Draw board background image ===
    static sf::Texture boardTex;
    static bool boardLoaded = boardTex.loadFromFile("assets/board.png");
    static sf::Sprite boardSprite(boardTex);

    if (boardLoaded) {
        float scaleX = boardSize / boardTex.getSize().x;
        float scaleY = boardSize / boardTex.getSize().y;
        boardSprite.setScale(scaleX, scaleY);
        boardSprite.setPosition(boardPos);
        w.draw(boardSprite);
    } else {
        sf::Color gridColor(255,255,255,220); float thick = 4.f;
        for(int i=1;i<3;i++){
            sf::RectangleShape l({thick, cell*3});
            l.setFillColor(gridColor);
            l.setPosition(boardPos.x + i*cell - thick/2.f, boardPos.y);
            w.draw(l);
        }
        for(int i=1;i<3;i++){
            sf::RectangleShape l({cell*3, thick});
            l.setFillColor(gridColor);
            l.setPosition(boardPos.x, boardPos.y + i*cell - thick/2.f);
            w.draw(l);
        }
    }

    // === Draw pieces with soft shadows ===
    for(int r=0; r<3; r++){
        for(int c=0; c<3; c++){
            char piece = Board[r][c];
            if(piece != 'X' && piece != 'O') continue;

            sf::Texture& tex = (piece == 'X') ? xTex : oTex;

            sf::Sprite shadow(tex);
            sf::Sprite sprite(tex);

            float sc = cell / tex.getSize().x * 0.85f;
            shadow.setScale(sc, sc);
            sprite.setScale(sc, sc);

            float x = boardPos.x + c * cell + (cell - sprite.getGlobalBounds().width) / 2;
            float y = boardPos.y + r * cell + (cell - sprite.getGlobalBounds().height) / 2;

            // shadow offset and color
            shadow.setColor(sf::Color(0, 0, 0, 100));
            shadow.setPosition(x + 6, y + 6);

            sprite.setPosition(x, y);

            // draw shadow first, then the piece
            w.draw(shadow);
            w.draw(sprite);
        }
    }

    // === Game Over Icons ===
    if(state == GState::GAME_OVER){
        if(msgStr.find("Draw") != string::npos)
            w.draw(drawIcon);
        else
            w.draw(winIcon);
    }

    bRestart.draw(w);
    bMenu.draw(w);
}


        if(transitioning) w.draw(fade);
        w.display();
    }

    return 0;
}

