#include "dynamic_array.hh"
#include "menu_generation.hh"
#include "potionmaker_game.hh"

auto main() -> int {
  bool user_wants_to_exit = false;

  while (!user_wants_to_exit) {
    int choice = 0;
    potmaker::print_option_menu("Welcome to Potion Maker!",
                                {"Start game", "About & Credits", "Quit"});
    while (choice < 1 || choice > 3) {
      choice = potmaker::receive_input<int>("Your choice");

      switch (choice) {
      case 1: {

        auto name = potmaker::receive_input<std::string>("What is your name?");
        potmaker::main_menu(name);
        break;
      }
      case 2: {

        potmaker::print_credits();
        break;
      }
      case 3: {
        std::cout << "(In a very convincing Super Mario for the Nintendo DS "
                     "voice) Bye bye!";
        user_wants_to_exit = true;
        break;
      }
      default:
        break;
      }
    }
  }
  return 0;
}
