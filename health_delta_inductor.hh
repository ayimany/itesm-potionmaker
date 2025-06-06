#ifndef DAMAGE_SOURCE_HH
#define DAMAGE_SOURCE_HH
#include <string>

namespace potmaker {

class health_delta_inductor {
public:
  [[nodiscard]] virtual auto inductor_display_string() -> std::string = 0;
};

} // namespace potmaker

#endif // DAMAGE_SOURCE_HH
