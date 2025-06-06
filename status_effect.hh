#ifndef STATUS_EFFECT_HH
#define STATUS_EFFECT_HH
#include "health_delta_inductor.hh"

#include <string_view>

#include "element_type.hh"

namespace potmaker {

class entity;

class status_effect : virtual public health_delta_inductor {
public:
  status_effect(std::string_view name, int turns, int potency, double dpt);
  virtual ~status_effect() = default;

  auto tick() -> void;

  virtual auto transform_healing_received(entity *e, double amount)
      -> double = 0;
  virtual auto transform_damage_taken(entity *e, double amount) -> double = 0;
  virtual auto transform_attack_damage(entity const *e, double amount)
      -> double = 0;

  virtual auto on_turn(entity *e) -> void;

  [[nodiscard]] auto total_dpt() const -> double;
  [[nodiscard]] auto turns() const -> int;
  [[nodiscard]] auto potency() const -> int;
  [[nodiscard]] virtual auto element() const -> element_type = 0;
  [[nodiscard]] auto pretty_string() const -> std::string;
  [[nodiscard]] virtual auto description() const -> std::string = 0;

protected:
  std::string_view name_;
  int turns_;
  int potency_;
  double dpt_;
};

class burning final : public status_effect {
public:
  burning(int turns, int potency);
  auto transform_healing_received(entity *e, double amount) -> double override;
  auto transform_damage_taken(entity *e, double amount) -> double override;
  auto transform_attack_damage(entity const *e, double amount)
      -> double override;
  [[nodiscard]] auto element() const -> element_type override;
  auto inductor_display_string() -> std::string override;
  auto description() const -> std::string override;
};

class frozen final : public status_effect {
public:
  frozen(int turns, int potency);
  auto transform_healing_received(entity *e, double amount) -> double override;
  auto transform_damage_taken(entity *e, double amount) -> double override;
  auto transform_attack_damage(entity const *e, double amount)
      -> double override;
  auto on_turn(entity *e) -> void override;
  [[nodiscard]] auto element() const -> element_type override;
  auto inductor_display_string() -> std::string override;
  auto description() const -> std::string override;
};

class poison final : public status_effect {
public:
  poison(int turns, int potency);
  auto transform_healing_received(entity *e, double amount) -> double override;
  auto transform_damage_taken(entity *e, double amount) -> double override;
  auto transform_attack_damage(entity const *e, double amount)
      -> double override;
  auto on_turn(entity *e) -> void override;
  [[nodiscard]] auto element() const -> element_type override;
  auto inductor_display_string() -> std::string override;
  auto description() const -> std::string override;
};

class wither final : public status_effect {
public:
  wither(int turns, int potency);
  auto transform_healing_received(entity *e, double amount) -> double override;
  auto transform_damage_taken(entity *e, double amount) -> double override;
  auto transform_attack_damage(entity const *e, double amount)
      -> double override;
  [[nodiscard]] auto element() const -> element_type override;
  auto inductor_display_string() -> std::string override;
  auto description() const -> std::string override;
};

class regeneration final : public status_effect {
public:
  regeneration(int turns, int potency);
  auto transform_healing_received(entity *e, double amount) -> double override;
  auto transform_damage_taken(entity *e, double amount) -> double override;
  auto transform_attack_damage(entity const *e, double amount)
      -> double override;
  [[nodiscard]] auto element() const -> element_type override;
  auto inductor_display_string() -> std::string override;
  auto description() const -> std::string override;
};

class protection final : public status_effect {
public:
  protection(int turns, int potency);
  auto transform_healing_received(entity *e, double amount) -> double override;
  auto transform_damage_taken(entity *e, double amount) -> double override;
  auto transform_attack_damage(entity const *e, double amount)
      -> double override;
  [[nodiscard]] auto element() const -> element_type override;
  auto inductor_display_string() -> std::string override;
  auto description() const -> std::string override;
};

class strength final : public status_effect {
public:
  strength(int turns, int potency);
  auto transform_healing_received(entity *e, double amount) -> double override;
  auto transform_damage_taken(entity *e, double amount) -> double override;
  auto transform_attack_damage(entity const *e, double amount)
      -> double override;
  [[nodiscard]] auto element() const -> element_type override;
  auto inductor_display_string() -> std::string override;
  auto description() const -> std::string override;
};

class confused final : public status_effect {
public:
  confused(int turns, int potency);
  auto transform_healing_received(entity *e, double amount) -> double override;
  auto transform_damage_taken(entity *e, double amount) -> double override;
  auto transform_attack_damage(entity const *e, double amount)
      -> double override;
  auto on_turn(entity *e) -> void override;
  [[nodiscard]] auto element() const -> element_type override;
  auto inductor_display_string() -> std::string override;
  auto description() const -> std::string override;
};

} // namespace potmaker

#endif // STATUS_EFFECT_HH