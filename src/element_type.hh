#ifndef ELEMENT_HH
#define ELEMENT_HH

#include <string_view>

enum class element_type : int {
    fire,
    ice,
    nature,
    underworld,
    regenerating,
    healing,
    protective,
    strengthening,
    purifying,
    chaotic,
    boring
};

[[nodiscard]] constexpr auto
element_type_to_str(const element_type type) noexcept -> std::string_view
{
    switch (type) {
    case element_type::fire:
        return "Fire";
    case element_type::ice:
        return "Ice";
    case element_type::nature:
        return "Nature";
    case element_type::underworld:
        return "Underworld";
    case element_type::regenerating:
        return "Regenerating";
    case element_type::healing:
        return "Healing";
    case element_type::protective:
        return "Protective";
    case element_type::strengthening:
        return "Strengthening";
    case element_type::purifying:
        return "Purifying";
    case element_type::boring:
        return "Boring";
    default:;
        return "Unknown";
    }
    return "Unknown";
}

#endif // ELEMENT_HH
