#ifndef ENTITY_HH
#define ENTITY_HH
#include "element_type.hh"
#include "status_effect.hh"
#include "util.hh"
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace potmaker {

    template<element_type element_t> class status_effect;
    class ingredient;

    class entity : public named {
    public:
        explicit entity(std::string name, element_type element,
                        double max_health, double damage);

        auto tick() -> void;

        auto modify_health(const double amount) -> void;

        auto add_status_effect(status_effect_variant&& effect) -> void;

        auto clear_status_effects() -> void;

        [[nodiscard]] auto max_health() const -> double;
        [[nodiscard]] auto health() const -> double;
        [[nodiscard]] auto damage() const -> double;
        [[nodiscard]] auto is_dead() const -> bool;
        [[nodiscard]] auto is_frozen() const -> bool;
        [[nodiscard]] auto status_effects()
                -> std::vector<status_effect_variant>&;

    protected:
        std::vector<status_effect_variant> status_effects_;
        element_type element_;
        std::string_view name_;
        double max_health_;
        double health_;
        double damage_;
        bool skips_turn_;
    };

    class player final : public entity {
    public:
        player(std::string name, double max_health, double damage, double gold);

        auto add_gold(double gold) -> void;
        auto remove_gold(double gold) -> void;

        auto store_ingredient(ingredient* ing) -> void;

        [[nodiscard]] auto stored_ingredients() -> std::vector<ingredient*>&;

        [[nodiscard]] auto gold() const -> double;

    private:
        std::vector<ingredient*> stored_ingredients_;
        double gold_;
    };

    class enemy : public entity {
    public:
        explicit enemy(std::string name, element_type element,
                       std::int32_t level, double max_health, double damage);

        virtual auto act(player& p, std::vector<enemy*>& party) -> void = 0;

        [[nodiscard]] auto level() const -> std::int32_t;

    protected:
        std::int32_t level_;
    };

    class flaming_enemy final : public enemy {
    public:
        explicit flaming_enemy(std::string name, std::int32_t level);
        auto act(player& p, std::vector<enemy*>& party) -> void override;
    };

    class chilling_enemy final : public enemy {
    public:
        explicit chilling_enemy(std::string name, std::int32_t level);
        auto act(player& p, std::vector<enemy*>& party) -> void override;
    };

    class poisonous_enemy final : public enemy {
    public:
        explicit poisonous_enemy(std::string name, std::int32_t level);
        auto act(player& p, std::vector<enemy*>& party) -> void override;
    };

    class withering_enemy final : public enemy {
    public:
        explicit withering_enemy(std::string name, std::int32_t level);
        auto act(player& p, std::vector<enemy*>& party) -> void override;
    };

    class healing_enemy final : public enemy {
    public:
        explicit healing_enemy(std::string name, std::int32_t level);
        auto act(player& p, std::vector<enemy*>& party) -> void override;
    };

    class regenerative_enemy final : public enemy {
    public:
        explicit regenerative_enemy(std::string name, std::int32_t level);
        auto act(player& p, std::vector<enemy*>& party) -> void override;
    };

    class protective_enemy final : public enemy {
    public:
        explicit protective_enemy(std::string name, std::int32_t level);
        auto act(player& p, std::vector<enemy*>& party) -> void override;
    };

    class strengthening_enemy final : public enemy {
    public:
        explicit strengthening_enemy(std::string name, std::int32_t level);
        auto act(player& p, std::vector<enemy*>& party) -> void override;
    };

    class cleansing_enemy final : public enemy {
    public:
        explicit cleansing_enemy(std::string name, std::int32_t level);
        auto act(player& p, std::vector<enemy*>& party) -> void override;
    };

} // namespace potmaker

#endif // ENTITY_HH
