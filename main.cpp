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
    std::vector<int> cards; // Placeholder for cards
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
        if (cards[card] > 0) {
            cards[card]--;
            return true;
        }
        return false;
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
};

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

    // Optional: print the starting deck
    // deck.printDeck();

    return 0;
}

// Function to prompt user for their name and return it
std::string getName() {
    std::string name;
    std::cout << "Enter name: ";
    std::getline(std::cin, name);
    return name;
}
