#include "entity.hh"
#include "status_effect.hh"
#include "util.hh"
#include <climits>
#include <cmath>
#include <cstdint>
#include <format>
#include <sstream>
#include <string_view>

namespace potmaker {
    entity::entity(std::string name, const element_type element,
                   const double max_health, const double damage)
        : named(std::move(name)), element_(element), max_health_(max_health),
          health_(max_health), damage_(damage), skips_turn_(false)
    {}

    auto entity::tick() -> void
    {
        for (auto it = status_effects_.begin(); it != status_effects_.end();) {
            try {
                if (it->valueless_by_exception()) {
                    it = status_effects_.erase(it);
                    continue;
                }

                std::visit(
                        [this](auto&& effect) {
                            const double damage
                                    = effect.total_damage_per_turn();
                            this->modify_health(-damage);
                            effect.tick();
                        },
                        *it);

                const bool should_remove = std::visit(
                        [](const auto& effect) {
                            return effect.turns_left() <= 0;
                        },
                        *it);

                if (should_remove) { it = status_effects_.erase(it); }
                else {
                    ++it;
                }
            }
            catch (const std::bad_variant_access& e) {
                it = status_effects_.erase(it);
            }
        }
    }

    auto entity::modify_health(const double amount) -> void
    {
        health_ += amount;
    }

    auto entity::add_status_effect(status_effect_variant&& effect) -> void
    {
        if (!effect.valueless_by_exception()) {
            status_effects_.emplace_back(std::move(effect));
        }
    }

    auto entity::clear_status_effects() -> void
    {
        status_effects_.clear();
    }

    [[nodiscard]] auto entity::max_health() const -> double
    {
        return max_health_;
    }
    [[nodiscard]] auto entity::health() const -> double
    {
        return health_;
    }
    [[nodiscard]] auto entity::damage() const -> double
    {
        return damage_;
    }

    [[nodiscard]] auto entity::is_dead() const -> bool
    {
        return health_ <= 0;
    }

    [[nodiscard]] auto entity::is_frozen() const -> bool
    {
        for (const auto& effect_variant: status_effects_) {
            try {
                if (effect_variant.valueless_by_exception()) { continue; }

                const bool found_ice_effect = std::visit(
                        [](const auto& effect) -> bool {
                            return effect.element() == element_type::ice;
                        },
                        effect_variant);

                if (found_ice_effect) { return true; }
            }
            catch (const std::bad_variant_access& e) {
                continue; // which is basically unnecessary
            }
        }
        return false;
    }

    auto entity::status_effects() -> std::vector<status_effect_variant>&
    {
        return status_effects_;
    }

    // PLAYER

    player::player(std::string name, const double max_health,
                   const double damage, double const gold)
        : entity(std::move(name), element_type::boring, max_health, damage),
          gold_(gold)
    {}

    auto player::add_gold(const double gold) -> void
    {
        gold_ += gold;
    }

    auto player::remove_gold(const double gold) -> void
    {
        gold_ -= gold;
    }

    auto player::store_ingredient(ingredient* ing) -> void
    {
        stored_ingredients_.push_back(ing);
    }

    auto player::stored_ingredients() -> std::vector<ingredient*>&
    {
        return stored_ingredients_;
    }

    auto player::gold() const -> double
    {
        return gold_;
    }

    // ENEMY

    enemy::enemy(std::string name, const element_type element,
                 const std::int32_t level, const double max_health,
                 const double damage)
        : entity(std::move(name), element, max_health, damage), level_(level)
    {}

    auto enemy::level() const -> std::int32_t
    {
        return level_;
    }

#define POTMK_ENEMY_CONSTRUCTOR(ctor_name, type, MAXHP, DMG)                   \
    ctor_name::ctor_name(std::string name, const std::int32_t level)           \
        : enemy(std::move(name), type, level, MAXHP, DMG)                      \
    {}

    // Flaming: Medium HP (80-120 range), Medium DMG (8-12 range)
    POTMK_ENEMY_CONSTRUCTOR(
            flaming_enemy, element_type::fire,
            static_cast<uint16_t>(80 + (level * 4) * random_double(0.9, 1.1)),
            static_cast<uint16_t>((8 + (level / 3))
                                  * random_double(0.85, 1.15)));

    // Chilling: Low HP (50-80 range), Low DMG (4-7 range)
    POTMK_ENEMY_CONSTRUCTOR(
            chilling_enemy, element_type::ice,
            static_cast<uint16_t>(50 + (level * 3) * random_double(0.85, 1.15)),
            static_cast<uint16_t>((4 + (level / 4)) * random_double(0.8, 1.2)));

    // Poison: Medium HP (70-100 range), Low DMG (5-8 range)
    POTMK_ENEMY_CONSTRUCTOR(
            poisonous_enemy, element_type::nature,
            static_cast<uint16_t>(70 + (level * 3) * random_double(0.9, 1.1)),
            static_cast<uint16_t>((5 + (level / 5))
                                  * random_double(0.8, 1.15)));

    // Wither: Low HP (40-70 range), High DMG (12-18 range)
    POTMK_ENEMY_CONSTRUCTOR(
            withering_enemy, element_type::underworld,
            static_cast<uint16_t>(40 + (level * 3) * random_double(0.8, 1.2)),
            static_cast<uint16_t>((12 + (level / 2))
                                  * random_double(0.9, 1.15)));

    // Healing: Low HP (50-80 range), Low DMG (3-6 range)
    POTMK_ENEMY_CONSTRUCTOR(
            healing_enemy, element_type::regenerating,
            static_cast<uint16_t>(50 + (level * 3) * random_double(0.85, 1.15)),
            static_cast<uint16_t>((3 + (level / 5))
                                  * random_double(0.75, 1.25)));

    // Regenerative: Low HP (50-80 range), Low DMG (3-6 range)
    POTMK_ENEMY_CONSTRUCTOR(
            regenerative_enemy, element_type::healing,
            static_cast<uint16_t>(50 + (level * 3) * random_double(0.85, 1.15)),
            static_cast<uint16_t>((3 + (level / 5))
                                  * random_double(0.75, 1.25)));

    // Protective: High HP (120-180 range), Medium DMG (7-10 range)
    POTMK_ENEMY_CONSTRUCTOR(
            protective_enemy, element_type::protective,
            static_cast<uint16_t>(120 + (level * 6) * random_double(0.85, 1.1)),
            static_cast<uint16_t>((7 + (level / 4)) * random_double(0.9, 1.1)));

    // Strengthening: Low HP (40-60 range), High DMG (12-16 range)
    POTMK_ENEMY_CONSTRUCTOR(
            strengthening_enemy, element_type::strengthening,
            static_cast<uint16_t>(40 + (level * 2) * random_double(0.8, 1.2)),
            static_cast<uint16_t>((12 + (level / 3))
                                  * random_double(0.85, 1.15)));

    // Cleansing: Low HP (50-80 range), Low DMG (3-6 range)
    POTMK_ENEMY_CONSTRUCTOR(
            cleansing_enemy, element_type::purifying,
            static_cast<uint16_t>(50 + (level * 3) * random_double(0.85, 1.15)),
            static_cast<uint16_t>((3 + (level / 5))
                                  * random_double(0.75, 1.25)));

    auto flaming_enemy::act(player& p, std::vector<enemy*>& party) -> void
    {
        // 40% chance to attempt burning (slightly more aggressive than the
        // example)
        if (roll_chances(5)) { // 20% chance for special behavior
            // Sometimes spreads fire to multiple targets
            if (roll_chances(3)) {
                print_action(
                        std::format("{} engulfs everyone in flames!", name_));
                for (auto& target: party) {
                    if (roll_chances(2)) { // 50% chance to affect each ally
                        target->add_status_effect(
                                burning(1 * level_, level_ / 2));
                    }
                }
                p.add_status_effect(burning(2 * level_, level_));
            } // Sometimes does a powerful burn
            else {
                print_action(std::format("{} unleashes a searing blaze on {}!",
                                         name_, p.name()));
                p.add_status_effect(burning(3 * level_, level_ * 1.5));
            }
        }
        else if (roll_chances(2)) { // 50% chance for standard burn
            print_action(std::format("{} scorches {}", name_, p.name()));
            p.add_status_effect(burning(2 * level_, level_));
            // Small chance to chain burn to adjacent enemies
            if (roll_chances(5) && !party.empty()) {
                size_t random_index = random_int(0, party.size() - 1);
                print_action(std::format("The flames spread to {}!",
                                         party[random_index]->name()));
                party[random_index]->add_status_effect(
                        burning(1 * level_, level_ / 2));
            }
        }
        else { // Default attack (with fiery flavor)
            print_action(std::format("{} attacks {} with burning fury!", name_,
                                     p.name()));
            // Burning enemies deal slightly more damage when not applying
            // status
            p.modify_health(-damage_ * 1.2);
        }
    }

    // CHILLING ENEMY - Freezes often but can miss
    auto chilling_enemy::act(player& p, std::vector<enemy*>& party) -> void
    {
        if (roll_chances(2)) { // 50% chance to try freezing
            if (roll_chances(4)) { // 25% chance to actually hit
                print_action(std::format("{} freezes {}", name_, p.name()));
                p.add_status_effect(freezing(2 * level_, level_));
            }
            else {
                print_action(std::format("{} attempts to freeze {} but misses!",
                                         name_, p.name()));
            }
        }
        else {
            print_action(std::format("{} attacks {}", name_, p.name()));
            p.modify_health(-damage_);
        }
    }

    // POISONOUS ENEMY - Seeks to poison frequently
    auto poisonous_enemy::act(player& p, std::vector<enemy*>& party) -> void
    {
        if (roll_chances(3)) { // 33% chance to poison
            print_action(std::format("{} poisons {}", name_, p.name()));
            p.add_status_effect(poison(3 * level_, level_));
        }
        else {
            print_action(std::format("{} attacks {}", name_, p.name()));
            p.modify_health(-damage_);
        }
    }

    // WITHERING ENEMY - Occasionally withers
    auto withering_enemy::act(player& p, std::vector<enemy*>& party) -> void
    {
        if (roll_chances(5)) { // 20% chance to wither
            print_action(std::format("{} withers {}", name_, p.name()));
            p.add_status_effect(wither(2 * level_, level_));
        }
        else {
            print_action(std::format("{} attacks {}", name_, p.name()));
            p.modify_health(-damage_);
        }
    }

    // HEALING ENEMY - Focuses on healing most wounded ally
    auto healing_enemy::act(player& p, std::vector<enemy*>& party) -> void
    {
        // Find most wounded ally
        enemy* most_wounded = nullptr;
        double lowest_health = 1.0;

        for (auto& ally: party) {
            if (ally != this) {
                double health_percent = ally->health() / ally->max_health();
                if (health_percent < lowest_health) {
                    lowest_health = health_percent;
                    most_wounded = ally;
                }
            }
        }

        // Heal if ally is below 50% health
        if (most_wounded && lowest_health < 0.5 && !roll_chances(4)) {
            double heal_amount = 10 + (level_ * 2);
            print_action(std::format("{} heals {} for {:.1f} HP", name_,
                                     most_wounded->name(), heal_amount));
            most_wounded->modify_health(heal_amount);
        }
        else {
            print_action(std::format("{} attacks {}", name_, p.name()));
            p.modify_health(-damage_);
        }
    }

    // REGENERATIVE ENEMY - Applies regeneration
    auto regenerative_enemy::act(player& p, std::vector<enemy*>& party) -> void
    {
        // Find most wounded ally
        enemy* most_wounded = nullptr;
        double lowest_health = 1.0;

        for (auto& ally: party) {
            if (ally != this) {
                const double health_percent
                        = ally->health() / ally->max_health();
                if (health_percent < lowest_health) {
                    lowest_health = health_percent;
                    most_wounded = ally;
                }
            }
        }

        if (most_wounded && lowest_health < 0.75 && !roll_chances(3)) {
            print_action(std::format("{} regenerates {}", name_,
                                     most_wounded->name()));
            most_wounded->add_status_effect(regeneration(3 * level_, level_));
        }
        else {
            print_action(std::format("{} attacks {}", name_, p.name()));
            p.modify_health(-damage_);
        }
    }

    // PROTECTIVE ENEMY - Frequently protects allies
    auto protective_enemy::act(player& p, std::vector<enemy*>& party) -> void
    {
        if (roll_chances(2)) { // 50% chance to protect
            // Find least protected ally
            enemy* least_protected = nullptr;
            int min_protection = INT_MAX;

            for (auto& ally: party) {
                if (ally != this) {
                    int prot = 0;
                    for (auto& effect: ally->status_effects()) {
                        if (std::holds_alternative<protection>(effect)) {
                            prot += std::get<protection>(effect).potency();
                        }
                    }
                    if (prot < min_protection) {
                        min_protection = prot;
                        least_protected = ally;
                    }
                }
            }

            if (least_protected) {
                print_action(std::format("{} protects {}", name_,
                                         least_protected->name()));
                least_protected->add_status_effect(
                        protection(2 * level_, level_));
            }
            else {
                p.modify_health(-damage_);
            }
        }
        else {
            print_action(std::format("{} attacks {}", name_, p.name()));
            p.modify_health(-damage_);
        }
    }

    // STRENGTHENING ENEMY - Occasionally strengthens allies
    auto strengthening_enemy::act(player& p, std::vector<enemy*>& party) -> void
    {
        if (roll_chances(4)) { // 25% chance to strengthen
            // Find ally with lowest strength
            enemy* weakest = nullptr;
            int min_strength = INT_MAX;

            for (auto& ally: party) {
                if (ally != this) {
                    int strngth = 0;
                    for (auto& effect: ally->status_effects()) {
                        if (std::holds_alternative<strength>(effect)) {
                            strngth += std::get<strength>(effect).potency();
                        }
                    }
                    if (strngth < min_strength) {
                        min_strength = strngth;
                        weakest = ally;
                    }
                }
            }

            if (weakest) {
                print_action(std::format("{} strengthens {}", name_,
                                         weakest->name()));
                weakest->add_status_effect(strength(3 * level_, level_));
            }
            else {
                p.modify_health(-damage_);
            }
        }
        else {
            print_action(std::format("{} attacks {}", name_, p.name()));
            p.modify_health(-damage_);
        }
    }

    // CLEANSING ENEMY - Purifies negative effects
    auto cleansing_enemy::act(player& p, std::vector<enemy*>& party) -> void
    {
        // Find ally with most negative effects
        enemy* most_afflicted = nullptr;
        int max_negative = 0;

        for (auto& ally: party) {
            if (ally != this) {
                int negative_count = 0;
                for (auto& effect: ally->status_effects()) {
                    if (std::holds_alternative<burning>(effect)
                        || std::holds_alternative<freezing>(effect)
                        || std::holds_alternative<poison>(effect)
                        || std::holds_alternative<wither>(effect)) {
                        negative_count++;
                    }
                }
                if (negative_count > max_negative) {
                    max_negative = negative_count;
                    most_afflicted = ally;
                }
            }
        }

        if (most_afflicted && max_negative > 0 && !roll_chances(3)) {
            print_action(std::format("{} cleanses {}", name_,
                                     most_afflicted->name()));
            most_afflicted->clear_status_effects();
        }
        else {
            print_action(std::format("{} attacks {}", name_, p.name()));
            p.modify_health(-damage_);
        }
    }
} // namespace potmaker
