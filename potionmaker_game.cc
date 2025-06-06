#include "potionmaker_game.hh"

#include "enemy_utils.hh"
#include "ingredient.hh"
#include "ingredient_utils.hh"
#include "menu_generation.hh"
#include "status_effect.hh"
#include "util.hh"

#include <cmath>
#include <sstream>

namespace potmaker {

auto main_menu(const std::string &player_name) -> void {
  player the_player(player_name, 100, 100, -10);
  bool user_wants_to_quit = false;
  int stage = 1;

  for (int i = 0; i < 5; ++i) {
    the_player.add_ingredient(constants::generate_ingredient(stage));
  }

  while (!user_wants_to_quit) {
    dynamic_array<std::string> options{"Move to the next stage",
                                       "Visit the store", "Exit"};

    print_option_menu("What will you do?", options);

    const int choice = receive_input_in_range<int>("Choice", 1, options.size());

    switch (choice) {
    case 1:
      print_divider("Now Entering: The Arena");
      enter_battle(the_player, stage++);
      break;
    case 2:
      print_divider("Now Entering: The Shop");
      enter_shop(the_player, stage);
      break;
    case 3:
      user_wants_to_quit = true;
      break;
    default:
      throw std::invalid_argument(
          "[potionmaker_game::main_menu::initial_choice] Invalid choice "
          "should have been handled correctly");
    }
  }

  print_divider("Now entering: Main Menu");
}

auto enter_battle(player &the_player, const int stage) -> void {
  const int enemy_count = static_cast<int>(std::ceil(0.5 * stage)) +
                          random_int(0, std::floor(stage / 3.0));
  dynamic_array<enemy *> active_enemies(enemy_count);

  for (int i = 0; i < enemy_count; ++i) {
    enemy *en = constants::generate_enemy(stage);
    active_enemies.assign(en, i);
  }

  print_special("Ready to fight? You will be fighting the following enemies:");
  print_enemy_list(active_enemies);
  enter_anything_to_continue();

  bool battle_has_ended = false;

  while (!battle_has_ended) {
    bool turn_spent = false;
    dynamic_array<std::string> options{"Display Enemies", "Inspect Self",
                                       "Create a potion", "Run"};

    print_option_menu("Options", options);

    switch (receive_input_in_range<int>("Choice", 1, options.size())) {
    case 1: {
      bool done_inspecting = false;
      while (!done_inspecting) {
        print_enemy_list(active_enemies);
        print_special("Enter a number to inspect an enemy. Enter 0 to leave");
        const int choice =
            receive_input_in_range<int>("Choice", 0, active_enemies.size());
        if (choice == 0) {
          done_inspecting = true;
        } else {
          print_special(active_enemies[choice - 1]->inspection_string());
          enter_anything_to_continue();
        }
      }
      break;
    };
    case 2: {
      print_special(the_player.inspection_string());
      enter_anything_to_continue();
      break;
    }
    case 3: {
      if (the_player.is_skipping()) {
        print_action("The player is frozen! Turn skipped");
        the_player.on_turn_run();
        break;
      }

      dynamic_array<ingredient *> ingredients = create_potion(the_player);

      if (ingredients.size() == 0) {
        print_special(
            "This potion is empty. Do you want to throw an empty bottle?");
        print_option_menu("Confirm?", {"Yes", "No"});

        if (const int confirmation =
                receive_input_in_range<int>("Choice", 1, 2);
            confirmation == 2) {
          print_action("You put the bottle back in your satchel.");
          break;
        }
      }

      print_option_menu("Use potion on:", {"Enemy", "Yourself"});
      const int target_choice = receive_input_in_range<int>("Choice", 1, 2);

      const double damage = the_player.damage();
      the_player.on_turn_run();

      if (target_choice == 1) {
        enemy *target = choose_enemy(active_enemies);
        target->modify_health(&the_player, damage);

        for (ingredient *ingredient : ingredients) {
          ingredient->on_applied(target);
          delete ingredient;
        }
      } else {
        the_player.modify_health(&the_player, damage);

        for (ingredient *ingredient : ingredients) {
          ingredient->on_applied(&the_player);
          delete ingredient;
        }
      }

      ingredients.clear();
      turn_spent = true;
      break;
    }
    case 4: {
      print_action("You try to run...");
      if (roll_chances(3)) {
        print_action("Success! You have run away.");
        return;
      }
      print_action("Oh no! You did not manage to run away");
      turn_spent = true;
      break;
    }
    default: {
      throw std::invalid_argument(":(");
      break;
    }
    }

    if (!turn_spent)
      continue;

    // The player has a chance to attack and have effects applied before death.
    if (the_player.is_dead()) {
      print_special(
          std::format("{} has died! Goodbye, friend.", the_player.name()));
      std::cout << the_player.name() << " made it to stage " << stage
                << " with " << the_player.cash() << "gold\n";
      return;
    }

    // Here, we double pass the removal protocol after applying effects
    // A remove-if method would go well in these cases
    dynamic_array<std::size_t> indices(0);

    for (std::size_t i = 0; i < active_enemies.size(); ++i) {
      if (const enemy *en = active_enemies[i]; en->is_dead()) {
        indices.add(i);
        print_special(std::format("{} has died!", en->name()));
        delete en;
      }
    }
    active_enemies.remove_bulk_indices(indices);
    indices.clear();

    // Update enemies
    for (enemy *en : active_enemies) {
      en->on_turn_run();
    }

    // Second run after effects
    for (std::size_t i = 0; i < active_enemies.size(); ++i) {
      if (const enemy *en = active_enemies[i]; en->is_dead()) {
        indices.add(i);
        print_special(std::format("{} has died!", en->name()));
        delete en;
      }
    }
    active_enemies.remove_bulk_indices(indices);

    // Enemies now get to act
    for (enemy *en : active_enemies) {
      en->act(the_player, active_enemies);
    }

    if (active_enemies.size() == 0) {
      battle_has_ended = true;
    }
  }

  if (active_enemies.size() == 0) {
    double cash_reward = random_double(stage * 100.0, stage * 500.0);
    print_special("You have won this battle!");
    print_special(std::format("You have won {} gold!", cash_reward));
    the_player.add_cash(cash_reward);
  }
}

auto enter_shop(player &p, int const stage) -> void {
  dynamic_array<ingredient *> ingredients_(10);
  print_special("Welcome to the shop!");
  for (int i = 0; i < 10; ++i) {
    ingredients_.add(constants::generate_ingredient(stage));
  }
  bool done_shopping = false;
  while (!done_shopping) {
    print_divider("Shop Inventory");
    print_special(std::format("Your gold: {}", p.cash()));
    dynamic_array<std::string> options;
    for (std::size_t i = 0; i < ingredients_.size(); ++i) {
      auto ing = ingredients_[i];
      options.add(std::format("{}. {} (Potency: {}) - {} gold", i + 1,
                              ing->name(), ing->potency(),
                              ing->potency() * 10));
    }
    options.add("Exit shop");
    print_option_menu("What would you like to buy?", options);
    const int choice = receive_input_in_range<int>("Choice", 1, options.size());
    if (choice == options.size()) {
      done_shopping = true;
      print_action("You leave the shop.");
    } else {
      auto selected = ingredients_[choice - 1];
      int price = selected->potency() * 10;
      if (p.cash() >= price) {
        p.subtract_cash(price);
        p.add_ingredient(selected);
        print_special(
            std::format("You bought {} for {} gold!", selected->name(), price));
        ingredients_.remove(choice - 1);
        ingredients_.add(constants::generate_ingredient(stage), choice - 1);
      } else {
        print_action("You don't have enough gold for that!");
      }
    }
    enter_anything_to_continue();
  }

  for (auto ing : ingredients_) {
    delete ing;
  }
}
auto print_enemy_list(const dynamic_array<enemy *> &arr) -> void {
  const dynamic_array<std::string> data =
      arr.map<std::string>([](enemy *const &e) { return e->pretty_string(); });

  print_option_menu("Enemy List", data);
}

auto choose_enemy(dynamic_array<enemy *> const &arr) -> enemy * {
  print_action("Choose an enemy from the list");
  print_enemy_list(arr);
  const int index = receive_input_in_range<int>("Your Choice", 1, arr.size());
  return arr[index - 1];
}

auto create_potion(player &p) -> dynamic_array<ingredient *> {
  auto &ingredients = p.get_ingredients();
  auto names = ingredients.map<std::string>(
      [](ingredient *const &e) { return std::string(e->name()); });

  if (ingredients.size() == 0) {
    print_action("You have no ingredients!");
    return dynamic_array<ingredient *>(0);
  }

  print_option_menu("Available Ingredients", names);
  print_action(
      "Which ingredients will you add? List all you want! (0 to mark done)");
  dynamic_array<ingredient *> arr(ingredients.size());
  dynamic_array<std::size_t> indices(ingredients.size());
  bool is_done = false;

  while (!is_done) {
    int const choice =
        receive_input_in_range<int>("Choice", 0, ingredients.size(), false);

    if (choice == 0) {
      is_done = true;
    } else if (!indices.contains(choice - 1)) {
      std::cout << "Added index " << choice - 1 << std::endl;
      indices.add(choice - 1);
    }
  }

  print_array(indices);

  for (const std::size_t i : indices) {
    arr.add(ingredients[i]);
  }

  ingredients.remove_bulk_indices(indices);

  return arr;
}

} // namespace potmaker
