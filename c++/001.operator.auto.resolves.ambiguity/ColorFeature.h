// -*- c++ -*-
#pragma once
#include "Feature.h"

// Imagine we're going to add a Color feature, let's define some colors:
namespace Colors
{
  // Hide Color enum from user namespace.
  namespace internal
  {
    enum class color {
      black, // this is the default value (aka. zero value)
      red,
      green,
      blue,
    };
  }

  // We're going to use this instead of the Color enum directly. Because
  // an enum is very limited, the Feature template class extends enums
  // with advance functions (it's commonly required in many program).
  using FeatureType = Feature<internal::color>;

  // Define color features for user namespace:
  inline constexpr Feature<internal::color>
    Black = internal::color::black,
    Red   = internal::color::red,
    Green = internal::color::green,
    Blue  = internal::color::blue;
}

// Alias in global user namespace:
using ColorFeature = Colors::FeatureType;
