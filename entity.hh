#ifndef ENTITY_HH
#define ENTITY_HH
#include "dynamic_array.hh"
#include "element_type.hh"
#include "health_delta_inductor.hh"

#include <cstdint>
#include <string>
#include <string_view>

namespace potmaker {

class status_effect;
class ingredient;

class entity : virtual public health_delta_inductor {
public:
  explicit entity(std::string_view name, element_type element,
                  double max_health, double health, double damage);
  virtual ~entity() = default;

  auto on_turn_run() -> void;

  [[nodiscard]] auto name() const -> std::string_view;
  [[nodiscard]] auto max_health() const -> double;
  [[nodiscard]] auto health() const -> double;
  [[nodiscard]] auto damage() const -> double;
  [[nodiscard]] auto is_skipping() const -> bool;
  [[nodiscard]] auto element() const -> element_type;
  [[nodiscard]] auto is_dead() const -> bool;
  [[nodiscard]] auto is_confused() const -> bool;
  [[nodiscard]] auto status_effects() -> dynamic_array<status_effect *> &;
  [[nodiscard]] auto inspection_string() const -> std::string;

  auto modify_health(health_delta_inductor *inductor, double amount) -> void;

  auto add_status_effect(status_effect *effect) -> void;
  auto clear_status_effects() -> void;

  auto flag_skip() -> void;

  auto pretty_string() -> std::string;

protected:
  dynamic_array<status_effect *> status_effects_;
  element_type element_;
  std::string_view name_;
  double max_health_;
  double health_;
  double damage_;
  bool skips_turn_;
};

class player final : public entity {
public:
  player(std::string_view name, double max_health_, double health,
         double damage);

  auto get_ingredients() -> dynamic_array<ingredient *> &;
  auto add_ingredient(ingredient *ing) -> void;
  [[nodiscard]] auto cash() const -> double;
  auto add_cash(double cash) -> void;
  auto subtract_cash(double cash) -> void;

  auto inductor_display_string() -> std::string override;

private:
  dynamic_array<ingredient *> ingredients_;
  double cash_;
};

class enemy : public entity {
public:
  explicit enemy(std::string_view name, element_type element,
                 std::uint16_t level, double max_health, double health,
                 double damage, bool affects_party);

  ~enemy() override = default;

  auto attack(entity *e) -> void;
  auto act(player &p, dynamic_array<enemy *> &party) -> void;
  virtual auto apply_effect(entity *e) -> void = 0;

  [[nodiscard]] auto level() const -> std::uint16_t;
  [[nodiscard]] auto to_string() const -> std::string;

  auto inductor_display_string() -> std::string override;

protected:
  std::uint16_t level_;
  bool affects_party_;
};

class flaming_enemy final : public enemy {
public:
  explicit flaming_enemy(std::string_view const &name, std::uint16_t level);
  auto apply_effect(entity *e) -> void override;
};

class chilling_enemy final : public enemy {
public:
  explicit chilling_enemy(std::string_view const &name, std::uint16_t level);
  auto apply_effect(entity *e) -> void override;
};

class poisonous_enemy final : public enemy {
public:
  explicit poisonous_enemy(std::string_view const &name, std::uint16_t level);
  auto apply_effect(entity *e) -> void override;
};

class withering_enemy final : public enemy {
public:
  explicit withering_enemy(std::string_view const &name, std::uint16_t level);
  auto apply_effect(entity *e) -> void override;
};

class healing_enemy final : public enemy {
public:
  explicit healing_enemy(std::string_view const &name, std::uint16_t level);
  auto apply_effect(entity *e) -> void override;
};

class regenerative_enemy final : public enemy {
public:
  explicit regenerative_enemy(std::string_view const &name,
                              std::uint16_t level);
  auto apply_effect(entity *e) -> void override;
};

class protective_enemy final : public enemy {
public:
  explicit protective_enemy(std::string_view const &name, std::uint16_t level);
  auto apply_effect(entity *e) -> void override;
};

class strengthening_enemy final : public enemy {
public:
  explicit strengthening_enemy(std::string_view const &name,
                               std::uint16_t level);
  auto apply_effect(entity *e) -> void override;
};

class cleansing_enemy final : public enemy {
public:
  explicit cleansing_enemy(std::string_view const &name, std::uint16_t level);
  auto apply_effect(entity *e) -> void override;
};

class joker_enemy final : public enemy {
public:
  explicit joker_enemy(std::string_view const &name, std::uint16_t level);
  auto apply_effect(entity *e) -> void override;
};

} // namespace potmaker

#endif // ENTITY_HH
