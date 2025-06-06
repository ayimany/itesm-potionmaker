#ifndef ELEMENT_HH
#define ELEMENT_HH

#include <string_view>

enum class element_type : int {
  fire,
  ice,
  nature,
  underworld,
  blessing,
  holy,
  earth,
  blood,
  purity,
  chaos,
  boring
};

[[nodiscard]] constexpr auto
element_type_to_str(const element_type type) noexcept -> std::string_view {
  switch (type) {
  case element_type::fire:
    return "Fire";
  case element_type::ice:
    return "Ice";
  case element_type::nature:
    return "Nature";
  case element_type::underworld:
    return "Underworld";
  case element_type::blessing:
    return "Blessed";
  case element_type::holy:
    return "Holy";
  case element_type::earth:
    return "Earth";
  case element_type::blood:
    return "Blood";
  case element_type::purity:
    return "Purity";
  case element_type::chaos:
    return "Chaos";
  case element_type::boring:
    return "Boring";
  }
  return "Unknown";
}

#endif // ELEMENT_HH
