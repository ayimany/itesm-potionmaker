#include "status_effect.hh"

#include "entity.hh"

#include "util.hh"

#include <complex>

namespace potmaker {

status_effect::status_effect(const std::string_view name, const int turns,
                             const int potency, double dpt)
    : name_(name), turns_(turns), potency_(potency), dpt_(dpt) {}

auto status_effect::tick() -> void { turns_--; }

auto status_effect::on_turn(entity *e) -> void {
  e->modify_health(this, total_dpt());
}

auto status_effect::total_dpt() const -> double { return dpt_ * potency_; }
auto status_effect::turns() const -> int { return turns_; }
auto status_effect::potency() const -> int { return potency_; }
auto status_effect::pretty_string() const -> std::string {
  return std::format("{}, {} Turns, {} Damage / Turn", name_, turns_, dpt_);
}

#define POTMK_STATUS_EFFECT_CONSTRUCTOR(type, dpt)                             \
  type::type(int turns, int potency)                                           \
      : status_effect(#type, turns, potency, dpt) {}

POTMK_STATUS_EFFECT_CONSTRUCTOR(burning, -3);
POTMK_STATUS_EFFECT_CONSTRUCTOR(frozen, -1);
POTMK_STATUS_EFFECT_CONSTRUCTOR(poison, -5);
POTMK_STATUS_EFFECT_CONSTRUCTOR(wither, -1);
POTMK_STATUS_EFFECT_CONSTRUCTOR(regeneration, 3);
POTMK_STATUS_EFFECT_CONSTRUCTOR(protection, 0);
POTMK_STATUS_EFFECT_CONSTRUCTOR(strength, 0);
POTMK_STATUS_EFFECT_CONSTRUCTOR(confused, 0);

auto burning::transform_healing_received(entity *e, const double amount)
    -> double {
  return amount;
}

auto burning::transform_damage_taken(entity *e, const double amount) -> double {
  return amount * std::pow(1.1, potency_);
}

auto burning::transform_attack_damage(entity const *e, const double amount)
    -> double {
  return amount;
}

auto burning::element() const -> element_type { return element_type::fire; }

auto burning::description() const -> std::string { return "(+Damage Taken)"; }

auto frozen::transform_healing_received(entity *e, const double amount)
    -> double {
  return amount;
}

auto burning::inductor_display_string() -> std::string {
  return std::format("Scorching Flames (POT {})", potency_);
}

auto frozen::transform_damage_taken(entity *e, const double amount) -> double {
  return amount * std::pow(1.25, potency_);
}

auto frozen::transform_attack_damage(entity const *e, const double amount)
    -> double {
  return amount * std::pow(0.9, potency_);
}

auto frozen::on_turn(entity *e) -> void { e->flag_skip(); }

auto frozen::element() const -> element_type { return element_type::ice; }

auto frozen::inductor_display_string() -> std::string {
  return std::format("Frigid Ice (POT {})", potency_);
}

auto frozen::description() const -> std::string {
  return "(+Damage Taken, -Attack Damage, +Skips Turn)";
}

auto poison::transform_healing_received(entity *e, const double amount)
    -> double {
  return amount / std::pow(1.5, potency_);
}

auto poison::transform_damage_taken(entity *e, const double amount) -> double {
  return amount;
}

auto poison::transform_attack_damage(entity const *e, const double amount)
    -> double {
  return amount;
}

auto poison::on_turn(entity *e) -> void {
  const double health_10p = e->max_health() * 0.1;
  const double damage = -5 * potency_;

  if (e->health() - damage > health_10p) {
    e->modify_health(this, damage);
  }
}

auto poison::element() const -> element_type { return element_type::nature; }

auto poison::inductor_display_string() -> std::string {
  return std::format("Sickening Poison (POT {})", potency_);
}

auto poison::description() const -> std::string {
  return "(-Healing Received, Stops at 10% MAXHP)";
}

auto wither::transform_healing_received(entity *e, const double amount)
    -> double {
  return 0.0;
}
auto wither::transform_damage_taken(entity *e, const double amount) -> double {
  return amount;
}
auto wither::transform_attack_damage(entity const *e, const double amount)
    -> double {
  return amount * std::pow(0.75, potency_);
}

auto wither::element() const -> element_type {
  return element_type::underworld;
}

auto wither::inductor_display_string() -> std::string {
  return std::format("Weakening Wither (POT {})", potency_);
}

auto wither::description() const -> std::string {
  return "(-No Healing, -Attack Damage)";
}

auto regeneration::transform_healing_received(entity *e, const double amount)
    -> double {
  return amount;
}
auto regeneration::transform_damage_taken(entity *e, const double amount)
    -> double {
  return amount;
}
auto regeneration::transform_attack_damage(entity const *e, const double amount)
    -> double {
  return amount;
}

auto regeneration::element() const -> element_type {
  return element_type::blessing;
}

auto regeneration::inductor_display_string() -> std::string {
  return std::format("Soothing Regenration (POT {})", potency_);
}

auto regeneration::description() const -> std::string { return ""; }

auto protection::transform_healing_received(entity *e, const double amount)
    -> double {
  return amount;
}
auto protection::transform_damage_taken(entity *e, const double amount)
    -> double {
  return amount * std::pow(0.8, potency_);
}
auto protection::transform_attack_damage(entity const *e, const double amount)
    -> double {
  return amount;
}

auto protection::element() const -> element_type { return element_type::earth; }

auto protection::inductor_display_string() -> std::string {
  return std::format("Guarding Protection (POT {})", potency_);
}

auto protection::description() const -> std::string {
  return "(-Damage Taken)";
}

auto strength::transform_healing_received(entity *e, const double amount)
    -> double {
  return amount;
}
auto strength::transform_damage_taken(entity *e, const double amount)
    -> double {
  return amount * std::pow(0.9, potency_);
}
auto strength::transform_attack_damage(entity const *e, const double amount)
    -> double {
  return amount * std::pow(1.5, potency_);
}

auto strength::element() const -> element_type { return element_type::blood; }

auto strength::inductor_display_string() -> std::string {
  return std::format("Powerful Strength (POT {})", potency_);
}

auto strength::description() const -> std::string {
  return "(-Damage Taken, +Damage Dealt)";
}

auto confused::transform_healing_received(entity *e, const double amount)
    -> double {
  const double amt = std::abs(amount);
  return random_double(-amt, amt);
}
auto confused::transform_damage_taken(entity *e, const double amount)
    -> double {
  const double amt = std::abs(amount);
  return random_double(-amt, amt);
}
auto confused::transform_attack_damage(entity const *e, const double amount)
    -> double {
  const double amt = std::abs(amount);
  return random_double(-amt, amt);
}
auto confused::on_turn(entity *e) -> void {
  if (roll_chances(5)) {
    e->flag_skip();
  }
}

auto confused::inductor_display_string() -> std::string {
  return std::format("Strange Confusion (POT {})", potency_);
}

auto confused::element() const -> element_type { return element_type::chaos; }

auto confused::description() const -> std::string {
  return "(Damage Dealt, Damage Taken and Healing Randomized)";
}

} // namespace potmaker