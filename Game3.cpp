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
    sf::Sprite sprite; sf::FloatRect bounds;
    float hoverAlpha=210, normalAlpha=255;
    void set(const sf::Texture& t, sf::Vector2f pos, sf::Vector2f size){
        sprite.setTexture(t);
        float s=min(size.x/t.getSize().x,size.y/t.getSize().y);
        sprite.setScale(s,s);
        sprite.setPosition(pos.x+(size.x-t.getSize().x*s)/2,pos.y+(size.y-t.getSize().y*s)/2);
        bounds=sprite.getGlobalBounds();
    }
    bool contains(sf::Vector2i m)const{return bounds.contains(m.x,m.y);}
    void updateHover(sf::Vector2i m){bounds=sprite.getGlobalBounds(); sprite.setColor(sf::Color(255,255,255,contains(m)?hoverAlpha:normalAlpha));}
    void draw(sf::RenderWindow&w)const{w.draw(sprite);}
};

enum class Difficulty { EASY, MEDIUM, HARD };

int main(){
    srand(time(nullptr));
    const int WIN_W=960,WIN_H=720;
    sf::RenderWindow w(sf::VideoMode(WIN_W,WIN_H),"TicTacToe+",sf::Style::Close);
    w.setFramerateLimit(60);

    // ----- Load Textures -----
    sf::Texture xTex,oTex,bgTex,startTex,exitTex,restartTex,pvpTex,aiTex,easyTex,medTex,hardTex;
    xTex.loadFromFile("assets/battle-axe.png");
    oTex.loadFromFile("assets/broadsword.png");
    bgTex.loadFromFile("assets/background.png");
    startTex.loadFromFile("assets/start-button.png");
    exitTex.loadFromFile("assets/exit.png");
    restartTex.loadFromFile("assets/restart.png");
    pvpTex.loadFromFile("assets/swords.png");
    aiTex.loadFromFile("assets/versus.png");
    easyTex.loadFromFile("assets/battery-25.png");
    medTex.loadFromFile("assets/battery-50.png");
    hardTex.loadFromFile("assets/battery-75.png");

    // ----- Audio -----
    sf::SoundBuffer clickBuf, moveBuf, winBuf;
    sf::Music bgm;
    clickBuf.loadFromFile("assets/click.wav");
    moveBuf.loadFromFile("assets/move.wav");
    winBuf.loadFromFile("assets/win.wav");
    bgm.openFromFile("assets/bgm.ogg");
    bgm.setLoop(true); bgm.setVolume(40.f); bgm.play();
    sf::Sound click(clickBuf), move(moveBuf), winSnd(winBuf);

    // ----- Fonts -----
    sf::Font font; font.loadFromFile("assets/montserrat/Montserrat-SemiBold.ttf");

    sf::Sprite bg(bgTex);
    bg.setScale((float)WIN_W/bgTex.getSize().x,(float)WIN_H/bgTex.getSize().y);

    // ----- Buttons -----
    ImageButton bStart,bExit,bPvp,bAi,bEasy,bMed,bHard,bRestart,bMenu;
    auto setBtn=[&](ImageButton&b,sf::Texture&t,sf::Vector2f p,sf::Vector2f s){b.set(t,p,s);};
    setBtn(bStart,startTex,{(WIN_W-360)/2.f,200},{360,64});
    setBtn(bExit,exitTex,{(WIN_W-360)/2.f,290},{360,64});
    setBtn(bPvp,pvpTex,{(WIN_W-360)/2.f,180},{360,64});
    setBtn(bAi,aiTex,{(WIN_W-360)/2.f,280},{360,64});
    setBtn(bEasy,easyTex,{(WIN_W-380)/2.f,140},{120,36});
    setBtn(bMed,medTex,{(WIN_W-380)/2.f+140,140},{120,36});
    setBtn(bHard,hardTex,{(WIN_W-380)/2.f+280,140},{120,36});
    setBtn(bRestart,restartTex,{140,640},{220,56});
    setBtn(bMenu,exitTex,{600,640},{220,56});

    sf::Text title("TIC-TAC-TOE",font,44); title.setFillColor(sf::Color::White);
    sf::Text msg("",font,36);

    sf::RectangleShape fade(sf::Vector2f(WIN_W,WIN_H)); fade.setFillColor(sf::Color(0,0,0,0));
    float transitionAlpha=0,transTimer=0; bool transitioning=false; int phase=0; const float dur=0.3f;
    enum class GState{MAIN_MENU,MODE_SELECT,DIFFICULTY,PLAYING,GAME_OVER}; 
    GState state=GState::MAIN_MENU,next=GState::MAIN_MENU;

    vector<vector<char>> Board=getBoard();
    bool vsAI=false,isXturn=true,gameOver=false; string msgStr;
    Difficulty diff=Difficulty::HARD;

    float boardSize=540,cell=boardSize/3; sf::Vector2f boardPos((WIN_W-boardSize)/2,110);

    auto mouseToCell=[&](sf::Vector2i m){return make_pair((int)((m.y-boardPos.y)/cell),(int)((m.x-boardPos.x)/cell));};
    auto startTransition=[&](GState to){transitioning=true;phase=0;transTimer=0;transitionAlpha=0;next=to;};
    auto aiMove=[&](vector<vector<char>>&b,Difficulty d){
        if(d==Difficulty::EASY){
            vector<pair<int,int>> e; for(int r=0;r<3;r++)for(int c=0;c<3;c++)if(b[r][c]=='#')e.push_back({r,c});
            if(!e.empty()){auto p=e[rand()%e.size()];b[p.first][p.second]='O';}
            return;
        }
        int depth=(d==Difficulty::MEDIUM)?3:6;
        State root(b,nullptr,depth); auto move=root.makeAIMove('O'); if(move) b=move->Board;
    };

    sf::Clock clk;
    while(w.isOpen()){
        float dt=clk.restart().asSeconds(); sf::Event e;
        while(w.pollEvent(e)){
            if(e.type==sf::Event::Closed) w.close();
            sf::Vector2i ms=sf::Mouse::getPosition(w);
            bStart.updateHover(ms);bExit.updateHover(ms);bPvp.updateHover(ms);bAi.updateHover(ms);
            bEasy.updateHover(ms);bMed.updateHover(ms);bHard.updateHover(ms);
            bRestart.updateHover(ms);bMenu.updateHover(ms);

            if(e.type==sf::Event::MouseButtonPressed && e.mouseButton.button==sf::Mouse::Left && !transitioning){
                click.play();
                if(state==GState::MAIN_MENU){
                    if(bStart.contains(ms))startTransition(GState::MODE_SELECT);
                    else if(bExit.contains(ms))w.close();
                }else if(state==GState::MODE_SELECT){
                    if(bPvp.contains(ms)){vsAI=false;Board=getBoard();gameOver=false;isXturn=true;msgStr.clear();startTransition(GState::PLAYING);}
                    else if(bAi.contains(ms)){vsAI=true;Board=getBoard();gameOver=false;isXturn=true;msgStr.clear();startTransition(GState::DIFFICULTY);}
                }else if(state==GState::DIFFICULTY){
                    if(bEasy.contains(ms)){diff=Difficulty::EASY;startTransition(GState::PLAYING);}
                    else if(bMed.contains(ms)){diff=Difficulty::MEDIUM;startTransition(GState::PLAYING);}
                    else if(bHard.contains(ms)){diff=Difficulty::HARD;startTransition(GState::PLAYING);}
                }else if(state==GState::PLAYING||state==GState::GAME_OVER){
                    if(bRestart.contains(ms)){Board=getBoard();gameOver=false;isXturn=true;msgStr.clear();startTransition(GState::PLAYING);}
                    else if(bMenu.contains(ms)){startTransition(GState::MAIN_MENU);}
                    else if(!gameOver){
                        auto [r,c]=mouseToCell(ms);
                        if(r>=0&&r<3&&c>=0&&c<3&&Board[r][c]=='#'){
                            move.play();
                            if(!vsAI){
                                char sym=isXturn?'X':'O';
                                Board[r][c]=sym;
                                if(checkWin(Board,sym)){msgStr=string(1,sym)+" wins!";gameOver=true;winSnd.play();}
                                else if(checkDraw(Board)){msgStr="Draw!";gameOver=true;winSnd.play();}
                                else isXturn=!isXturn;
                            }else{
                                Board[r][c]='X';
                                if(checkWin(Board,'X')){msgStr="You win!";gameOver=true;winSnd.play();}
                                else if(checkDraw(Board)){msgStr="Draw!";gameOver=true;winSnd.play();}
                                else{aiMove(Board,diff);move.play();if(checkWin(Board,'O')){msgStr="AI wins!";gameOver=true;winSnd.play();}else if(checkDraw(Board)){msgStr="Draw!";gameOver=true;winSnd.play();}}
                            }
                            if(gameOver)startTransition(GState::GAME_OVER);
                        }
                    }
                }
            }
        }

        if(transitioning){
            transTimer+=dt;float t=transTimer/dur;
            if(phase==0){if(t>=1){phase=1;transTimer=0;state=next;}else transitionAlpha=t*255;}
            else{if(t>=1){transitioning=false;transitionAlpha=0;}else transitionAlpha=(1-t)*255;}
            fade.setFillColor(sf::Color(0,0,0,(sf::Uint8)transitionAlpha));
        }

        // ----- Render -----
        w.clear(); w.draw(bg);
        if(state==GState::MAIN_MENU){title.setPosition((WIN_W-title.getGlobalBounds().width)/2,100);w.draw(title);bStart.draw(w);bExit.draw(w);}
        else if(state==GState::MODE_SELECT){sf::Text t("Choose Mode",font,40);t.setPosition((WIN_W-t.getGlobalBounds().width)/2,50);w.draw(t);bPvp.draw(w);bAi.draw(w);}
        else if(state==GState::DIFFICULTY){sf::Text t("Select AI Difficulty",font,40);t.setPosition((WIN_W-t.getGlobalBounds().width)/2,50);w.draw(t);bEasy.draw(w);bMed.draw(w);bHard.draw(w);}
        else if(state==GState::PLAYING||state==GState::GAME_OVER){
            sf::Color gridColor(255,255,255,220);float thick=4;
            for(int i=1;i<3;i++){sf::RectangleShape l({thick,cell*3});l.setFillColor(gridColor);l.setPosition(boardPos.x+i*cell-thick/2,boardPos.y);w.draw(l);}
            for(int i=1;i<3;i++){sf::RectangleShape l({cell*3,thick});l.setFillColor(gridColor);l.setPosition(boardPos.x,boardPos.y+i*cell-thick/2);w.draw(l);}
            for(int r=0;r<3;r++)for(int c=0;c<3;c++){
                sf::Sprite s; if(Board[r][c]=='X')s.setTexture(xTex);else if(Board[r][c]=='O')s.setTexture(oTex);else continue;
                float sc=cell/s.getTexture()->getSize().x*0.85f; s.setScale(sc,sc);
                s.setPosition(boardPos.x+c*cell+(cell-s.getGlobalBounds().width)/2,boardPos.y+r*cell+(cell-s.getGlobalBounds().height)/2);
                w.draw(s);
            }
            if(state==GState::GAME_OVER){msg.setString(msgStr);msg.setPosition((WIN_W-msg.getGlobalBounds().width)/2,40);w.draw(msg);}
            bRestart.draw(w);bMenu.draw(w);
        }
        if(transitioning)w.draw(fade);
        w.display();
    }
}

