#include "util.hh"
#include <iostream>
#include <random>
#include <string>
#include <utility>

namespace potmaker {

    named::named(std::string name): name_(std::move(name)) {}

    auto named::name() const -> std::string
    {
        return name_;
    }

    // Ideally, there would be a separate handler for these in class form
    // to avoid re-opening the random device.
    auto random_int(const int min, const int max) -> int
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution dis(min, max);

        return dis(gen);
    }

    auto random_double(const double min, const double max) -> double
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution dis(min, max);

        return dis(gen);
    }

    auto roll_chances(const int odds) -> bool
    {
        return random_int(1, odds) == 1;
    }

    auto print_action(const std::string& act) -> void
    {
        std::cout << ">>> " << act << "\n";
    }

    auto print_divider(const std::string& act) -> void
    {
        std::cout << "[=== " << act << " ===]\n\n";
    }

    auto print_special(const std::string& act) -> void
    {
        std::cout << "-> " << act << "\n";
    }

} // namespace potmaker
