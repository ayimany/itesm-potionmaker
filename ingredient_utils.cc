#include "ingredient_utils.hh"

#include "util.hh"

#include <array>
#include <stdexcept>
#include <string_view>

namespace potmaker {

auto constants::generate_ingredient(const int stage) -> ingredient * {
  const int index = random_int(0, 9);
  const int potency = random_int(stage, stage + 3);
  const int name_index = random_int(0, 9);

#define POTMK_CREATE_INGREDIENT(type)                                          \
  const std::string_view name = type##_ingredient_names[name_index];           \
  return new type##_ingredient(name, potency);

  switch (index) {
  case 0: {
    POTMK_CREATE_INGREDIENT(flaming)
  }
  case 1: {
    POTMK_CREATE_INGREDIENT(chilling)
  }
  case 2: {
    POTMK_CREATE_INGREDIENT(withering)
  }
  case 3: {
    POTMK_CREATE_INGREDIENT(healing)
  }
  case 4: {
    POTMK_CREATE_INGREDIENT(regenerative)
  }
  case 5: {
    POTMK_CREATE_INGREDIENT(healing)
  }
  case 6: {
    POTMK_CREATE_INGREDIENT(regenerative)
  }
  case 7: {
    POTMK_CREATE_INGREDIENT(protective)
  }
  case 8: {
    POTMK_CREATE_INGREDIENT(strengthening)
  }
  case 9: {
    POTMK_CREATE_INGREDIENT(joker)
  }
  default:
    throw std::runtime_error(
        "[FATAL] Invalid ingredient index from random operation");
  }
}

} // namespace potmaker