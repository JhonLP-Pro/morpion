#pragma once
#include <SDL2/SDL.h>
#include <array>

class Game {
public:
    enum class Player { NONE, X, O };
    enum class GameMode { PVP, AI };

    Game();
    ~Game();
    void run();
    
private:
    static const int WINDOW_WIDTH = 600;
    static const int WINDOW_HEIGHT = 600;
    static const int CELL_SIZE = 200;
    
    SDL_Window* window;
    SDL_Renderer* renderer;
    std::array<std::array<Player, 3>, 3> board;
    Player currentPlayer;
    GameMode gameMode;
    bool running;
    bool gameOver;
    
    void initialize();
    void handleEvents();
    void update();
    void render();
    void drawGrid();
    void drawSymbols();
    void drawX(int row, int col);
    void drawO(int row, int col);
    bool makeMove(int row, int col);
    bool checkWin(Player player);
    bool isBoardFull();
    void aiMove();
    std::pair<int, int> getBestMove();
    int minimax(bool isMaximizing, int depth);
    void reset();
    void askForReplay();
}; 