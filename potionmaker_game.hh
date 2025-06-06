#ifndef POTIONMAKER_HH
#define POTIONMAKER_HH
#include "entity.hh"

#include <string>

namespace potmaker {

auto main_menu(const std::string &player_name) -> void;
auto enter_battle(player &p, int stage) -> void;
auto enter_shop(player &p, int stage) -> void;
auto print_enemy_list(dynamic_array<enemy *> const &arr) -> void;
auto choose_enemy(dynamic_array<enemy *> const &arr) -> enemy *;
auto create_potion(player &p) -> dynamic_array<ingredient *>;

} // namespace potmaker

#endif // POTIONMAKER_HH