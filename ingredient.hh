#ifndef INGREDIENT_HH
#define INGREDIENT_HH

#include "health_delta_inductor.hh"

#include <cstdint>
#include <string>

namespace potmaker {

class status_effect;
class entity;

class ingredient : virtual public health_delta_inductor {
public:
  explicit ingredient(std::string_view name, std::uint16_t potency,
                      double damage_contribution);

  virtual ~ingredient() = default;

  virtual auto on_applied(entity *e) -> void = 0;

  [[nodiscard]] auto name() const -> std::string_view;

  [[nodiscard]] auto potency() const -> std::uint16_t;

  [[nodiscard]] auto to_string() const -> std::string;

protected:
  std::string_view name_;
  std::uint16_t potency_;
  double damage_contribution_;
};

class flaming_ingredient final : public ingredient {
public:
  explicit flaming_ingredient(const std::string_view &name,
                              std::uint16_t potency);
  auto on_applied(entity *e) -> void override;
  auto inductor_display_string() -> std::string override;
};

class chilling_ingredient final : public ingredient {
public:
  explicit chilling_ingredient(std::string_view const &name,
                               std::uint16_t potency);
  auto on_applied(entity *e) -> void override;
  auto inductor_display_string() -> std::string override;
};

class poisonous_ingredient final : public ingredient {
public:
  explicit poisonous_ingredient(std::string_view const &name,
                                std::uint16_t potency);
  auto on_applied(entity *e) -> void override;
  auto inductor_display_string() -> std::string override;
};

class withering_ingredient final : public ingredient {
public:
  explicit withering_ingredient(std::string_view const &name,
                                std::uint16_t potency);
  auto on_applied(entity *e) -> void override;
  auto inductor_display_string() -> std::string override;
};

class healing_ingredient final : public ingredient {
public:
  explicit healing_ingredient(std::string_view const &name,
                              std::uint16_t potency);
  auto on_applied(entity *e) -> void override;
  auto inductor_display_string() -> std::string override;
};

class regenerative_ingredient final : public ingredient {
public:
  explicit regenerative_ingredient(std::string_view const &name,
                                   std::uint16_t potency);
  auto on_applied(entity *e) -> void override;
  auto inductor_display_string() -> std::string override;
};

class protective_ingredient final : public ingredient {
public:
  explicit protective_ingredient(std::string_view const &name,
                                 std::uint16_t potency);
  auto on_applied(entity *e) -> void override;
  auto inductor_display_string() -> std::string override;
};

class strengthening_ingredient final : public ingredient {
public:
  explicit strengthening_ingredient(std::string_view const &name,
                                    std::uint16_t potency);
  auto on_applied(entity *e) -> void override;
  auto inductor_display_string() -> std::string override;
};

class cleansing_ingredient final : public ingredient {
public:
  explicit cleansing_ingredient(std::string_view const &name,
                                std::uint16_t potency);

  auto on_applied(entity *e) -> void override;
  auto inductor_display_string() -> std::string override;
};

class joker_ingredient final : public ingredient {
public:
  explicit joker_ingredient(std::string_view const &name,
                            std::uint16_t potency);

  auto on_applied(entity *e) -> void override;
  auto inductor_display_string() -> std::string override;
};

} // namespace potmaker

#endif // INGREDIENT_HH
