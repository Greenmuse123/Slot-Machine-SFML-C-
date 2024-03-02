#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <random>
#include <sstream>
#include <iomanip> // For std::setprecision
#include <SFML/Audio.hpp>


int main() {
    // Assuming enum Symbol and symbolIDs are defined as before
enum Symbol {
    BAR = 0,
    BELL,
    CHERRY,
    SEVEN,
    NONE // For unmatched symbols
};

std::vector<Symbol> currentSymbols(3, NONE);

    sf::RenderWindow window(sf::VideoMode(800, 600), "Slot Machine Game");

    sf::SoundBuffer spinSoundBuffer;
if (!spinSoundBuffer.loadFromFile("Sound/slotmachineRolling.wav")) {
    std::cerr << "Failed to load spin sound" << std::endl;
    return 1; // or handle the error appropriately
}
sf::SoundBuffer winSoundBuffer;
if (!winSoundBuffer.loadFromFile("Sound/winSound.wav")) {
    std::cerr << "Failed to load win sound" << std::endl;
    return 1; // Handle error appropriately
}

sf::SoundBuffer lossSoundBuffer;
if (!lossSoundBuffer.loadFromFile("Sound/loseSound.wav")) {
    std::cerr << "Failed to load loss sound" << std::endl;
    return 1; // Handle error appropriately
}

sf::Sound spinSound;
spinSound.setBuffer(spinSoundBuffer);

sf::Sound winSound;
winSound.setBuffer(winSoundBuffer);

sf::Sound lossSound;
lossSound.setBuffer(lossSoundBuffer);
lossSound.setLoop(true); // Make the loss sound loop

    // Initialize balance, bets, font, and text
    float balance = 100.00;
    std::vector<float> bets = {0.50, 1.00, 2.00, 3.00, 5.00};
    size_t currentBetIndex = 0;
    bool isMaxBet = false; // Flag for MAX BET

    sf::Font font;
    if (!font.loadFromFile("Arial.ttf")) {
        std::cerr << "Failed to load Arial.ttf" << std::endl;
        return 1;
    }

    sf::Text balanceText("Balance: $" + std::to_string(balance), font, 20);
    balanceText.setFillColor(sf::Color::White);
    balanceText.setPosition(10, 10);

    sf::Text betText("Bet: $" + std::to_string(bets[currentBetIndex]), font, 20);
    betText.setFillColor(sf::Color::White);
    betText.setPosition(650, 10);
    
    // Load the background image
    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("Sprites/SlotMachine/casinoBackground.png")) {
        std::cerr << "Failed to load background image" << std::endl;
        return 1;
    }
    sf::Sprite background(backgroundTexture);

    // Load the slot machine image
    sf::Texture slotMachineTexture;
    if (!slotMachineTexture.loadFromFile("Sprites/SlotMachine/slot-machine1.png")) {
        std::cerr << "Failed to load slot machine image" << std::endl;
        return 1;
    }
    sf::Sprite slotMachine(slotMachineTexture);
    slotMachine.setScale(0.5f, 0.8f);
    slotMachine.setPosition(200, 100);

    // Load the slot panel image
    sf::Texture slotPanelTexture;
    if (!slotPanelTexture.loadFromFile("Sprites/SlotMachine/slotPanel.png")) {
        std::cerr << "Failed to load slot panel image" << std::endl;
        return 1;
    }
    sf::Sprite slotPanel(slotPanelTexture);
    slotPanel.setScale(0.5f, 0.8f);
    slotPanel.setPosition(200, 100);

    // Load the lever textures
    sf::Texture leverUpTexture, leverDownTexture;
    if (!leverUpTexture.loadFromFile("Sprites/SlotMachine/leverUp.png")) {
        std::cerr << "Failed to load lever up image" << std::endl;
    }
    if (!leverDownTexture.loadFromFile("Sprites/SlotMachine/leverDown.png")) {
        std::cerr << "Failed to load lever down image" << std::endl;
    }
    sf::Sprite leverSprite(leverUpTexture);
    leverSprite.setScale(0.5, 0.8);
    leverSprite.setPosition(200, 100);

    // Load symbol textures
    sf::Texture barTexture, bellTexture, cherryTexture, sevenTexture;
    barTexture.loadFromFile("Sprites/SlotMachine/barSymbol.png");
    bellTexture.loadFromFile("Sprites/SlotMachine/bellSymbol.png");
    cherryTexture.loadFromFile("Sprites/SlotMachine/cherrySymbol.png");
    sevenTexture.loadFromFile("Sprites/SlotMachine/sevenSymbol.png");

    std::vector<sf::Texture*> textures = {&barTexture, &bellTexture, &cherryTexture, &sevenTexture};
    std::vector<sf::Sprite> symbols(3); // Assuming 3 symbols for simplicity

    

    // Random number generation setup
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> uni(0, textures.size() - 1);

    // Initial symbols setup
    for (auto& symbol : symbols) {
        int texIndex = uni(rng);
        symbol.setTexture(*textures[texIndex]);
        symbol.setScale(0.5f, 0.5f); // Scale symbols to fit your design
    }

    symbols[0].setPosition(317, 350); // Adjust positions as needed
    symbols[1].setPosition(382, 350);
    symbols[2].setPosition(448, 350);

     // Animation control variables
    bool leverPulled = false;
    bool animate = false;
    sf::Clock clock;
    float animationDuration = 2.0f;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            // Handle keyboard input for changing bet and placing MAX BET
        if (event.type == sf::Event::KeyPressed) {
            switch (event.key.code) {
                case sf::Keyboard::Up:
                    currentBetIndex = (currentBetIndex + 1) % bets.size();
                    isMaxBet = false;
                    break;
                case sf::Keyboard::Down:
                    if (currentBetIndex == 0) currentBetIndex = bets.size() - 1;
                    else --currentBetIndex;
                    isMaxBet = false;
                    break;
                case sf::Keyboard::M: // Assuming 'M' for MAX BET
                    isMaxBet = true;
                    break;
                default:
                    break;
            }
            
        }
        
            // Keyboard and mouse input for betting and lever pulling
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    if (leverSprite.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window)))) {
                        if (!animate && balance >= bets[currentBetIndex]) {
                            balance -= bets[currentBetIndex]; // Deduct bet from balance
                            leverSprite.setTexture(leverDownTexture);
                            leverPulled = true;
                            animate = true;
                            clock.restart(); // Start animation
                            spinSound.play(); // Play the spinning sound
                            lossSound.stop();//stop the loss sound when the wheel is spinning
                        }
                    }
                }
            }

            if (event.type == sf::Event::MouseButtonReleased) {
                if (event.mouseButton.button == sf::Mouse::Left && leverPulled) {
                    leverSprite.setTexture(leverUpTexture); // Reset lever appearance
                    leverPulled = false;
                }
            }
        }
    // Calculate the bet amount (handle MAX BET separately)
    float betAmount = isMaxBet ? balance : bets[currentBetIndex];

 // Before starting the animation and betting logic:
        if (isMaxBet) {
            betAmount = balance; // Set betAmount to the entire balance for MAX BET
        } else {
            betAmount = bets[currentBetIndex]; // Use the selected bet amount
        }

        if (event.type == sf::Event::MouseButtonPressed) {
            // Existing mouse button pressed logic...
            if (!animate && balance >= betAmount) { // Check against betAmount
                balance -= betAmount; // Deduct betAmount from balance
                // Existing logic to start animation and play spin sound...
            }
        }
      // Update the bet display (include MAX BET handling)
    std::stringstream betStream;
    if (isMaxBet) {
        betStream << "Bet: MAX ($" << std::fixed << std::setprecision(2) << betAmount << ")";
    } else {
        betStream << "Bet: $" << std::fixed << std::setprecision(2) << betAmount;
    }
    betText.setString(betStream.str());


        // Animation and game logic
        if (animate && clock.getElapsedTime().asSeconds() <= animationDuration) {
            for (auto& symbol : symbols) {
                symbol.move(0, 1); // Simple downward movement

                if (symbol.getPosition().y > 400) {
                    int texIndex = uni(rng); // Randomize symbol texture
                    symbol.setTexture(*textures[texIndex]);
                    symbol.setPosition(symbol.getPosition().x, 300); // Reset position
                }
            }
        } // After the animation logic
else if (animate) {
    animate = false; // Stop the animation
    leverPulled = false;
    spinSound.stop(); // Stop the spinning sound

    // Reset displayed symbols for each spin
    std::fill(currentSymbols.begin(), currentSymbols.end(), NONE);

    // Determine the symbols currently displayed
    for (size_t i = 0; i < symbols.size(); ++i) {
        const auto& symbolTexture = symbols[i].getTexture();
        for (size_t j = 0; j < textures.size(); ++j) {
            if (symbolTexture == textures[j]) {
                currentSymbols[i] = static_cast<Symbol>(j); // Cast to Symbol enum
                break;
            }
        }
    }

    // Check for winning combinations (simple case: all symbols match)
    bool win = false; // Flag to determine if the spin resulted in a win
    if (currentSymbols[0] == currentSymbols[1] && currentSymbols[1] == currentSymbols[2] && currentSymbols[0] != NONE) {
        float winMultiplier = 0.0f;
        switch (currentSymbols[0]) {
            case BAR: winMultiplier = 10.0f; break;
            case BELL: winMultiplier = 8.0f; break;
            case CHERRY: winMultiplier = 5.0f; break;
            case SEVEN: winMultiplier = 15.0f; break;
            default: break;
        }
        float winAmount = bets[currentBetIndex] * winMultiplier;
        balance += winAmount; // Update balance with winnings
        std::cout << "Win! Awarded: $" << winAmount << std::endl;
        win = true; // Mark as win
    }

    // Play the win or loss sound based on the outcome
    if (win) {
        winSound.play(); // Play the win sound
        lossSound.stop();
    } else {
        lossSound.play(); // Play the loss sound if no win
    }
}
// Update balance display with fixed two decimal places
std::stringstream balanceStream;
balanceStream << std::fixed << std::setprecision(2); // Set stream to fixed point notation with 2 decimal places
balanceStream << "Balance: $" << balance;
balanceText.setString(balanceStream.str());


        window.clear();
        window.draw(background);
        window.draw(slotMachine);
        window.draw(slotPanel);
        for (const auto& symbol : symbols) {
            window.draw(symbol);
        }
        window.draw(leverSprite);
        window.draw(balanceText);
        window.draw(betText);
        window.display();
    }

    return 0;
}
