#include "ingredient.hh"

#include "entity.hh"
#include "status_effect.hh"
#include "util.hh"

#include <cmath>
#include <cstdint>
#include <format>
#include <string>
#include <string_view>

namespace potmaker {

ingredient::ingredient(const std::string_view name, const std::uint16_t potency,
                       const double damage_contribution)
    : name_(name), potency_(potency),
      damage_contribution_(damage_contribution) {}

auto ingredient::name() const -> std::string_view { return name_; }

auto ingredient::potency() const -> std::uint16_t { return potency_; }

auto ingredient::to_string() const -> std::string {
  return std::format("{} Potency {}", name_, potency_);
}

// The following constructors are pretty much the exact same, so we'll
// define them with a macro
#define POTMK_INGREDIENT_CONSTRUCTOR(ctor_name, contrib)                       \
  ctor_name::ctor_name(const std::string_view &name,                           \
                       const std::uint16_t potency)                            \
      : ingredient(name, potency, contrib) {}

POTMK_INGREDIENT_CONSTRUCTOR(flaming_ingredient, 5);
POTMK_INGREDIENT_CONSTRUCTOR(chilling_ingredient, 1);
POTMK_INGREDIENT_CONSTRUCTOR(poisonous_ingredient, 3);
POTMK_INGREDIENT_CONSTRUCTOR(withering_ingredient, 7);
POTMK_INGREDIENT_CONSTRUCTOR(healing_ingredient, -10);
POTMK_INGREDIENT_CONSTRUCTOR(regenerative_ingredient, 0);
POTMK_INGREDIENT_CONSTRUCTOR(protective_ingredient, 0);
POTMK_INGREDIENT_CONSTRUCTOR(strengthening_ingredient, 0);
POTMK_INGREDIENT_CONSTRUCTOR(cleansing_ingredient, 0);
POTMK_INGREDIENT_CONSTRUCTOR(joker_ingredient, 0);

#define POTMK_INDUCTOR_MESSAGE(name, disp)                                     \
  auto name::inductor_display_string() -> std::string {                        \
    return std::format("A " disp ", {}, (POT {})", name_, potency_);           \
  }

POTMK_INDUCTOR_MESSAGE(flaming_ingredient, "Flaming Ingredient");
POTMK_INDUCTOR_MESSAGE(chilling_ingredient, "Chilling Ingredient");
POTMK_INDUCTOR_MESSAGE(poisonous_ingredient, "Poisonous Ingredient");
POTMK_INDUCTOR_MESSAGE(withering_ingredient, "Withering Ingredient");
POTMK_INDUCTOR_MESSAGE(healing_ingredient, "Healing Ingredient");
POTMK_INDUCTOR_MESSAGE(regenerative_ingredient, "Regenerative Ingredient");
POTMK_INDUCTOR_MESSAGE(protective_ingredient, "Protective Ingredient");
POTMK_INDUCTOR_MESSAGE(strengthening_ingredient, "Strengthening Ingredient");
POTMK_INDUCTOR_MESSAGE(cleansing_ingredient, "Cleansing Ingredient");
POTMK_INDUCTOR_MESSAGE(joker_ingredient, "Chaos Ingredient");

// There are better ways to do all this. There is a lot of boilerplate. We are
// forcing polymorphism.

auto flaming_ingredient::on_applied(entity *e) -> void {
  if (roll_chances(2)) {
    print_action(std::format("{} caused {} to burn!", name_, e->name()));
    e->add_status_effect(new burning(2 * potency_, potency_));
  } else {
    print_action(std::format("{} had no effect.", name_));
  }
}

auto chilling_ingredient::on_applied(entity *e) -> void {
  if (roll_chances(2)) {
    print_action(std::format("{} froze {}!", name_, e->name()));
    e->add_status_effect(new frozen(std::ceil(0.5 * potency_), potency_));
  } else {
    print_action(std::format("{} had no effect.", name_));
  }
}

auto poisonous_ingredient::on_applied(entity *e) -> void {
  if (roll_chances(2)) {
    print_action(std::format("{} poisoned {}!", name_, e->name()));
    e->add_status_effect(new poison(4 * potency_, potency_));
  } else {
    print_action(std::format("{} had no effect.", name_));
  }
}

auto withering_ingredient::on_applied(entity *e) -> void {
  if (roll_chances(3)) {
    print_action(std::format("{} withered {}!", name_, e->name()));
    e->add_status_effect(new wither(2 * potency_, potency_));
  } else {
    print_action(std::format("{} had no effect.", name_));
  }
}

auto healing_ingredient::on_applied(entity *e) -> void {
  double delta = 15 * potency_;
  print_action(std::format("{} healed {} for {} hp!", name_, e->name(), delta));
  e->modify_health(this, delta);
}

auto regenerative_ingredient::on_applied(entity *e) -> void {
  if (roll_chances(2)) {
    print_action(std::format("{} is regenerating {}!", name_, e->name()));
    e->add_status_effect(new regeneration(2 * potency_, potency_));
  } else {
    print_action(std::format("{} had no effect.", name_));
  }
}

auto protective_ingredient::on_applied(entity *e) -> void {
  if (roll_chances(2)) {
    print_action(std::format("{} is protecting {}!", name_, e->name()));
    e->add_status_effect(new protection(2 * potency_, potency_));
  } else {
    print_action(std::format("{} had no effect.", name_));
  }
}

auto strengthening_ingredient::on_applied(entity *e) -> void {
  if (roll_chances(3)) {
    print_action(std::format("{} strengthened {}!", name_, e->name()));
    e->add_status_effect(new protection(potency_, potency_));
  } else {
    print_action(std::format("{} had no effect.", name_));
  }
}

auto cleansing_ingredient::on_applied(entity *e) -> void {
  print_action(std::format("{} cleansed {}!", name_, e->name()));
  e->clear_status_effects();
}

auto joker_ingredient::on_applied(entity *e) -> void {
  if (roll_chances(3)) {
    print_action(std::format("{} confused {}!", name_, e->name()));
    e->add_status_effect(new confused(2 * potency_, potency_));
  } else {
    print_action(std::format("{} had no effect.", name_));
  }
}

} // namespace potmaker
