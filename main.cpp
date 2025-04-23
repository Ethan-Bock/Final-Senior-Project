#include <SDL.h>
#include <SDL_ttf.h>
#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <functional>
#include "game.h"
#include <limits.h>
#include "players.h"
#include "achievements.h"

// Allows to display text
void renderText(SDL_Renderer *renderer, TTF_Font *font, const std::string &text, SDL_Color color, int x, int y) {
    SDL_Surface *surface = TTF_RenderText_Solid(font, text.c_str(), color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect textRect = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, nullptr, &textRect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

//Creation of Slider
struct Slider {
    int x, y, w, h;
    int minValue, maxValue, step, value;
    SDL_Rect bar, handle;
    bool dragging = false;

    Slider(int _x, int _y, int _w, int _h, int _min, int _max, int _step, int _value)
        : x(_x), y(_y), w(_w), h(_h), minValue(_min), maxValue(_max), step(_step), value(_value) {
        bar = {x, y, w, h};
        handle = {x+(w/2), y-3, h, h+6}; // Handle starts at the left
    }

    int getValue() const {
        return value;
    }

    void render(SDL_Renderer* renderer) {
        // Draw bar
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
        SDL_RenderFillRect(renderer, &bar);

        // Draw handle
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &handle);
    }

    void handleEvent(SDL_Event& e) {
        if (e.type == SDL_MOUSEBUTTONDOWN) {
            int mouseX = e.button.x, mouseY = e.button.y;
            if (mouseX >= handle.x && mouseX <= handle.x + handle.w &&
                mouseY >= handle.y && mouseY <= handle.y + handle.h) {
                dragging = true;
            }
        } else if (e.type == SDL_MOUSEBUTTONUP) {
            dragging = false;
        } else if (e.type == SDL_MOUSEMOTION && dragging) {
            int newX = e.motion.x - handle.w / 2;
            if (newX < x) newX = x;
            if (newX > x + w - handle.w) newX = x + w - handle.w;

            handle.x = newX;
            float percent = (float)(handle.x - x) / (w - handle.w);
            value = minValue + (int)(percent * (maxValue - minValue));

            // Snap to nearest step
            int remainder = (value - minValue) % step;
            if (remainder >= step / 2) value += (step - remainder);
            else value -= remainder;
        }
    }
};


class Menu {
    SDL_Renderer* renderer;
    TTF_Font* font;
    std::vector<std::string> labels;
    std::vector<SDL_Texture*> menuTextures;
    std::vector<SDL_Rect> positions;
    int selectedItem = 0;
    int selectedSubMenuItem = 0;
    bool hoverStartButton = false;

    bool expandButton1 = false; // Track if button1 is expanded
    std::vector<std::string> subMenuLabels = {"Aggressive AI", "Cautious AI", "Adaptive AI"};
    std::vector<SDL_Rect> subMenuPositions;
    Slider winningPointsSlider = Slider(300, 250, 400, 20, 1000, 20000, 500, 10000);

    //FOR TWO PLAYER SUBMENU
    SDL_Rect player1InputBox = {170, 300, 410, 30};  // Input box for Player 1
    SDL_Rect player2InputBox = {170, 350, 410, 30};  // Input box for Player 2
    SDL_Rect startButton = {610, 315, 160, 50};  // Button to start the game

    bool inputBoxActive1 = false, inputBoxActive2 = false;  // Track active text inputs
    std::string player1_name = "Player 1", player2_name = "Player 2"; // Default names
    
    //FOR TUTORIAL
    int selectedTutorialIndex = 0;
    int currentTutorialIndex = 0;
    std::vector<std::string> tutorialLabels = {"Rules", "Controls", "Points", "Hands"};
    std::vector<SDL_Rect> tutorialPositions;
    std::vector<Dice> tutorialDie;

public:
    int currentSelectedItem = -1;

    Menu(SDL_Renderer* ren, TTF_Font* f, std::vector<std::string> options) 
        : renderer(ren), font(f), labels(options) {

        tutorialDie.resize(NUM_DICE); //Resize for dice
        int startY = 450; // Starting Y position
        int spacingX = SCREEN_WIDTH / 4;  // Space for first row (3 buttons)
        int centerX = SCREEN_WIDTH / 2;   // Center for second row

        for (size_t i = 0; i < labels.size(); ++i) {
            SDL_Surface* textSurface = TTF_RenderText_Solid(font, labels[i].c_str(), {255, 255, 255});
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, textSurface);
            menuTextures.push_back(texture);

            SDL_Rect rect;
            
            if (i < 3) { 
                // First row (button1, button2, button3)
                rect = {spacingX * (i + 1) - textSurface->w / 2, startY, textSurface->w, textSurface->h};
            } else { 
                // Second row (button4, button5)
                int secondRowY = startY + 80; // Increase Y position for second row
                int xOffset = (i == 3) ? centerX - SCREEN_WIDTH / 6 : centerX + SCREEN_WIDTH / 6; // Offset for button4 & button5
                
                rect = {xOffset - textSurface->w / 2, secondRowY, textSurface->w, textSurface->h};
            }

            positions.push_back(rect);
            SDL_FreeSurface(textSurface);
        }
    }

    ~Menu() {
        for (auto tex : menuTextures) {
            SDL_DestroyTexture(tex);
        }
    }

    void render() {
        //TITLE CARD
        if (currentSelectedItem < 2) {
            TTF_Font* titleFont = TTF_OpenFont("assets/fonts/TiltPrism.ttf", 120);
            std::string TitleText = "ZILCH";
            SDL_Color textColor = {255, 255, 255};  
            renderText(renderer, titleFont, TitleText, textColor, (SCREEN_WIDTH / 2) - 150 , 50);
            TTF_CloseFont(titleFont);
        }

        if (currentSelectedItem != 3) {
            for (size_t i = 0; i < labels.size(); ++i) {
                SDL_Color color = (i == selectedItem) ? SDL_Color{255, 0, 0} : SDL_Color{255, 255, 255};
                renderText(renderer, font, labels[i], color, positions[i].x, positions[i].y);
            }
        }

        //Render submenu buttons with outline
        renderSubMenu(renderer);

        //Renders tutorial if in tutorial
        renderTutorial(renderer);

    }

    void renderSubMenu(SDL_Renderer* renderer) {
        if (currentSelectedItem < 0 || currentSelectedItem > 2) return;
        if (currentSelectedItem == 0){
            for (size_t i = 0; i < subMenuPositions.size(); ++i) {
                SDL_Rect rect = subMenuPositions[i];
        
                // Fill the button with a solid color
                SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
                SDL_RenderFillRect(renderer, &rect);
        
                // Draw the outline of the button
                SDL_Color textColor = (i == selectedSubMenuItem) ? SDL_Color{255, 0, 0} : SDL_Color{255, 255, 0}; // Red for selected, Yellow for others

                // Render the start button background
                // Change outline color based on selection
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                SDL_RenderDrawRect(renderer, &rect);
        
                // Render the text inside the button
                //SDL_Color textColor = {255, 255, 0}; // Yellow text
                renderText(renderer, font, subMenuLabels[i], textColor, rect.x + 8, rect.y + 5);
            }

            // Render the slider
            winningPointsSlider.render(renderer);
            // Render slider value above the slider
            std::string sliderValueText = "Win Condition: " + std::to_string(winningPointsSlider.getValue());
            SDL_Color textColor = {255, 255, 255};  
            renderText(renderer, font, sliderValueText, textColor, winningPointsSlider.x, winningPointsSlider.y - 30);
        }

        if (currentSelectedItem == 1){
            // Render the slider
            winningPointsSlider.render(renderer);
            // Render slider value above the slider
            std::string sliderValueText = "Win Condition: " + std::to_string(winningPointsSlider.getValue());
            SDL_Color sliderColor = {255, 255, 255};  
            renderText(renderer, font, sliderValueText, sliderColor, winningPointsSlider.x, winningPointsSlider.y - 30);
            
            std::string displayedName1 = player1_name.empty() ? "Player 1" : player1_name;
            std::string displayedName2 = player2_name.empty() ? "Player 2" : player2_name;


            // Render the player input boxes
            renderText(renderer, font, displayedName1, {255, 255, 255}, player1InputBox.x + 5, player1InputBox.y);
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Draw the outline of the input box
            SDL_RenderDrawRect(renderer, &player1InputBox);

            renderText(renderer, font, displayedName2, {255, 255, 255}, player2InputBox.x + 5, player2InputBox.y);
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Draw the outline of the input box
            SDL_RenderDrawRect(renderer, &player2InputBox);
            
            // Render the start button
            // Change color if the mouse is hovering over the start button
            SDL_Color textColor = hoverStartButton ? SDL_Color{255, 0, 0} : SDL_Color{255, 255, 0}; // Red text if hovered

            // Render the start button background
            SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
            SDL_RenderFillRect(renderer, &startButton);

            // Render the button outline
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderDrawRect(renderer, &startButton);

            // Render the text inside the button
            renderText(renderer, font, "Start Game", textColor, startButton.x + 10, startButton.y + 10);
        }

        if(currentSelectedItem == 2){
            SDL_Surface* medalUnlockedSurface = IMG_Load("assets/textures/medal_unlocked.png");
            SDL_Surface* medalLockedSurface = IMG_Load("assets/textures/medal_locked.png");
            if (!medalUnlockedSurface || !medalLockedSurface) {
                std::cerr << "Failed to load images SDL_image Error: " << IMG_GetError() << std::endl;
            }

            SDL_Texture* medalUnlocked = SDL_CreateTextureFromSurface(renderer, medalUnlockedSurface);
            SDL_Texture* medalLocked = SDL_CreateTextureFromSurface(renderer, medalLockedSurface);

            SDL_FreeSurface(medalUnlockedSurface);
            SDL_FreeSurface(medalLockedSurface);
            
            renderAwardsAndStatistics(renderer, font, medalUnlocked, medalLocked);
        }
    }




    void displayPossibleHolds(SDL_Renderer *renderer, TTF_Font *font, const std::vector<Dice>& tutorialDie) {
        std::map<int, int> counts;
        for (const Dice& die : tutorialDie) {
            counts[die.value]++;  // Count how many times each die value appears
        }
    
        int startX = 50;
        int startY = 400;
        int yOffset = 60;
    
        bool nothing = true; // Flag to check if there's a valid combination
    
        SDL_Color textColor = {255, 255, 255};  // White color for the text
    
        // Check for Straight
        if (tutorialStraight(tutorialDie)) {
            renderText(renderer, font, "Straight  ->  1750 points", textColor, startX, startY);
            startY += yOffset;
            nothing = false;
        }
    
        // Check for Three Pairs
        if (counts.size() == 3 && std::all_of(counts.begin(), counts.end(), [](const std::pair<int, int>& p) { return p.second == 2; })) {
            renderText(renderer, font, "Three Pairs  ->  1500 points", textColor, startX, startY);
            startY += yOffset;
            nothing = false;
        }
    
        // Check for 6 of a Kind
        if (auto it = std::find_if(counts.begin(), counts.end(), [](const std::pair<int, int>& p) { return p.second == 6; }); it != counts.end()) {
            std::string label = "Six " + std::to_string(it->first) + "s  ->  2500 points";
            renderText(renderer, font, label, textColor, startX, startY);
            startY += yOffset;
            nothing = false;
        }
    
        // Check for 5 of a Kind
        if (auto it = std::find_if(counts.begin(), counts.end(), [](const std::pair<int, int>& p) { return p.second == 5; }); it != counts.end()) {
            std::string label = "Five " + std::to_string(it->first) + "s  ->  " + std::to_string(tutorialManyOfAKindPoints(it->first, 5)) + " points";
            renderText(renderer, font, label, textColor, startX, startY);
            startY += yOffset;
            nothing = false;
        }
    
        // Check for 4 of a Kind
        if (auto it = std::find_if(counts.begin(), counts.end(), [](const std::pair<int, int>& p) { return p.second == 4; }); it != counts.end()) {
            std::string label = "Four " + std::to_string(it->first) + "s  ->  " + std::to_string(tutorialManyOfAKindPoints(it->first, 4)) + " points";
            renderText(renderer, font, label, textColor, startX, startY);
            startY += yOffset;
            nothing = false;
        }
    
        // Check for 3 of a Kind
        for (const auto& pair : counts) {
            if (pair.second == 3) {
                std::string label = "Three " + std::to_string(pair.first) + "s  ->  " + std::to_string(tutorialManyOfAKindPoints(pair.first, 3)) + " points";
                renderText(renderer, font, label, textColor, startX, startY);
                startY += yOffset;
                nothing = false;
            }
        }
    
        // Check for Single 1
        if (counts[1] == 1) {
            renderText(renderer, font, "Single 1  ->  100 points", textColor, startX, startY);
            startY += yOffset;
            nothing = false;
        }
    
        // Check for Double 1
        if (auto it = std::find_if(counts.begin(), counts.end(), [](const std::pair<int, int>& p) { return (p.second == 2 && p.first == 1); }); it != counts.end()) {
            renderText(renderer, font, "Double 1  ->  200 points", textColor, startX, startY);
            startY += yOffset;
            nothing = false;
        }
    
        // Check for Single 5
        if (counts[5] == 1) {
            renderText(renderer, font, "Single 5  ->  50 points", textColor, startX, startY);
            startY += yOffset;
            nothing = false;
        }
    
        // Check for Double 5
        if (auto it = std::find_if(counts.begin(), counts.end(), [](const std::pair<int, int>& p) { return (p.second == 2 && p.first == 5); }); it != counts.end()) {
            renderText(renderer, font, "Double 5  ->  100 points", textColor, startX, startY);
            startY += yOffset;
            nothing = false;
        }
    
        // If no valid combination, show "Nothing"
        if (tutorialDie.size() == 6 && nothing) {
            renderText(renderer, font, "Nothing  ->  500 points", textColor, startX, startY);
            startY += yOffset;
        }
    }

    int tutorialManyOfAKindPoints(int diceNumber, int numberOfDice) {
        // Base points for three-of-a-kind
        int basePoints[] = {0, 1000, 200, 300, 400, 500, 600}; 
        
        if (diceNumber < 1 || diceNumber > 6 || numberOfDice < 3) {
            return 0; // Invalid input case
        }
        
        return basePoints[diceNumber] * (1 << (numberOfDice - 3)); // Multiply by 2^(numberOfDice - 3)
    }

    bool tutorialStraight(const std::vector<Dice>& dice) {

        if(dice.size() != 6){return false;}
    
        std::vector<int> sortedDice;
        for (const auto& die : dice) {sortedDice.push_back(die.value);}
        
        std::sort(sortedDice.begin(), sortedDice.end());
    
        // Check for duplicates
        for (std::size_t i = 0; i < sortedDice.size() - 1; ++i) {
            if (sortedDice[i] == sortedDice[i + 1]) {
                return false;
            }
        }
    
        // Check for consecutive numbers
        for (std::size_t i = 0; i < sortedDice.size() - 1; ++i) {
            if (sortedDice[i] + 1 != sortedDice[i + 1]) {
                return false;
            }
        }
        return true;
    }

    void renderTutorial(SDL_Renderer* renderer) {
        if (currentSelectedItem != 3) return;

        SDL_Color normalColor = {255, 255, 255};
        SDL_Color selectedColor = {173, 216, 230}; // light blue

        // ------------------------ Top Buttons ------------------------
        for (int i = 0; i < 4; ++i) {
            SDL_Rect rect = tutorialPositions[i];
            SDL_SetRenderDrawColor(renderer, (i == currentTutorialIndex) ? selectedColor.r : 50,
                                (i == currentTutorialIndex) ? selectedColor.g : 50,
                                (i == currentTutorialIndex) ? selectedColor.b : 50, 255);
            SDL_RenderFillRect(renderer, &rect);
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderDrawRect(renderer, &rect);
            renderText(renderer, font, tutorialLabels[i], normalColor, rect.x + 10, rect.y + 5);
        }

        // ------------------------ Bottom-Right Button ------------------------
        SDL_Rect bottomRight = {750, 710, 190, 40}; // Adjust as needed
        SDL_SetRenderDrawColor(renderer, 80, 80, 80, 255);
        SDL_RenderFillRect(renderer, &bottomRight);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(renderer, &bottomRight);
        renderText(renderer, font, "Back to Menu", normalColor, bottomRight.x + 10, bottomRight.y + 5);

        // ------------------------ Tutorial Content ------------------------
        if (currentTutorialIndex == 0) { //Rules
            //SCORING
            renderText(renderer, font, "SCORING", normalColor, 50, 90);
            renderText(renderer, font, "Roll the dice to start your turn. If you roll scoring dice hand, then", normalColor, 50, 120);
            renderText(renderer, font, "you must aquire soft-points before you can roll again. You aquire", normalColor, 50, 150);
            renderText(renderer, font, "soft-points by selecting one or more combinations to hold. Which will", normalColor, 50, 180);
            renderText(renderer, font, "hold the corresponding dice. Held dice won't be rolled, unless all", normalColor, 50, 210);
            renderText(renderer, font, "six dice are held. If all six dice are held the next roll, will reroll", normalColor, 50, 240);
            renderText(renderer, font, "all die. If you don't roll a scoring hand, then you rolled a zilch.", normalColor, 50, 270);
            renderText(renderer, font, "You will lose all soft-points and your turn ends. If you end 3 turns", normalColor, 50, 300);
            renderText(renderer, font, "in a row through zilches you lose 500 hard-points.", normalColor, 50, 330);

            //BANKING
            renderText(renderer, font, "BANKING", normalColor, 50, 380);
            renderText(renderer, font, "Banking allows the player to transform all the soft-points they have", normalColor, 50, 410);
            renderText(renderer, font, "accumulated into hard-points and doing so will end the players turn.", normalColor, 50, 440);
            renderText(renderer, font, "A player must have accumulated 300 or more points in order to bank.", normalColor, 50, 470);

            //WINNING
            renderText(renderer, font, "WINNING", normalColor, 50, 520);
            renderText(renderer, font, "In order for a player to win, their hard-points must exceed the winning", normalColor, 50, 550);
            renderText(renderer, font, "of the game. After one player passes the threshold of points needed to", normalColor, 50, 580);
            renderText(renderer, font, "win, the other player is given one last chance to catch up and try to win.", normalColor, 50, 610);
            renderText(renderer, font, "When the other player either banks or rolls a zilch, the hard-points of", normalColor, 50, 640);
            renderText(renderer, font, "each player are compared and the one with more points wins.", normalColor, 50, 670);

        } else if (currentTutorialIndex == 1) { //Controls
            renderText(renderer, font, "All interactivity is used by clicking using the left mouse button.", normalColor, 50, 120);
            renderText(renderer, font, "Buttons such as those used to select combinations can be clicked again", normalColor, 50, 170);
            renderText(renderer, font, "to toggle them on and off.", normalColor, 50, 200);
            renderText(renderer, font, "While an AI is taking its turn the player can't interact with the game.", normalColor, 50, 250);      

        } else if (currentTutorialIndex == 2) { //Points
            renderText(renderer, font, "Here are all possible combinations of points and example hands:", normalColor, 50, 85);
            renderText(renderer, font, "Special Cases:", normalColor, 50, 125);
            renderText(renderer, font, "Straight  [(1, 2, 3, 4, 5, 6),   (4, 6, 3, 1, 5, 2)]  -->", normalColor, 50, 155);
            renderText(renderer, font, "1750 points", normalColor, 720, 155);
            renderText(renderer, font, "Three Pairs  [(2, 2, 3, 3, 5, 5),   (1, 2, 5, 1, 5, 2)]  -->", normalColor, 50, 185);
            renderText(renderer, font, "1500 points", normalColor, 720, 185);
            renderText(renderer, font, "Any Six of a kind  [(2, 2, 2, 2, 2, 2),   (1, 1, 1, 1, 1, 1)]  -->", normalColor, 50, 215);
            renderText(renderer, font, "2500 points", normalColor, 720, 215);


            renderText(renderer, font, "Singles and Doubles Cases:", normalColor, 50, 265);
            renderText(renderer, font, "Single 1  [(1, 2, 3, 4, 4, 3),   (4, 4, 3, 1, 5, 2)]  -->", normalColor, 50, 295);
            renderText(renderer, font, "100 points", normalColor, 720, 295);
            renderText(renderer, font, "Double 1  [(1, 1, 3, 4, 4, 2),   (1, 2, 3, 1, 5, 2)]  -->", normalColor, 50, 325);
            renderText(renderer, font, "200 points", normalColor, 720, 325);
            renderText(renderer, font, "Single 5  [(5, 2, 3, 2, 3, 6),   (1, 5, 2, 2, 3, 6)]  -->", normalColor, 50, 355);
            renderText(renderer, font, "50 points", normalColor, 720, 355);
            renderText(renderer, font, "Double 5  [(5, 5, 1, 3, 2, 3),   (5, 1, 3, 1, 6, 5)]  -->", normalColor, 50, 385);
            renderText(renderer, font, "100 points", normalColor, 720, 385);

            renderText(renderer, font, "#-of-a-Kind Cases:", normalColor, 50, 435);
            renderText(renderer, font, "When calculating a #-of-a-Kind points case an equation is used", normalColor, 50, 465);
            renderText(renderer, font, "(100 * face-of-dice) * 2", normalColor, 50, 495);
            TTF_Font* exponentFont = TTF_OpenFont("assets/fonts/Rye-Regular.ttf", 15);
            std::string exponentText = "(#-of-dice - 3)";
            SDL_Color textColor = {255, 255, 255};  
            renderText(renderer, exponentFont, exponentText, textColor, 330, 495);
            renderText(renderer, font, "However if the face of the die is a 1 it uses a different function", normalColor, 50, 525);
            renderText(renderer, font, "1000 * 2", normalColor, 50, 555);
            exponentText = "(#-of-dice - 3)";  
            renderText(renderer, exponentFont, exponentText, textColor, 155, 555);
            TTF_CloseFont(exponentFont);

            renderText(renderer, font, "Very Special Case:", normalColor, 50, 605);
            renderText(renderer, font, "Nothing [(2, 2, 3, 6, 4, 3), (4, 4, 3, 6, 6, 2)]  -->", normalColor, 50, 635);
            renderText(renderer, font, "500 points", normalColor, 720, 635);
            renderText(renderer, font, "This only appears if no die are held and no other combos are found.", normalColor, 75, 665);
        
        } else if (currentTutorialIndex == 3) { //Hands
            // ------------------------ Render Dice ------------------------
            renderText(renderer, font, "Click the dice to cycle through their numbers and show possible hands.", normalColor, 50, 120);
            
            int diceX = 100;
            int diceY = 250;
            int diceSize = 75;
            for (int i = 0; i < 6; ++i) {
                std::string imagePath = "assets/textures/" + std::to_string(tutorialDie[i].value) + "-face.png";
                SDL_Surface* diceSurface = IMG_Load(imagePath.c_str());

                if (!diceSurface) {
                    std::cerr << "Failed to load image: " << imagePath 
                            << " SDL_image Error: " << IMG_GetError() << std::endl;
                } else {
                    SDL_Texture* diceTexture = SDL_CreateTextureFromSurface(renderer, diceSurface);
                    SDL_FreeSurface(diceSurface);
                    SDL_Rect tutorialDiceRect = {diceX, diceY, diceSize, diceSize};
                    SDL_RenderCopy(renderer, diceTexture, nullptr, &tutorialDiceRect);
                    SDL_DestroyTexture(diceTexture);
                    tutorialDie[i].rect = tutorialDiceRect;
                    diceX += 120;
                }
            }

            // ------------------------ Render Hand Name + Points ------------------------
            displayPossibleHolds(renderer, font, tutorialDie);
        }
    }



    void handleEvent(SDL_Event& e, Game& game, SDL_Renderer* renderer, bool& inMenu, bool& startGame, bool& inTutorial) {
        winningPointsSlider.handleEvent(e);

        int winCondition = winningPointsSlider.value;
        if (e.type == SDL_MOUSEMOTION) {
            int mouseX = e.motion.x;
            int mouseY = e.motion.y;
    
            // Check if the mouse is hovering over any menu item
            for (size_t i = 0; i < positions.size(); ++i) {
                if (mouseX >= positions[i].x && mouseX <= positions[i].x + positions[i].w &&
                    mouseY >= positions[i].y && mouseY <= positions[i].y + positions[i].h) {
                    selectedItem = i;  // Highlight the hovered item
                }
            }
            if (currentSelectedItem == 0) {
                for (size_t i = 0; i < subMenuPositions.size(); ++i) {
                    SDL_Rect& subButton = subMenuPositions[i]; // Reference for clarity
    
                    if (mouseX >= subButton.x && mouseX <= subButton.x + subButton.w &&
                        mouseY >= subButton.y && mouseY <= subButton.y + subButton.h) {
                        selectedSubMenuItem = i;
                    }
                }
            }
            if (currentSelectedItem == 1) {
                if (mouseX >= startButton.x && mouseX <= startButton.x + startButton.w &&
                    mouseY >= startButton.y && mouseY <= startButton.y + startButton.h) {
                    hoverStartButton = true;
                }else{
                    hoverStartButton = false;
                }
            }
            if (currentSelectedItem == 3) {
                for (size_t i = 0; i < tutorialPositions.size(); ++i) {
                    SDL_Rect& subButton = tutorialPositions[i]; // Reference for clarity
    
                    if (mouseX >= subButton.x && mouseX <= subButton.x + subButton.w &&
                        mouseY >= subButton.y && mouseY <= subButton.y + subButton.h) {
                        selectedTutorialIndex = i;
                    }
                }
            }
        }
    
        if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
            int mouseX = e.button.x;
            int mouseY = e.button.y;
    
            // Check if a menu item is clicked
            for (size_t i = 0; i < positions.size(); ++i) {
                if (mouseX >= positions[i].x && mouseX <= positions[i].x + positions[i].w &&
                    mouseY >= positions[i].y && mouseY <= positions[i].y + positions[i].h) {
                    selectedItem = i; // Ensure correct selection

                    if (selectedItem == 0) { // 1 Player (VS. CPU)
                        currentSelectedItem = 0;
                        generateSubMenu(font);


                    } else if (selectedItem == 1) { // 2 Player Game
                        currentSelectedItem = 1;
                        // CREATE PLAYERS

                    } else if (selectedItem == 2) { // Awards & Statistics
                        currentSelectedItem = 2;
                    } else if (selectedItem == 3) { // Rules and Scoring
                        currentSelectedItem = 3;
                        generateTutorial(font);


                    } else if (selectedItem == 4) { // QUIT
                        SDL_Quit();
                        exit(0);
                    }
                }
            }

            if (currentSelectedItem == 0) {
                for (size_t i = 0; i < subMenuPositions.size(); ++i) {
                    SDL_Rect& subButton = subMenuPositions[i]; // Reference for clarity
    
                    if (mouseX >= subButton.x && mouseX <= subButton.x + subButton.w &&
                        mouseY >= subButton.y && mouseY <= subButton.y + subButton.h) {
                        
                        if (i == 0) {
                            std::string player1_name = "Player 1";
                            std::string player2_name = "Aggressive AI";
                            game.addPlayer(player1_name);
                            game.addPlayer(player2_name, true, "aggressive");

                            game.setWinConditionPoints(winCondition);
                            game.setFirstTurn();
                            inMenu = false;
                            startGame = true;

                        } else if (i == 1) {
                            std::string player1_name = "Player 1";
                            std::string player2_name = "Cautious AI";
                            game.addPlayer(player1_name);
                            game.addPlayer(player2_name, true, "cautious");

                            game.setWinConditionPoints(winCondition);
                            game.setFirstTurn();
                            inMenu = false;
                            startGame = true;
                        } else if (i == 2) {
                            std::string player1_name = "Player 1";
                            std::string player2_name = "Adaptive AI";
                            game.addPlayer(player1_name);
                            game.addPlayer(player2_name, true, "adaptive");

                            game.setWinConditionPoints(winCondition);
                            game.setFirstTurn();
                            inMenu = false;
                            startGame = true;
                        }
                    }
                }
            }

            // Handle events for submenu when "2 Player Game" is selected
            if (currentSelectedItem == 1) {
                if (mouseX >= player1InputBox.x && mouseX <= player1InputBox.x + player1InputBox.w &&
                    mouseY >= player1InputBox.y && mouseY <= player1InputBox.y + player1InputBox.h) {
                    inputBoxActive1 = true;  // Activate Player 1 input box
                    inputBoxActive2 = false;
                } else if (mouseX >= player2InputBox.x && mouseX <= player2InputBox.x + player2InputBox.w &&
                           mouseY >= player2InputBox.y && mouseY <= player2InputBox.y + player2InputBox.h) {
                    inputBoxActive2 = true;  // Activate Player 2 input box
                    inputBoxActive1 = false;
                } else {
                    inputBoxActive1 = false;
                    inputBoxActive2 = false;  // Deactivate both input boxes if the click is outside
                }
            
                if (mouseX >= startButton.x && mouseX <= startButton.x + startButton.w &&
                    mouseY >= startButton.y && mouseY <= startButton.y + startButton.h) {

                    // Just incase starting with blank name
                    std::string playerName1 = player1_name.empty() ? "Player 1" : player1_name;
                    std::string playerName2 = player2_name.empty() ? "Player 2" : player2_name;

                    game.addPlayer(playerName1);
                    game.addPlayer(playerName2);
                    game.setWinConditionPoints(winCondition);
                    game.setFirstTurn();
                    inMenu = false;
                    startGame = true;
                }
            }

            // Deactivate adding to inputs
            if (currentSelectedItem != 1) {
                inputBoxActive1 = false;
                inputBoxActive2 = false;
            }

            if (currentSelectedItem == 3) {
                for (size_t i = 0; i < tutorialPositions.size(); ++i) {
                    SDL_Rect& subTutorialButton = tutorialPositions[i]; // Reference for clarity
                    if (mouseX >= subTutorialButton.x && mouseX <= subTutorialButton.x + subTutorialButton.w &&
                        mouseY >= subTutorialButton.y && mouseY <= subTutorialButton.y + subTutorialButton.h) {
                        if (i == 0) {           //Intro
                            currentTutorialIndex = 0;
                        } else if (i == 1) {    //Controls
                            currentTutorialIndex = 1;
                        } else if (i == 2) {    //Tips
                            currentTutorialIndex = 2;
                        } else if (i == 3) {    //Hand button
                            currentTutorialIndex = 3;
                        }
                    }
                }

                //CHECK FOR BOTTOM RIGHT BUTTON     SDL_Rect bottomRight = {750, 710, 190, 40}; // Adjust as needed
                if (mouseX >= 750 && mouseX <= 750 + 190 &&
                    mouseY >= 710 && mouseY <= 710 + 40) {
                    //SENDS BACK TO MENU
                    selectedItem = 3;
                    currentSelectedItem = -1;
                    currentTutorialIndex = 0;
                }

                // HANDLE CLICKING DICE TO CHANGE THEM
                for (size_t i = 0; i < tutorialDie.size(); ++i) {
                    SDL_Rect& tutorialDice = tutorialDie[i].rect;
                    if (mouseX >= tutorialDice.x && mouseX <= tutorialDice.x + tutorialDice.w &&
                        mouseY >= tutorialDice.y && mouseY <= tutorialDice.y + tutorialDice.h) {
                        tutorialDie[i].value = (tutorialDie[i].value % 6) + 1;
                    }
                }
            }            
        }

        if (e.type == SDL_TEXTINPUT) {
            if (inputBoxActive1 && player1_name.length() < 12) {
                player1_name += e.text.text;  // Append the typed character to Player 1's name
            } else if (inputBoxActive2 && player2_name.length() < 12) {
                player2_name += e.text.text;  // Append the typed character to Player 2's name
            }
        }

        if (e.type == SDL_KEYDOWN) {
            if (e.key.keysym.sym == SDLK_BACKSPACE) {
                if (inputBoxActive1 && player1_name.length() > 0) {
                    player1_name.pop_back();  // Remove last character from Player 1's name
                } else if (inputBoxActive2 && player2_name.length() > 0) {
                    player2_name.pop_back();  // Remove last character from Player 2's name
                }
            }
        }

    }

    void generateSubMenu(TTF_Font* font) {
        subMenuPositions.clear();
    
        int button1X = positions[0].x + 150;
        int button1Y = positions[0].y - 75; // Position above button1
    
        int totalWidth = 0;
        std::vector<int> buttonWidths;
    
        // Measure text sizes and store widths
        for (const auto& label : subMenuLabels) {
            int textW, textH;
            TTF_SizeText(font, label.c_str(), &textW, &textH);
            int buttonW = textW + 10; // Add 5px margin on both sides
            buttonWidths.push_back(buttonW);
            totalWidth += buttonW + 15; // Add spacing
        }
    
        totalWidth -= 15; // Remove last extra spacing
        int startX = button1X + (positions[0].w / 2) - (totalWidth / 2); // Center the submenu
    
        for (size_t i = 0; i < subMenuLabels.size(); ++i) {
            SDL_Rect rect = {
                startX - 5, 
                button1Y - 50, 
                buttonWidths[i] + 10, 
                40  // Keep height fixed or set `textH + 10`
            };
            subMenuPositions.push_back(rect);
            startX += buttonWidths[i] + 100; // Move to next position
        }
    }

    void generateTutorial(TTF_Font* font) {
        tutorialPositions.clear();
        int buttonWidth = 150;
        int buttonHeight = 40;
        int topMargin = 30;
        int spacing = 50;
        int totalWidth = tutorialLabels.size() * buttonWidth + (tutorialLabels.size() - 1) * spacing;
        int startX = (1000 - totalWidth) / 2;
        
        for (int i = 0; i < 4; ++i) {
            SDL_Rect rect = {
                startX + i * (buttonWidth + spacing),
                topMargin,
                buttonWidth,
                buttonHeight
            };
            tutorialPositions.push_back(rect);
        }
    }

    void renderAwardsAndStatistics(SDL_Renderer* renderer, TTF_Font* font, SDL_Texture* medalUnlocked, SDL_Texture* medalLocked) {
        SDL_Color textColor = {255, 255, 255};
    
        // Load statistics and achievements
        std::unordered_map<std::string, Statistic> stats = Achievements::getStatistics();
        std::vector<Achievement> achievementList = Achievements::getAchievements();
    
        int statX = 50;    // Left margin for stats
        int statY = 100;   // Start position for stats
        int spacing = 50;  // Space between each stat
    
        int medalX = 600;  // Right margin for achievements
        int medalY = 100;  // Align medals with statistics
        int medalSpacing = 60;
    
        int medalSize = 50; // Size of the medal icon
    
        int index = 0;
    
        // Render statistics on the left
        for (const auto& stat : stats) {
            std::string statText = stat.second.name + ": " + std::to_string(stat.second.count);
            renderText(renderer, font, statText, textColor, statX, statY + (index * spacing));
            index++;
        }
    
        // Render medals on the right
        for (size_t i = 0; i < achievementList.size(); ++i) {
            Achievement& ach = achievementList[i];
    
            SDL_Rect medalRect = {medalX + (i % 5) * medalSpacing, medalY + (i / 5) * (medalSize + 10), medalSize, medalSize};
    
            SDL_Texture* texture = ach.unlocked ? medalUnlocked : medalLocked;
            SDL_RenderCopy(renderer, texture, nullptr, &medalRect);
    
            // If hovered, display achievement details
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);
    
            if (mouseX >= medalRect.x && mouseX <= medalRect.x + medalSize &&
                mouseY >= medalRect.y && mouseY <= medalRect.y + medalSize) {
                std::string details = ach.name + " - ";
                renderText(renderer, font, details, textColor, 50, 600); //  Name of achievement
                renderText(renderer, font, ach.description, textColor, 50, 630);    // Line 2: Description (slightly lower)
            }
        }
    }

};






int main() {
    // Initialize SDL and SDL_ttf
    if (SDL_Init(SDL_INIT_VIDEO) < 0 || TTF_Init() < 0) {
        std::cerr << "SDL or SDL_ttf could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    //Create a Window
    SDL_Window *window = SDL_CreateWindow("Zilch", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    TTF_Font* font = TTF_OpenFont("assets/fonts/Rye-Regular.ttf", 24);
    if (!window || !renderer || !font) {
        std::cerr << "Failed to initialize resources! SDL_Error: " << SDL_GetError() << std::endl;
        return -1;
    }


    // Setting up Background
    std::string bgImagePath = "assets/textures/Wooden-Background.jpg";
    SDL_Surface* bgSurface = IMG_Load(bgImagePath.c_str());    if (!bgSurface) {
        std::cerr << "Failed to load background image: " << IMG_GetError() << std::endl;
        return -1;
    }

    // Transform Background from a surface to a texture
    SDL_Texture* bgTexture = SDL_CreateTextureFromSurface(renderer, bgSurface);
    SDL_FreeSurface(bgSurface);  // Free surface after creating texture
    if (!bgTexture) {
        std::cerr << "Failed to create background texture: " << SDL_GetError() << std::endl;
        return -1;
    }

    // Get the window size (assuming full-screen background)
    SDL_Rect bgRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};


    // Seed random number generator
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // Main loop Setup
    bool inMenu = true;
    bool inTutorial = false;
    bool startGame = false;
    Menu menu(renderer, font, {"1 Player (VS. CPU)", "2 Player Game", "Awards & Statistics", "Rules and Scoring", "QUIT"});
    Game game;
    Achievements achievements;
    bool quit = false;
    SDL_Event e;
    int statisticsUpdated = 0;

    // Create Buttons for the game
    Button rollButton = {{350, 400, 100, 50}, "Roll", {0, 128, 255, 255}};
    Button bankButton = {{350, 500, 100, 50}, "Bank", {0, 128, 255, 255}};

    // Restart button
    Button restartButton = {{450, 450, 115, 50}, "Restart", {0, 128, 255, 255}};
    Button mainmenuButton = {{370, 550, 265, 50}, "Back to Main Menu", {0, 128, 255, 255}}; //At the end of a game
    Button returnmenuButton = {{660, 700, 290, 50}, "Return to Main Menu", {0, 128, 255, 255}}; //In the middle of Game or Tutorial

    // CREATE PLAYERS
    std::string player1_name = "Player 1";
    std::string player2_name = "Player 2";
    

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if(e.type == SDL_WINDOWEVENT){
                if (e.window.event == SDL_WINDOWEVENT_RESIZED) {
                    SDL_SetWindowSize(window, SCREEN_WIDTH, SCREEN_HEIGHT);
                }
            }
            if (e.type == SDL_QUIT) {
                quit = true;
            }

            if (inMenu) {
                statisticsUpdated = 0;
                menu.handleEvent(e, game, renderer, inMenu, startGame, inTutorial);
            } else {

                if (game.getCurrentPlayerIsAI() && !game.checkGameEnd()) {
                    //Retrieve currentPlayer for the AI
                    game.getPlayers()[game.getCurrentPlayer()]->takeTurn(game, renderer, font, rollButton, bankButton, returnmenuButton, bgTexture, bgRect, game.getPlayers()[game.getCurrentPlayer()]);
                    if (e.type == SDL_MOUSEBUTTONDOWN) {
                        int mouseX, mouseY;
                        SDL_GetMouseState(&mouseX, &mouseY);
    
                          
                        if (returnmenuButton.isClicked(mouseX, mouseY)) {
                            game.clearGame();
                            inMenu = true;
                            startGame = false;
                            menu.currentSelectedItem = -1;
                        }

                    }
                }

                // Handle mouse click
                else if (e.type == SDL_MOUSEBUTTONDOWN) {
                    int x = e.button.x;
                    int y = e.button.y;
                    int mouseX, mouseY;
                    SDL_GetMouseState(&mouseX, &mouseY);

                    if(!game.checkGameEnd()){

                        //Retrieve current Player for display calls
                        //game.getPlayers()[game.getCurrentPlayer()]
                        

                        // Check if roll button was clicked
                        if (rollButton.isClicked(mouseX, mouseY)) {
                            bool canRoll = false;
                            // It can roll again as long as one button is selected OR if it is their first roll
                            for (Button& btn : game.getHoldButtons()) {
                                if(btn.hasBeenSelected){
                                    canRoll = true;
                                }
                            }

                            //Retrive data for player to setup for checking first roll
                            int currentPlayer = game.getCurrentPlayer();

                            if(canRoll || game.getPlayers()[currentPlayer]->getFirstRoll()){
                                if(game.getPlayers()[currentPlayer]->getFirstRoll()){
                                    game.getPlayers()[currentPlayer]->firstRolled();
                                }
                                game.rollDice(renderer, font, bgTexture, bgRect);
                                game.getPossibleHolds();
                            }
                        }

                        // Check if bank button was and meets proper resources
                        if (bankButton.isClicked(mouseX, mouseY)) {
                            bool canBank = false;
                            // It can roll again as long as one button is selected OR if it is their first roll
                            for (Button& btn : game.getHoldButtons()) {
                                if(btn.hasBeenSelected){
                                    canBank = true;
                                }
                            }

                            if(canBank){
                                game.bankCurrentPlayerScore();
                            }
                        }

                        for (Button& btn : game.getHoldButtons()) {
                            if (btn.isClicked(x, y)) {
                                if (btn.onClick) {
                                    // Setting up so that rolls can be done after a single button selected
                                    btn.toggleSelected();
                                    
                                    // Call the assigned function
                                    btn.onClick();
                                }
                            }
                        }

                        if (returnmenuButton.isClicked(mouseX, mouseY)) {
                            game.clearGame();
                            inMenu = true;
                            startGame = false;
                            menu.currentSelectedItem = -1;
                        }    
                    }
                    if (game.checkGameEnd()) {    
                        if (restartButton.isClicked(mouseX, mouseY)) {
                            statisticsUpdated = 0;
                            game.restartGame();
                        }
                        if (mainmenuButton.isClicked(mouseX, mouseY)) {
                            game.clearGame();
                            inMenu = true;
                            startGame = false;
                            menu.currentSelectedItem = -1;
                        }
                    }       
                }
            }
        }
        // Render screen
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, bgTexture, nullptr, &bgRect);

        if (inMenu) {
            menu.render();
        } else {

            // Checking if the game is over, and displays the winner
            if (game.checkGameEnd()) {
                TTF_Font* winnerFont = TTF_OpenFont("assets/fonts/TiltPrism.ttf", 80);

                std::string winnerText = game.getWinningPlayerName(achievements, statisticsUpdated);

                statisticsUpdated = 1;
                achievements.saveProgress();
                SDL_Color white = {255, 255, 255, 255};
                
                // Create text surface and texture
                SDL_Surface* surface = TTF_RenderText_Solid(winnerFont, winnerText.c_str(), white);
                SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

                // Center the text on screen
                int x = (SCREEN_WIDTH - surface->w) / 2;
                int y = (SCREEN_HEIGHT - surface->h) / 2;
                SDL_Rect textRect = {x, (y - 150), surface->w, surface->h};
                SDL_RenderCopy(renderer, texture, nullptr, &textRect);

                // Clean up
                SDL_FreeSurface(surface);
                SDL_DestroyTexture(texture);
                TTF_CloseFont(winnerFont);

                // Render
                restartButton.render(renderer, font);
                mainmenuButton.render(renderer, font);
            } else {
                // In the middle of a game
                rollButton.render(renderer, font);  // Render the roll button
                bankButton.render(renderer, font);  // Render the bank button
                returnmenuButton.render(renderer, font); //render the return menu button
                
                // Render each button so that holds can be called individually
                for (Button& btn : game.getHoldButtons()) {
                    btn.render(renderer, font);
                }
                
                game.displaySoftScore(renderer, font); // Render the score for soft points
                game.displayHardScore(renderer, font); // Render the score for hard points
                game.displayHistory(renderer, font, game.getPlayers()[game.getCurrentPlayer()]); //Render History
                game.displayDice(renderer);  // Render dice and their hold states
            }
        }

        SDL_RenderPresent(renderer);  // Update the screen
    }

    // Cleanup
    SDL_DestroyTexture(bgTexture);
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    return 0;
}