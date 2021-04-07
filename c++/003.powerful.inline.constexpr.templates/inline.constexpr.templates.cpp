#include <iostream>
#include <type_traits>

template <class T> inline constexpr unsigned size = 0;
template <> inline constexpr unsigned size<bool> = sizeof(bool);
template <> inline constexpr unsigned size<char> = sizeof(char);
template <> inline constexpr unsigned size<int>  = sizeof(int);
template <> inline constexpr unsigned size<long> = sizeof(long);

template <class T>
constexpr void check_size()
{
  if constexpr (size<T> >= 4) {
    std::cout << "size is 4, which is good" << std::endl;
  } else {
    std::cout << "size is not supported" << std::endl;
  }
}

struct foo_char { char _; };
struct foo_int  { int _; };
struct foo_long { long _; };

template <> inline constexpr unsigned size<foo_char> = sizeof(foo_char);
template <> inline constexpr unsigned size<foo_int> = sizeof(foo_int);

/** Meta Programming (defined in std)

    integral_constant
      bool_constant
        true_type
        flase_type

**/
template <class T> struct meta_size   : std::integral_constant<size_t, 0> {}; // { static inline constexpr size_t value = 0; };
template <> struct meta_size<bool>    : std::integral_constant<size_t, 1> {}; //{ static inline constexpr size_t value = sizeof(bool); };
template <> struct meta_size<foo_int> : std::integral_constant<size_t, 4> {}; //{ static inline constexpr size_t value = sizeof(foo_int); };

template <class T> inline constexpr int foo_value = 0;
template <       > inline constexpr int foo_value<std::false_type> = 1;
template <       > inline constexpr int foo_value<std::true_type> =  2;
template <class T, T V> inline constexpr int foo_value<std::integral_constant<T, V>> = 10 + V;

template <class T>
constexpr auto recommand_value_for()
{
  if constexpr (size<T> == 1) { // type is known and size is one byte
    return short(128);
  } else if constexpr (size<T> == 8) { // type is known and size is 8
    return long(1280000);
  } else {
    return false;
  }
}

int main()
{
  check_size<bool>();
  check_size<char>();
  check_size<int>();
  check_size<foo_char>();
  check_size<foo_int>();

  std::cout << "bool: " << size<bool> << std::endl;
  std::cout << "char: " << size<char> << std::endl;
  std::cout << "int: " << size<int> << std::endl;
  std::cout << "long: " << size<long> << std::endl;
  std::cout << "short: " << size<short> << std::endl; // not having a size specialization, so '0'
  std::cout << "foo_long: " << size<foo_long> << std::endl; // '0'

  std::cout << "meta_size<bool>: " << meta_size<bool>::value << std::endl;
  std::cout << "meta_size<int>: " << meta_size<int>::value << std::endl;
  std::cout << "meta_size<foo_int>: " << meta_size<foo_int>::value << std::endl;
  std::cout << "meta_size<char>: " << meta_size<char>::value << std::endl;
  std::cout << "meta_size<long>: " << meta_size<long>::value << std::endl;

  std::cout << "foo_value<int>: " << foo_value<int> << std::endl;
  std::cout << "foo_value<true>: " << foo_value<std::bool_constant<true>> << std::endl;
  std::cout << "foo_value<false>: " << foo_value<std::bool_constant<false>> << std::endl;
  std::cout << "foo_value<integral<1>>: " << foo_value<std::integral_constant<int, 1>> << std::endl;
  std::cout << "foo_value<integral<2>>: " << foo_value<std::integral_constant<int, 2>> << std::endl;
  std::cout << "foo_value<integral<3>>: " << foo_value<std::integral_constant<int, 3>> << std::endl;


  std::cout << "recommand_value_for<true>: " << recommand_value_for<bool>() << std::endl;
  std::cout << "recommand_value_for<char>: " << recommand_value_for<char>() << std::endl;
  std::cout << "recommand_value_for<int>: " << recommand_value_for<int>() << std::endl;
  std::cout << "recommand_value_for<long>: " << recommand_value_for<long>() << std::endl;
  std::cout << "recommand_value_for<foo_int>: " << recommand_value_for<foo_int>() << std::endl;

  std::cout << "recommand_value_for<foo_int>: " << sizeof(recommand_value_for<foo_int>()) << std::endl;
  return EXIT_SUCCESS;
}
