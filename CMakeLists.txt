cmake_minimum_required(VERSION 3.31)
project(potion-maker)

set(CMAKE_CXX_STANDARD 20)

add_executable(fuit_farm_2 main.cc
        ingredient.cc
        ingredient.hh
        entity.cc
        entity.hh
        status_effect.cc
        status_effect.hh
        dynamic_array.hh
        menu_generation.hh
        menu_generation.cc
        ingredient_utils.hh
        util.cc
        util.hh
        ingredient_utils.cc
        potionmaker_game.cc
        potionmaker_game.hh
        enemy_utils.cc
        enemy_utils.hh
        element_type.hh
        health_delta_inductor.hh
)
