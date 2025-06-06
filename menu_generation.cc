#include "menu_generation.hh"

#include <string>
#include <iostream>
#include <format>
#include <cstdint>

namespace potmaker {
auto print_option_menu(std::string const &title,
                       dynamic_array<std::string> const &options) -> void {
  const dynamic_array<std::size_t> sizes =
      options.map<std::size_t>([](std::string const &s) { return s.length(); });

  std::size_t largest = 0;
  for (const std::size_t size : sizes) {
    if (size > largest) {
      largest = size;
    }
  }

  const std::size_t pads = largest / 2;
  const std::size_t internal_size = 2 * pads + title.length() + 2;
  std::cout << "+" << std::string(pads, '-') << " " << title << " "
            << std::string(pads, '-') << "+" << "\n";

  for (std::size_t i = 0; i < options.size(); ++i) {
    std::string optstr = std::format("| [{}] {}", i + 1, options[i]);
    const std::size_t optlength = optstr.length();
    const std::size_t filler = internal_size - optlength + 1;
    std::cout << optstr << std::string(filler, ' ') << "|\n";
  }
  std::cout << "+" << std::string(internal_size, '-') << "+\n";
}

auto print_credits() -> void {
  std::cout
      << "It is currently 3:18 AM. My family has long since fallen asleep and\n"
         "I am in front of my monitor writing this text. Wonder How I got\n"
         "here? Re-Making a 4 week project in a single day? My first project\n"
         "was a bit too ambitious and I had to re-make it. I didn't like the\n"
         "second one. And now, we're here with Potion Maker. I am not exactly\n"
         "proud of myself but I am proud of the project itself. I really do\n"
         "not want to implement the shop interface; I want to procrastinate\n"
         "it but I know I shouldn't. - Diego Pasaye, A01708525."
      << "\n";

  enter_anything_to_continue();
}

/**
 * This function exists because I was unable to make a perfectly portable
 * version of "Press enter to continue..." without bloating the project with
 * platform specific code and risking an OS or implementation specific error.
 * Sorry.
 */
auto enter_anything_to_continue() -> void {
  auto _ =
      receive_input<std::string>("Enter anything (but a newline) to continue");
  // Then do nothing with it.
}

} // namespace potmaker
