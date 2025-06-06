#include "enemy_utils.hh"

#include "util.hh"

#include <stdexcept>
#include <string_view>

namespace potmaker {

auto constants::generate_enemy(const int stage) -> enemy * {
  const int index = random_int(0, 9);
  const int level = random_int(1, stage);
  const int name_index = random_int(0, 9);

#define POTMK_CREATE_ENEMY(type)                                               \
  const std::string_view name = type##_enemy_names[name_index];                \
  return new type##_enemy(name, level);

  switch (index) {
  case 0: {
    const std::string_view name = flaming_enemy_names[name_index];
    return new flaming_enemy(name, level);
  }
  case 1: {
    POTMK_CREATE_ENEMY(chilling)
  }
  case 2: {
    POTMK_CREATE_ENEMY(withering)
  }
  case 3: {
    POTMK_CREATE_ENEMY(healing)
  }
  case 4: {
    POTMK_CREATE_ENEMY(regenerative)
  }
  case 5: {
    POTMK_CREATE_ENEMY(healing)
  }
  case 6: {
    POTMK_CREATE_ENEMY(regenerative)
  }
  case 7: {
    POTMK_CREATE_ENEMY(protective)
  }
  case 8: {
    POTMK_CREATE_ENEMY(strengthening)
  }
  case 9: {
    POTMK_CREATE_ENEMY(joker)
  }
  default:
    throw std::runtime_error(
        "[FATAL] Invalid enemy index from random operation");
  }
}
} // namespace potmaker
