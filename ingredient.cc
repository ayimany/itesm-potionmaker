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

    ingredient::ingredient(std::string name, const std::int32_t potency)
        : named(std::move(name)), potency_(potency)
    {}

    auto ingredient::potency() const -> std::int32_t
    {
        return potency_;
    }

#define POTMK_INGREDIENT_CONSTRUCTOR(ctor_name)                                \
    ctor_name::ctor_name(std::string name, const std::int32_t potency)         \
        : ingredient(std::move(name), potency)                                 \
    {}

    POTMK_INGREDIENT_CONSTRUCTOR(flaming_ingredient);
    POTMK_INGREDIENT_CONSTRUCTOR(chilling_ingredient);
    POTMK_INGREDIENT_CONSTRUCTOR(poisonous_ingredient);
    POTMK_INGREDIENT_CONSTRUCTOR(withering_ingredient);
    POTMK_INGREDIENT_CONSTRUCTOR(healing_ingredient);
    POTMK_INGREDIENT_CONSTRUCTOR(regenerative_ingredient);
    POTMK_INGREDIENT_CONSTRUCTOR(protective_ingredient);
    POTMK_INGREDIENT_CONSTRUCTOR(strengthening_ingredient);
    POTMK_INGREDIENT_CONSTRUCTOR(cleansing_ingredient);
    POTMK_INGREDIENT_CONSTRUCTOR(joker_ingredient);

    // FLAMING - High damage burst with chance to burn
    auto flaming_ingredient::on_applied(entity& e) -> void
    {
        // Always deal immediate fire damage (scaled with potency)
        const double fire_dmg = 15.0 * potency_;
        print_action(std::format("{} explodes in flames on {}! (-{:.1f} HP)",
                                 name_, e.name(), fire_dmg));
        e.modify_health(-fire_dmg);

        // 30% chance to apply powerful burn
        if (roll_chances(3)) {
            print_action(std::format("{} is burning!", e.name()));
            e.add_status_effect(burning(3 * potency_, potency_ * 1.5));
        }
    }

    // CHILLING - Freeze chance with slowing effect
    auto chilling_ingredient::on_applied(entity& e) -> void
    {
        // Small immediate damage
        const double chill_dmg = 5.0 * potency_;
        print_action(std::format("{} chills {}! (-{:.1f} HP)", name_, e.name(),
                                 chill_dmg));
        e.modify_health(-chill_dmg);

        // 40% chance to freeze (shorter duration but strong effect)
        if (roll_chances(5)) {
            print_action(std::format("{} is frozen solid!", e.name()));
            e.add_status_effect(freezing(1 * potency_, potency_));
        }
        else if (roll_chances(2)) { // 50% chance for minor slow
            print_action(std::format("{} is slowed!", e.name()));
            e.add_status_effect(freezing(1, 1)); // Weak version
        }
    }

    // POISONOUS - Reliable poison application
    auto poisonous_ingredient::on_applied(entity& e) -> void
    {
        // Moderate initial damage
        const double poison_dmg = 8.0 * potency_;
        print_action(std::format("{} poisons {}! (-{:.1f} HP)", name_, e.name(),
                                 poison_dmg));
        e.modify_health(-poison_dmg);

        // 75% chance to poison (high success rate)
        if (!roll_chances(4)) { // 3 in 4 chance
            print_action(std::format("{} is poisoned!", e.name()));
            e.add_status_effect(poison(4 * potency_, potency_));
        }
    }

    // WITHERING - Chance for strong debuff
    auto withering_ingredient::on_applied(entity& e) -> void
    {
        // Moderate initial damage
        const double wither_dmg = 10.0 * potency_;
        print_action(std::format("{} withers {}! (-{:.1f} HP)", name_, e.name(),
                                 wither_dmg));
        e.modify_health(-wither_dmg);

        // 33% chance for strong wither
        if (roll_chances(3)) {
            print_action(std::format("{} is withered!", e.name()));
            e.add_status_effect(wither(2 * potency_, potency_ * 2));
        }
    }

    // HEALING - Direct healing (no chance to fail)
    auto healing_ingredient::on_applied(entity& e) -> void
    {
        const double heal_amt = 20.0 * potency_;
        print_action(std::format("{} heals {}! (+{:.1f} HP)", name_, e.name(),
                                 heal_amt));
        e.modify_health(heal_amt);
    }

    // REGENERATIVE - Guaranteed regeneration
    auto regenerative_ingredient::on_applied(entity& e) -> void
    {
        print_action(std::format("{} regenerates {}!", name_, e.name()));
        e.add_status_effect(regeneration(3 * potency_, potency_));

        // Small initial heal as well
        const double initial_heal = 5.0 * potency_;
        e.modify_health(initial_heal);
    }

    // PROTECTIVE - Guaranteed protection
    auto protective_ingredient::on_applied(entity& e) -> void
    {
        print_action(std::format("{} protects {}!", name_, e.name()));
        e.add_status_effect(protection(3 * potency_, potency_));

        // Small damage reduction immediately
        const double damage_reduction = 0.1 * potency_;
        e.modify_health(damage_reduction * e.max_health());
    }

    // STRENGTHENING - Guaranteed strength boost
    auto strengthening_ingredient::on_applied(entity& e) -> void
    {
        print_action(std::format("{} strengthens {}!", name_, e.name()));
        e.add_status_effect(strength(2 * potency_, potency_));
    }

    // CLEANSING - Guaranteed cleanse
    auto cleansing_ingredient::on_applied(entity& e) -> void
    {
        print_action(std::format("{} cleanses {}!", name_, e.name()));
        e.clear_status_effects();

        // Small heal if cleansing was needed
        if (!e.status_effects().empty()) {
            const double cleanse_heal = 5.0 * potency_;
            e.modify_health(cleanse_heal);
        }
    }

    // JOKER - Random powerful effect
    auto joker_ingredient::on_applied(entity& e) -> void
    {
        const int effect = random_int(1, 5);

        switch (effect) {
        case 1: // Mega burn
            print_action(std::format("{} spontaneously combusts!", e.name()));
            e.add_status_effect(burning(5 * potency_, potency_ * 2));
            break;
        case 2: // Deep freeze
            print_action(std::format("{} is flash frozen!", e.name()));
            e.add_status_effect(freezing(3 * potency_, potency_ * 2));
            break;
        case 3: // Super heal
            print_action(
                    std::format("{} is supercharged with health!", e.name()));
            e.modify_health(30 * potency_);
            break;
        case 4: // Stat flip
            print_action(std::format("{}'s stats go wild!", e.name()));
            e.modify_health(random_double(-20, 20) * potency_);
            break;
        case 5: // Lucky strike
            print_action(std::format("{} gets a lucky break!", e.name()));
            e.modify_health(-25 * potency_);
            break;
        default:
            throw std::invalid_argument("invalid chaos effect generation");
        }
    }

    auto to_description(ingredient const& ing) -> std::string
    {
        std::stringstream ss;
        ss << "[ ( Ingredient : " << ing.name() << " ) ]\n";
        ss << "* Potency: " << ing.potency() << "\n";
        return ss.str();
    }

} // namespace potmaker
