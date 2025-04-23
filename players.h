#ifndef PLAYERS_H
#define PLAYERS_H

#include <string>
#include <cmath>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

// Forward declaration to avoid circular dependency
class Game;
class Button;


class Player {
public:
    Player(std::string name, bool isAI = false);
    virtual ~Player();

    void addHardPoints(int amount);
    void addSoftPoints(int amount);
    void addZilch();

    void resetSoftPoints();
    void resetHardPoints();
    void resetZilches();
    void resetFirstRoll();

    void setTurn(bool turnStatus);
    void firstRolled();

    bool isTurn() const;
    int getHardPoints() const;
    int getSoftPoints() const;
    int getZilches() const;
    std::string getName() const;

    bool getFirstRoll() const;

    // Virtual function for AI behavior
    bool isAIPlayer() const;
    virtual void takeTurn(Game& game, SDL_Renderer* renderer, TTF_Font* font, Button rollButton, Button bankButton, Button mainmenu, SDL_Texture* bgTexture, SDL_Rect bgRect, std::unique_ptr<Player>& currentPlayer); //Placeholder that is overriden by AI classes

    // Banking History
    void addToHistory(int points, bool isZilch);
    void clearHistory();
    const std::vector<std::pair<int, bool>>& getHistory() const;

private:
    std::string name;// Player's name
    std::vector<std::pair<int, bool>> player_history; // To track History
    bool turn;       // True if it's this player's turn
    int hardPoints;  // Permanent points
    int softPoints;  // Temporary points
    int zilches;     // Number of times the player has zilched
    bool firstRoll;  // Be able to roll the first time without selecting a button
    bool isAI;
};


///// AI CLASSES

/// STANDARD CLASS
class AIPlayer : public Player {
    public:
        AIPlayer(std::string name, bool isAI = true);
        virtual ~AIPlayer() = default;
    
        virtual void takeTurn(Game& game, SDL_Renderer* renderer, TTF_Font* font, Button rollButton, Button bankButton, Button mainmenu, SDL_Texture* bgTexture, SDL_Rect bgRect, std::unique_ptr<Player>& currentPlayer) override = 0;
    
    protected:
        virtual void selectHands(Game& game) = 0;
        virtual bool shouldBank(Game& game) = 0;
};
    
/// Aggressive AI
class AggressiveAI : public AIPlayer {
    public:
        AggressiveAI(std::string name);
        void takeTurn(Game& game, SDL_Renderer* renderer, TTF_Font* font, Button rollButton, Button bankButton, Button mainmenu, SDL_Texture* bgTexture, SDL_Rect bgRect, std::unique_ptr<Player>& currentPlayer) override;
    
    private:
        void selectHands(Game& game) override;
        bool shouldBank(Game& game) override;
        bool zilched;
        int rolledAgain;
};
    
/// Cautious AI
class CautiousAI : public AIPlayer {
    public:
        CautiousAI(std::string name);
        void takeTurn(Game& game, SDL_Renderer* renderer, TTF_Font* font, Button rollButton, Button bankButton, Button mainmenu, SDL_Texture* bgTexture, SDL_Rect bgRect, std::unique_ptr<Player>& currentPlayer) override;
    
    private:
        void selectHands(Game& game) override;
        bool shouldBank(Game& game) override;
        bool zilched;
        int rolledAgain;
};
    
    
/// Advanced/Adaptive AI
class AdaptiveAI : public AIPlayer {
    public:
        AdaptiveAI(std::string name);
        void takeTurn(Game& game, SDL_Renderer* renderer, TTF_Font* font, Button rollButton, Button bankButton, Button mainmenu, SDL_Texture* bgTexture, SDL_Rect bgRect, std::unique_ptr<Player>& currentPlayer) override;
        
    private:
        void selectHands(Game& game) override;
        bool shouldBank(Game& game) override;
        float safeProbability(int remainingDice);
        long long combination(int n, int r);
        bool zilched;
        int rolledAgain;
};

#endif
