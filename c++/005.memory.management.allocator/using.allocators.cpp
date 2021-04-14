#include <memory>
#include <stdexcept>
#include <iostream>

class Example
{
public:
  Example() {}
};

template<class T>
class MyStorage
{
  enum {size = 10};
  std::byte buffer[size * sizeof(T)];
  bool amap[size];

public:
  MyStorage()
  {
    std::memset(buffer, 0, size * sizeof(T));
    std::memset(amap, 0, size * sizeof(bool));
  }

  T* get()
  {
    T* buf = reinterpret_cast<T*>(this->buffer);
    for (auto p = buf; p != buf + size; ++p)
      if (this->amap[p - buf] == false) return p;
    return nullptr;
  }

  void put(T* v)
  {
    T* buf = reinterpret_cast<T*>(this->buffer);
    for (auto p = buf; p != buf + size; ++p)
      if (this->amap[p - buf] && v == p) {
        this->amap[p - buf] = false;
        return;
      }
    throw std::runtime_error("not my belonging");
  }
};

template<class T>
class MyAllocator
{
  MyStorage<T> *storage;

public:
  using value_type = T;
  using pointer    = T*;
  using reference  = T&;
  using const_reference = const T&;
  using difference_type = std::ptrdiff_t;
  using size_type = std::size_t;

  MyAllocator(MyStorage<T> *s) : storage(s) {}

  pointer allocate(size_type n)
  {
    if (n > 1) throw std::runtime_error("requesting more than one object");
    if (auto p = storage->get(); p) return p;
    throw std::runtime_error("out of memory");
  }

  void deallocate(pointer v, size_type n)
  {
    if (n > 1) throw std::runtime_error("requested more than one object");
    storage->put(v);
  }
};

template<class T, class Allocator = MyAllocator<T>> // std::allocator_for<T>
class MyCustmization
{
  using allocator = Allocator;
  using alloc_traits = std::allocator_traits<allocator>;

  allocator alloc;
  T *instance;

public:
  MyCustmization(MyStorage<T>& storage, T v) : alloc(&storage), instance(alloc.allocate(1))
  {
    new (instance) T(v); //alloc_traits::construct(instance, v);
  }

  ~MyCustmization()
  {
    instance->~T(); // destruction, //alloc_traits::destruct(instance);
    alloc.deallocate(instance, 1);
  }

  const T& get() const { return *instance; }
  T& get() { return *instance; }
};

void unusual_allocator_usage() // DANGER!!!
{
  std::allocator<Example> a;
  std::unique_ptr<Example> p2( a.allocate(1)/*, allocator_deleter(a)*/ ); // DANGER!!
  new (p2.get()) Example; // consturction
}

int main()
{
  MyStorage<int> storage;
  std::unique_ptr<MyCustmization<int>> p2( new MyCustmization(storage, 1) );
  std::cout << p2->get() << std::endl;
  return EXIT_SUCCESS;
}
