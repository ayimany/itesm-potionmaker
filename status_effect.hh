#ifndef STATUS_EFFECT_HH
#define STATUS_EFFECT_HH

#include "element_type.hh"
#include "util.hh"
#include <sstream>
#include <string_view>
#include <utility>
#include <variant>

namespace potmaker {

    class entity;

    /**
     * Modifies how an entity attacks, heals and receives damage
     * throughout the game's course. May also deal damage per turn.
     * @tparam element_t
     */
    template<element_type element_t> class status_effect : public named {
    public:
        status_effect(std::string name, const int turns, const int potency,
                      const double base_damage_per_turn)
            : named(std::move(name)), dmg_per_turn_(base_damage_per_turn),
              turns_(turns), potency_(potency)
        {}

        /**
         * Transforms the healing received by the afflicted entity
         * @param amt The original input healing
         * @return The new healing value after being transformed
         */
        virtual auto modify_healing(const double amt) -> double { return amt; }

        /**
         * Transforms the damage received by the afflicted entity
         * @param amt The original input damage
         * @return The new damage value after being transformed
         */
        virtual auto modify_damage(const double amt) -> double { return amt; }

        /**
         * Transforms the attack dealt by the afflicted entity
         * @param amt The original output attack
         * @return The new attack value after being transformed
         */
        virtual auto modify_attack(const double amt) -> double { return amt; }

        /**
         * Ticks down the turn timer
         */
        auto tick() -> void { turns_--; }

        /**
         * @return The damage per turn this status effect deals
         */
        [[nodiscard]] auto total_damage_per_turn() const -> double
        {
            return dmg_per_turn_ * potency_;
        }

        /**
         * @return How many turns this effect has left
         */
        [[nodiscard]] auto turns_left() const -> int { return turns_; }

        /**
         * @return The numerical potency of this effect
         */
        [[nodiscard]] auto potency() const -> int { return potency_; }

        /**
         * @return This status effect's type of element
         */
        [[nodiscard]] static constexpr auto element() -> element_type
        {
            return element_t;
        }

    protected:
        element_type element_ = element_t;
        std::string_view name_;
        double dmg_per_turn_;
        int turns_;
        int potency_;
    };

    class burning final : public status_effect<element_type::fire> {
    public:
        burning(int turns, int potency);
        auto modify_damage(double amount) -> double override;
    };

    class freezing final : public status_effect<element_type::ice> {
    public:
        freezing(int turns, int potency);
        auto modify_damage(double amount) -> double override;
        auto modify_attack(double amount) -> double override;
    };

    class poison final : public status_effect<element_type::nature> {
    public:
        poison(int turns, int potency);
        auto modify_healing(double amount) -> double override;
    };

    class wither final : public status_effect<element_type::underworld> {
    public:
        wither(int turns, int potency);
        auto modify_healing(double amount) -> double override;
        auto modify_attack(double amount) -> double override;
    };

    class regeneration final
        : public status_effect<element_type::regenerating> {
    public:
        regeneration(int turns, int potency);
    };

    class protection final : public status_effect<element_type::protective> {
    public:
        protection(int turns, int potency);
        auto modify_damage(double amount) -> double override;
    };

    class strength final : public status_effect<element_type::strengthening> {
    public:
        strength(int turns, int potency);
        auto modify_damage(double amount) -> double override;
        auto modify_attack(double amount) -> double override;
    };

    template<element_type element_t>
    [[nodiscard]] auto to_description(status_effect<element_t>& effect)
            -> std::string
    {
        std::stringstream ss;

        auto damage_mod = effect.modify_damage(10) <=> 10;
        auto healing_mod = effect.modify_healing(10) <=> 10;
        auto attack_mod = effect.modify_attack(10) <=> 10;

        ss << "[ ( Status Effect : " << effect.name() << " ) ]\n";
        ss << "* Potency: " << effect.potency() << "\n";
        ss << "* Lasts for: " << effect.turns() << " turns\n";
        ss << "* Damage: " << effect.total_damage_per_turn() << " / turn\n";

        switch (damage_mod) {
        case std::partial_ordering::greater: {
            ss << "+ Increases damage received\n";
        } break;
        case std::partial_ordering::less: {
            ss << "- Decreases damage received\n";
        } break;
        default: {
        } break;
        }

        switch (healing_mod) {
        case std::partial_ordering::greater: {
            ss << "+ Increases healing received\n";
        } break;
        case std::partial_ordering::less: {
            ss << "- Decreases healing received\n";
        } break;
        default: {
        } break;
        }

        switch (attack_mod) {
        case std::partial_ordering::greater: {
            ss << "+ Increases damage dealt\n";
        } break;
        case std::partial_ordering::less: {
            ss << "- Decreases damage dealt\n";
        } break;
        default: {
        } break;
        }

        return ss.str();
    }

    using status_effect_variant
            = std::variant<burning, freezing, poison, wither, regeneration,
                           protection, strength>;

} // namespace potmaker

#endif // STATUS_EFFECT_HH