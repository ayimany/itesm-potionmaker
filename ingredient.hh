#ifndef INGREDIENT_HH
#define INGREDIENT_HH

#include "element_type.hh"
#include "util.hh"
#include <cstdint>
#include <string>
#include <string_view>

enum class element_type;
namespace potmaker {

    template<element_type element_t> class status_effect;
    class entity;

    /**
     * Can be mixed into a potion to inflict special status effects on entities
     */
    class ingredient : public named {
    public:
        /**
         * Constructs a new ingredient
         * @param name The name of the ingredient
         * @param potency The potency of the ingredient
         */
        explicit ingredient(std::string name, std::int32_t potency);

        /**
         * Determines what happens to an entity when they are affected by a
         * potion containing this ingredient
         * @param e The afflicted entity
         */
        virtual auto on_applied(entity& e) -> void = 0;

        /**
         * @return The numerical potency of this ingredient
         */
        [[nodiscard]] auto potency() const -> std::int32_t;

    protected:
        std::string name_;
        std::int32_t potency_;
    };

    class flaming_ingredient final : public ingredient {
    public:
        explicit flaming_ingredient(std::string name, std::int32_t potency);
        auto on_applied(entity& e) -> void override;
    };

    class chilling_ingredient final : public ingredient {
    public:
        explicit chilling_ingredient(std::string name, std::int32_t potency);
        auto on_applied(entity& e) -> void override;
    };

    class poisonous_ingredient final : public ingredient {
    public:
        explicit poisonous_ingredient(std::string name, std::int32_t potency);
        auto on_applied(entity& e) -> void override;
    };

    class withering_ingredient final : public ingredient {
    public:
        explicit withering_ingredient(std::string name, std::int32_t potency);
        auto on_applied(entity& e) -> void override;
    };

    class healing_ingredient final : public ingredient {
    public:
        explicit healing_ingredient(std::string name, std::int32_t potency);
        auto on_applied(entity& e) -> void override;
    };

    class regenerative_ingredient final : public ingredient {
    public:
        explicit regenerative_ingredient(std::string name,
                                         std::int32_t potency);
        auto on_applied(entity& e) -> void override;
    };

    class protective_ingredient final : public ingredient {
    public:
        explicit protective_ingredient(std::string name, std::int32_t potency);
        auto on_applied(entity& e) -> void override;
    };

    class strengthening_ingredient final : public ingredient {
    public:
        explicit strengthening_ingredient(std::string name,
                                          std::int32_t potency);
        auto on_applied(entity& e) -> void override;
    };

    class cleansing_ingredient final : public ingredient {
    public:
        explicit cleansing_ingredient(std::string name, std::int32_t potency);

        auto on_applied(entity& e) -> void override;
    };

    class joker_ingredient final : public ingredient {
    public:
        explicit joker_ingredient(std::string name, std::int32_t potency);

        auto on_applied(entity& e) -> void override;
    };

    [[nodiscard]] auto to_description(ingredient const& ing) -> std::string;

} // namespace potmaker

#endif // INGREDIENT_HH
