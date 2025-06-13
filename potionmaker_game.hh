#ifndef POTIONMAKER_HH
#define POTIONMAKER_HH
#include "entity.hh"
#include "ingredient.hh"
#include <memory>
#include <string>
#include <vector>

namespace potmaker {

    /**
     * Represents an item that you can acquire in the game shop
     */
    struct shop_item {
        ingredient* item;
        double price;
        shop_item(ingredient* ing, double cost);
    };

    /**
     * Holds the entire game ecosystem. This object manages the flow of the game
     * as well as it holds the current state of it
     */
    class game_state {
    public:
        /**
         * Creates a new game state for the specified user
         * @param player_name The user's name
         */
        explicit game_state(std::string player_name);

        ~game_state();

        /**
         * Starts the game and its main loop
         */
        auto run() -> void;

        /**
         * Displays the game's main menu interface
         */
        auto main_menu() -> void;

        /**
         * Display's the game's actual game interface
         */
        auto start_game() -> void;

        /**
         * Displays a menu where the player can choose to attack the enemies
         * they are facing in their current stage
         */
        auto fight_menu() -> void;

        /**
         * Displays a menu where the player can acquire ingredients for potions
         * in exchange for gold
         */
        auto shop_menu() -> void;

        /**
         * Displays the game's credits
         */
        static auto credits() -> void;

        /**
         * Creates a list of enemies for the player to fight
         * @return The enemies that the player must fight
         */
        auto generate_enemies() -> std::vector<enemy*>;

        /**
         * Displays the name and HO of the enemies in the given vector of
         * enemies
         * @param enemies The enemies o display
         */
        static auto display_enemies(const std::vector<enemy*>& enemies) -> void;

        /**
         * Displays a menu where the user can build a potion
         * @return The ingredients that the custom potion contains
         */
        auto create_potion() -> std::vector<ingredient*>;

        /**
         * Displays a menu where the user can choose which enemy to apply the
         * potion to
         * @param potion The potion to throw
         * @param enemies The enemies to choose from
         */
        auto apply_potion(const std::vector<ingredient*>& potion,
                          const std::vector<enemy*>& enemies) -> void;

        /**
         * Displays the menu corresponding to an ongoing fight
         * @param enemies The enemies in the fight
         * @param initial_attack_type The attack type to perform
         * @return If the user won or not
         */
        auto fight_round(std::vector<enemy*>& enemies, int initial_attack_type)
                -> bool;

        /**
         * Handles an enemy's actions as well as updating their per-turn state
         * @param enemies The enemies to that will perform an action this round
         */
        auto enemy_turn(std::vector<enemy*>& enemies) const -> void;

        /**
         * Handles the player's attack choice and it's effects based on their
         * state (ingredients or no ingredients)
         * @param enemies The enemies to act on
         * @param attack_type The attack type chosen
         * @return If this action resulted in a win
         */
        auto player_turn(std::vector<enemy*>& enemies, int attack_type) -> bool;

        /**
         * Displays a menu where the user can choose to perform a basic attack
         * on an enemy
         * @param enemies The available enemies to pick from
         */
        auto basic_attack(const std::vector<enemy*>& enemies) -> void;

        /**
         * Handles emptying the enemy vector and free-ing the entities
         * @param enemies The entities to dispose of
         */
        static auto cleanup_dead_enemies(std::vector<enemy*>& enemies) -> void;

        /**
         * Helps repopulate the shop items
         */
        auto generate_shop_items() -> void;

        /**
         * Displays the shop menu
         */
        auto display_shop() const -> void;

        /**
         * Removes an item from the shop at the given index and gives it to the
         * user
         * @param index The index of the item
         * @return If the purchase was successful
         */
        auto buy_item(int index) -> bool;

        /**
         * Prints the player's state
         */
        auto display_player_status() -> void;

        /**
         * Prints the player's inventory
         */
        auto display_inventory() const -> void;

        /**
         * Helper for getting a number in a valid range
         * @param min The minimum accepetd value
         * @param max The maximum accepted value
         * @return The user's number
         */
        static auto get_user_choice(int min, int max) -> int;

        /**
         * Calculates how much gold the user earns after completing a battle
         * @param defeated_enemies The enemies that the user has defeated
         * @return The gold reward
         */
        auto
        calculate_gold_reward(const std::vector<enemy*>& defeated_enemies) const
                -> double;

    private:
        player* player_;
        std::vector<shop_item> shop_items_;
        int current_stage_;
        bool game_running_;
        // Memory management helpers
        std::vector<ingredient*> owned_ingredients_;
        std::vector<enemy*> owned_enemies_;
        auto cleanup_ingredients() -> void;
        auto cleanup_enemies() -> void;
    };

    /**
     * Allocates a new random ingredient
     * @param min_potency The min potency it can have
     * @param max_potency The max potency it can have
     * @return The allocated ingredient
     */
    auto create_random_ingredient(int min_potency = 1, int max_potency = 3)
            -> ingredient*;

    /**
     * Allocates a new enemy
     * @param level The level of the enemy
     * @return The enemy
     */
    auto create_random_enemy(int level) -> enemy*;

    /**
     * Dynamically creates an ingredient based on a type index
     * @param type The type of the ingredient as a number
     * @param name The name of the ingredient
     * @param potency The potenyc of the ingredient
     * @return The new ingredient
     */
    auto create_ingredient_by_type(int type, const std::string& name,
                                   int potency) -> ingredient*;
    /**
     * Dynamically creates an enemy based on a type index
     * @param type The type of the enemy as a number
     * @param name The name of the enemy
     * @param level The level of the enemy
     * @return
     */
    auto create_enemy_by_type(int type, const std::string& name, int level)
            -> enemy*;

    /**
     * Obtains a random ingredient name based on its type
     * @param type The type
     * @return a random name
     */
    auto get_random_ingredient_name(int type) -> std::string;

    /**
     * Obtains a random enemy name based on its type
     * @param type The type
     * @return a random name
     */
    auto get_random_enemy_name(int type) -> std::string;

} // namespace potmaker

#endif // POTIONMAKER_HH