#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

// Function declaration
std::string getName();

enum class PlayerStatus { ACTIVE, STAY, BUST, FROZEN };

struct Player {
    std::string name;
    int score = 0;              // cumulative across all rounds
    int roundScore = 0;         // points earned this round only
    int secondChanceCount = 0;
    std::vector<std::string> cards;
    PlayerStatus status = PlayerStatus::ACTIVE;
};


class Deck {
private:
    std::map<std::string, int> cards;

public:
    Deck() {
        // Number cards
        cards["0"] = 1; cards["1"] = 1; cards["2"] = 2; cards["3"] = 3;
        cards["4"] = 4; cards["5"] = 5; cards["6"] = 6; cards["7"] = 7;
        cards["8"] = 8; cards["9"] = 9; cards["10"] = 10; cards["11"] = 11; cards["12"] = 12;

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
        auto it = cards.find(card);
        if (it != cards.end() && it->second > 0) {
            it->second--;
            return true;
        }
        return false;
    }

    int getCount(const std::string& card) const {
        auto it = cards.find(card);
        return (it != cards.end()) ? it->second : 0;
    }

    void printDeck() const {
        for (const auto& [card, count] : cards)
            std::cout << card << ": " << count << std::endl;
    }

    const std::map<std::string, int>& getCardsMap() const { return cards; }
};

// Probability of failure (drawing a duplicate number card)
double probabilityOfFailure(const Player& player, const Deck& deck) {
    int failureCount = 0;
    int totalRemaining = 0;

    for (const auto& [card, count] : deck.getCardsMap())
        totalRemaining += count;

    for (const auto& c : player.cards) {
        if (c >= "0" && c <= "12")
            failureCount += deck.getCount(c);
    }

    if (totalRemaining == 0) return 0.0;
    return double(failureCount) / totalRemaining;
}

// Update scores for the *current round*
void updateRoundScores(std::vector<Player>& players) {
    for (auto& player : players) {
        if (player.status == PlayerStatus::BUST) {
            player.roundScore = 0; // no points this round
            continue;
        }

        int total = 0;
        for (const auto& c : player.cards) {
            try {
                total += std::stoi(c); // only numeric cards
            } catch (...) {}
        }
        player.roundScore = total;
    }
}


// Find player by name (helper)
Player* findPlayer(std::vector<Player>& players, const std::string& name) {
    for (auto& p : players)
        if (p.name == name) return &p;
    return nullptr;
}

int main() {
    Deck deck;
    int numberPlayers;
    std::cout << "Hello! Welcome to Flip 7 Helper.\n";
    std::cout << "Please enter the number of players: ";
    std::cin >> numberPlayers;
    std::cin.ignore();

    std::vector<Player> players(numberPlayers);
    for (int i = 0; i < numberPlayers; i++) {
        std::cout << "Player #" << i + 1 << ":\n";
        players[i].name = getName();
    }

    std::cout << "\nPlayers:\n";
    for (const auto& p : players) std::cout << p.name << std::endl;

    std::string command;
    while (true) {
        std::cout << "\nCommands:\n- round : start a new round\n- scores : show current scores\n- deck : show current deck\n- bonus : give someone a flip seven bonus\n- exit : quit program\n"; /*- hands : show each players current hand\n*/
        std::cout << "\nEnter command: ";
        std::getline(std::cin, command);
    
        if (command == "exit") break;

        else if (command == "scores") {
            updateRoundScores(players);
            std::cout << "\nCurrent scores:\n";
            for (const auto& p : players)
                std::cout << p.name << ": " << p.score << std::endl;
        }
        
        // this is a hidden command, its not really useful round to round...
        else if (command == "hands") {
            std::cout << "\nPlayers' hands:\n";
            for (const auto& p : players) {
                std::cout << p.name << ": ";
                for (const auto& c : p.cards) std::cout << c << " ";
                std::cout << std::endl;
            }
        }
        
        else if (command == "deck") {
            std::cout << "\nRemaining deck:\n";
            deck.printDeck();
        }

        else if (command == "round") {
            // Reset players for a new round
            for (auto& p : players) {
                p.cards.clear();
                p.secondChanceCount = 0;
                p.status = PlayerStatus::ACTIVE;
            }

            std::cout << "\n--- New Round ---\n";

            bool roundOver = false;
            while (!roundOver) {
                roundOver = true; // will flip back to false if anyone is still active

                for (auto& player : players) {
                    if (player.status != PlayerStatus::ACTIVE) continue;

                    roundOver = false; // at least one player is still playing

                    double prob = probabilityOfFailure(player, deck);
                    std::cout << "\n" << player.name << ", probability of failure this turn: "
                            << prob * 100 << "%\n";
                    std::cout << (prob >= 0.5 ? "Suggestion: STAY\n" : "Suggestion: HIT\n");

                    std::string card;
                    bool valid = false;
                    while (!valid) {
                        std::cout << player.name << ", enter the card you were dealt (or type 'stay'): ";
                        std::getline(std::cin, card);

                        if (card == "stay") {
                            player.status = PlayerStatus::STAY;
                            std::cout << player.name << " chose to stay.\n";
                            valid = true;
                        } 
                        else if (deck.removeCard(card)) {
                            // === Handle special cards ===
                            if (card == "Freeze") {
                                std::string target;
                                std::cout << "Choose a player to freeze: ";
                                std::getline(std::cin, target);
                                Player* t = findPlayer(players, target);
                                if (t && t != &player) {
                                    t->status = PlayerStatus::FROZEN;
                                    std::cout << t->name << " is frozen!\n";
                                }
                            }
                            else if (card == "Flip Three") {
                                std::string target;
                                std::cout << "Choose a player to draw three cards: ";
                                std::getline(std::cin, target);
                                Player* t = findPlayer(players, target);
                                if (t) {
                                    for (int i = 0; i < 3 && t->status == PlayerStatus::ACTIVE; i++) {
                                        std::string extraCard;
                                        bool validExtra = false;
                                        while (!validExtra) {
                                            std::cout << t->name << ", enter extra card #" << i + 1 << ": ";
                                            std::getline(std::cin, extraCard);
                                            if (deck.removeCard(extraCard)) {
                                                bool isDuplicate = std::find(t->cards.begin(), t->cards.end(), extraCard) != t->cards.end();
                                                if (extraCard >= "0" && extraCard <= "12") {
                                                    if (isDuplicate) {
                                                        if (t->secondChanceCount > 0) {
                                                            t->secondChanceCount--;
                                                            std::cout << "Second Chance used!\n";
                                                        } else {
                                                            t->status = PlayerStatus::BUST;
                                                            std::cout << t->name << " busted!\n";
                                                        }
                                                    } else {
                                                        t->cards.push_back(extraCard);
                                                    }
                                                } else if (extraCard == "Second Chance") {
                                                    t->secondChanceCount++;
                                                } else if (extraCard == "Freeze" || extraCard == "Flip Three") {
                                                    std::cout << "Special card drawn during Flip Three ignored for simplicity.\n";
                                                }
                                                validExtra = true;
                                            } else {
                                                std::cout << "Invalid card, try again.\n";
                                            }
                                        }
                                    }
                                }
                            }
                            else if (card == "Second Chance") {
                                player.secondChanceCount++;
                                if (player.secondChanceCount > 1) {
                                    std::string giveTo;
                                    std::cout << "Give a Second Chance to another player: ";
                                    std::getline(std::cin, giveTo);
                                    Player* t = findPlayer(players, giveTo);
                                    if (t && t != &player) t->secondChanceCount++;
                                    player.secondChanceCount--;
                                }
                            } 
                            else { // === Number cards ===
                                bool isDuplicate = std::find(player.cards.begin(), player.cards.end(), card) != player.cards.end();
                                if (isDuplicate) {
                                    if (player.secondChanceCount > 0) {
                                        player.secondChanceCount--;
                                        std::cout << "Second Chance used to avoid bust!\n";
                                    } else {
                                        player.status = PlayerStatus::BUST;
                                        std::cout << player.name << " busted by drawing a duplicate!\n";
                                    }
                                } else {
                                    player.cards.push_back(card);

                                    // Check Flip 7 win condition
                                    int uniqueNumbers = 0;
                                    for (const auto& c : player.cards) {
                                        if (c >= "0" && c <= "12") uniqueNumbers++;
                                    }
                                    if (uniqueNumbers >= 7) {
                                        std::cout << player.name << " flipped 7! +15 bonus points!\n";
                                        player.score += 15;
                                        roundOver = true; // end round immediately
                                    }
                                }
                            }
                            valid = true;
                        } 
                        else {
                            std::cout << "Invalid card or not available. Try again.\n";
                        }
                    }
                }
            }
            updateRoundScores(players);
            // Apply round scores to cumulative totals
            for (auto& p : players) {
                p.score += p.roundScore;
            }
            std::cout << "\n--- Round Over ---\n";
            for (const auto& p : players) {
                std::cout << p.name << ": +" << p.roundScore 
                        << " this round (Total: " << p.score << ")\n";
            }

        }
        else if (command == "bonus") {
            std::string playerName;
            std::cout << "Enter the player's name to give 15 bonus points: ";
            std::getline(std::cin, playerName);

            Player* p = findPlayer(players, playerName);
            if (p) {
                p->score += 15;
                std::cout << p->name << " received +15 bonus points! Total score: " << p->score << "\n";
            } else {
                std::cout << "Player not found.\n";
            }
        }

        else {
            std::cout << "Unknown command.\n";
        }
    }

    std::cout << "Game ended.\n";
    return 0;
}

std::string getName() {
    std::string name;
    std::cout << "Enter name: ";
    std::getline(std::cin, name);
    return name;
}
