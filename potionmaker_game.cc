#include "potionmaker_game.hh"
#include "entity_names.hh"
#include "ingredient_names.hh"
#include "util.hh"
#include <algorithm>
#include <format>
#include <iostream>
#include <limits>

namespace potmaker {

    game_state::game_state(std::string player_name)
        : player_(new player(std::move(player_name), 100.0, 15.0, 50.0)),
          current_stage_(1), game_running_(true)
    {
        generate_shop_items();
    }

    game_state::~game_state()
    {
        delete player_;
        cleanup_ingredients();
        cleanup_enemies();
    }

    auto game_state::cleanup_ingredients() -> void
    {
        for (auto* ing: owned_ingredients_) { delete ing; }
        owned_ingredients_.clear();

        for (auto& shop_item: shop_items_) { delete shop_item.item; }
        shop_items_.clear();
    }

    auto game_state::cleanup_enemies() -> void
    {
        for (auto* enemy: owned_enemies_) { delete enemy; }
        owned_enemies_.clear();
    }

    auto game_state::run() -> void
    {
        print_divider("WELCOME TO POTIONMAKER!");
        std::cout << "A mystical adventure awaits, " << player_->name()
                  << "!\n\n";

        while (game_running_) { main_menu(); }
    }

    auto game_state::main_menu() -> void
    {
        std::cout << "\n=== MAIN MENU ===\n";
        std::cout << "1. Start Game\n";
        std::cout << "2. Credits\n";
        std::cout << "3. Quit\n";
        std::cout << "Choose an option: ";

        int choice = get_user_choice(1, 3);

        switch (choice) {
        case 1:
            start_game();
            break;
        case 2:
            credits();
            break;
        case 3:
            std::cout << "Thanks for playing Potionmaker!\n";
            game_running_ = false;
            break;
        }
    }

    auto game_state::start_game() -> void
    {
        bool in_game = true;

        while (in_game && game_running_) {
            display_player_status();

            std::cout << "\n=== GAME MENU ===\n";
            std::cout << "1. Fight (Stage " << current_stage_ << ")\n";
            std::cout << "2. Shop\n";
            std::cout << "3. View Inventory\n";
            std::cout << "4. Return to Main Menu\n";
            std::cout << "Choose an option: ";

            int choice = get_user_choice(1, 4);

            switch (choice) {
            case 1:
                fight_menu();
                break;
            case 2:
                shop_menu();
                break;
            case 3:
                display_inventory();
                break;
            case 4:
                in_game = false;
                break;
            }
        }
    }

    auto game_state::fight_menu() -> void
    {
        print_divider(std::format("STAGE {} BATTLE", current_stage_));

        auto enemies = generate_enemies();

        std::cout << "You encounter:\n";
        display_enemies(enemies);

        std::cout << "\n1. Attack with Potion\n";
        std::cout << "2. Run Away\n";
        std::cout << "Choose an action: ";

        int choice = get_user_choice(1, 2);

        if (choice == 2) {
            std::cout << "You flee from the battle!\n";
            cleanup_enemies();
            return;
        }

        bool battle_won = fight_round(enemies);

        if (battle_won) {
            double gold_reward = calculate_gold_reward(enemies);
            player_->add_gold(gold_reward);

            print_action(std::format("Victory! You earned {:.1f} gold!",
                                     gold_reward));
            current_stage_++;

            if (current_stage_ % 3 == 0) {
                print_special("You feel stronger from your victories!");
                // Small stat boost every 3 stages
                player_->modify_health(10);
            }
        }
        else {
            print_action("You have been defeated!");
            std::cout << "Game Over! You reached stage " << current_stage_
                      << "\n";
            game_running_ = false;
        }

        cleanup_enemies();
    }

    auto game_state::shop_menu() -> void
    {
        bool in_shop = true;

        while (in_shop) {
            print_divider("MYSTICAL SHOP");
            std::cout << "Gold: " << player_->gold() << "\n\n";

            display_shop();

            std::cout << "\nEnter item number to buy (0 to return): ";
            int choice
                    = get_user_choice(0, static_cast<int>(shop_items_.size()));

            if (choice == 0) { in_shop = false; }
            else {
                if (buy_item(choice - 1)) {
                    print_action("Purchase successful!");
                }
                else {
                    print_action("Not enough gold!");
                }
            }
        }
    }

    auto game_state::credits() -> void
    {
        print_divider("CREDITS");
        std::cout << "Potionmaker - A Text Adventure Game\n\n";
        std::cout << "Created by: [Your Name Here]\n";
        std::cout << "Engine: C++ with custom game framework\n";
        std::cout << "Special thanks to: DeepSeek AI for ingredient and enemy "
                     "names\n\n";
        std::cout << "Thank you for playing!\n\n";
        std::cout << "Press Enter to continue...";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    auto game_state::generate_enemies() -> std::vector<enemy*>
    {
        std::vector<enemy*> enemies;

        // Number of enemies increases with stage
        int enemy_count = 1 + (current_stage_ - 1) / 2;
        enemy_count = std::min(enemy_count, 4); // Cap at 4 enemies

        for (int i = 0; i < enemy_count; ++i) {
            auto* new_enemy = create_random_enemy(current_stage_);
            enemies.push_back(new_enemy);
            owned_enemies_.push_back(new_enemy);
        }

        return enemies;
    }

    auto game_state::display_enemies(const std::vector<enemy*>& enemies) -> void
    {
        for (size_t i = 0; i < enemies.size(); ++i) {
            auto* enemy = enemies[i];
            std::cout << std::format("{}. {} (Level {}) - HP: {:.1f}/{:.1f}\n",
                                     i + 1, enemy->name(), enemy->level(),
                                     enemy->health(), enemy->max_health());
        }
    }

    auto game_state::create_potion() -> std::vector<ingredient*>
    {
        std::vector<ingredient*> potion;
        auto& inventory = player_->stored_ingredients();

        if (inventory.empty()) {
            std::cout << "You have no ingredients to make a potion!\n";
            return potion;
        }

        std::cout << "\n=== POTION CRAFTING ===\n";
        display_inventory();

        std::cout << "Select ingredients for your potion (0 to finish):\n";

        while (true) {
            std::cout << "Add ingredient (0 to finish): ";
            int choice = get_user_choice(0, static_cast<int>(inventory.size()));

            if (choice == 0) break;

            auto* selected = inventory[choice - 1];
            potion.push_back(selected);
            inventory.erase(inventory.begin() + choice - 1);

            std::cout << "Added " << selected->name() << " to potion!\n";

            if (inventory.empty()) {
                std::cout << "No more ingredients available!\n";
                break;
            }
        }

        return potion;
    }

    auto game_state::apply_potion(const std::vector<ingredient*>& potion,
                                  std::vector<enemy*>& enemies) -> void
    {
        if (potion.empty()) {
            std::cout << "You throw an empty bottle! Nothing happens.\n";
            return;
        }

        print_action("You throw your potion!");

        // Apply each ingredient to all enemies
        for (auto* ingredient: potion) {
            std::cout << "\n" << ingredient->name() << " activates!\n";
            for (auto* enemy: enemies) {
                if (!enemy->is_dead()) { ingredient->on_applied(*enemy); }
            }
        }
    }

    auto game_state::player_turn(std::vector<enemy*>& enemies) -> bool
    {
        std::cout << "\n=== YOUR TURN ===\n";

        auto potion = create_potion();
        apply_potion(potion, enemies);

        cleanup_dead_enemies(enemies);

        return enemies.empty();
    }

    auto game_state::enemy_turn(std::vector<enemy*>& enemies) -> void
    {
        std::cout << "\n=== ENEMY TURN ===\n";

        for (auto* enemy: enemies) {
            if (!enemy->is_dead()) {
                enemy->tick(); // Apply status effects
                if (!enemy->is_dead() && !enemy->is_frozen()) {
                    enemy->act(*player_, enemies);
                }
            }
        }

        cleanup_dead_enemies(enemies);
    }

    auto game_state::fight_round(std::vector<enemy*>& enemies) -> bool
    {
        while (!enemies.empty() && !player_->is_dead()) {
            // Player turn
            bool player_won = player_turn(enemies);
            if (player_won) { return true; }

            // Enemy turn
            enemy_turn(enemies);

            // Player status effects
            player_->tick();

            // Display status
            std::cout << "\n--- STATUS ---\n";
            std::cout << std::format("{}: {:.1f}/{:.1f} HP\n", player_->name(),
                                     player_->health(), player_->max_health());

            if (!enemies.empty()) {
                std::cout << "Enemies remaining:\n";
                display_enemies(enemies);
            }
        }

        return !player_->is_dead();
    }

    auto game_state::cleanup_dead_enemies(std::vector<enemy*>& enemies) -> void
    {
        enemies.erase(
                std::remove_if(enemies.begin(), enemies.end(),
                               [](const enemy* e) { return e->is_dead(); }),
                enemies.end());
    }

    auto game_state::generate_shop_items() -> void
    {
        // Clear existing items
        for (auto& item: shop_items_) { delete item.item; }
        shop_items_.clear();

        // Generate 6-8 random items
        int item_count = random_int(6, 8);

        for (int i = 0; i < item_count; ++i) {
            auto* ingredient = create_random_ingredient(1, current_stage_ + 1);
            double price
                    = 10.0 + (ingredient->potency() * random_double(8.0, 15.0));
            shop_items_.emplace_back(ingredient, price);
        }
    }

    auto game_state::display_shop() -> void
    {
        std::cout << "Available Items:\n";
        for (size_t i = 0; i < shop_items_.size(); ++i) {
            const auto& item = shop_items_[i];
            std::cout << std::format("{}. {} (Potency: {}) - {:.1f} gold\n",
                                     i + 1, item.item->name(),
                                     item.item->potency(), item.price);
        }
    }

    auto game_state::buy_item(int index) -> bool
    {
        if (index < 0 || index >= static_cast<int>(shop_items_.size())) {
            return false;
        }

        const auto& item = shop_items_[index];
        if (player_->gold() >= item.price) {
            player_->remove_gold(item.price);
            player_->store_ingredient(item.item);

            // Remove from shop and regenerate
            shop_items_.erase(shop_items_.begin() + index);

            // Add a new item to keep shop stocked
            auto* new_ingredient
                    = create_random_ingredient(1, current_stage_ + 1);
            double price
                    = 10.0
                      + (new_ingredient->potency() * random_double(8.0, 15.0));
            shop_items_.emplace_back(new_ingredient, price);

            return true;
        }

        return false;
    }

    auto game_state::display_player_status() -> void
    {
        std::cout << "\n=== PLAYER STATUS ===\n";
        std::cout << std::format("Name: {}\n", player_->name());
        std::cout << std::format("Health: {:.1f}/{:.1f}\n", player_->health(),
                                 player_->max_health());
        std::cout << std::format("Gold: {:.1f}\n", player_->gold());
        std::cout << std::format("Current Stage: {}\n", current_stage_);
        std::cout << std::format("Ingredients: {}\n",
                                 player_->stored_ingredients().size());
    }

    auto game_state::display_inventory() -> void
    {
        std::cout << "\n=== INVENTORY ===\n";
        const auto& ingredients = player_->stored_ingredients();

        if (ingredients.empty()) {
            std::cout << "No ingredients in inventory.\n";
            return;
        }

        for (size_t i = 0; i < ingredients.size(); ++i) {
            std::cout << std::format("{}. {} (Potency: {})\n", i + 1,
                                     ingredients[i]->name(),
                                     ingredients[i]->potency());
        }
    }

    auto game_state::get_user_choice(int min, int max) -> int
    {
        int choice;
        while (true) {
            std::cin >> choice;

            if (std::cin.fail() || choice < min || choice > max) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(),
                                '\n');
                std::cout << "Invalid choice. Please enter a number between "
                          << min << " and " << max << ": ";
            }
            else {
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(),
                                '\n');
                return choice;
            }
        }
    }

    auto game_state::calculate_gold_reward(
            const std::vector<enemy*>& defeated_enemies) -> double
    {
        double total_reward = 0.0;

        for (const auto* enemy: defeated_enemies) {
            // Base reward based on enemy level
            double base_reward = 5.0 + (enemy->level() * 3.0);

            // Bonus for higher stages
            double stage_bonus = current_stage_ * 2.0;

            total_reward += base_reward + stage_bonus;
        }

        return total_reward * random_double(0.8, 1.2); // Add some variation
    }

    // Factory functions
    auto create_random_ingredient(int min_potency, int max_potency)
            -> ingredient*
    {
        int type = random_int(0, 9); // 10 types of ingredients
        int potency = random_int(min_potency, max_potency);
        std::string name = get_random_ingredient_name(type);

        return create_ingredient_by_type(type, name, potency);
    }

    auto create_random_enemy(int level) -> enemy*
    {
        int type = random_int(0, 8); // 9 types of enemies
        std::string name = get_random_enemy_name(type);

        return create_enemy_by_type(type, name, level);
    }

    auto create_ingredient_by_type(int type, const std::string& name,
                                   int potency) -> ingredient*
    {
        switch (type) {
        case 0:
            return new flaming_ingredient(name, potency);
        case 1:
            return new chilling_ingredient(name, potency);
        case 2:
            return new poisonous_ingredient(name, potency);
        case 3:
            return new withering_ingredient(name, potency);
        case 4:
            return new healing_ingredient(name, potency);
        case 5:
            return new regenerative_ingredient(name, potency);
        case 6:
            return new protective_ingredient(name, potency);
        case 7:
            return new strengthening_ingredient(name, potency);
        case 8:
            return new cleansing_ingredient(name, potency);
        case 9:
            return new joker_ingredient(name, potency);
        default:
            return new flaming_ingredient(name, potency);
        }
    }

    auto create_enemy_by_type(int type, const std::string& name, int level)
            -> enemy*
    {
        switch (type) {
        case 0:
            return new flaming_enemy(name, level);
        case 1:
            return new chilling_enemy(name, level);
        case 2:
            return new poisonous_enemy(name, level);
        case 3:
            return new withering_enemy(name, level);
        case 4:
            return new healing_enemy(name, level);
        case 5:
            return new regenerative_enemy(name, level);
        case 6:
            return new protective_enemy(name, level);
        case 7:
            return new strengthening_enemy(name, level);
        case 8:
            return new cleansing_enemy(name, level);
        default:
            return new flaming_enemy(name, level);
        }
    }

    auto get_random_ingredient_name(int type) -> std::string
    {
        using namespace constants;

        switch (type) {
        case 0:
            return std::string(flaming_ingredient_names[random_int(0, 9)]);
        case 1:
            return std::string(chilling_ingredient_names[random_int(0, 9)]);
        case 2:
            return std::string(poisonous_ingredient_names[random_int(0, 9)]);
        case 3:
            return std::string(withering_ingredient_names[random_int(0, 9)]);
        case 4:
            return std::string(healing_ingredient_names[random_int(0, 9)]);
        case 5:
            return std::string(regenerative_ingredient_names[random_int(0, 9)]);
        case 6:
            return std::string(protective_ingredient_names[random_int(0, 9)]);
        case 7:
            return std::string(
                    strengthening_ingredient_names[random_int(0, 9)]);
        case 8:
            return std::string(cleansing_ingredient_names[random_int(0, 9)]);
        case 9:
            return std::string(joker_ingredient_names[random_int(0, 9)]);
        default:
            return std::string(flaming_ingredient_names[random_int(0, 9)]);
        }
    }

    auto get_random_enemy_name(int type) -> std::string
    {
        using namespace constants;

        switch (type) {
        case 0:
            return std::string(flaming_enemy_names[random_int(0, 9)]);
        case 1:
            return std::string(chilling_enemy_names[random_int(0, 9)]);
        case 2:
            return std::string(poisonous_enemy_names[random_int(0, 9)]);
        case 3:
            return std::string(withering_enemy_names[random_int(0, 9)]);
        case 4:
            return std::string(healing_enemy_names[random_int(0, 9)]);
        case 5:
            return std::string(regenerative_enemy_names[random_int(0, 9)]);
        case 6:
            return std::string(protective_enemy_names[random_int(0, 9)]);
        case 7:
            return std::string(strengthening_enemy_names[random_int(0, 9)]);
        case 8:
            return std::string(cleansing_enemy_names[random_int(0, 9)]);
        default:
            return std::string(flaming_enemy_names[random_int(0, 9)]);
        }
    }

    // Main entry point function
    auto main_menu(const std::string& player_name) -> void
    {
        game_state game(player_name);
        game.run();
    }

} // namespace potmaker