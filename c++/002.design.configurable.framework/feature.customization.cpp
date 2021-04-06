template <class T>
inline constexpr bool enable_foobar_feature = false; // compile time customization feature

template <class Base, bool = enable_foobar_feature<Base>>
class Example : public Base
{
public:
  int value() const { return 1; }
};

template <class Base>
class Example<Base, true> : public Base
{
public:
  int value() const { return 2; }
};

////////////// use code //////////

#include <iostream>

class MyBase
{
};

template <>
inline constexpr bool enable_foobar_feature<MyBase> = true;

using MyExample = Example<MyBase>;

int main()
{
  MyExample x;

  std::cout << x.value() << std::endl;

  return EXIT_SUCCESS;
}
