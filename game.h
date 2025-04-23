#ifndef GAME_H
#define GAME_H


#include <cstdlib>
#include <ctime>
#include <vector>
#include <SDL.h>
#include <SDL_ttf.h>
#include <iostream>
#include <functional>
#include <memory>
#include <SDL_image.h>

//For Regular Holding
#include <map>

// For Players
#include "players.h"

// For Achievements
#include "achievements.h"

//  Constants
const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 800;
const int DIE_SIZE = 100;
const int BUTTON_HEIGHT = 50;
const int NUM_DICE = 6; 

class Button {
    public:
        SDL_Rect rect;
        std::string label;
        SDL_Color color;
        std::function<void()> onClick;
        
        // This is for the being able to roll again only if a button is selected
        bool hasBeenSelected;

        bool getSelected();
        std::string getLabel();
        void toggleSelected();
        bool isClicked(int x, int y);
        void render(SDL_Renderer* renderer, TTF_Font* font);
};

class Dice {
    public:
        int value;
        bool held;
        SDL_Rect rect;

        Dice(int v = 1, bool h = false, SDL_Rect r = {0, 0, 100, 100});
        bool operator==(int v) const;
        void roll();
};

enum RollType {
    SIX_OF_A_KIND, STRAIGHT, THREE_PAIRS,
    THREE_OF_A_KIND, FOUR_OF_A_KIND,
    FIVE_OF_A_KIND, SINGLE_1,
    DOUBLE_1, SINGLE_5, DOUBLE_5,
    NOTHING, ALL_HELD, ZILCH
};

bool isStraight(const std::vector<Dice>& dice);

// Game class to manage dice rolling and display
class Game {
    public:
        Game();
        void rollDice(SDL_Renderer* renderer, TTF_Font* font, SDL_Texture* bgTexture, SDL_Rect bgRect);
        void toggleHold(int dieIndex);
        void displayDice(SDL_Renderer* renderer);
        void addHoldButton(const std::string& text, int x, int y, std::function<void(Button&)> action);
        void getPossibleHolds();
        std::vector<Button>& getHoldButtons() { return holdButtons; }
        std::vector<Dice>& getDice() {return die;}
        bool getButtonLock(){return lockOtherButtons;}
        bool getReverseButtonLock(){return reverseLockOtherButtons;}
    
        // For Players
        void nextTurn();

        // aiType: "aggressive" or "cautious" (only used if isAI is true)
        void addPlayer(std::string name, bool isAI = false, const std::string& aiType = "");
        void setFirstTurn();
        int getCurrentPlayer(){ return currentPlayerIndex;}
        bool getCurrentPlayerIsAI(){return players[getCurrentPlayer()]->isAIPlayer();}


        void displaySoftScore(SDL_Renderer* renderer, TTF_Font* font);
        void displayHardScore(SDL_Renderer* renderer, TTF_Font* font);
        int manyOfAKindPoints(int diceNumber, int numberOfDice);
        void bankCurrentPlayerScore();
        std::vector<std::unique_ptr<Player>>& getPlayers(){return players;}
    
    
        // FOR WIN CONDITIONS
        void setWinConditionPoints(int WinningNumber){winningPoints = WinningNumber;}  //Created for custom games
        int getWinConditionPoints(){ return winningPoints;} 
        bool checkGameEnd();
        std::string getWinningPlayerName(Achievements& achievements, int statisticsUpdated);
        void restartGame();
        void clearGame();

        // For Displaying History
        void displayHistory(SDL_Renderer* renderer, TTF_Font* font, std::unique_ptr<Player>& currentPlayer);

    private:
        std::vector<Dice> die;
        std::vector<Button> holdButtons;
        std::vector<int> previousHeldDice;
    
        // For Players
        std::vector<std::unique_ptr<Player>> players; // Store players
        int currentPlayerIndex; // Track whose turn it is
    
        //For win conditions
        int winningPoints;
        bool gameOver;

        // For buttons
        bool lockOtherButtons = false;
        bool reverseLockOtherButtons = false;

        // For Achievement instances
        Achievements achievements;  // Add an Achievements instance
    };


#endif
