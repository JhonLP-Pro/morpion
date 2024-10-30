#include "Game.hpp"
#include <iostream>
#include <cmath>

Game::Game() : currentPlayer(Player::X), running(true), gameOver(false) {
    initialize();
    // Initialiser le tableau vide
    for (auto& row : board) {
        row.fill(Player::NONE);
    }
}

Game::~Game() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Game::initialize() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "Erreur SDL_Init : " << SDL_GetError() << std::endl;
        return;
    }

    window = SDL_CreateWindow("Morpion", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                            WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Demander le mode de jeu
    std::cout << "Choisissez le mode de jeu (1: Joueur vs Joueur, 2: Joueur vs IA): ";
    int choice;
    std::cin >> choice;
    gameMode = (choice == 1) ? GameMode::PVP : GameMode::AI;
}

void Game::run() {
    while (running) {
        handleEvents();
        update();
        render();
    }
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
        }
        else if (event.type == SDL_MOUSEBUTTONDOWN && !gameOver) {
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);
            int row = mouseY / CELL_SIZE;
            int col = mouseX / CELL_SIZE;
            
            if (row < 3 && col < 3) {
                if (makeMove(row, col)) {
                    if (checkWin(currentPlayer)) {
                        std::cout << "Joueur " << (currentPlayer == Player::X ? "X" : "O") << " gagne!" << std::endl;
                        gameOver = true;
                        askForReplay();
                    }
                    else if (isBoardFull()) {
                        std::cout << "Match nul!" << std::endl;
                        gameOver = true;
                        askForReplay();
                    }
                    else {
                        currentPlayer = (currentPlayer == Player::X) ? Player::O : Player::X;
                        if (gameMode == GameMode::AI && currentPlayer == Player::O && !gameOver) {
                            aiMove();
                        }
                    }
                }
            }
        }
    }
}

void Game::update() {
    // Mise à jour du jeu si nécessaire
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
    
    drawGrid();
    drawSymbols();
    
    SDL_RenderPresent(renderer);
}

void Game::drawGrid() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    
    // Lignes verticales
    for (int i = 1; i < 3; ++i) {
        SDL_RenderDrawLine(renderer, i * CELL_SIZE, 0, i * CELL_SIZE, WINDOW_HEIGHT);
    }
    
    // Lignes horizontales
    for (int i = 1; i < 3; ++i) {
        SDL_RenderDrawLine(renderer, 0, i * CELL_SIZE, WINDOW_WIDTH, i * CELL_SIZE);
    }
}

void Game::drawSymbols() {
    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 3; ++col) {
            if (board[row][col] == Player::X) {
                drawX(row, col);
            }
            else if (board[row][col] == Player::O) {
                drawO(row, col);
            }
        }
    }
}

void Game::drawX(int row, int col) {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    int padding = 20;
    int x = col * CELL_SIZE;
    int y = row * CELL_SIZE;
    
    SDL_RenderDrawLine(renderer, x + padding, y + padding,
                      x + CELL_SIZE - padding, y + CELL_SIZE - padding);
    SDL_RenderDrawLine(renderer, x + CELL_SIZE - padding, y + padding,
                      x + padding, y + CELL_SIZE - padding);
}

void Game::drawO(int row, int col) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    int centerX = col * CELL_SIZE + CELL_SIZE / 2;
    int centerY = row * CELL_SIZE + CELL_SIZE / 2;
    int radius = CELL_SIZE / 2 - 20;
    
    for (int i = 0; i < 360; i++) {
        double angle = i * M_PI / 180;
        int x = centerX + radius * cos(angle);
        int y = centerY + radius * sin(angle);
        SDL_RenderDrawPoint(renderer, x, y);
    }
}

bool Game::makeMove(int row, int col) {
    if (board[row][col] == Player::NONE) {
        board[row][col] = currentPlayer;
        return true;
    }
    return false;
}

bool Game::checkWin(Player player) {
    // Vérification des lignes et colonnes
    for (int i = 0; i < 3; ++i) {
        if (board[i][0] == player && board[i][1] == player && board[i][2] == player) return true;
        if (board[0][i] == player && board[1][i] == player && board[2][i] == player) return true;
    }
    
    // Vérification des diagonales
    if (board[0][0] == player && board[1][1] == player && board[2][2] == player) return true;
    if (board[0][2] == player && board[1][1] == player && board[2][0] == player) return true;
    
    return false;
}

bool Game::isBoardFull() {
    for (const auto& row : board) {
        for (const auto& cell : row) {
            if (cell == Player::NONE) return false;
        }
    }
    return true;
}

void Game::aiMove() {
    auto [row, col] = getBestMove();
    makeMove(row, col);
    
    if (checkWin(currentPlayer)) {
        std::cout << "L'IA gagne!" << std::endl;
        gameOver = true;
        askForReplay();
    }
    else if (isBoardFull()) {
        std::cout << "Match nul!" << std::endl;
        gameOver = true;
        askForReplay();
    }
    else {
        currentPlayer = Player::X;
    }
}

std::pair<int, int> Game::getBestMove() {
    int bestScore = -1000;
    std::pair<int, int> bestMove = {-1, -1};
    
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (board[i][j] == Player::NONE) {
                board[i][j] = Player::O;
                int score = minimax(false, 0);
                board[i][j] = Player::NONE;
                
                if (score > bestScore) {
                    bestScore = score;
                    bestMove = {i, j};
                }
            }
        }
    }
    
    return bestMove;
}

int Game::minimax(bool isMaximizing, int depth) {
    if (checkWin(Player::O)) return 10 - depth;
    if (checkWin(Player::X)) return depth - 10;
    if (isBoardFull()) return 0;
    
    if (isMaximizing) {
        int bestScore = -1000;
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                if (board[i][j] == Player::NONE) {
                    board[i][j] = Player::O;
                    bestScore = std::max(bestScore, minimax(false, depth + 1));
                    board[i][j] = Player::NONE;
                }
            }
        }
        return bestScore;
    }
    else {
        int bestScore = 1000;
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                if (board[i][j] == Player::NONE) {
                    board[i][j] = Player::X;
                    bestScore = std::min(bestScore, minimax(true, depth + 1));
                    board[i][j] = Player::NONE;
                }
            }
        }
        return bestScore;
    }
}

void Game::reset() {
    // Réinitialiser le plateau
    for (auto& row : board) {
        row.fill(Player::NONE);
    }
    currentPlayer = Player::X;
    gameOver = false;
}

void Game::askForReplay() {
    std::cout << "Voulez-vous rejouer ? (1: Oui, 0: Non): ";
    int choice;
    std::cin >> choice;
    if (choice == 1) {
        reset();
        // Demander si on veut changer le mode de jeu
        std::cout << "Voulez-vous changer le mode de jeu ? (1: Oui, 0: Non): ";
        std::cin >> choice;
        if (choice == 1) {
            std::cout << "Choisissez le mode de jeu (1: Joueur vs Joueur, 2: Joueur vs IA): ";
            std::cin >> choice;
            gameMode = (choice == 1) ? GameMode::PVP : GameMode::AI;
        }
    } else {
        running = false;
    }
} 