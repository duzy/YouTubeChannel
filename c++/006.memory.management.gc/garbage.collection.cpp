#include <new>
#include <cstddef>
#include <stdexcept>
#include <iostream>

namespace my {
  namespace GarbageCollection {
    struct FeaturePoint
    {
      void* alloc(std::byte*& mem, std::size_t size) const;
      void recycle(std::byte* mem) const;
    };
  }

  inline constexpr GarbageCollection::FeaturePoint gc {};

  template<class T>
  class AutoCollectPointer
  {

  };
}

void* my::GarbageCollection::FeaturePoint::alloc(std::byte*& mem, std::size_t size) const
{
  enum {
    PaddingLen = sizeof(std::size_t) % 8,
    SysMemShift = sizeof(std::size_t) + PaddingLen,
  };
  if (mem == nullptr) {
    const std::size_t MemSize = size * 100 + size % 8;
    const std::size_t AmapLen = MemSize / 8 + SysMemShift;
    const std::size_t MemAlign = AmapLen % 8;
    mem = static_cast<std::byte*>(malloc( MemSize + AmapLen + MemAlign ));
    *reinterpret_cast<std::size_t*>(mem) = MemSize;
  }

  const std::size_t MemSize = *reinterpret_cast<const std::size_t*>(mem);
  const std::size_t AmapLen = MemSize / 8 + SysMemShift;
  const std::size_t MemAlign = AmapLen % 8;
  std::byte* Amap = mem + SysMemShift;
  std::byte* Trunk = Amap + AmapLen + MemAlign;
  for (auto p = Amap; p != Amap + AmapLen; ++p) {
    for (int i = 0; i < 8; ++i) {
      auto bit = static_cast<unsigned>(1) << i;
      if (!(unsigned(*p) & bit)) {
        *p |= static_cast<std::byte>(bit);
        return Trunk + (p - Amap) + i;
      }
    }
  }

  return nullptr;
}

void my::GarbageCollection::FeaturePoint::recycle(std::byte* mem) const
{
  std::cout << "TODO: recycle " << mem << std::endl;
}

namespace {
  // TODO: thread safety, it's better to put it into the feature point
  static std::byte* SysMem = nullptr;
}

void destroy_sys_mem() { ::free(SysMem); }

void* operator new(std::size_t size, my::GarbageCollection::FeaturePoint feature)
{
  if (void* p = feature.alloc(SysMem, size)) {
    return p;
  }
  throw std::runtime_error("out of memory");
}

struct ExampleType
{
  int value;
  int value2, value3, value4;
  int test() { return value & 0x000000FF; }
};

int main(int argc, char**argv)
{
  ExampleType *p1 = new (my::gc) ExampleType { 1, 11, 111, 1111 };
  ExampleType *p2 = new (my::gc) ExampleType { 2, 22, 222, 2222 };
  ExampleType *p3 = new (my::gc) ExampleType { 3, 33, 333, 3333 };
  std::cout << p1 << ": " << p1->test() << std::endl;
  std::cout << p2 << ": " << p2->test() << std::endl;
  std::cout << p3 << ": " << p3->test() << std::endl;

  my::gc.recycle(SysMem);

  destroy_sys_mem();
  return EXIT_SUCCESS;
}
