#include "potionmaker_game.hh"
#include "entity_names.hh"
#include "ingredient_names.hh"
#include "util.hh"
#include <algorithm>
#include <format>
#include <iostream>
#include <limits>
#include <ranges>
#include <string>
#include <utility>
#include <vector>

namespace potmaker {

    shop_item::shop_item(ingredient* ing, const double cost)
        : item(ing), price(cost)
    {}

    game_state::game_state(std::string player_name)
        : player_(new player(std::move(player_name), 100.0, 15.0, 50.0)),
          current_stage_(1), game_running_(true)
    {
        generate_shop_items();
    }

    // We allocate several objects, so we need to perform cleanup
    game_state::~game_state()
    {
        delete player_;
        cleanup_ingredients();
        cleanup_enemies();
    }

    auto game_state::cleanup_ingredients() -> void
    {
        for (const auto* ing: owned_ingredients_) { delete ing; }
        owned_ingredients_.clear();

        for (const auto& shop_item: shop_items_) { delete shop_item.item; }
        shop_items_.clear();
    }

    auto game_state::cleanup_enemies() -> void
    {
        for (const auto* enemy: owned_enemies_) { delete enemy; }
        owned_enemies_.clear();
    }

    auto game_state::run() -> void
    {
        print_divider("WELCOME!");
        std::cout << "An adventure awaits. Get ready, " << player_->name()
                  << "!\n\n";

        // "main loop" of the engine
        while (game_running_) { main_menu(); }
    }

    auto game_state::main_menu() -> void
    {
        std::cout << "\n=== MAIN MENU ===\n";
        std::cout << "1. Start Game\n";
        std::cout << "2. Credits\n";
        std::cout << "3. Quit\n";
        std::cout << "Choose an option: ";

        const int choice = get_user_choice(1, 3);

        switch (choice) {
        case 1:
            start_game();
            break;
        case 2:
            credits();
            break;
        case 3:
            std::cout << "Thanks for playing!\n";
            game_running_ = false;
            break;
        default:
            throw std::runtime_error("Invalid [impossible @ main_menu]");
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

            const int choice = get_user_choice(1, 4);

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
            default:
                throw std::runtime_error("Invalid [impossible @ start_game]");
            }
        }
    }

    auto game_state::fight_menu() -> void
    {
        print_divider(std::format("STAGE {} BATTLE", current_stage_));

        std::vector<enemy*> enemies = generate_enemies();

        std::cout << "You encounter:\n";
        display_enemies(enemies);

        std::cout << "\n1. Attack with Potion\n";
        std::cout << "2. Basic Attack\n";
        std::cout << "3. Surrender\n";
        std::cout << "Choose an action: ";

        const int choice = get_user_choice(1, 3);

        bool surrendered = false;
        if (choice == 3) {
            std::cout << "You surrender at the will of your foes.\n";
            cleanup_enemies();
            surrendered = true;
        }

        const bool battle_won
                = surrendered ? false : fight_round(enemies, choice);

        if (battle_won) {
            double gold_reward = calculate_gold_reward(enemies);
            player_->add_gold(gold_reward);

            print_action(std::format("Victory! You earned {:.1f} gold!",
                                     gold_reward));
            current_stage_++;

            // Little reward
            if (current_stage_ % 3 == 0) {
                print_special("You feel stronger.");
                player_->modify_health(25);
            }
        }
        else {
            print_action("DEFEAT");
            std::cout << "You reached stage " << current_stage_ << "\n";
            game_running_ = false;
        }

        cleanup_enemies();
    }

    auto game_state::shop_menu() -> void
    {
        bool in_shop = true;

        // Shop loop
        while (in_shop) {
            print_divider("INGREDIENT SHOP");
            std::cout << "Gold: " << player_->gold() << "\n\n";

            display_shop();

            std::cout << "\nEnter item number to buy (0 to return): ";
            const int choice
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
        std::cout << "Potionmaker: A Text RPG\n\n";
        std::cout << "Created by: Diego Pasaye, A01708525\n";
        std::cout << "Special thanks to deepseek for giving enemy name ideas\n";
        std::cout
                << "I remade this project 3 times and refactored it entirely "
                   "once just because I didn't like it. I am really tired. I "
                   "may have gone a bit overboard. "
                   "I scream internally every time I see my friends' projects. "
                   "I could have made this so much simpler. I may need a "
                   "therapist.\n";
        std::cout << "Press Enter to continue...";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    auto game_state::generate_enemies() -> std::vector<enemy*>
    {
        std::vector<enemy*> enemies;

        // Stage Increases -> Enemies Increase
        const int enemy_count = 1 + (current_stage_ - 1) / 2;

        for (int i = 0; i < enemy_count; ++i) {
            enemy* new_enemy = create_random_enemy(current_stage_);
            enemies.push_back(new_enemy);
            owned_enemies_.push_back(new_enemy);
        }

        // Cleanup happens elsewhere.
        // We delegate cleanup responsibility to cleanup methods in this class
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
        std::vector<ingredient*>& inventory = player_->stored_ingredients();
        std::vector<int> selected_indices;

        if (inventory.empty()) {
            std::cout << "You have no ingredients to make a potion!\n";
            return potion;
        }

        std::cout << "\n=== POTION CRAFTING ===\n";
        display_inventory();

        std::cout << "Select ingredients for your potion (0 to finish):\n";

        while (true) {
            std::cout << "Add ingredient (0 to finish): ";
            const int choice
                    = get_user_choice(0, static_cast<int>(inventory.size()));

            if (choice == 0) break;

            int actual_index = choice - 1;
            if (std::ranges::find(selected_indices, actual_index)
                != selected_indices.end()) {
                std::cout << "You already selected that ingredient!\n";
                continue;
            }

            ingredient* selected = inventory[actual_index];
            potion.push_back(selected);
            selected_indices.push_back(actual_index);

            std::cout << "Added " << selected->name() << " to potion!\n";

            if (selected_indices.size() == inventory.size()) {
                std::cout << "No more ingredients available!\n";
                break;
            }
        }

        // Remove ingredients
        std::sort(selected_indices.rbegin(), selected_indices.rend());
        for (const int index: selected_indices) {
            inventory.erase(inventory.begin() + index);
        }

        return potion;
    }

    auto game_state::apply_potion(const std::vector<ingredient*>& potion,
                                  const std::vector<enemy*>& enemies) -> void
    {
        if (potion.empty()) {
            std::cout << "You throw an empty bottle! Nothing happens. Consider "
                         "doing a basic attack.\n";
            return;
        }

        // Target choosing
        std::cout << "\nChoose your target:\n";
        display_enemies(enemies);
        std::cout << "Target enemy (1-" << enemies.size() << "): ";

        const int target_choice
                = get_user_choice(1, static_cast<int>(enemies.size()));
        enemy* target = enemies[target_choice - 1];

        print_action(
                std::format("You throw your potion at {}!", target->name()));

        // Every ingredient applies an effect on the target
        for (auto* ingredient: potion) {
            std::cout << "\n" << ingredient->name() << " activates!\n";
            if (!target->is_dead()) { ingredient->on_applied(*target); }
        }
    }

    auto game_state::player_turn(std::vector<enemy*>& enemies, int attack_type)
            -> bool
    {
        std::cout << "\n=== YOUR TURN ===\n";

        // The player uses a potion but mr has-no-ingredients has no ingredients
        if (attack_type == 1) {
            const std::vector<ingredient*>& inventory
                    = player_->stored_ingredients();

            if (inventory.empty()) {
                std::cout << "You have no ingredients to make a potion!\n";
                std::cout << "1. Basic Attack\n";
                std::cout << "2. Do nothing\n";
                std::cout << "Choose an action: ";

                const int fallback_choice = get_user_choice(1, 2);
                if (fallback_choice == 2) {
                    std::cout << "You do nothing.";
                    return false;
                }
                basic_attack(enemies);
            }
            else {
                const std::vector<ingredient*> potion = create_potion();
                apply_potion(potion, enemies);
            }
        }
        else if (attack_type == 2) {
            basic_attack(enemies);
        }

        // Clean up allocated enemies
        cleanup_dead_enemies(enemies);
        return enemies.empty();
    }

    auto game_state::enemy_turn(std::vector<enemy*>& enemies) const -> void
    {
        std::cout << "\n=== ENEMY TURN ===\n";

        for (auto* enemy: enemies) {
            if (!enemy->is_dead()) {
                enemy->tick();
                // Skip turn if dead or frozen
                if (!enemy->is_dead() && !enemy->is_frozen()) {
                    enemy->act(*player_, enemies);
                }
            }
        }

        cleanup_dead_enemies(enemies);
    }

    auto game_state::basic_attack(const std::vector<enemy*>& enemies) -> void
    {
        // Probably should have abstracted this out
        std::cout << "\nChoose your target:\n";
        display_enemies(enemies);
        std::cout << "Target enemy (1-" << enemies.size() << "): ";

        const int target_choice
                = get_user_choice(1, static_cast<int>(enemies.size()));
        enemy* target = enemies[target_choice - 1];

        double damage = player_->damage() * random_double(0.8, 1.2);
        print_action(std::format("You attack {} for {:.1f} damage!",
                                 target->name(), damage));
        target->modify_health(-damage);
    }

    auto game_state::fight_round(std::vector<enemy*>& enemies,
                                 int initial_attack_type) -> bool
    {
        bool player_surrendered = false;
        while (!enemies.empty() && !player_->is_dead() && !player_surrendered) {
            // Simple battle loop: Player -> Enemy -> Tick Effects -> Restart
            const bool player_won = player_turn(enemies, initial_attack_type);
            if (player_won) { return true; }
            if (enemies.empty()) { return true; }

            enemy_turn(enemies);

            player_->tick();

            std::cout << "\n--- STATUS ---\n";
            std::cout << std::format("{}: {:.1f}/{:.1f} HP\n", player_->name(),
                                     player_->health(), player_->max_health());

            if (!enemies.empty()) {
                std::cout << "Enemies remaining:\n";
                display_enemies(enemies);
            }

            // Ask again
            if (!enemies.empty() && !player_->is_dead()) {
                std::cout << "\n=== CHOOSE YOUR ACTION ===\n";
                std::cout << "1. Attack with Potion\n";
                std::cout << "2. Basic Attack\n";
                std::cout << "3. Surrender\n";
                std::cout << "Choose an action: ";

                const int choice = get_user_choice(1, 3);

                if (choice == 3) {
                    std::cout << "You surrender before your enemies.\n";
                    player_surrendered = true;
                    return false;
                }

                initial_attack_type = choice;
            }
        }

        return !player_->is_dead() && !player_surrendered;
    }

    auto game_state::cleanup_dead_enemies(std::vector<enemy*>& enemies) -> void
    {
        std::erase_if(enemies, [](const enemy* e) { return e->is_dead(); });
    }

    auto game_state::generate_shop_items() -> void
    {
        // Clear existing items
        for (const auto& item: shop_items_) { delete item.item; }
        shop_items_.clear();

        // Generate other random items
        const int item_count = random_int(6, 8);

        for (int i = 0; i < item_count; ++i) {
            ingredient* ing = create_random_ingredient(1, current_stage_ + 1);
            double price = 10.0 + (ing->potency() * random_double(8.0, 15.0));
            shop_items_.emplace_back(ing, price);
        }
    }

    auto game_state::display_shop() const -> void
    {
        // Take shop items and show them to the player
        std::cout << "Available Items:\n";
        for (size_t i = 0; i < shop_items_.size(); ++i) {
            const shop_item& item = shop_items_[i];
            std::cout << std::format("{}. {} (Potency: {}) - {:.1f} gold\n",
                                     i + 1, item.item->name(),
                                     item.item->potency(), item.price);
        }
    }

    auto game_state::buy_item(const int index) -> bool
    {
        if (index < 0 || index >= static_cast<int>(shop_items_.size())) {
            return false;
        }

        const shop_item& item = shop_items_[index];
        if (player_->gold() >= item.price) {
            player_->remove_gold(item.price);
            player_->store_ingredient(item.item);

            // Replace bought item
            shop_items_.erase(shop_items_.begin() + index);

            ingredient* new_ingredient
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

    auto game_state::display_inventory() const -> void
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

    auto game_state::get_user_choice(const int min, const int max) -> int
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
            const std::vector<enemy*>& defeated_enemies) const -> double
    {
        double total_reward = 0.0;

        for (const auto* enemy: defeated_enemies) {
            // Base reward
            const double base_reward = 5.0 + (enemy->level() * 3.0);

            // Bonus
            const double stage_bonus = current_stage_ * 2.0;

            total_reward += base_reward + stage_bonus;
        }

        return total_reward * random_double(0.8, 1.2); // Add some variation
    }

    // Factories and Creation
    auto create_random_ingredient(int min_potency, int max_potency)
            -> ingredient*
    {
        const int type = random_int(0, 9); // 10 types of ingredients
        const int potency = random_int(min_potency, max_potency);
        const std::string name = get_random_ingredient_name(type);

        return create_ingredient_by_type(type, name, potency);
    }

    auto create_random_enemy(const int level) -> enemy*
    {
        const int type = random_int(0, 8);
        const std::string name = get_random_enemy_name(type);

        return create_enemy_by_type(type, name, level);
    }

    auto create_ingredient_by_type(const int type, const std::string& name,
                                   const int potency) -> ingredient*
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

    auto create_enemy_by_type(const int type, const std::string& name,
                              const int level) -> enemy*
    {
        // Cleanup is delegated to another method
        // The freeing responsibility is being delegated
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

    auto get_random_ingredient_name(const int type) -> std::string
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

    auto get_random_enemy_name(const int type) -> std::string
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