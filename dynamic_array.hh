#ifndef DYNAMIC_ARRAY_HH
#define DYNAMIC_ARRAY_HH

#include <algorithm>
#include <format>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <stdexcept>
#include <utility>

/*
 * My teacher has expressed being strict about vectors.
 * I do not know what he means.
 * I will not risk it.
 * I am making my own structure.
 */

namespace potmaker {

template <typename t> class dynamic_array {
public:
  explicit dynamic_array()
      : elements(nullptr), allocated_element_count_(0),
        stored_element_count_(0) {}

  explicit dynamic_array(const std::size_t initial_size)
      : elements(new t[initial_size]), allocated_element_count_(initial_size),
        stored_element_count_(0) {}

  dynamic_array(std::initializer_list<t> list) : dynamic_array(list.size()) {
    std::copy(list.begin(), list.end(), elements);
    stored_element_count_ = list.size();
  }

  dynamic_array(const dynamic_array &other)
      : dynamic_array(other.stored_element_count_) {
    std::copy(other.elements, other.elements + other.stored_element_count_,
              elements);
    stored_element_count_ = other.stored_element_count_;
  }

  dynamic_array(dynamic_array &&other) noexcept
      : elements(other.elements),
        allocated_element_count_(other.allocated_element_count_),
        stored_element_count_(other.stored_element_count_) {
    other.elements = nullptr;
    other.allocated_element_count_ = 0;
    other.stored_element_count_ = 0;
  }

  ~dynamic_array() { delete[] elements; }

  auto operator=(const dynamic_array &other) -> dynamic_array & {
    if (this != &other) {
      dynamic_array temp(other);
      swap(*this, temp);
    }
    return *this;
  }

  auto operator=(dynamic_array &&other) noexcept -> dynamic_array & {
    if (this != &other) {
      delete[] elements;
      elements = other.elements;
      allocated_element_count_ = other.allocated_element_count_;
      stored_element_count_ = other.stored_element_count_;

      other.elements = nullptr;
      other.allocated_element_count_ = 0;
      other.stored_element_count_ = 0;
    }
    return *this;
  }

  friend void swap(dynamic_array &first, dynamic_array &second) noexcept {
    using std::swap;
    swap(first.elements, second.elements);
    swap(first.allocated_element_count_, second.allocated_element_count_);
    swap(first.stored_element_count_, second.stored_element_count_);
  }

  auto add(t const &element, const std::size_t resize_delta_on_full = 1)
      -> void {
    if (stored_element_count_ == allocated_element_count_) {
      resize(allocated_element_count_ + resize_delta_on_full);
    }
    elements[stored_element_count_++] = element;
  }

  auto add(t &&element, const std::size_t resize_delta_on_full = 1) -> void {
    if (stored_element_count_ == allocated_element_count_) {
      resize(allocated_element_count_ + resize_delta_on_full);
    }
    elements[stored_element_count_++] = std::move(element);
  }

  auto assign(t const &element, const std::size_t index) -> void {
    if (index >= allocated_element_count_) {
      throw std::out_of_range(
          std::format("Dynamic Array (assign): index {} out of range ({} "
                      "elements allocated)",
                      index, allocated_element_count_));
    }
    elements[index] = element;
    if (index >= stored_element_count_) {
      stored_element_count_ = index + 1;
    }
  }

  auto assign(t &&element, const std::size_t index) -> void {
    if (index >= allocated_element_count_) {
      throw std::out_of_range(
          std::format("Dynamic Array (assign): index {} out of range ({} "
                      "elements allocated)",
                      index, allocated_element_count_));
    }
    elements[index] = std::move(element);
    if (index >= stored_element_count_) {
      stored_element_count_ = index + 1;
    }
  }

  [[nodiscard]] auto get(std::size_t index) const -> t & {
    if (index >= stored_element_count_) {
      throw std::out_of_range(std::format(
          "Dynamic Array (get): index {} out of range ({} elements stored)",
          index, stored_element_count_));
    }
    return elements[index];
  }

  auto remove(std::size_t index) -> void {
    if (index >= stored_element_count_) {
      throw std::out_of_range(
          std::format("Dynamic Array (remove): index {} out of range ({} "
                      "elements stored)",
                      index, stored_element_count_));
    }

    for (std::size_t i = index; i < stored_element_count_ - 1; ++i) {
      elements[i] = std::move(elements[i + 1]);
    }

    --stored_element_count_;
  }

  auto remove_value(const t &value) -> bool {
    for (std::size_t i = 0; i < stored_element_count_; ++i) {
      if (elements[i] == value) {
        remove(i);
        return true;
      }
    }
    return false;
  }

  auto remove_bulk_indices(const dynamic_array<std::size_t> &indices_to_remove)
      -> void {
    if (indices_to_remove.size() == 0) {
      return;
    }

    dynamic_array<std::size_t> sorted_indices = indices_to_remove;
    std::ranges::sort(sorted_indices);
    const auto last = std::ranges::unique(sorted_indices).begin();
    sorted_indices.set_stored_element_count(last - sorted_indices.begin());

    for (unsigned long &sorted_index : sorted_indices) {
      if (sorted_index >= stored_element_count_) {
        throw std::out_of_range(
            std::format("Dynamic Array (remove_bulk_indices): index {} out of "
                        "range ({} elements stored)",
                        sorted_index, stored_element_count_));
      }
    }

    for (std::size_t i = sorted_indices.size(); i-- > 0;) {
      remove(sorted_indices[i]);
    }
  }

  auto extract(std::size_t index) -> t {
    if (index >= stored_element_count_) {
      throw std::out_of_range(
          std::format("Dynamic Array (extract): index {} out of range ({} "
                      "elements stored)",
                      index, stored_element_count_));
    }

    t element = std::move(elements[index]);
    remove(index);
    return element;
  }

  auto clear(const bool release_memory = false) -> void {
    for (std::size_t i = 0; i < stored_element_count_; ++i) {
      elements[i].~t();
    }
    stored_element_count_ = 0;
    if (release_memory) {
      delete[] elements;
      elements = nullptr;
      allocated_element_count_ = 0;
    }
  }

  [[nodiscard]] auto contains(t element) -> bool {
    return std::find(begin(), end(), element) != end();
  }

  template <typename TN>
  [[nodiscard]] auto map(std::function<TN(t const &)> const &mapper) const
      -> dynamic_array<TN> {
    dynamic_array<TN> result(stored_element_count_);

    for (std::size_t i = 0; i < stored_element_count_; ++i) {
      result.assign(mapper(elements[i]), i);
    }

    return result;
  }

  [[nodiscard]] auto operator[](const std::size_t index) const -> const t & {
    return get(index);
  }

  [[nodiscard]] auto operator[](const std::size_t index) -> t & {
    if (index >= stored_element_count_) {
      throw std::out_of_range(
          std::format("Dynamic Array (operator[]): index {} out of range ({} "
                      "elements stored)",
                      index, stored_element_count_));
    }
    return elements[index];
  }

  [[nodiscard]] auto size() const noexcept -> std::size_t {
    return stored_element_count_;
  }

  [[nodiscard]] auto begin() noexcept -> t * { return elements; }
  [[nodiscard]] auto end() noexcept -> t * {
    return elements + stored_element_count_;
  }
  [[nodiscard]] auto begin() const noexcept -> const t * { return elements; }
  [[nodiscard]] auto end() const noexcept -> const t * {
    return elements + stored_element_count_;
  }
  [[nodiscard]] auto cbegin() const noexcept -> const t * { return elements; }
  [[nodiscard]] auto cend() const noexcept -> const t * {
    return elements + stored_element_count_;
  }

  // Must be exposed
  auto set_stored_element_count(const std::size_t new_size) -> void {
    stored_element_count_ = new_size;
  }

private:
  t *elements;
  std::size_t allocated_element_count_;
  std::size_t stored_element_count_;

  auto resize(const std::size_t new_size) -> void {
    t *new_elements = new t[new_size];

    for (std::size_t i = 0; i < stored_element_count_; ++i) {
      new_elements[i] = std::move(elements[i]);
    }

    delete[] elements;
    elements = new_elements;
    allocated_element_count_ = new_size;
  }
};

template <typename T>
auto print_array(dynamic_array<T> const &array) noexcept -> void {
  int index = 0;
  for (auto const &element : array) {
    std::cout << "[" << index++ << "] " << std::format("{}", element) << "\n";
  }
}

} // namespace potmaker

#endif // DYNAMIC_ARRAY_HH