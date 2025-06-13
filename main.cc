#include "potionmaker_game.hh"
#include <iostream>
#include <string>

auto main() -> int
{
    // Name prompt
    std::cout << "=== WELCOME TO POTIONMAKER ===\n";
    std::cout << "Enter your name: ";
    std::string player_name;

    // We don't use cin because we want to allow spaces in the player's name
    // which also prevents input getting stuck elsewhere
    std::getline(std::cin, player_name);

    if (player_name.empty()) { player_name = "Anonymous"; }

    // Start game
    potmaker::game_state game(player_name);
    game.main_menu();

    return 0;
}