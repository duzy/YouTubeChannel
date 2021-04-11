#include <memory>
#include <iostream>
#include <stdexcept>

class Example
{
  std::unique_ptr<int> __i1;
  std::shared_ptr<int> __i2; // shared
  std::weak_ptr<int>   __i3; // weak shared (no ownership)

public:
  // rvalue reference (right value reference) -- move semantic
  explicit Example(std::unique_ptr<int>&& i) : __i1(std::forward<std::unique_ptr<int>>(i)) {}
  // explicit Example(std::unique_ptr<int> i) : __i1(i) {} // ERROR!!
  explicit Example(int* i) : __i1(i) {} // !

  int* get_int_value() const { return __i1 ? __i1.get() : nullptr; }
};

class FileHandle // RAII file handle
{
  FILE *__file = nullptr; // requires c++17 or c++20, c++2a, c++2b
public:
  explicit FileHandle(FILE* f) : __file(f) {
    if (__file == nullptr) {
      std::cout << "file is null: " << strerror(errno) << std::endl;
    }
  }
  ~FileHandle()
  {
    if (__file) {
      ::fclose(__file);
      std::cout << "file " << __file << " closed" << std::endl;
    } else {
      std::cout << "file is null (~)" << std::endl;
    }
  }
};

using ExampleAllocator = std::allocator<Example>;
using ExampleAllocatorTraits = std::allocator_traits<ExampleAllocator>;

void using_unique_pointer(const std::unique_ptr<Example>& p)
{
  if (auto ip = p->get_int_value(); ip) {
    std::cout << __FUNCTION__ << ": Example.int: " << *ip << std::endl;
  }
}

void using_unique_pointer(std::unique_ptr<Example>&& p)
{
  if (auto ip = p->get_int_value(); ip) {
    std::cout << __FUNCTION__ << ": Example.int: " << *ip << std::endl;
  }

  // p is deleted here!
  p.reset(); // for moved object
}

void unique_ptr_example()
{
  // RAII: Resource Acquisition Is Initialization
  std::unique_ptr<Example> p1( new Example(new int(1)) ); // RAII (IMPORTANT!!)
  std::unique_ptr<FILE, int(*)(FILE*)> p2( ::fopen("/dev/random", "r"), ::fclose ); // IMPORTANT!!
  FileHandle fh( ::fopen("raii.cpp", "r") );

  using_unique_pointer(p1);

  std::cout << "p1 before moved: " << p1.get() << std::endl;
  using_unique_pointer(std::move(p1)); // p1 is no longer avaliable, don't use it again!!
  // !!!!!!!!! p1 should never be used again!!!
  std::cout << "p1 after moved: " << p1.get() << std::endl;
  // *p1
  // p1->....
}

void using_shared_pointer1(const std::shared_ptr<Example>& p)
{
  if (auto ip = p->get_int_value(); ip) {
    std::cout << __FUNCTION__ << ": Example.int: " << *ip << std::endl;
  }
}

void using_shared_pointer2(std::shared_ptr<Example> p) // shared the ownership
{
  if (auto ip = p->get_int_value(); ip) {
    std::cout << __FUNCTION__ << ": Example.int: " << *ip << std::endl;
  }
}

void using_shared_pointer3(std::weak_ptr<Example> p) // using the Example without the ownership
{
  // At this point, there's no gurantee for 'p' to be valid. So we have to try to lock the ownership
  // for usage!!!
  if (std::shared_ptr<Example> sp = p.lock(); sp) { //!!!! IMPORTANT !!!!!!
    if (int* ip = sp->get_int_value(); ip) {
      std::cout << __FUNCTION__ << ": Example.int: " << *ip << std::endl;
    }
  }
}

void shared_ptr_example()
{
  std::shared_ptr<Example> p1( new Example(new int(1)) ); // RAII, but shared
  using_shared_pointer1(p1);
  using_shared_pointer2(p1);
  using_shared_pointer3(p1);
}

void use_file_handle_for_writing_something(const FileHandle& fh)
{
  // using fh for writing ....
  throw std::runtime_error("unexpected error");

  // But not std::abort and std::terminate, still risks of leaking memory!!!
  // std::terminate(); // Still DANGER!!
  // std::abort();     // Still DANGER!!
}

void raii_is_important()
{
  // FILE*  (Raw File Handle)
  FileHandle fh( ::fopen("/dev/random", "r") ); // RAII

  use_file_handle_for_writing_something(fh); // this could raise an exception, or might simply abort the program

  // using fh to do more jobs....
}

int main()
{
  unique_ptr_example();
  shared_ptr_example();
  try { raii_is_important(); } catch (std::exception& e) {
    std::cout << e.what() << std::endl;
  }
  return EXIT_SUCCESS;
}
