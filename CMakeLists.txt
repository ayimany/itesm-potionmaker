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
        ingredient_names.hh
        util.cc
        util.hh
        potionmaker_game.cc
        potionmaker_game.hh
        entity_names.hh
        element_type.hh
)
