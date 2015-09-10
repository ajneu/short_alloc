#include "short_alloc.h"
#include <vector>
#include <new>
#include <iostream>
#include <unordered_map>
#include <map>
#include <typeindex>

std::size_t memory = 0;
std::size_t alloc = 0;

void* operator new(std::size_t s) throw(std::bad_alloc)
{
  memory += s;
  ++alloc;
  return malloc(s);
}

void  operator delete(void* p) throw()
{
  --alloc;
  free(p);
}

void memuse()
{
  std::cout << "memory = " << memory << '\n';
  std::cout << "alloc = " << alloc << '\n';
}

int main()
{
  {  
    const unsigned N = 100;

    using mymap_t       = std::map<std::type_index, int>;
    typedef short_alloc<mymap_t::value_type, N> Alloc;
    arena<N> a;
    
    using mymap_alloc_t = std::map<std::type_index, int, std::less<std::type_index>, Alloc>;
    
    std::pair<std::type_index, int> p = std::make_pair(std::type_index(typeid(int)), 1);
    const mymap_alloc_t m7({p, {std::type_index(typeid(char)), 2}}, std::less<std::type_index>(), Alloc(a));
    memuse();
  }
  {  
    const unsigned N = 150;

    using mymap_t       = std::unordered_map<std::type_index, int>;
    typedef short_alloc<mymap_t::value_type, N> Alloc;
    arena<N> a;
    
    using mymap_alloc_t = std::unordered_map<std::type_index, int, std::hash<std::type_index>, std::equal_to<std::type_index>, Alloc>;
    
    std::pair<std::type_index, int> p = std::make_pair(std::type_index(typeid(int)), 1);
    const mymap_alloc_t m7({p, {std::type_index(typeid(char)), 2}}, 10, std::hash<std::type_index>(), std::equal_to<std::type_index>(), Alloc(a));
    memuse();
  }

}
