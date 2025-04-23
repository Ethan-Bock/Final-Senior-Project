#include "game.h"
#include "players.h"
#include "achievements.h"

// Button struct functions
bool Button::getSelected() {return hasBeenSelected;}
std::string Button::getLabel(){return label;}

void Button::toggleSelected() {hasBeenSelected = !hasBeenSelected;}

bool Button::isClicked(int x, int y)  {
    return x > rect.x && x < rect.x + rect.w && y > rect.y && y < rect.y + rect.h;
}

void Button::render(SDL_Renderer* renderer, TTF_Font* font)  {
    // Draw button
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &rect);

    // If selected, draw yellow outline
    if (hasBeenSelected) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Yellow
        SDL_RenderDrawRect(renderer, &rect);
        int thickness = 3;  // Thickness in pixels
        for (int t = 0; t < thickness; ++t) {
            SDL_Rect r = {
                rect.x + t,
                rect.y + t,
                rect.w - 2 * t,
                rect.h - 2 * t
            };
            SDL_RenderDrawRect(renderer, &r);
        }
    }

    // Render button text
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, label.c_str(), {255, 255, 255});
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FreeSurface(textSurface);
    SDL_Rect textRect = {rect.x + 10, rect.y + 10, textSurface->w, textSurface->h};
    SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
    SDL_DestroyTexture(textTexture);
}


Dice::Dice(int v, bool h, SDL_Rect r) : value(v), held(h), rect(r) {}

bool Dice::operator==(int v) const {
    return value == v;
}

// Roll the die (set a random value between 1 and 6)
void Dice::roll() {
    if (!held) {
        value = rand() % 6 + 1;
    }
}


// Helper function to check if the dice is a straight
bool isStraight(const std::vector<Dice>& dice) {

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



void Game::toggleHold(int dieIndex) {
    if (dieIndex >= 0 && dieIndex < NUM_DICE) {
        die[dieIndex].held = !die[dieIndex].held;  // Toggle the hold state for the die
    }
}

void Game::rollDice(SDL_Renderer* renderer, TTF_Font* font, SDL_Texture* bgTexture, SDL_Rect bgRect) {
    //Deactivate lock buttons 
    lockOtherButtons = false;
    reverseLockOtherButtons = false;

    // Before rolling, store the indices of the held dice in previousHeldDice
    previousHeldDice.clear();  // Clear the vector to track the current roll's held dice
    for (int i = 0; i < NUM_DICE; i++) {
        if (die[i].held) {
            previousHeldDice.push_back(i);  // Store the index of the held dice
        }
    }

    bool allHeld = true;
    // Check if all dice are held
    for (int i = 0; i < NUM_DICE; i++) { 
        if (!die[i].held) {
            allHeld = false;
            break;
        }
    }

    if(allHeld) {
        for (int i = 0; i < NUM_DICE; ++i) {
            previousHeldDice.clear();
            die[i].held = false;
        }
    }

    // Retrieving current player for render purposes
    std::unique_ptr<Player>& currentPlayer = players[currentPlayerIndex];
    SDL_Delay(50);
    int animationFrames = 10;  // Number of frames for the rolling effect
    for (int frame = 0; frame < animationFrames; ++frame) {
        for (int i = 0; i < NUM_DICE; ++i) {
            if (!die[i].held) {
                die[i].value = (rand() % 6) + 1;  // Random temp value for animation
            }
        }
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, bgTexture, nullptr, &bgRect); //render background
        displaySoftScore(renderer, font); // Render the score for soft points
        displayHardScore(renderer, font); // Render the score for hard points

        // DISPLAY HISTORY
        
        displayHistory(renderer, font, currentPlayer);


        displayDice(renderer);
        SDL_Delay(50);  // Short delay to create a rolling effect
        SDL_RenderPresent(renderer);
        //SDL_Delay(50);  // Short delay to create a rolling effect
    }




    if(allHeld) {
        // If all dice are held, reset (roll) all dice
        for (int i = 0; i < NUM_DICE; ++i) {
            previousHeldDice.clear();
            die[i].held = false;
            die[i].roll();  // Roll all dice again
        }
    } else {
        // If not all dice are held, roll only the unheld dice
        for (int i = 0; i < NUM_DICE; ++i) {
            if (!die[i].held) {  // Only roll dice that are NOT held
                die[i].roll();   // Call the roll method of Dice to generate a random number between 1 and 6
            }
        }
    }
}


void Game::displayDice(SDL_Renderer* renderer) {
    int x = 100;
    for (int i = 0; i < NUM_DICE; ++i) {
        // Select the correct dice face image
        std::string imagePath = "assets/textures/" + std::to_string(die[i].value) + "-face.png";
        SDL_Surface* diceSurface = IMG_Load(imagePath.c_str());

        if (!diceSurface) {
            std::cerr << "Failed to load image: " << imagePath << " SDL_image Error: " << IMG_GetError() << std::endl;
            continue;  // Skip rendering this die if the image fails to load
        }

        SDL_Texture* diceTexture = SDL_CreateTextureFromSurface(renderer, diceSurface);
        SDL_FreeSurface(diceSurface);  // Free surface after creating texture

        // Resize the image to 50x50 pixels
        SDL_Rect diceRect = {x, 200, 75, 75};

        SDL_RenderCopy(renderer, diceTexture, nullptr, &diceRect);

        SDL_DestroyTexture(diceTexture);  // Destroy texture after rendering

        // Highlight held dice
        if (die[i].held) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);  // Yellow border
            SDL_Rect highlightRect = {x-2, 198, 79, 79};
        
            int thickness = 2;  // Thickness in pixels
            for (int t = 0; t < thickness; ++t) {
                SDL_Rect r = {
                    highlightRect.x + t,
                    highlightRect.y + t,
                    highlightRect.w - 2 * t,
                    highlightRect.h - 2 * t
                };
                SDL_RenderDrawRect(renderer, &r);
            }
        }

        x += 90;  // Offset for the next die
    }
}

void Game::addHoldButton(const std::string& text, int x, int y, std::function<void(Button&)> action){
    Button newButton;
    newButton.rect = {x, y, 165, 50};
    newButton.label = text;
    newButton.color = {0, 0, 255, 255}; // Blue color
    // newButton.onClick = action; // Assign the callback function
    // if (!action) {
    //     action = []() { std::cout << "Default button action triggered.\n"; };
    // }
    newButton.hasBeenSelected = false;

    holdButtons.push_back(newButton);

    Button& btnRef = holdButtons.back();

    // Now assign the onClick using the button reference
    btnRef.onClick = [&btnRef, action]() {
        action(btnRef);
    };
}


void Game::getPossibleHolds() {
    holdButtons.clear(); // Clear previous buttons before adding new ones

    // // Get the current player so we know who to give points to:
    // int player_number = getCurrentPlayer();

    // Create a list of remaining (non-held) dice
    std::vector<Dice> remainingDice;
    for (std::size_t i = 0; i < die.size(); ++i) {
        if (!die[i].held) {  // Only consider dice that are not held
            remainingDice.push_back(die[i]);
        }
    }

    // Now, decide which dice to hold based on the roll type
    std::map<int, int> counts;
    for (const Dice& die : remainingDice) {  // Use 'const Dice&' instead of 'int'
        counts[die.value]++;  // Access the 'value' of the Dice object
    }

    int startX = 50;
    int startY = 300;
    int yOffset = 60;

    // Check for straight (assuming dice are in consecutive order)
    if (isStraight(remainingDice)) {
        addHoldButton("Straight", startX, startY, [this](Button& btn) {
            if(!getReverseButtonLock()){
                bool isHolding = false;

                // Check if we're holding or releasing
                for (const auto& d : die) {
                    if (d.held) {
                        isHolding = true;
                        break;
                    }
                }

                //Toggle the Dice
                for (std::size_t i = 0; i < die.size(); ++i) {
                    toggleHold(i);
                }
                
                //Toggle Soft Points
                players[getCurrentPlayer()]->addSoftPoints(isHolding ? -1750 : 1750);

                // Update lock state
                lockOtherButtons = !isHolding;
            } else {
                btn.hasBeenSelected = false;
            }

        });
        startY += yOffset;
    }

    // Check for three pairs
    if (counts.size() == 3 && std::all_of(counts.begin(), counts.end(), [](const std::pair<int, int>& p) { return p.second == 2; })) {
        addHoldButton("Three Pairs", startX, startY, [this](Button& btn) {
            if(!getReverseButtonLock()){
                bool isHolding = false;

                // Check if we're holding or releasing
                for (const auto& d : die) {
                    if (d.held) {
                        isHolding = true;
                        break;
                    }
                }

                //Toggle the Dice
                for (std::size_t i = 0; i < die.size(); ++i) {
                    toggleHold(i);
                }

                //Toggle Soft Points
                players[getCurrentPlayer()]->addSoftPoints(isHolding ? -1500 : 1500);

                lockOtherButtons = !isHolding;
            } else {
                btn.hasBeenSelected = false;
            }
        });
        startY += yOffset;
    }

    // Check for 6 of a kind
    if (auto it = std::find_if(counts.begin(), counts.end(), [](const std::pair<int, int>& p) { return p.second == 6; }); it != counts.end()) {
        int number = it->first; // The number that appears six times
        std::string label = "Six " + std::to_string(number) + "s";

        addHoldButton(label, startX, startY, [this, number](Button& btn) {
            bool isHolding = false;

            // Check if we're holding or releasing
            for (const auto& d : die) {
                if (d.held) {
                    isHolding = true;
                    break;
                }
            }

            //Toggle the dice
            for (std::size_t i = 0; i < die.size(); ++i) {
                if (die[i].value == number) {
                    toggleHold(i);
                }
            }

            //Toggle Soft Points
            players[getCurrentPlayer()]->addSoftPoints(isHolding ? -2500 : 2500);
        });
        startY += yOffset;
    }

    // Check for five of a kind
    if (auto it = std::find_if(counts.begin(), counts.end(), [](const std::pair<int, int>& p) { return p.second == 5; }); it != counts.end()) {
        int number = it->first; // The number that appears six times
        std::string label = "Five " + std::to_string(number) + "s";

        addHoldButton(label, startX, startY, [this, number](Button& btn) {
            bool isHolding = false;

            // Check if we're holding or releasing
            for (std::size_t i = 0; i < die.size(); ++i) {
                if (die[i].held && die[i].value == number && std::find(previousHeldDice.begin(), previousHeldDice.end(), i) == previousHeldDice.end()) {
                    isHolding = true;
                    break;
                }
            }

            //Toggle Dice
            int count = 0;
            for (std::size_t i = 0; i < die.size(); ++i) {
                // If the die is not held and the index is not in previousHeldDice
                if (die[i].value == number && std::find(previousHeldDice.begin(), previousHeldDice.end(), i) == previousHeldDice.end()) {
                    toggleHold(i);  // Toggle the hold
                    count++;
                }
                // Stop once we have held 3 dice of this value
                if (count == 5) break;
            }

            // Calculate Points
            int points = manyOfAKindPoints(number,5);

            //Toggle Soft Points
            players[getCurrentPlayer()]->addSoftPoints(isHolding ? -points : points);

        });
        startY += yOffset;
    }

    // Check for four of a kind
    if (auto it = std::find_if(counts.begin(), counts.end(), [](const std::pair<int, int>& p) { return p.second == 4; }); it != counts.end()) {
        int number = it->first; // The number that appears six times
        std::string label = "Four " + std::to_string(number) + "s";

        addHoldButton(label, startX, startY, [this, number](Button& btn) {
            bool isHolding = false;

            // Check if we're holding or releasing
            for (std::size_t i = 0; i < die.size(); ++i) {
                if (die[i].held && die[i].value == number && std::find(previousHeldDice.begin(), previousHeldDice.end(), i) == previousHeldDice.end()) {
                    isHolding = true;
                    break;
                }
            }

            // Toggle Dice
            int count = 0;
            for (std::size_t i = 0; i < die.size(); ++i) {
                // If the die is not held and the index is not in previousHeldDice
                if (die[i].value == number && std::find(previousHeldDice.begin(), previousHeldDice.end(), i) == previousHeldDice.end()) {
                    toggleHold(i);  // Toggle the hold
                    count++;
                }
                // Stop once we have held 3 dice of this value
                if (count == 4) break;
            }

            // Calculate Points
            int points = manyOfAKindPoints(number,4);

            //Toggle Soft Points
            players[getCurrentPlayer()]->addSoftPoints(isHolding ? -points : points);
        });
        startY += yOffset;
    }

    // Check for 3 of a kind
    for (const auto& pair : counts) {
        if (pair.second == 3) {  // If exactly three of this number exist
            int number = pair.first; // The number that appears three times
            std::string label = "Three " + std::to_string(number) + "s";

            addHoldButton(label, startX, startY, [this, number](Button& btn) {
                bool isHolding = false;
            
                // Check if we're holding or releasing
                for (std::size_t i = 0; i < die.size(); ++i) {
                    if (die[i].held && die[i].value == number && std::find(previousHeldDice.begin(), previousHeldDice.end(), i) == previousHeldDice.end()) {
                        isHolding = true;
                        break;
                    }
                }

                // Toggle Dice
                int count = 0;
                for (std::size_t i = 0; i < die.size(); ++i) {
                    // If the die is not held and the index is not in previousHeldDice
                    if (die[i].value == number && std::find(previousHeldDice.begin(), previousHeldDice.end(), i) == previousHeldDice.end()) {
                        toggleHold(i);  // Toggle the hold
                        count++;
                    }
                    // Stop once we have held 3 dice of this value
                    if (count == 3) break;
                }

                // Calculate Points
                int points = manyOfAKindPoints(number,3);

                //Toggle Soft Points
                players[getCurrentPlayer()]->addSoftPoints(isHolding ? -points : points);
            });

            startY += yOffset; // Move down for the next button
        }
    }

    // Check for single 1
    if (counts[1] == 1) {
        // Loop through the die vector, looking for the index of the unheld '1'
        for (std::size_t i = 0; i < die.size(); ++i) {
            if (die[i].value == 1 && !die[i].held) {
                addHoldButton("Single 1", startX, startY, [this, i](Button& btn) {
                    if(!getButtonLock()){
                        bool isHolding = false;
            
                        // Check if we're holding or releasing
                        for (std::size_t i = 0; i < die.size(); ++i) {
                            if (die[i].held && die[i].value == 1 && std::find(previousHeldDice.begin(), previousHeldDice.end(), i) == previousHeldDice.end()) {
                                isHolding = true;
                                break;
                            }
                        }
    
                        // Hold the die at index i
                        toggleHold(i);  // Mark the die as held
    
                        //Get soft points
                        players[getCurrentPlayer()]->addSoftPoints(isHolding ? -100 : 100);

                        reverseLockOtherButtons = !isHolding;
                    } else {
                        btn.hasBeenSelected = false;
                    }
                });
                startY += yOffset;  // Adjust the Y position for the next button
                break;  // Only hold one '1', so break the loop after finding the first unheld '1'
            }
        }
    }

    // Check for double 1
    if (auto it = std::find_if(counts.begin(), counts.end(), [](const std::pair<int, int>& p) { return (p.second == 2 && p.first == 1); }); it != counts.end()) {        
        addHoldButton("Double 1", startX, startY, [this](Button& btn) {
            if(!getButtonLock()){
                int count = 0;
                bool isHolding = false;

                // Check if we're holding or releasing
                for (std::size_t i = 0; i < die.size(); ++i) {
                    if (die[i].held && die[i].value == 1 && std::find(previousHeldDice.begin(), previousHeldDice.end(), i) == previousHeldDice.end()) {
                        isHolding = true;
                        break;
                    }
                }

                // Toggle Dice
                for (std::size_t i = 0; i < die.size(); ++i) {
                    if (die[i].value == 1 && std::find(previousHeldDice.begin(), previousHeldDice.end(), i) == previousHeldDice.end()) {
                        toggleHold(i);
                        count++;
                    }
                    if (count == 2) break;
                }

                // Add or subtract points
                players[getCurrentPlayer()]->addSoftPoints(isHolding ? -200 : 200);

                reverseLockOtherButtons = !isHolding;
                
            } else {
                btn.hasBeenSelected = false;
            }
        });
        startY += yOffset;  // Adjust the Y position for the next button
    }

    // Check for single 5
    if (counts[5] == 1) {
        // Loop through the die vector, looking for the index of the unheld '1'
        for (std::size_t i = 0; i < die.size(); ++i) {
            if (die[i].value == 5 && !die[i].held) {
                addHoldButton("Single 5", startX, startY, [this, i](Button& btn) {
                    if(!getButtonLock()){
                        bool isHolding = false;
                
                        // Check if we're holding or releasing
                        for (std::size_t i = 0; i < die.size(); ++i) {
                            if (die[i].held && die[i].value == 5 && std::find(previousHeldDice.begin(), previousHeldDice.end(), i) == previousHeldDice.end()) {
                                isHolding = true;
                                break;
                            }
                        }

                        // Hold the die at index i
                        toggleHold(i);  // Mark the die as held

                        //Get soft points
                        players[getCurrentPlayer()]->addSoftPoints(isHolding ? -50 : 50);

                        reverseLockOtherButtons = !isHolding;
                    } else {
                        btn.hasBeenSelected = false;
                    }

                });
                startY += yOffset;  // Adjust the Y position for the next button
                break;  // Only hold one '1', so break the loop after finding the first unheld '1'
            }
        }
    }

    // Check for double 5
    if (auto it = std::find_if(counts.begin(), counts.end(), [](const std::pair<int, int>& p) { return (p.second == 2 && p.first == 5); }); it != counts.end()) {        
        addHoldButton("Double 5", startX, startY, [this](Button& btn) {
            if(!getButtonLock()){
                int count = 0;
                bool isHolding = false;

                // Check if we're holding or releasing
                for (std::size_t i = 0; i < die.size(); ++i) {
                    if (die[i].held && die[i].value == 5 && std::find(previousHeldDice.begin(), previousHeldDice.end(), i) == previousHeldDice.end()) {
                        isHolding = true;
                        break;
                    }
                }

                // Toggle Dice
                for (std::size_t i = 0; i < die.size(); ++i) {
                    if (die[i].value == 5 && std::find(previousHeldDice.begin(), previousHeldDice.end(), i) == previousHeldDice.end()) {
                        toggleHold(i);
                        count++;
                    }
                    if (count == 2) break;
                }

                // Add or subtract points
                players[getCurrentPlayer()]->addSoftPoints(isHolding ? -100 : 100);

                reverseLockOtherButtons = !isHolding;
            } else {
                btn.hasBeenSelected = false;
            }
        });
        startY += yOffset;  // Adjust the Y position for the next button
    }


    // If no match NOTHING;
    if (remainingDice.size() == 6) {
        if (holdButtons.empty()) {
            // Loop through the die vector, looking for the index of the unheld '1'
            addHoldButton("Nothing", startX, startY, [this](Button& btn) {
                bool isHolding = false;

                // Check if we're holding or releasing
                for (const auto& d : die) {
                    if (d.held) {
                        isHolding = true;
                        break;
                    }
                }

                //Toggle the Dice
                for (std::size_t i = 0; i < die.size(); ++i) {
                    toggleHold(i);
                }

                //Toggle Soft Points
                players[getCurrentPlayer()]->addSoftPoints(isHolding ? -500 : 500);
            });
        }
    }


    if (holdButtons.empty()) {
        addHoldButton("ZILCH", startX, startY, [this](Button& btn) {
            // Increment Zilch
            players[getCurrentPlayer()]->addZilch();
            

            // Zilch Pentalty
            if(players[getCurrentPlayer()]->getZilches() == 3){
                players[getCurrentPlayer()]->resetZilches();

                players[getCurrentPlayer()]->addToHistory(-500, false);
                players[getCurrentPlayer()]->addHardPoints(-500);
            }else{
                players[getCurrentPlayer()]->addToHistory(0, true);
            }

            
            // Reset Soft Points
            players[getCurrentPlayer()]->resetSoftPoints();

            // Reset Players First Roll
            players[getCurrentPlayer()]->resetFirstRoll();

            // Next turn starts
            nextTurn();
        });
    }
}



////////////// PLAYERS //////////////
Game::Game() : currentPlayerIndex(0) {
    std::srand(std::time(nullptr));  // Initialize random seed
    die.resize(NUM_DICE);            // Initialize vector for 6 dice
}


void Game::addPlayer(std::string name, bool isAI, const std::string& aiType){
    if (isAI) {
        if (aiType == "aggressive") {
            players.push_back(std::make_unique<AggressiveAI>(name)); // Store AIPlayer as Aggressive Player*
        } else if (aiType == "cautious") {
            players.push_back(std::make_unique<CautiousAI>(name)); // Store AIPlayer as Cautious Player*
        } else if (aiType == "adaptive") {
            players.push_back(std::make_unique<AdaptiveAI>(name)); // Store AIPlayer as Adaptive Player*
        }
    } else {
        players.push_back(std::make_unique<Player>(name));
    }
}

void Game::setFirstTurn(){
    gameOver = false; // Safety net to make sure that the game doesn't end immediately
    players[0]->setTurn(true); // Player 1 goes first
}


void Game::nextTurn() {
    currentPlayerIndex = (currentPlayerIndex + 1) % players.size(); // Cycle turns

    holdButtons.clear(); // Clear possible holds
    previousHeldDice.clear(); // Clear previous held dice

    // Clear all held dice 
    for (int i = 0; i < NUM_DICE; ++i) {
        die[i].held = false;
    }

    //CHECK if the game has ended
    if(players[currentPlayerIndex]->getHardPoints() >= winningPoints){
        gameOver = true;
    }else{
        gameOver = false;
    }
}


// THIS IS A HELPER FUNCTION TO CALCULATE the different 
int Game::manyOfAKindPoints(int diceNumber, int numberOfDice) {
    // Base points for three-of-a-kind
    int basePoints[] = {0, 1000, 200, 300, 400, 500, 600}; 
    
    if (diceNumber < 1 || diceNumber > 6 || numberOfDice < 3) {
        return 0; // Invalid input case
    }
    
    return basePoints[diceNumber] * (1 << (numberOfDice - 3)); // Multiply by 2^(numberOfDice - 3)
}



void Game::displaySoftScore(SDL_Renderer* renderer, TTF_Font* font) {
    std::unique_ptr<Player>& currentPlayer = players[currentPlayerIndex];

    // Display the soft points of the current player
    std::string scoreText = "Soft Points: " + std::to_string(currentPlayer->getSoftPoints());
    SDL_Surface* scoreSurface = TTF_RenderText_Solid(font, scoreText.c_str(), {255, 255, 255});
    SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
    SDL_Rect scoreRect = {200, 75, scoreSurface->w, scoreSurface->h};
    SDL_RenderCopy(renderer, scoreTexture, nullptr, &scoreRect);
    SDL_FreeSurface(scoreSurface);
    SDL_DestroyTexture(scoreTexture);
}

void Game::displayHardScore(SDL_Renderer* renderer, TTF_Font* font) {
    if (players.size() < 2) return; // Ensure at least two players exist

    // Define text color
    SDL_Color textColor = {255, 255, 255}; // White color

    // Get window width to align player 2 to the right
    int windowWidth;
    SDL_GetRendererOutputSize(renderer, &windowWidth, nullptr);

    // Define spacing
    int padding = 20;
    int lineSpacing = 5; // Space between name and score

    // Display Player 1 (players[0]) - Top-left corner
    std::string player1Name = players[0]->getName();
    SDL_Surface* surface1Name = TTF_RenderText_Solid(font, player1Name.c_str(), textColor);
    SDL_Texture* texture1Name = SDL_CreateTextureFromSurface(renderer, surface1Name);
    SDL_Rect rect1Name = {padding, padding, surface1Name->w, surface1Name->h};
    SDL_RenderCopy(renderer, texture1Name, nullptr, &rect1Name);
    SDL_FreeSurface(surface1Name);
    SDL_DestroyTexture(texture1Name);

    std::string player1Score = "Score: " + std::to_string(players[0]->getHardPoints());
    SDL_Surface* surface1Score = TTF_RenderText_Solid(font, player1Score.c_str(), textColor);
    SDL_Texture* texture1Score = SDL_CreateTextureFromSurface(renderer, surface1Score);
    SDL_Rect rect1Score = {padding, rect1Name.y + rect1Name.h + lineSpacing, surface1Score->w, surface1Score->h};
    SDL_RenderCopy(renderer, texture1Score, nullptr, &rect1Score);
    SDL_FreeSurface(surface1Score);
    SDL_DestroyTexture(texture1Score);

    // Display Player 2 (players[1]) - Top-right corner
    std::string player2Name = players[1]->getName();
    SDL_Surface* surface2Name = TTF_RenderText_Solid(font, player2Name.c_str(), textColor);
    SDL_Texture* texture2Name = SDL_CreateTextureFromSurface(renderer, surface2Name);

    // Store width before freeing surface
    int player2NameWidth = surface2Name->w;

    SDL_Rect rect2Name = {(windowWidth - player2NameWidth - padding) - 300, padding, surface2Name->w, surface2Name->h};
    SDL_RenderCopy(renderer, texture2Name, nullptr, &rect2Name);
    SDL_FreeSurface(surface2Name);
    SDL_DestroyTexture(texture2Name);

    std::string player2Score = "Score: " + std::to_string(players[1]->getHardPoints());
    SDL_Surface* surface2Score = TTF_RenderText_Solid(font, player2Score.c_str(), textColor);
    SDL_Texture* texture2Score = SDL_CreateTextureFromSurface(renderer, surface2Score);

    // Use stored width instead of accessing freed memory
    SDL_Rect rect2Score = {(windowWidth - player2NameWidth - padding) - 300, rect2Name.y + rect2Name.h + lineSpacing, surface2Score->w, surface2Score->h};
    SDL_RenderCopy(renderer, texture2Score, nullptr, &rect2Score);
    SDL_FreeSurface(surface2Score);
    SDL_DestroyTexture(texture2Score);
}

void Game::bankCurrentPlayerScore(){
    //CHECK THAT IT MEETS THE REQUIREMENTS
    if(players[currentPlayerIndex]->getSoftPoints() >= 300){


        // Add the soft points to the current players bank
        int softPoints = players[getCurrentPlayer()]->getSoftPoints();
        players[getCurrentPlayer()]->addHardPoints(softPoints);

        //ADD TO HISTORY
        players[getCurrentPlayer()]->addToHistory(softPoints, false);

        //Reset player's softPoints for the next turn
        players[getCurrentPlayer()]->resetSoftPoints();

        //Reset player's Zilches since they banked
        players[getCurrentPlayer()]->resetZilches();

        // Reset Player's First Roll
        players[getCurrentPlayer()]->resetFirstRoll();

        // Move automatically to the next term
        nextTurn(); // held, previousHeld, and holdButtons reset here
    }

}





////////////// Winning //////////////
std::string Game::getWinningPlayerName(Achievements& achievements, int statisticsUpdated) {

    int P1score = players[0]->getHardPoints();
    int P2score = players[1]->getHardPoints();

    //Call checkAchievements
    achievements.checkAchievements(players[0], players[1], winningPoints);


    if (P1score > P2score) {
        if (players[1]->isAIPlayer() && players[1]->getName() == "Aggressive AI" && statisticsUpdated == 0) {
            achievements.updateStatistics("Total Wins against AI");
            achievements.updateStatistics("Wins against Aggressive AI");
        } else if (players[1]->isAIPlayer() && players[1]->getName() == "Cautious AI" && statisticsUpdated == 0) {
            achievements.updateStatistics("Total Wins against AI");
            achievements.updateStatistics("Wins against Cautious AI");
        } else if (players[1]->isAIPlayer() && players[1]->getName() == "Adaptive AI" && statisticsUpdated == 0) {
            achievements.updateStatistics("Total Wins against AI");
            achievements.updateStatistics("Wins against Adaptive AI");
        }


        if (players[1]->isAIPlayer() && statisticsUpdated == 0){
            achievements.updateStatistics("Total number of Games against AI");
        }
    
        //Update file
        achievements.saveProgress();
        return players[0]->getName() + " wins!";

    } else if (P2score > P1score) {
        if (players[1]->isAIPlayer() && statisticsUpdated == 0){
            achievements.updateStatistics("Total number of Games against AI");
        }
    
        //Update file
        achievements.saveProgress();
        return players[1]->getName() + " wins!";
    } else {
        if (players[1]->isAIPlayer() && statisticsUpdated == 0){
            achievements.updateStatistics("Total number of Games against AI");
        }
    
        //Update file
        achievements.saveProgress();
        return "It's a tie!";
    }
}

void Game::restartGame(){
    players[0]->resetHardPoints();
    players[1]->resetHardPoints();
    players[0]->resetZilches();
    players[1]->resetZilches();

    //Clear Player History
    players[0]->clearHistory();
    players[1]->clearHistory();

    gameOver = false;
}

void Game::clearGame(){
    // Clear players
    players.clear();
    holdButtons.clear();
    previousHeldDice.clear();
    currentPlayerIndex = 0;
    lockOtherButtons = false;
    reverseLockOtherButtons = false;

    // Clear Dice
    for (int i = 0; i < NUM_DICE; ++i) {
        die[i].held = false;
    }
}

bool Game::checkGameEnd(){
    if (gameOver){
        // This code sets it to the opposite turn so that a game doesn't restart automatically
        if (currentPlayerIndex == 1){
            currentPlayerIndex = 0;
        }
    }
    return gameOver;
}

////////////// HISTORY  //////////////
void Game::displayHistory(SDL_Renderer* renderer, TTF_Font* font, std::unique_ptr<Player>& currentPlayer) {
    if (!currentPlayer) return;

    // Get player history
    const auto& history = currentPlayer->getHistory();

    // Define starting position for rendering
    int x = 750;  // Adjust based on window width
    int y = 200;  // Starting Y position for the history
    int boxWidth = 200; // Width of the player name box
    int boxHeight = 50; // Height of the player name box

    // Render the player's name box above the history
    std::string playerName = currentPlayer->getName();
    SDL_Color boxColor = {0, 0, 255, 255}; // Blue color for the box
    SDL_SetRenderDrawColor(renderer, boxColor.r, boxColor.g, boxColor.b, boxColor.a);
    SDL_Rect nameBox = {x, y - boxHeight - 10, boxWidth, boxHeight}; // Position the box above the history
    SDL_RenderFillRect(renderer, &nameBox);

    // Set text color for player name (white)
    SDL_Color textColor = {255, 255, 255};  
    SDL_Surface* surface = TTF_RenderText_Solid(font, playerName.c_str(), textColor);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    // Calculate center of the box to render the text
    SDL_Rect textRect = {x + (boxWidth - surface->w) / 2, y - boxHeight, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, nullptr, &textRect);

    // Free resources
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);

    // Render the history box outline (the white box surrounding the history entries)
    int historyBoxWidth = boxWidth;
    SDL_Rect historyBox = {x, y - 5, historyBoxWidth, 370};  // Add padding around the box
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White color for outline
    SDL_RenderDrawRect(renderer, &historyBox);

    // Get the number of history entries
    int historySize = history.size();
    int entriesToRender = (historySize < 12) ? historySize : 12;  // If there are fewer than 5 entries, only render those

    // Display the history entries in reverse order (12th-most recent to most recent)
    for (int i = historySize - entriesToRender; i < historySize; ++i) {
        const auto& entry = history[i];
        std::string text = entry.second ? "Zilch" : std::to_string(entry.first) + " pts";
        
        SDL_Surface* historySurface = TTF_RenderText_Solid(font, text.c_str(), {255, 255, 255});
        SDL_Texture* historyTexture = SDL_CreateTextureFromSurface(renderer, historySurface);
        
        SDL_Rect historyRect = {x + 30, y, historySurface->w, historySurface->h};
        SDL_RenderCopy(renderer, historyTexture, nullptr, &historyRect);
        
        SDL_FreeSurface(historySurface);
        SDL_DestroyTexture(historyTexture);
        
        y += 30;  // Spacing between history entries
    }
}
