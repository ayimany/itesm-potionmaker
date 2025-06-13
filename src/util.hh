#ifndef UTIL_HH
#define UTIL_HH
#include <string>

namespace potmaker {

    /**
     * Represents an object with a name
     */
    class named {
    public:
        virtual ~named() = default;
        explicit named(std::string name);

        /**
         * @return The name of the object
         */
        [[nodiscard]] auto name() const -> std::string;

    protected:
        std::string name_;
    };

    /**
     * Generates a random int in the [min, max] range
     * @param min The min value
     * @param max The max value
     * @return A random int in [min, max]
     */
    [[nodiscard]] auto random_int(int min, int max) -> int;

    /**
     * Generates a random double in the [min, max] range
     * @param min The min value
     * @param max The max value
     * @return A random double in [min, max]
     */
    [[nodiscard]] auto random_double(double min, double max) -> double;

    /**
     * Random chance generation. Like a die roll
     * @param odds The denominator of the odds
     * @return If a 1/odds chance lands
     */
    [[nodiscard]] auto roll_chances(int odds) -> bool;

    /**
     * Prints text in action form
     * @param act THe text to print
     */
    auto print_action(const std::string& act) -> void;

    /**
     * Prints text within a divider
     * @param act The text to print
     */
    auto print_divider(const std::string& act) -> void;

    /**
     * Prints text within a special divider
     * @param act The text to print
     */
    auto print_special(const std::string& act) -> void;

} // namespace potmaker

#endif // UTIL_HH
