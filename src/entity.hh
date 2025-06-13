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

    /**
     * A generic entity which interacts in the game world. Can kill and be
     * killed
     */
    class entity : public named {
    public:
        /**
         * Constructs a new entity
         * @param name The name of the entity
         * @param element The element of the entity
         * @param max_health The max health of the entity
         * @param damage The base damage this entity deals
         */
        explicit entity(std::string name, element_type element,
                        double max_health, double damage);

        /**
         * Ticks active status effects
         */
        auto tick() -> void;

        /**
         * Induces a change in health
         * @param amount The amount that health will change by
         */
        auto modify_health(const double amount) -> void;

        /**
         * Adds a new status effect to this entity
         * @param effect The effect to add
         */
        auto add_status_effect(status_effect_variant&& effect) -> void;

        /**
         * Clears this entity's status effects
         */
        auto clear_status_effects() -> void;

        /**
         * @return The max health
         */
        [[nodiscard]] auto max_health() const -> double;

        /**
         * @return The current health
         */
        [[nodiscard]] auto health() const -> double;

        /**
         * @return The damage this creature inflicts
         */
        [[nodiscard]] auto damage() const -> double;

        /**
         * @return Whether this creature is dead
         */
        [[nodiscard]] auto is_dead() const -> bool;

        /**
         * @return Whether this creature is frozen
         */
        [[nodiscard]] auto is_frozen() const -> bool;

        /**
         * @return The list of active status effects in the entity
         */
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
        /**
         * Constructs a new player
         * @param name Player's name
         * @param max_health Max Health
         * @param damage Base damage dealt by the player
         * @param gold Starting amount of gold
         */
        player(std::string name, double max_health, double damage, double gold);

        /**
         * Adds gold to the player's account
         * @param gold The amount of gold
         */
        auto add_gold(double gold) -> void;

        /**
         * Removes gold from the player's account
         * @param gold The amount of gold
         */
        auto remove_gold(double gold) -> void;

        /**
         * Adds an ingredient to the player's inventory
         * @param ing The ingredient to add
         */
        auto store_ingredient(ingredient* ing) -> void;

        /**
         * @return The player's inventory of ingredients
         */
        [[nodiscard]] auto stored_ingredients() -> std::vector<ingredient*>&;

        /**
         * @return The player's available gold
         */
        [[nodiscard]] auto gold() const -> double;

    private:
        std::vector<ingredient*> stored_ingredients_;
        double gold_;
    };

    class enemy : public entity {
    public:
        /**
         * Constructs a new enemy
         * @param name The enemy's name
         * @param element The enemy's element
         * @param level The enemy's level
         * @param max_health The enemy's max health
         * @param damage The enemy's base damage
         */
        explicit enemy(std::string name, element_type element,
                       std::int32_t level, double max_health, double damage);

        /**
         * Determines what the creature will do once it is its turn
         * @param p The player
         * @param party The enemy party
         */
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
