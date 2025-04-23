#include "game.h"
#include "players.h"

Player::~Player() {}
void Player::takeTurn(Game& game, SDL_Renderer* renderer, TTF_Font* font, Button rollButton, Button bankButton, Button mainmenu, SDL_Texture* bgTexture, SDL_Rect bgRect, std::unique_ptr<Player>& currentPlayer) {}

Player::Player(std::string name, bool isAI) 
    : name(name), turn(false), hardPoints(0), softPoints(0), zilches(0), firstRoll(true), isAI(isAI){}

void Player::addHardPoints(int amount) { hardPoints += amount; }

void Player::addSoftPoints(int amount) { softPoints += amount; }

void Player::addZilch() { zilches++; }

void Player::resetSoftPoints() { softPoints = 0; }

void Player::resetHardPoints() { hardPoints = 0; }

void Player::resetZilches() { zilches = 0; }

void Player::resetFirstRoll() { firstRoll = true; }

void Player::setTurn(bool turnStatus) { turn = turnStatus; }

void Player::firstRolled(){ firstRoll = false; }

bool Player::isTurn() const { return turn; }

int Player::getHardPoints() const { return hardPoints; }

int Player::getSoftPoints() const { return softPoints; }

int Player::getZilches() const { return zilches; }

std::string Player::getName() const { return name; }

bool Player::getFirstRoll() const { return firstRoll; }




void Player::addToHistory(int points, bool isZilch) {
    player_history.push_back({points, isZilch});
}

void Player::clearHistory() {
    player_history.clear();
}

const std::vector<std::pair<int, bool>>& Player::getHistory() const {
    return player_history;
}




/// AI BASIC STUFF 
bool Player::isAIPlayer() const { return isAI; }
AIPlayer::AIPlayer(std::string name, bool isAI) : Player(name, isAI) {}

std::vector<std::vector<std::string>> twoButtonCombos = {
    {"Three 1s", "Three 2s"},
    {"Three 1s", "Three 3s"},
    {"Three 1s", "Three 4s"},
    {"Three 1s", "Three 5s"},
    {"Three 1s", "Three 6s"},

    {"Three 2s", "Three 1s"},
    {"Three 2s", "Three 3s"},
    {"Three 2s", "Three 4s"},
    {"Three 2s", "Three 5s"},
    {"Three 2s", "Three 6s"},
    
    {"Three 3s", "Three 1s"},
    {"Three 3s", "Three 2s"},
    {"Three 3s", "Three 4s"},
    {"Three 3s", "Three 5s"},
    {"Three 3s", "Three 6s"},

    {"Three 4s", "Three 1s"},
    {"Three 4s", "Three 2s"},
    {"Three 4s", "Three 3s"},
    {"Three 4s", "Three 5s"},
    {"Three 4s", "Three 6s"},

    {"Three 5s", "Three 1s"},
    {"Three 5s", "Three 2s"},
    {"Three 5s", "Three 3s"},
    {"Three 5s", "Three 4s"},
    {"Three 5s", "Three 6s"},

    {"Three 6s", "Three 1s"},
    {"Three 6s", "Three 2s"},
    {"Three 6s", "Three 3s"},
    {"Three 6s", "Three 4s"},
    {"Three 6s", "Three 5s"},


    {"Five 6s", "Single 1"},
    {"Five 5s", "Single 1"},
    {"Five 4s", "Single 1"},
    {"Five 3s", "Single 1"},
    {"Five 2s", "Single 1"},


    {"Five 1s", "Single 5"},
    {"Five 6s", "Single 5"},
    {"Five 4s", "Single 5"},
    {"Five 3s", "Single 5"},
    {"Five 2s", "Single 5"},


    {"Four 6s", "Double 1"},
    {"Four 5s", "Double 1"},
    {"Four 4s", "Double 1"},
    {"Four 3s", "Double 1"},
    {"Four 2s", "Double 1"},


    {"Four 6s", "Single 1"},
    {"Four 5s", "Single 1"},
    {"Four 4s", "Single 1"},
    {"Four 3s", "Single 1"},
    {"Four 2s", "Single 1"},
    

    {"Four 1s", "Double 5"},
    {"Four 6s", "Double 5"},
    {"Four 4s", "Double 5"},
    {"Four 3s", "Double 5"},
    {"Four 2s", "Double 5"},


    {"Four 1s", "Single 5"},
    {"Four 6s", "Single 5"},
    {"Four 4s", "Single 5"},
    {"Four 3s", "Single 5"},
    {"Four 2s", "Single 5"},


    {"Three 6s", "Double 1"},
    {"Three 5s", "Double 1"},
    {"Three 4s", "Double 1"},
    {"Three 3s", "Double 1"},
    {"Three 2s", "Double 1"},


    {"Three 6s", "Single 1"},
    {"Three 5s", "Single 1"},
    {"Three 4s", "Single 1"},
    {"Three 3s", "Single 1"},
    {"Three 2s", "Single 1"},


    {"Three 1s", "Double 5"},
    {"Three 6s", "Double 5"},
    {"Three 4s", "Double 5"},
    {"Three 3s", "Double 5"},
    {"Three 2s", "Double 5"},
    

    {"Three 1s", "Single 5"},
    {"Three 6s", "Single 5"},
    {"Three 4s", "Single 5"},
    {"Three 3s", "Single 5"},
    {"Three 2s", "Single 5"},

    
    {"Double 1", "Double 5"},
    {"Double 1", "Single 5"},
    {"Single 1", "Double 5"},
    {"Single 1", "Single 5"}
};

std::vector<std::vector<std::string>> threeButtonCombos = {
    {"Four 2s", "Single 1", "Single 5"},
    {"Four 3s", "Single 1", "Single 5"},
    {"Four 4s", "Single 1", "Single 5"},
    {"Four 6s", "Single 1", "Single 5"},

    {"Three 2s", "Double 1", "Single 5"},
    {"Three 3s", "Double 1", "Single 5"},
    {"Three 4s", "Double 1", "Single 5"},
    {"Three 6s", "Double 1", "Single 5"},
    
    {"Three 2s", "Single 1", "Double 5"},
    {"Three 3s", "Single 1", "Double 5"},
    {"Three 4s", "Single 1", "Double 5"},
    {"Three 6s", "Single 1", "Double 5"},

    {"Three 2s", "Single 1", "Single 5"},
    {"Three 3s", "Single 1", "Single 5"},
    {"Three 4s", "Single 1", "Single 5"},
    {"Three 6s", "Single 1", "Single 5"},
};

std::vector<std::string> priorityLabels = {
    "Six 1s", "Six 6s", "Six 5s", "Six 4s", "Six 3s", "Six 2s",
    "Five 1s", "Five 6s", "Five 5s", "Five 4s", "Five 3s", "Five 2s",
    "Four 1s", "Four 6s", "Four 5s", "Four 4s", "Four 3s", "Four 2s",
    "Three 1s", "Three 6s", "Three 5s", "Three 4s", "Three 3s",
    "Three 2s", "Double 1", 
    "Single 1", "Double 5", 
    "Single 5"
};

std::vector<std::string> specialLabels = {"Nothing", "Straight", "Three Pairs"};


/// Aggressive AI ///
AggressiveAI::AggressiveAI(std::string name) : AIPlayer(name), zilched(false), rolledAgain(0) {}


void AggressiveAI::takeTurn(Game& game, SDL_Renderer* renderer, TTF_Font* font, Button rollButton, Button bankButton, Button mainmenu, SDL_Texture* bgTexture, SDL_Rect bgRect, std::unique_ptr<Player>& currentPlayer) {
    //playerIndex = game.getCurrentPlayer()
    if(!game.checkGameEnd()){
        
        // Variable added to see if all dice are held at any given point
        bool canRollAgain = false;

        while ((canRollAgain || !shouldBank(game)) && !zilched) { //CHECK IF THERE IS ANOTHER FREE ROLL
            if(canRollAgain){
                rolledAgain += 1;
            }

            canRollAgain = true;
            zilched = false;
            game.rollDice(renderer, font, bgTexture, bgRect);
            game.getPossibleHolds();

             // RENDERING OCCURS SO IT LOOKS LIKE THE AI ROLLED THE DICE
             SDL_RenderClear(renderer);
             SDL_RenderCopy(renderer, bgTexture, nullptr, &bgRect); // Render Background
             rollButton.render(renderer, font);  // Render the roll button
             bankButton.render(renderer, font);  // Render the bank button
             mainmenu.render(renderer, font);  // Render the return to menu button
                 
             // Render each button so that holds can be called individually
             for (Button& btn : game.getHoldButtons()) {
                 btn.render(renderer, font);
             }
                 
             game.displaySoftScore(renderer, font); // Render the score for soft points
             game.displayHardScore(renderer, font); // Render the score for hard points
             game.displayHistory(renderer, font, game.getPlayers()[game.getCurrentPlayer()]); //Render History
             game.displayDice(renderer);  // Render dice and their hold states
     
             SDL_RenderPresent(renderer);  // Update the screen



            
            selectHands(game);
            SDL_Delay(500);


            // SO THAT RENDERING OCCURS DURING AI's TURN
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, bgTexture, nullptr, &bgRect); // Render Background
            rollButton.render(renderer, font);  // Render the roll button
            bankButton.render(renderer, font);  // Render the bank button
            mainmenu.render(renderer, font);  // Render the return to menu button
                
            // Render each button so that holds can be called individually
            for (Button& btn : game.getHoldButtons()) {
                btn.render(renderer, font);
            }
                
            game.displaySoftScore(renderer, font); // Render the score for soft points
            game.displayHardScore(renderer, font); // Render the score for hard points
            game.displayHistory(renderer, font, game.getPlayers()[game.getCurrentPlayer()]); //Render History
            game.displayDice(renderer);  // Render dice and their hold states
    
            SDL_RenderPresent(renderer);  // Update the screen

            SDL_Delay(500);


            // Check if all dice are held
            for (int i = 0; i < NUM_DICE; i++) { 
                if (!game.getDice()[i].held) {
                    canRollAgain = false;
                    break;
                }
            }
        }
        
        if(!zilched){
            game.bankCurrentPlayerScore();
        } else {
            zilched = false;
            game.getPlayers()[game.getCurrentPlayer()]->addZilch();

            // Zilch Pentalty
            if(game.getPlayers()[game.getCurrentPlayer()]->getZilches() == 3){
                game.getPlayers()[game.getCurrentPlayer()]->resetZilches();
                game.getPlayers()[game.getCurrentPlayer()]->addToHistory(-500, false);
                game.getPlayers()[game.getCurrentPlayer()]->addHardPoints(-500);
            }else{
                game.getPlayers()[game.getCurrentPlayer()]->addToHistory(0, true);
            }
            
            // Reset Soft Points
            game.getPlayers()[game.getCurrentPlayer()]->resetSoftPoints();

            // Reset Players First Roll
            game.getPlayers()[game.getCurrentPlayer()]->resetFirstRoll();

            // Next turn starts
            game.nextTurn();
        }
    }
}

void AggressiveAI::selectHands(Game& game) {
    std::vector<Button>& buttons = game.getHoldButtons();
    std::unordered_map<std::string, Button*> buttonMap;
    for (Button& btn : buttons) {
        buttonMap[btn.getLabel()] = &btn;
    }

    for (const std::string& label : specialLabels) {
        if (buttonMap.count(label)) {
            buttonMap[label]->toggleSelected();
            buttonMap[label]->onClick();
            SDL_Delay(500); // Small delay for UI responsiveness
            return;
        }
    }

    // Try selecting a three-button combo
    for (const auto& combo : threeButtonCombos) {
        if (buttonMap.count(combo[0]) && buttonMap.count(combo[1]) && buttonMap.count(combo[2])) {
            buttonMap[combo[0]]->toggleSelected();
            buttonMap[combo[0]]->onClick();
            SDL_Delay(500);

            buttonMap[combo[1]]->toggleSelected();
            buttonMap[combo[1]]->onClick();
            SDL_Delay(500);

            buttonMap[combo[2]]->toggleSelected();
            buttonMap[combo[2]]->onClick();
            return;
        }
    }

    // Try selecting a two-button combo
    for (const auto& combo : twoButtonCombos) {
        if (buttonMap.count(combo[0]) && buttonMap.count(combo[1])) {
            buttonMap[combo[0]]->toggleSelected();
            buttonMap[combo[0]]->onClick();
            SDL_Delay(500);

            buttonMap[combo[1]]->toggleSelected();
            buttonMap[combo[1]]->onClick();
            return;
        }
    }


    for (const std::string& label : priorityLabels) {
        if (buttonMap.count(label)) {
            buttonMap[label]->toggleSelected();
            buttonMap[label]->onClick();
            return;
        }
    }
    zilched = true;
}


bool AggressiveAI::shouldBank(Game& game) {
    std::unique_ptr<Player>& aiPlayer = game.getPlayers()[game.getCurrentPlayer()];
    std::unique_ptr<Player>& realPlayer = game.getPlayers()[0];
    int numDiceHeld = 0;

    // Check if the Player already has met the win condition, if so keep rolling until score is higher than Player's
    if(realPlayer->getHardPoints() >= game.getWinConditionPoints()){
        if(realPlayer->getHardPoints() < (aiPlayer->getHardPoints() + aiPlayer->getSoftPoints())){
            // Enough points to beat player
            return true;
        } else {
            // Not enough points to beat player
            return false;
        }        
    }

    if(aiPlayer->getSoftPoints() < 300){
        return false;
    }

    // Check if the AI has enough to meet the win condition, if so then the AI should bank regardless
    if((aiPlayer->getHardPoints() + aiPlayer->getSoftPoints()) >= game.getWinConditionPoints()){
        // Enough points to win
        return true;       
    }


// IMPLEMENT AGGRESSIVE AI BEHAVIOR HERE //

    //Max amount it is willing to go before cashing in
    if(aiPlayer->getSoftPoints() >= 2000){
        return true;
    }
    
    // Collect the number of remaining dice that aren't held
    for (int i = 0; i < NUM_DICE; i++) { 
        if (game.getDice()[i].held) {
            numDiceHeld += 1;
        }
    }

    // Create Risk Factor
    int riskFactor = rand() % 100 + 1;

    // Behavior before rolling again
    if (rolledAgain == 0){
        if(aiPlayer->getSoftPoints() < 300){
            return false;
        } else if(aiPlayer->getSoftPoints() >= 800 && numDiceHeld == 4){
            if(riskFactor > 75){
                return true;
            }
        } else if(aiPlayer->getSoftPoints() >= 600 && numDiceHeld == 5){
            if(riskFactor > 60){
                return true;
            }
        } else if(aiPlayer->getSoftPoints() > 300 && numDiceHeld == 5){
            if(riskFactor > 55){
                return true;
            }
        } else if(aiPlayer->getSoftPoints() == 300 && numDiceHeld >= 4){
            if(riskFactor > 70){
                return true;
            }
        } else {
            if(riskFactor > 65){
                return true;
            }
        }
    }

    // Behavior after rolling again once
    else if (rolledAgain == 1){
        if(aiPlayer->getSoftPoints() >= 1000 && numDiceHeld >= 4){
            if(riskFactor > 22){
                return true;
            }
        } else {
            if(riskFactor > 35){
                return true;
            }
        }
    }

    // Behavior after rolling again twice
    else if (rolledAgain >= 2){
        if(numDiceHeld == 1){
            if(riskFactor > 35){
                return true;
            }
        } else if (numDiceHeld == 2){
            if(riskFactor > 25){
                return true;
            }
        } else if (numDiceHeld == 3){
            if(riskFactor > 15){
                return true;
            }
        } else {
            if(riskFactor > 8){
                return true;
            }
        }
    }

    return false; // IF NO TRUE RETURNED //
}







////// Cautious AI //////
CautiousAI::CautiousAI(std::string name) : AIPlayer(name), zilched(false), rolledAgain(0) {}

//This function should be no different then the Aggressive AI
void CautiousAI::takeTurn(Game& game, SDL_Renderer* renderer, TTF_Font* font, Button rollButton, Button bankButton, Button mainmenu, SDL_Texture* bgTexture, SDL_Rect bgRect, std::unique_ptr<Player>& currentPlayer) {
    if(!game.checkGameEnd()){
        bool canRollAgain = false;

        while ((canRollAgain || !shouldBank(game)) && !zilched) { //CHECK IF THERE IS ANOTHER FREE ROLL
            if(canRollAgain){
                rolledAgain += 1;
            }

            canRollAgain = true;
            zilched = false;
            game.rollDice(renderer, font, bgTexture, bgRect);
            game.getPossibleHolds();

            // RENDERING OCCURS SO IT LOOKS LIKE THE AI ROLLED THE DICE
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, bgTexture, nullptr, &bgRect); // Render Background
            rollButton.render(renderer, font);  // Render the roll button
            bankButton.render(renderer, font);  // Render the bank button
            mainmenu.render(renderer, font);  // Render the return to menu button
                 
            // Render each button so that holds can be called individually
            for (Button& btn : game.getHoldButtons()) {
                btn.render(renderer, font);
            }
                 
            game.displaySoftScore(renderer, font); // Render the score for soft points
            game.displayHardScore(renderer, font); // Render the score for hard points
            game.displayHistory(renderer, font, game.getPlayers()[game.getCurrentPlayer()]); //Render History
            game.displayDice(renderer);  // Render dice and their hold states
     
            SDL_RenderPresent(renderer);  // Update the screen



            SDL_Delay(50);
            selectHands(game);
            


            // SO THAT RENDERING OCCURS DURING AI's TURN
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, bgTexture, nullptr, &bgRect); // Render Background
            rollButton.render(renderer, font);  // Render the roll button
            bankButton.render(renderer, font);  // Render the bank button
            mainmenu.render(renderer, font);  // Render the return to menu button
                
            // Render each button so that holds can be called individually
            for (Button& btn : game.getHoldButtons()) {
                btn.render(renderer, font);
            }
                
            game.displaySoftScore(renderer, font); // Render the score for soft points
            game.displayHardScore(renderer, font); // Render the score for hard points
            game.displayHistory(renderer, font, game.getPlayers()[game.getCurrentPlayer()]); //Render History
            game.displayDice(renderer);  // Render dice and their hold states
    
            SDL_RenderPresent(renderer);  // Update the screen

            SDL_Delay(500);


            // Check if all dice are held
            for (int i = 0; i < NUM_DICE; i++) { 
                if (!game.getDice()[i].held) {
                    canRollAgain = false;
                    break;
                }
            }
        }
        
        if(!zilched){
            game.bankCurrentPlayerScore();
        } else {
            zilched = false;
            game.getPlayers()[game.getCurrentPlayer()]->addZilch();

            // Zilch Pentalty
            if(game.getPlayers()[game.getCurrentPlayer()]->getZilches() == 3){
                game.getPlayers()[game.getCurrentPlayer()]->resetZilches();
                game.getPlayers()[game.getCurrentPlayer()]->addToHistory(-500, false);
                game.getPlayers()[game.getCurrentPlayer()]->addHardPoints(-500);
            }else{
                game.getPlayers()[game.getCurrentPlayer()]->addToHistory(0, true);
            }
            
            // Reset Soft Points
            game.getPlayers()[game.getCurrentPlayer()]->resetSoftPoints();

            // Reset Players First Roll
            game.getPlayers()[game.getCurrentPlayer()]->resetFirstRoll();

            // Next turn starts
            game.nextTurn();
        }
    }
}

void CautiousAI::selectHands(Game& game) {
    std::vector<Button>& buttons = game.getHoldButtons();
    std::unordered_map<std::string, Button*> buttonMap;
    for (Button& btn : buttons) {
        buttonMap[btn.getLabel()] = &btn;
    }

    for (const std::string& label : specialLabels) {
        if (buttonMap.count(label)) {
            buttonMap[label]->toggleSelected();
            buttonMap[label]->onClick();
            SDL_Delay(500); // Small delay for UI responsiveness
            return;
        }
    }


////////// THIS BELOW SHOULD BE CHANGED
    int diceRemaining = NUM_DICE;

    for (int i = 0; i < NUM_DICE; i++) { 
        if (game.getDice()[i].held) {
            diceRemaining -= 1;
        }
    }

    // Filter three-button combos based on dice remaining
    std::vector<std::vector<std::string>> threeFilteredCombos;
    for (const auto& combo : threeButtonCombos) {
        int comboSize = 0;
        if (combo[0].find("Three") != std::string::npos) comboSize += 3;
        else if (combo[0].find("Four") != std::string::npos) comboSize += 4;
    
        if (combo[1].find("Double") != std::string::npos) comboSize += 2;
        else if (combo[1].find("Single") != std::string::npos) comboSize += 1;

        if (combo[2].find("Double") != std::string::npos) comboSize += 2;
        else if (combo[2].find("Single") != std::string::npos) comboSize += 1;

        if (comboSize == diceRemaining) {
            threeFilteredCombos.push_back(combo);
        }
    }

    // Try selecting filtered three-button combos
    for (const auto& combo : threeFilteredCombos) {
        if (buttonMap.count(combo[0]) && buttonMap.count(combo[1]) && buttonMap.count(combo[2])) {
            buttonMap[combo[0]]->toggleSelected();
            buttonMap[combo[0]]->onClick();
            SDL_Delay(500);
    
            buttonMap[combo[1]]->toggleSelected();
            buttonMap[combo[1]]->onClick();
            SDL_Delay(500);

            buttonMap[combo[2]]->toggleSelected();
            buttonMap[combo[2]]->onClick();
            return;
        }
    }


    // Filter two-button combos based on dice remaining
    std::vector<std::vector<std::string>> filteredCombos;
    for (const auto& combo : twoButtonCombos) {
        int comboSize = 0;
        if (combo[0].find("Three") != std::string::npos) comboSize += 3;
        else if (combo[0].find("Four") != std::string::npos) comboSize += 4;
        else if (combo[0].find("Five") != std::string::npos) comboSize += 5;
        else if (combo[0].find("Double") != std::string::npos) comboSize += 2;
        else if (combo[0].find("Single") != std::string::npos) comboSize += 1;
    
        if (combo[1].find("Three") != std::string::npos) comboSize += 3;
        else if (combo[1].find("Four") != std::string::npos) comboSize += 4;
        else if (combo[1].find("Five") != std::string::npos) comboSize += 5;
        else if (combo[1].find("Double") != std::string::npos) comboSize += 2;
        else if (combo[1].find("Single") != std::string::npos) comboSize += 1;
        if (comboSize == diceRemaining) {
            filteredCombos.push_back(combo);
        }
    }

    // Try selecting filtered two-button combos
    for (const auto& combo : filteredCombos) {
        if (buttonMap.count(combo[0]) && buttonMap.count(combo[1])) {
            buttonMap[combo[0]]->toggleSelected();
            buttonMap[combo[0]]->onClick();
            SDL_Delay(500);
    
            buttonMap[combo[1]]->toggleSelected();
            buttonMap[combo[1]]->onClick();
            return;
        }
    }


    for (const std::string& label : priorityLabels) {
        if (buttonMap.count(label)) {
            buttonMap[label]->toggleSelected();
            buttonMap[label]->onClick();
            return;
        }
    }
    zilched = true;
}


bool CautiousAI::shouldBank(Game& game) {
    std::unique_ptr<Player>& aiPlayer = game.getPlayers()[game.getCurrentPlayer()];
    std::unique_ptr<Player>& realPlayer = game.getPlayers()[0];
    int numDiceHeld = 0;

    // Check if the Player already has met the win condition, if so keep rolling until score is higher than Player's
    if(realPlayer->getHardPoints() >= game.getWinConditionPoints()){
        if(realPlayer->getHardPoints() < (aiPlayer->getHardPoints() + aiPlayer->getSoftPoints())){
            // Enough points to beat player
            return true;
        } else {
            // Not enough points to beat player
            return false;
        }        
    }

    if(aiPlayer->getSoftPoints() < 300){
        return false;
    }


    // Check if the AI has enough to meet the win condition, if so then the AI should bank regardless
    if((aiPlayer->getHardPoints() + aiPlayer->getSoftPoints()) >= game.getWinConditionPoints()){
        // Enough points to win
        return true;       
    }

// IMPLEMENT Cautious AI BEHAVIOR HERE //

    //Max amount it is willing to go before cashing in
    if(aiPlayer->getSoftPoints() >= 1500){
        return true;
    }
    
    // Collect the number of remaining dice that aren't held
    for (int i = 0; i < NUM_DICE; i++) { 
        if (game.getDice()[i].held) {
            numDiceHeld += 1;
        }
    }

    // Create Risk Factor
    int riskFactor = rand() % 100 + 1;

    // Behavior before rolling again
    if (rolledAgain == 0){
        if(aiPlayer->getSoftPoints() < 300){
            return false;
        } else if(aiPlayer->getSoftPoints() >= 300 && numDiceHeld == 3){
            if(riskFactor > 88){
                return true;
            }
        } else if(aiPlayer->getSoftPoints() >= 500 && numDiceHeld == 4){
            if(riskFactor > 60){
                return true;
            }
        } else if(aiPlayer->getSoftPoints() >= 450 && numDiceHeld == 5){
            if(riskFactor > 55){
                return true;
            }
        } else if(aiPlayer->getSoftPoints() > 300 && numDiceHeld == 5){
            if(riskFactor > 55){
                return true;
            }
        } else if(aiPlayer->getSoftPoints() == 300 && numDiceHeld >= 4){
            if(riskFactor > 70){
                return true;
            }
        } else if(aiPlayer->getSoftPoints() > 1000){
            return true;
        } else {
            //FAIL SAFE
            if(riskFactor > 55){
                return true;
            }
        }
    }

    // Behavior after rolling again once
    else if (rolledAgain == 1){
        if(aiPlayer->getSoftPoints() < 500){
            return false;
        } else if(aiPlayer->getSoftPoints() > 1000){
            if(riskFactor > 15){
                return true;
            }
        } else if(aiPlayer->getSoftPoints() == 500 && numDiceHeld == 1){
            if(riskFactor > 65){
                return true;
            }
        } else if(aiPlayer->getSoftPoints() >= 500 && numDiceHeld == 1){
            if(riskFactor > 58){
                return true;
            }
        } else if(aiPlayer->getSoftPoints() < 600 && numDiceHeld == 2){
            if(riskFactor > 55){
                return true;
            }
        } else if(aiPlayer->getSoftPoints() >= 600 && numDiceHeld == 2){
            if(riskFactor > 50){
                return true;
            }
        } else if(aiPlayer->getSoftPoints() < 800 && numDiceHeld == 3){
            if(riskFactor > 42){
                return true;
            }
        } else if(aiPlayer->getSoftPoints() >= 800 && numDiceHeld == 3){
            if(riskFactor > 33){
                return true;
            }
        } else if(numDiceHeld >= 4){
            return true;
        } else {
            //FAIL SAFE
            if(riskFactor > 35){
                return true;
            }
        }
    }


    // Behavior after rolling again twice
    else if (rolledAgain >= 2){
        if(aiPlayer->getSoftPoints() < 1200){
            return true;
        } else if(numDiceHeld == 1){
            if(riskFactor > 18){
                return true;
            }
        } else if (numDiceHeld == 2){
            if(riskFactor > 12){
                return true;
            }
        } else {
            return true;
        }
    }

    return false; // IF NO TRUE RETURNED //
}


































////// Adaptive AI //////
AdaptiveAI::AdaptiveAI(std::string name) : AIPlayer(name), zilched(false), rolledAgain(0) {}

//This function should be no different then the Aggressive AI
void AdaptiveAI::takeTurn(Game& game, SDL_Renderer* renderer, TTF_Font* font, Button rollButton, Button bankButton, Button mainmenu, SDL_Texture* bgTexture, SDL_Rect bgRect, std::unique_ptr<Player>& currentPlayer) {
    rolledAgain = 0;
    if(!game.checkGameEnd()){
        bool canRollAgain = false;

        while ((canRollAgain || !shouldBank(game)) && !zilched) { //CHECK IF THERE IS ANOTHER FREE ROLL
            canRollAgain = true;
            zilched = false;
            game.rollDice(renderer, font, bgTexture, bgRect);

            // rolledAgain is special for Adaptive AI //
            rolledAgain += 1;            
            game.getPossibleHolds();

             // RENDERING OCCURS SO IT LOOKS LIKE THE AI ROLLED THE DICE
             SDL_RenderClear(renderer);
             SDL_RenderCopy(renderer, bgTexture, nullptr, &bgRect); // Render Background
             rollButton.render(renderer, font);  // Render the roll button
             bankButton.render(renderer, font);  // Render the bank button
             mainmenu.render(renderer, font);  // Render the return to menu button
                 
             // Render each button so that holds can be called individually
             for (Button& btn : game.getHoldButtons()) {
                 btn.render(renderer, font);
             }
                 
             game.displaySoftScore(renderer, font); // Render the score for soft points
             game.displayHardScore(renderer, font); // Render the score for hard points
             game.displayHistory(renderer, font, game.getPlayers()[game.getCurrentPlayer()]); //Render History
             game.displayDice(renderer);  // Render dice and their hold states
     
             SDL_RenderPresent(renderer);  // Update the screen



            
            selectHands(game);
            SDL_Delay(500);


            // SO THAT RENDERING OCCURS DURING AI's TURN
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, bgTexture, nullptr, &bgRect); // Render Background
            rollButton.render(renderer, font);  // Render the roll button
            bankButton.render(renderer, font);  // Render the bank button
            mainmenu.render(renderer, font);  // Render the return to menu button
                
            // Render each button so that holds can be called individually
            for (Button& btn : game.getHoldButtons()) {
                btn.render(renderer, font);
            }
                
            game.displaySoftScore(renderer, font); // Render the score for soft points
            game.displayHardScore(renderer, font); // Render the score for hard points
            game.displayHistory(renderer, font, game.getPlayers()[game.getCurrentPlayer()]); //Render History
            game.displayDice(renderer);  // Render dice and their hold states
    
            SDL_RenderPresent(renderer);  // Update the screen

            SDL_Delay(500);


            // Check if all dice are held
            for (int i = 0; i < NUM_DICE; i++) { 
                if (!game.getDice()[i].held) {
                    canRollAgain = false;
                    break;
                }
            }
        }
        
        if(!zilched){
            game.bankCurrentPlayerScore();
        } else {
            zilched = false;
            game.getPlayers()[game.getCurrentPlayer()]->addZilch();

            // Zilch Pentalty
            if(game.getPlayers()[game.getCurrentPlayer()]->getZilches() == 3){
                game.getPlayers()[game.getCurrentPlayer()]->resetZilches();
                game.getPlayers()[game.getCurrentPlayer()]->addToHistory(-500, false);
                game.getPlayers()[game.getCurrentPlayer()]->addHardPoints(-500);
            }else{
                game.getPlayers()[game.getCurrentPlayer()]->addToHistory(0, true);
            }
            
            // Reset Soft Points
            game.getPlayers()[game.getCurrentPlayer()]->resetSoftPoints();

            // Reset Players First Roll
            game.getPlayers()[game.getCurrentPlayer()]->resetFirstRoll();

            // Next turn starts
            game.nextTurn();
        }
    }
}

void AdaptiveAI::selectHands(Game& game) {
    std::vector<Button>& buttons = game.getHoldButtons();
    std::unordered_map<std::string, Button*> buttonMap;
    for (Button& btn : buttons) {
        buttonMap[btn.getLabel()] = &btn;
    }

    for (const std::string& label : specialLabels) {
        if (buttonMap.count(label)) {
            buttonMap[label]->toggleSelected();
            buttonMap[label]->onClick();
            SDL_Delay(500); // Small delay for UI responsiveness
            return;
        }
    }

    int diceRemaining = NUM_DICE;

    for (int i = 0; i < NUM_DICE; i++) { 
        if (game.getDice()[i].held) {
            diceRemaining -= 1;
        }
    }

    // Filter three-button combos based on dice remaining
    std::vector<std::vector<std::string>> threeFilteredCombos;
    for (const auto& combo : threeButtonCombos) {
        int comboSize = 0;
        if (combo[0].find("Three") != std::string::npos) comboSize += 3;
        else if (combo[0].find("Four") != std::string::npos) comboSize += 4;
    
        if (combo[1].find("Double") != std::string::npos) comboSize += 2;
        else if (combo[1].find("Single") != std::string::npos) comboSize += 1;

        if (combo[2].find("Double") != std::string::npos) comboSize += 2;
        else if (combo[2].find("Single") != std::string::npos) comboSize += 1;

        if (comboSize == diceRemaining) {
            threeFilteredCombos.push_back(combo);
        }
    }

    // Try selecting filtered three-button combos
    for (const auto& combo : threeFilteredCombos) {
        if (buttonMap.count(combo[0]) && buttonMap.count(combo[1]) && buttonMap.count(combo[2])) {
            buttonMap[combo[0]]->toggleSelected();
            buttonMap[combo[0]]->onClick();
            SDL_Delay(500);
    
            buttonMap[combo[1]]->toggleSelected();
            buttonMap[combo[1]]->onClick();
            SDL_Delay(500);

            buttonMap[combo[2]]->toggleSelected();
            buttonMap[combo[2]]->onClick();
            return;
        }
    }


    // Filter two-button combos based on dice remaining
    std::vector<std::vector<std::string>> filteredCombos;
    for (const auto& combo : twoButtonCombos) {
        int comboSize = 0;
        if (combo[0].find("Three") != std::string::npos) comboSize += 3;
        else if (combo[0].find("Four") != std::string::npos) comboSize += 4;
        else if (combo[0].find("Five") != std::string::npos) comboSize += 5;
        else if (combo[0].find("Double") != std::string::npos) comboSize += 2;
        else if (combo[0].find("Single") != std::string::npos) comboSize += 1;
    
        if (combo[1].find("Three") != std::string::npos) comboSize += 3;
        else if (combo[1].find("Double") != std::string::npos) comboSize += 2;
        else if (combo[1].find("Single") != std::string::npos) comboSize += 1;
        if (comboSize == diceRemaining) {
            filteredCombos.push_back(combo);
        }
    }

    // Try selecting filtered two-button combos
    for (const auto& combo : filteredCombos) {
        if (buttonMap.count(combo[0]) && buttonMap.count(combo[1])) {
            buttonMap[combo[0]]->toggleSelected();
            buttonMap[combo[0]]->onClick();
            SDL_Delay(500);
    
            buttonMap[combo[1]]->toggleSelected();
            buttonMap[combo[1]]->onClick();
            return;
        }
    }

    // InitialProbability of a safe roll
    float currentProbability = safeProbability(diceRemaining);
    // The nuance of each full roll adds more weight
    currentProbability -= rolledAgain * 0.03;

    // 1. Check if we should select a 3-button combo
    if (currentProbability < 0.92 && diceRemaining == 6) {
        for (const auto& combo : threeButtonCombos) {
            if (buttonMap.count(combo[0]) && buttonMap.count(combo[1]) && buttonMap.count(combo[2])) {
                // Select all three buttons
                buttonMap[combo[0]]->toggleSelected();
                buttonMap[combo[0]]->onClick();
                SDL_Delay(300);
    
                buttonMap[combo[1]]->toggleSelected();
                buttonMap[combo[1]]->onClick();
                SDL_Delay(300);
    
                buttonMap[combo[2]]->toggleSelected();
                buttonMap[combo[2]]->onClick();
                return;
            }
        }
    }

    // Store the best selection
    bool shouldHold = false;
    std::vector<std::string> bestCombo;
    float bestProbabilityDrop = 0.35; // Initialize with the threshold


    // Try selecting each two-button combo and evaluate its probability effect
    for (const auto& combo : twoButtonCombos) {
        if (buttonMap.count(combo[0]) && buttonMap.count(combo[1])) {
            // Temporarily assume we select this combo
            int newDiceRemaining = diceRemaining;

            if (combo[0].find("Four") != std::string::npos) newDiceRemaining -= 4;
            else if (combo[0].find("Three") != std::string::npos) newDiceRemaining -= 3;
            else if (combo[0].find("Double") != std::string::npos) newDiceRemaining -= 2;
            else if (combo[0].find("Single") != std::string::npos) newDiceRemaining -= 1;

            if (combo[1].find("Double") != std::string::npos) newDiceRemaining -= 2;
            else if (combo[1].find("Single") != std::string::npos) newDiceRemaining -= 1;

            if (newDiceRemaining < 0) continue; // Skip invalid combos

            // Compute probability after hypothetical selection
            float newProbability = safeProbability(newDiceRemaining);
            newProbability -= (rolledAgain * 0.01) + 0.01; //Accounting for this roll
            float probabilityDrop = currentProbability - newProbability;

            if (probabilityDrop < bestProbabilityDrop) {
                shouldHold = true;
                bestProbabilityDrop = probabilityDrop;
                bestCombo = combo;  // Store the best combo
            }
        }
    }


    // If we found a valid best combo, execute the selection logic
    if (shouldHold) {
        buttonMap[bestCombo[0]]->toggleSelected();
        buttonMap[bestCombo[0]]->onClick();
        SDL_Delay(500);

        buttonMap[bestCombo[1]]->toggleSelected();
        buttonMap[bestCombo[1]]->onClick();
    }

    // If it can see no better option select one thing
    for (const std::string& label : priorityLabels) {
        if (buttonMap.count(label)) {
            buttonMap[label]->toggleSelected();
            buttonMap[label]->onClick();
            return;
        }
    }
    zilched = true;
}


bool AdaptiveAI::shouldBank(Game& game) {
    
    std::unique_ptr<Player>& aiPlayer = game.getPlayers()[game.getCurrentPlayer()];
    std::unique_ptr<Player>& realPlayer = game.getPlayers()[0];
    int numDiceHeld = 0;

    // Check if the Player already has met the win condition, if so keep rolling until score is higher than Player's
    if(realPlayer->getHardPoints() >= game.getWinConditionPoints()){
        if(realPlayer->getHardPoints() < (aiPlayer->getHardPoints() + aiPlayer->getSoftPoints())){
            return true;
        } else {
            return false;
        }        
    }

    // Check if the AI can even bank
    if(aiPlayer->getSoftPoints() < 300){
        return false;
    }
    

    // Check if the AI has enough to meet the win condition, if so then the AI should bank regardless
    if((aiPlayer->getHardPoints() + aiPlayer->getSoftPoints()) >= game.getWinConditionPoints()){
        return true;       
    }

// IMPLEMENT Adaptive AI BEHAVIOR HERE //

    // Collect the number of remaining dice that aren't held
    for (int i = 0; i < NUM_DICE; i++) { 
        if (game.getDice()[i].held) {
            numDiceHeld += 1;
        }
    }

    // All possible combinations of remaining die
    int remainingDice = NUM_DICE - numDiceHeld;
    float probability = safeProbability(remainingDice);

    // Compute zilch probability
    //float zilchProbability = 1.0 - probability;

    // The nuance of each full roll adds more weight
    probability -= rolledAgain * 0.03;

    // Implement probability tolerance based on how well the player is doing
    // Should add an extra layer of caution based on SoftPoints 

    if(realPlayer->getHardPoints() >= aiPlayer->getHardPoints()){
        // If player is 1000+ ahead
        if(realPlayer->getHardPoints() >= (aiPlayer->getHardPoints()+ 1000)){
            if(probability < 0.3){
                return true;
            }
        } else if(realPlayer->getHardPoints() >= (aiPlayer->getHardPoints()+ 500)){
            // If player is 500+ ahead
            if(probability < 0.4){
                return true;
            }
        } else {        // If player = AI
            if(probability < 0.5){
                return true;
            }
        }

    } else { // Player points < AI points
        // If AI is 1000+ ahead
        if((realPlayer->getHardPoints() + 1000) <= aiPlayer->getHardPoints()){
            if(probability < 0.8){
                return true;
            }
        }
        // If AI is 500+ ahead
        else if(realPlayer->getHardPoints() <= aiPlayer->getHardPoints()){
            if(probability < 0.7){
                return true;
            }
        }

        // If AI is ahead
        else {
            if(probability < 0.55){
                return true;
            }
        }
    }
    return false;
}





float AdaptiveAI::safeProbability(int remainingDice){
    float totalOutcomes = pow(6, remainingDice);
    float favorableOutcomes = 0;

    if(remainingDice == 5){     // 98.6%
        // 5-of-a-kind, 4-of-a-kind, 3-of-a-kind, Doubles, and At least one "1" or "5"
        favorableOutcomes += 6;
        favorableOutcomes += 6 * 5 * combination(5, 4);
        favorableOutcomes += 6 * combination(5, 3) * 5 * 4;
        favorableOutcomes += totalOutcomes - pow(4, remainingDice);

        // **Correction Terms** (Avoid Double Counting)

        // Remove cases where three 1s or 5s were counted twice
        favorableOutcomes -= 2 * combination(5, 3) * pow(4, 2);

        // Remove cases where four 1s or 5s were counted multiple times
        favorableOutcomes -= 2 * combination(5, 4) * 4;

        // Remove cases where exactly two dice are 1s or 5s in a three-of-a-kind
        favorableOutcomes -= 4 * combination(5, 2) * 2;

        // Add back 5-of-a-kind cases (since they were subtracted too many times)
        favorableOutcomes += 2;

    } else if(remainingDice == 4){  // 83.9%
        // 4-of-a-kind, 3-of-a-kind, and At least one "1" or "5"  
        favorableOutcomes += 6;
        favorableOutcomes += 6 * combination(4, 3) * 5;
        favorableOutcomes += totalOutcomes - pow(4, 4);

        // **Correction Terms** (Avoid Double Counting)

        // Remove cases where three 1s or 5s were counted twice
        favorableOutcomes -= 2 * combination(4, 3) * 4;

        // Remove cases where exactly two dice are 1s or 5s in a three-of-a-kind
        favorableOutcomes -= 4 * combination(4, 2) * 2;

        // Add back 4-of-a-kind cases (since they were subtracted too many times)
        favorableOutcomes += 2;

    } else if(remainingDice == 3){  // 72.2%
        // 3-of-a-kind, and At least one "1" or "5" 
        favorableOutcomes += 6;
        favorableOutcomes += totalOutcomes - pow(4, 3);

        // **Correction Terms**
        // Remove cases where all three dice are 1s or 5s
        favorableOutcomes -= 2;

    } else if(remainingDice == 2){  // 55.6%
        // There is at least one "1" or "5"
        favorableOutcomes += totalOutcomes - pow(4, 2);
        
    } else if(remainingDice == 1){  // 33.3%
        // Only 1 and 5 are favorable outcomes
        favorableOutcomes += 2;
    } else {
        // This is because 6 dice are remaining guaranteeing points
        return 1.0;
    }

    float trueProbability = favorableOutcomes / totalOutcomes;
    return trueProbability;
}

//Helper function for probability
long long AdaptiveAI::combination(int n, int r) {
    if (r > n) return 0;  // Invalid case
    if (r == 0 || r == n) return 1;  // Base case

    long long result = 1;
    for (int i = 0; i < r; ++i) {
        result *= (n - i);
        result /= (i + 1);
    }
    return result;
}


//(current + remainingDice) * p + (1 - p) * 0