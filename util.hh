#ifndef UTIL_HH
#define UTIL_HH
#include <string>

namespace potmaker {

[[nodiscard]] auto random_int(int min, int max) -> int;
[[nodiscard]] auto random_double(double min, double max) -> double;
[[nodiscard]] auto roll_chances(int odds) -> bool;
auto print_action(const std::string &act) -> void;
auto print_divider(const std::string &act) -> void;

} // namespace potmaker

#endif // UTIL_HH
