#ifndef POTIONMAKER_HH
#define POTIONMAKER_HH
#include "entity.hh"
#include "ingredient.hh"
#include <memory>
#include <string>
#include <vector>

namespace potmaker {

    struct shop_item {
        ingredient* item;
        double price;
        shop_item(ingredient* ing, double cost): item(ing), price(cost) {}
    };

    class game_state {
    public:
        explicit game_state(std::string player_name);
        ~game_state();
        // Core game loop methods
        auto run() -> void;
        auto main_menu() -> void;
        auto start_game() -> void;
        auto fight_menu() -> void;
        auto shop_menu() -> void;
        auto credits() -> void;
        // Fight system
        auto generate_enemies() -> std::vector<enemy*>;
        auto display_enemies(const std::vector<enemy*>& enemies) -> void;
        auto create_potion() -> std::vector<ingredient*>;
        auto apply_potion(const std::vector<ingredient*>& potion,
                          std::vector<enemy*>& enemies) -> void;
        auto player_turn(std::vector<enemy*>& enemies) -> bool;
        auto enemy_turn(std::vector<enemy*>& enemies) -> void;
        auto fight_round(std::vector<enemy*>& enemies) -> bool;
        auto cleanup_dead_enemies(std::vector<enemy*>& enemies) -> void;
        // Shop system
        auto generate_shop_items() -> void;
        auto display_shop() -> void;
        auto buy_item(int index) -> bool;
        // Utility methods
        auto display_player_status() -> void;
        auto display_inventory() -> void;
        auto get_user_choice(int min, int max) -> int;
        auto calculate_gold_reward(const std::vector<enemy*>& defeated_enemies)
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

    // Factory functions for creating ingredients and enemies
    auto create_random_ingredient(int min_potency = 1, int max_potency = 3)
            -> ingredient*;
    auto create_random_enemy(int level) -> enemy*;
    auto create_ingredient_by_type(int type, const std::string& name,
                                   int potency) -> ingredient*;
    auto create_enemy_by_type(int type, const std::string& name, int level)
            -> enemy*;

    // Utility functions
    auto get_random_ingredient_name(int type) -> std::string;
    auto get_random_enemy_name(int type) -> std::string;

} // namespace potmaker

#endif // POTIONMAKER_HH