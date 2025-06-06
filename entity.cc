#include "entity.hh"

#include "status_effect.hh"
#include "util.hh"

#include <cmath>
#include <sstream>

namespace potmaker {
entity::entity(const std::string_view name, const element_type element,
               const double max_health, const double health,
               const double damage)
    : status_effects_(0), element_(element), name_(name),
      max_health_(max_health), health_(health), damage_(damage),
      skips_turn_(false) {}

auto entity::on_turn_run() -> void {
  dynamic_array<std::size_t> ticked_out_effects(0);

  for (std::size_t i = 0; i < status_effects_.size(); i++) {
    status_effect *effect = status_effects_[i];

    effect->tick();
    effect->on_turn(this);

    if (effect->turns() <= 0) {
      ticked_out_effects.add(i);
      delete effect;
    }
  }

  status_effects_.remove_bulk_indices(ticked_out_effects);
}

auto entity::name() const -> std::string_view { return name_; }

auto entity::modify_health(health_delta_inductor *inductor, const double amount)
    -> void {
  double result = amount;
  if (amount < 0) {
    for (status_effect *effect : status_effects_) {
      result = effect->transform_damage_taken(this, result);
    }
  } else {
    for (status_effect *effect : status_effects_) {
      result = effect->transform_healing_received(this, result);
    }
  }

  if (result < 0) {
    print_action(std::format("{} has been hurt ({:.2f} HP) by {}", name_,
                             result, inductor->inductor_display_string()));
  } else {
    print_action(std::format("{} has been healed ({:.2f} HP) by {}", name_,
                             result, inductor->inductor_display_string()));
  }

  health_ = std::min(health_ + result, max_health_);
}

auto entity::flag_skip() -> void { skips_turn_ = true; }

auto entity::max_health() const -> double { return max_health_; }

auto entity::health() const -> double { return health_; }

auto entity::damage() const -> double {
  double dmg = damage_;
  for (status_effect *effect : status_effects_) {
    dmg = effect->transform_attack_damage(this, dmg);
  }

  return dmg;
}

auto entity::is_skipping() const -> bool { return skips_turn_; }

auto entity::element() const -> element_type { return element_; }

auto entity::is_dead() const -> bool { return health_ <= 0; }

auto entity::is_confused() const -> bool {
  return status_effects_
      .map<bool>([](status_effect *const &effect) {
        return effect->element() == element_type::chaos;
      })
      .contains(true);
}

auto entity::status_effects() -> dynamic_array<status_effect *> & {
  return status_effects_;
}

auto entity::add_status_effect(status_effect *effect) -> void {
  status_effects_.add(effect);
}

auto entity::clear_status_effects() -> void {
  // Since these effects will be passed in as allocations that later become our
  // responsibility, we'll free them
  for (const status_effect *effect : status_effects_) {
    delete effect;
  }

  status_effects_.clear();
}

auto entity::pretty_string() -> std::string {
  return std::format("{}, {:.2f} HP, {} Status Effects", name_, health_,
                     status_effects_.size());
}

auto entity::inspection_string() const -> std::string {
  std::stringstream ss;
  ss << "Name: " << name_ << "\n";
  ss << "Health: " << health_ << "\n";
  ss << "Type: " << element_type_to_str(element()) << "\n";

  if (status_effects_.size() > 0) {
    ss << "Status Effects:\n";
    for (const status_effect *effect : status_effects_) {
      ss << "- " << effect->pretty_string() << " " << effect->description()
         << "\n";
    }
  }

  return ss.str();
}

player::player(const std::string_view name, const double max_health_,
               const double health, const double damage)
    : entity(name, element_type::boring, max_health_, health, damage),
      ingredients_(0), cash_(0) {}

auto player::get_ingredients() -> dynamic_array<ingredient *> & {
  return ingredients_;
}

auto player::add_ingredient(ingredient *ing) -> void { ingredients_.add(ing); }

auto player::cash() const -> double { return cash_; }
auto player::add_cash(const double cash) -> void { cash_ += cash; }
auto player::subtract_cash(const double cash) -> void { cash_ -= cash; }

auto player::inductor_display_string() -> std::string {
  return std::format("The player, {}", name_);
}

enemy::enemy(const std::string_view name, const element_type element,
             const std::uint16_t level, const double max_health,
             const double health, const double damage, const bool affects_party)
    : entity(name, element, max_health, health, damage), level_(level),
      affects_party_(affects_party) {}

auto enemy::attack(entity *e) -> void {
  // Our health delta works with attack = -1; heal = 1, so the order is correct.
  const double dmg = random_double(damage(), damage() / 2.0);
  print_action(
      std::format("{} attacks {} for {:.2f} damage", name_, e->name(), dmg));
  e->modify_health(this, dmg);
}

auto enemy::act(player &p, dynamic_array<enemy *> &party) -> void {
  if (skips_turn_) {
    print_action(std::format("{} is frozen! Turn skipped.", name_));
    return;
  }

  const bool confusion_roll = is_confused() && roll_chances(3);
  const bool attacks = roll_chances(2);

  if (attacks) {
    if (confusion_roll) {
      attack(this);
    } else {
      attack(&p);
    }
  } else {
    if (affects_party_ && confusion_roll ||
        !affects_party_ && !confusion_roll) {
      apply_effect(&p);
    } else {
      entity *entity = party[random_int(0, party.size() - 1)];
      apply_effect(entity);
    }
  }
}

auto enemy::level() const -> std::uint16_t { return level_; }

auto enemy::to_string() const -> std::string {
  return std::format("{} - Level {}", name_, level_);
}

auto enemy::inductor_display_string() -> std::string {
  return std::format("The fiend, {}, LVL {}", name_, level_);
}

#define POTMK_ENEMY_CONSTRUCTOR(ctor_name, type, benevolent)                   \
  ctor_name::ctor_name(const std::string_view &name,                           \
                       const std::uint16_t level)                              \
      : enemy(name, type, level, 50.0 + (level * 10), 50.0 + (level * 10),     \
              -5.0 - (level * 2), benevolent) {}

POTMK_ENEMY_CONSTRUCTOR(flaming_enemy, element_type::fire, false);
POTMK_ENEMY_CONSTRUCTOR(chilling_enemy, element_type::ice, false);
POTMK_ENEMY_CONSTRUCTOR(poisonous_enemy, element_type::nature, false);
POTMK_ENEMY_CONSTRUCTOR(withering_enemy, element_type::underworld, false);
POTMK_ENEMY_CONSTRUCTOR(healing_enemy, element_type::blessing, true);
POTMK_ENEMY_CONSTRUCTOR(regenerative_enemy, element_type::holy, true);
POTMK_ENEMY_CONSTRUCTOR(protective_enemy, element_type::earth, true);
POTMK_ENEMY_CONSTRUCTOR(strengthening_enemy, element_type::blood, true);
POTMK_ENEMY_CONSTRUCTOR(cleansing_enemy, element_type::purity, true);
POTMK_ENEMY_CONSTRUCTOR(joker_enemy, element_type::chaos, false);

// There are better ways to do this, but we have to force polymorphism in a bit
// for this project

auto flaming_enemy::apply_effect(entity *e) -> void {
  status_effect *effect = new burning(2 * level_, level_);
  print_action(std::format("{} burns {}", name_, e->name(), effect->turns(),
                           effect->total_dpt()));

  e->add_status_effect(effect);
}

auto chilling_enemy::apply_effect(entity *e) -> void {
  status_effect *effect = new frozen(std::ceil(0.25 * level_), level_);
  print_action(std::format("{} freezes {}", name_, e->name(), effect->turns(),
                           effect->total_dpt()));

  e->add_status_effect(effect);
}

auto poisonous_enemy::apply_effect(entity *e) -> void {
  status_effect *effect = new poison(2 * level_, level_);
  print_action(std::format("{} poisons {}", name_, e->name(), effect->turns(),
                           effect->total_dpt()));

  e->add_status_effect(effect);
}

auto withering_enemy::apply_effect(entity *e) -> void {
  status_effect *effect = new wither(4 * level_, level_);
  print_action(std::format("{} withers {}", name_, e->name(), effect->turns(),
                           effect->total_dpt()));

  e->add_status_effect(effect);
}

auto healing_enemy::apply_effect(entity *e) -> void {
  const double delta = 5.5 * level_;

  print_action(std::format("{} heals {}", name_, e->name(), delta));

  e->modify_health(e, delta);
}

auto regenerative_enemy::apply_effect(entity *e) -> void {
  status_effect *effect = new regeneration(2 * level_, level_);
  print_action(std::format("{} applies regeneration to {}", name_, e->name(),
                           effect->turns(), -effect->total_dpt()));

  e->add_status_effect(effect);
}

auto protective_enemy::apply_effect(entity *e) -> void {
  status_effect *effect = new protection(4 * level_, level_);
  print_action(
      std::format("{} protects {}", name_, e->name(), effect->turns()));

  e->add_status_effect(effect);
}

auto strengthening_enemy::apply_effect(entity *e) -> void {
  status_effect *effect = new strength(2 * level_, level_);
  print_action(
      std::format("{} strengthens {}", name_, e->name(), effect->turns()));

  e->add_status_effect(effect);
}

auto cleansing_enemy::apply_effect(entity *e) -> void {
  print_action(std::format("{} cleanses {}", name_, e->name()));

  e->clear_status_effects();
}

auto joker_enemy::apply_effect(entity *e) -> void {
  status_effect *effect = new confused(2 * level_, level_);
  print_action(
      std::format("{} confuses {}", name_, e->name(), effect->turns()));

  e->add_status_effect(effect);
}

} // namespace potmaker