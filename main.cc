#include "potionmaker_game.hh"
#include <iostream>
#include <string>

auto main() -> int
{
    std::cout << "=== WELCOME TO POTIONMAKER ===\n";
    std::cout << "Enter your name, brave alchemist: ";
    std::string player_name;
    std::getline(std::cin, player_name);
    if (player_name.empty()) { player_name = "Anonymous Alchemist"; }
    potmaker::game_state game(player_name);
    game.main_menu();

    return 0;
}