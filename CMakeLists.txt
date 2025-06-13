cmake_minimum_required(VERSION 3.31)
project(potion-maker)

set(CMAKE_CXX_STANDARD 20)

add_executable(fuit_farm_2 src/main.cc
        src/ingredient.cc
        src/ingredient.hh
        src/entity.cc
        src/entity.hh
        src/status_effect.cc
        src/status_effect.hh
        src/ingredient_names.hh
        src/util.cc
        src/util.hh
        src/potionmaker_game.cc
        src/potionmaker_game.hh
        src/entity_names.hh
        src/element_type.hh
)
