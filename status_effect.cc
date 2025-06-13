#include "status_effect.hh"
#include "entity.hh"
#include "util.hh"
#include <cmath>
#include <complex>
#include <cstdint>
#include <format>
#include <string>
#include <string_view>

namespace potmaker {

#define POTMK_STATUS_EFFECT_CONSTRUCTOR(type, dpt)                             \
    type::type(int turns, int potency)                                         \
        : status_effect(#type, turns, potency, dpt)                            \
    {}

    POTMK_STATUS_EFFECT_CONSTRUCTOR(burning, -3);
    POTMK_STATUS_EFFECT_CONSTRUCTOR(freezing, -1);
    POTMK_STATUS_EFFECT_CONSTRUCTOR(poison, -5);
    POTMK_STATUS_EFFECT_CONSTRUCTOR(wither, -1);
    POTMK_STATUS_EFFECT_CONSTRUCTOR(regeneration, 3);
    POTMK_STATUS_EFFECT_CONSTRUCTOR(protection, 0);
    POTMK_STATUS_EFFECT_CONSTRUCTOR(strength, 0);

    // BURNING

    auto burning::modify_damage(const double amount) -> double
    {
        return amount * std::pow(1.1, potency_);
    }

    // FROZEN

    auto freezing::modify_damage(const double amount) -> double
    {
        return amount * std::pow(1.25, potency_);
    }

    auto freezing::modify_attack(const double amount) -> double
    {
        return amount * std::pow(0.9, potency_);
    }

    // POISON

    auto poison::modify_healing(const double amount) -> double
    {
        return amount / std::pow(1.5, potency_);
    }

    // WITHER

    auto wither::modify_healing(const double amount) -> double
    {
        return 0.0;
    }

    auto wither::modify_attack(const double amount) -> double
    {
        return amount * std::pow(0.75, potency_);
    }

    // REGENERATION

    // No overrides

    // PROTECTION

    auto protection::modify_damage(const double amount) -> double
    {
        return amount * std::pow(0.8, potency_);
    }

    // STRENGTH

    auto strength::modify_damage(const double amount) -> double
    {
        return amount * std::pow(0.9, potency_);
    }

    auto strength::modify_attack(const double amount) -> double
    {
        return amount * std::pow(1.5, potency_);
    }

} // namespace potmaker