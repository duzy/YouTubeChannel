// -*- c++ -*-
#pragma once
#include <type_traits>

// It's the declaration of the Feature template class:
template <class T> requires std::is_enum_v<T> || std::is_integral_v<T>
class Feature
{
public: // everything public
  using enum_type = T;
  using value_type = std::underlying_type_t<enum_type>; // FIXME: only is_enum_v<T>

  value_type value; // stores the underlying value instead of enum

  // Initialize from zero value
  constexpr Feature() : value(0) {}

  // Initialize from the enum value
  constexpr Feature(enum_type e) : value(static_cast<value_type>(e)) {}

  // Constructor from value_type (integer) fixes the error:
  //  "statement requires expression of integer type ..." ???
  constexpr Feature(value_type v) : value(v) {}

  // The type conversion operators to `enum_type` and `bool` causes "Multiple Conversions"
  // error, this is the point in this video!!
#if 0

  // Add type conversion operator for the switch selection
  constexpr operator enum_type() const { return static_cast<enum_type>(value); }
  constexpr operator bool() const { return value != 0; }

#elsif 0

  // We use the `operator auto` to fix this "Multiple Conversions", but it does not support
  // the switch selection statement (aka. `switch (feature) { ... }`). Switch needs integer-like
  // to work (FIXME)!!!
  constexpr operator auto() const { return value; }

#else

  // So we have to use the `value_type` conversion instead! As it's an integer, it supports boolean
  // testable too. But it does not convert back to enum type (FIXME)!!!
  constexpr operator value_type() const { return value; }

#endif

  // Comparisions:
  constexpr bool operator==(Feature o) const { return o.value == value; }
  constexpr bool operator==(enum_type e) const { return value == e; }
};
