#include <iostream>
#include <string>
#include <vector>
#include <map>

// Function declaration
std::string getName();

// Player struct to store player info
struct Player {
    std::string name;
    int score = 0;
    std::vector<std::string> cards; // <--- change from int to string
};

// Deck class
class Deck {
private:
    std::map<std::string, int> cards;

public:
    Deck() {
        // Number cards
        cards["0"] = 1;
        cards["1"] = 1;
        cards["2"] = 2;
        cards["3"] = 3;
        cards["4"] = 4;
        cards["5"] = 5;
        cards["6"] = 6;
        cards["7"] = 7;
        cards["8"] = 8;
        cards["9"] = 9;
        cards["10"] = 10;
        cards["11"] = 11;
        cards["12"] = 12;

        // Special cards
        cards["Freeze"] = 3;
        cards["Flip Three"] = 3;
        cards["Second Chance"] = 3;
        cards["Score x2"] = 1;
        cards["Score +2"] = 1;
        cards["Score +4"] = 1;
        cards["Score +6"] = 1;
        cards["Score +8"] = 1;
        cards["Score +10"] = 1;
    }

    bool removeCard(const std::string& card) {
        auto it = cards.find(card);       // search for the card
        if (it != cards.end() && it->second > 0) {
            it->second--;                // decrement count
            return true;                 // card successfully removed
        }
        return false;                     // card not available
    }


    int getCount(const std::string& card) const {
        auto it = cards.find(card);
        return (it != cards.end()) ? it->second : 0;
    }

    void printDeck() const {
        for (const auto& [card, count] : cards) {
            std::cout << card << ": " << count << std::endl;
        }
    }

    // getter to access the internal map for probability calculation
    const std::map<std::string, int>& getCardsMap() const {
        return cards;
    }
};

// Function to calculate probability of failure for a given player
double probabilityOfFailure(const Player& player, const Deck& deck) {
    int failureCount = 0;
    int totalRemaining = 0;

    // Count total cards remaining in the deck
    for (const auto& [card, count] : deck.getCardsMap()) {
        totalRemaining += count;
    }

    // Count cards that the player already has (number cards 0-12)
    for (const auto& c : player.cards) {
        try {
            int cardNum = std::stoi(c);  // convert string to integer
            if (cardNum >= 0 && cardNum <= 12) {
                failureCount += deck.getCount(c);
            }
        } catch (...) {
            // ignore non-number cards (special cards)
        }
    }

    if (totalRemaining == 0) return 0.0; // avoid division by zero
    return double(failureCount) / totalRemaining;
}

void updateScores(std::vector<Player>& players) {
    for (auto& player : players) {
        int total = 0;
        for (const auto& c : player.cards) {
            try {
                int val = std::stoi(c); // only number cards contribute
                total += val;
            } catch (...) {
                // ignore non-number cards for now
            }
        }
        player.score = total;
    }
}

// ------------------- main -------------------
int main() {
    Deck deck;  // fully usable here

    int numberPlayers;

    std::cout << "Hello! Welcome to Flip 7 Helper." << std::endl;
    std::cout << "Please enter the number of players: ";
    std::cin >> numberPlayers;
    std::cin.ignore(); // Clear newline left in input buffer

    std::vector<Player> players(numberPlayers);

    // Loop to get each player's name
    for (int i = 0; i < numberPlayers; i++) {
        std::cout << "Player #" << i + 1 << ":" << std::endl;
        players[i].name = getName();
    }

    // Print all player names
    std::cout << "\nPlayers:" << std::endl;
    for (const auto& player : players) {
        std::cout << player.name << std::endl;
    }

    std::cout << "\nStart dealing cards manually via commands!\n";

    while (true) {
        std::string command;
        std::cout << "\nEnter a command (turn, scores, hands, deck, quit): ";
        std::getline(std::cin, command);

        if (command == "quit") {
            std::cout << "Exiting Flip 7 Helper.\n";
            break;
        }
        else if (command == "turn") {
            std::cout << "\n--- New Turn ---\n";
            for (auto& player : players) {
                // Show probability for this player BEFORE drawing
                double prob = probabilityOfFailure(player, deck);
                std::cout << player.name << ", probability of failure this turn: "
                        << prob * 100 << "%\n";

                // Suggest hit or stay
                if (prob >= 0.4) {
                    std::cout << "Suggestion: STAY this turn.\n";
                } else {
                    std::cout << "Suggestion: HIT this turn.\n";
                }

                std::string card;
                bool valid = false;
                while (!valid) {
                    std::cout << player.name << ", enter the card you were dealt: ";
                    std::getline(std::cin, card);

                    if (deck.removeCard(card)) {
                        player.cards.push_back(card);
                        std::cout << card << " added to " << player.name << "'s hand.\n\n";
                        valid = true;
                    } else {
                        std::cout << "\nInvalid card or card not available. Please try again.\n\n";
                    }
                }
            }
            updateScores(players);
        }
        else if (command == "scores") {
            std::cout << "\nCurrent scores:\n";
            for (const auto& player : players) {
                std::cout << player.name << ": " << player.score << std::endl;
            }
        }
        else if (command == "hands") {
            std::cout << "\nPlayers' hands:\n";
            for (const auto& player : players) {
                std::cout << player.name << ": ";
                for (const auto& c : player.cards) {
                    std::cout << c << " ";
                }
                std::cout << std::endl;
            }
        }
        else if (command == "deck") {
            std::cout << "\nRemaining deck:\n";
            deck.printDeck();
        }
        else {
            std::cout << "Unknown command. Try again.\n";
        }
    }

    return 0;
}


// Function to prompt user for their name and return it
std::string getName() {
    std::string name;
    std::cout << "Enter name: ";
    std::getline(std::cin, name);
    return name;
}
