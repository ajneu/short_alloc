#include "short_alloc.h"
#include <vector>
#include <new>
#include <iostream>
#include <unordered_map>
#include <map>
#include <typeindex>
#include <cassert>

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

template <typename T, size_t N>
class MyVec : arena<N*sizeof(T)>, std::vector<T, short_alloc<int, N*sizeof(T)>> {
   using arena_t  = arena<N*sizeof(T)>;
   using alloc_t  = short_alloc<T, N*sizeof(T)>;
   using vector_t = std::vector<T, alloc_t>;
public:
   MyVec(std::initializer_list<T> list)
      : arena_t{}, vector_t{list, alloc_t(static_cast<arena_t &>(*this))}
   {
      assert(N == list.size());
   }

   // Element Access
   using vector_t::at;
   using vector_t::operator[];
   using vector_t::front;
   using vector_t::back;
   using vector_t::data;

   // Iterators
   using vector_t::begin;
   using vector_t::cbegin;
   using vector_t::end;
   using vector_t::cend;
   using vector_t::rbegin;
   using vector_t::crbegin;
   using vector_t::rend;
   using vector_t::crend;

   // Capacity
   using vector_t::empty;
   using vector_t::size;
   //using vector_t::max_size;
   //using vector_t::reserve;
   //using vector_t::capacity;
   //using vector_t::shrink_to_fit;

   // Modifiers
   // (not exposed!)

   template<class U, size_t M >
      friend
      bool operator==( const MyVec<U,M> &lhs,
                       const MyVec<U,M> &rhs );
};

template<class T, size_t N>
bool operator==( const MyVec<T,N> &lhs,
                 const MyVec<T,N> &rhs )
{
   using vec_t = const typename MyVec<T,N>::vector_t;
   return static_cast<vec_t &>(lhs) == static_cast<vec_t &>(rhs);
}

int main()
{
   {
      // fixed-size std::vector on the stack (not heap!)
      MyVec<int, 3> vec{1, 2, 3};
      vec[0] = 11;
      for (const auto &v : vec) {
         std::cout << v << std::endl;
      }

      MyVec<int, 3> vec2{11, 2, 3};
      assert(vec == vec2);
   }
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
