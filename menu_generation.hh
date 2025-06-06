#ifndef MENU_HH
#define MENU_HH
#include "dynamic_array.hh"

#include <bits/ostream.tcc>
#include <format>
#include <iostream>
#include <string>
#include <type_traits>

namespace potmaker {

auto print_option_menu(std::string const &title,
                       dynamic_array<std::string> const &options) -> void;

auto print_credits() -> void;

template <typename t>
[[nodiscard]] auto receive_input(std::string const &prompt,
                                 const bool newline = true) -> t {
  t input;
  std::cout << prompt << ": ";
  std::cin >> input;

  if (newline) {
    std::cout << "\n";
  }
  return input;
}

template <typename t>
  requires std::is_integral_v<t>
[[nodiscard]] auto receive_input_in_range(std::string const &prompt, t min,
                                          t max, const bool newline = true)
    -> t {
  t in = receive_input<t>(prompt, newline);
  while (in < min || in > max) {
    in = receive_input<t>(prompt, newline);
  }

  return in;
}

auto enter_anything_to_continue() -> void;
auto print_special(const std::string &act) -> void;

} // namespace potmaker

#endif // MENU_HH
