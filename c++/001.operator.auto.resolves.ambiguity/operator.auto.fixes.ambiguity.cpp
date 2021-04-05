// In this video, I'm going to show a special use case of `operator auto()` in c++.
//
// The `operator auto` is special and useful in this case to fix an ambiguity error,
// or so-called "multiple conversions" error.
//
// Multiple Conversions - is an ambiguity error that the compiler is trying to resolve
// a implicit type conversion on the current compilation context yet finds multiple
// possible path to fit the statement context.
//
// In such case, the compiler can't decide which conversion should be implicitly applied.
//
// In order to demonstrate this use case, I'm going do introduce a design for the
// template Feature class:

#include "ColorFeature.h"
#include <iostream>

void PrintColor(ColorFeature feature)
{
  const char *name = "";

  // It needs an explicit conversion to integer, but we expect it not like `unsigned(feature)`!!!
  switch (feature) {
  case Colors::Red:   name = "red";   break;
  case Colors::Green: name = "green"; break;
  case Colors::Blue:  name = "blue";  break;
  case Colors::Black: name = "black"; break;
  default:            name = "???"; break;
  }

  std::cout << "color: " << name << std::endl;
}

int main(int argc, char**argv)
{
  // We're expecting the ColorFeature to be used like this:
  ColorFeature c0; // default construction
  ColorFeature c1 { Colors::Red };
  ColorFeature c2 = { Colors::Green };

  // It supports comparation:
  if (c0 == Colors::Black) {
    std::cout << "OKAY" << std::endl;
  }

  // It supports boolean testing:
  if (c0) { // need a conversion to boolean (aka. boolean testable)
    std::cout << "OKAY" << std::endl;
  }

  if (c1) {
    std::cout << "OKAY" << std::endl;
  }

  switch (c0) {
  case Colors::Black:
    std::cout << "OKAY" << std::endl;
    break;
  default:
    std::cout << "ERROR" << std::endl;
    break;
  }

  PrintColor(c0);
  PrintColor(c1);
  PrintColor(c2);

  /** CONCLUSION:

      We need the `operator auto` or `operator decltype(auto)` to be a "Context Type Conversion"!!

      By "Context Type Conversion", it means "convert to the type where the context looks for". For example:

        switch (feature) {
        case Colors::Red:   ...;  // NOTE: Colors::XXX here are types of Feature<xxx>, rather than integer!!!
        case Colors::Green: ...;
        case Colors::Blue:  ...;
        }

      The `feature` should be able to implicitly convert integer-like value.

      And in this example:

        if (feature) {
          ...;
        }

      The `feature` should be able to implicitly convert to boolean testable value.

   **/

  return EXIT_SUCCESS;
}
