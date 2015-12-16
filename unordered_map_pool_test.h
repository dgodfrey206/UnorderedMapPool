#ifndef BP_CORE_TEST_UNORDERED_MAP_POOL_TEST_HPP
#define BP_CORE_TEST_UNORDERED_MAP_POOL_TEST_HPP "$Id: 986874fb822e9976c43f95a3a0ffe5c3248e9e27 $"

#include "bp/core/unordered_map_pool.hpp"
#include "unordered_map_pool_2.hpp"
#include <map>
#include "bp/sys/machine_timer.hpp"
#include <iostream>
#include <ctime>
#include "bp/sys/processor_timer.hpp"
#include "../unordered_map.hpp"

typedef UnorderedMapPool<int,int> Pool;

template<typename Key, typename Value>
std::ostream& operator<<(std::ostream& os, UnorderedMapPool<Key, Value> const& map) {
  for (typename UnorderedMapPool<Key, Value>::const_iterator it = map.begin(); it != map.end(); it++) {
    os << it->first << " " << it->second << '\n';
  }
  os << '\n';
  return os;
}

template<typename Key, typename Value>
std::ostream& operator<<(std::ostream& os, UnorderedMapPool2<Key, Value> const& map) {
  for (typename UnorderedMapPool2<Key, Value>::const_iterator it = map.begin(); it != map.end(); it++) {
    os << it->first << " " << it->second << '\n';
  }
  os << '\n';
  return os;
}

namespace bp_core_test_unordered_map_pool_test_hpp {
using namespace bp::core;
/*
TEST(UnorderedMapPool, TimedInsertion)
{
  std::cout << "Timed Insertion: \n";
  //bp::sys::ProcessorTimer timer;
  //timer.Seconds();
  UnorderedMapPool<int,int> myMap;
  UnorderedMap<int,int> stdMap;
  myMap.reserve(9000);
  bp::sys::MachineTimer mtimer;
  for (int i = 0; i < 9000; ++i) {
    myMap[i] = i;
  }
  double x = mtimer.Seconds();
  std::cout << "My map: " << x << '\n';
  stdMap.reserve(9000);
  mtimer.Begin();
  for (int i = 0; i < 9000; ++i) {
    stdMap[i] = i;
  }
  double y = mtimer.Seconds();
  std::cout << "STD map: " << y << '\n';
  std::cout << "Fastest is: " << (std::min(x,y)==x?"My map\n":"STD map\n") <<'\n';
}
TEST(UnorderedMapPool, TimedIteration) {
  std::cout << "Timed Iteration: \n";
  typedef UnorderedMapPool<int,int> Pool1;
  typedef UnorderedMapPool2<int,int> Pool2;
  Pool1 p1;
  Pool2 p2;
  for (int i = 0; i < 1000; ++i) {
    p1[i]=i;
    p2[i]=i;
  }
  bp::sys::MachineTimer mtimer;
  for (Pool1::iterator it = p1.begin(); it != p1.end(); ++it) {
  }
  double x = mtimer.Seconds();
  std::cout << "Map with begin_node_: " << x << '\n';
  mtimer.Begin();
  for (Pool2::iterator it = p2.begin(); it != p2.end(); ++it) {
  }
  double y = mtimer.Seconds();
  std::cout << "Map with bucket iteration: " << y << '\n';
  std::cout << "Fastest is: " << (std::min(x,y)==x?"begin_node_\n":"bucket iter\n") <<'\n';
}
TEST(UnorderedMapPool, Something) {
  Pool test;
  test.reserve(5);
  test[3] = 2;
  Pool::iterator it = test.begin();
  it->second = 4;
}
TEST(UnorderedMapPool, SingleErase)
{
  Pool test;
  ASSERT_EQ(test.begin(), test.end());
  test[5] = 3;
  ASSERT_NE(test.begin(), test.end());
  test.erase(5);
  ASSERT_EQ(test.begin(), test.end());
  test.erase(0xFFF);
  ASSERT_EQ(test.empty(), true);
}
TEST(UnorderedMapPool, DuplicateHandling) {
  Pool test;
  const size_t kInsertionCount = 58;
  for (int i = 0; i < kInsertionCount; ++i) {
    test[i] = i*2;
  }
  ASSERT_EQ(test.size(), kInsertionCount);
  for (int i = 0; i < kInsertionCount; ++i) {
    test[i] = i;
  }
  ASSERT_EQ(test.size(), kInsertionCount);
  test.clear();
}
TEST(UnorderedMapPool, OverwriteValue) {
  Pool test;
  const size_t kInsertionCount = 1000;
  test[1] = 10;
  ASSERT_EQ(10, test[1]);
  test[1] = 20;
  ASSERT_EQ(20, test[1]);
  test[0] = 10;
  ASSERT_EQ(10, test[0]);
  ASSERT_EQ(20, test[1]);
}
TEST(UnorderedMapPool, Find) {
  Pool test;
  test[5] = 3;
  Pool::iterator it = test.find(5);
  ASSERT_EQ(it->second, 3);
  Pool::iterator it2 = test.find(13);
  ASSERT_EQ(it2, test.end());
}
TEST(UnorderedMapPool, Erase)
{
  Pool test;
  test.reserve(500);
  test[2] = 43;
  test[6] = 22;
  Pool::iterator it = test.find(2);
  // it = test.erase(it); // This will crash if it==end()
  ASSERT_NE(it, test.end());
  ASSERT_EQ(it->first, 2);
  it = test.find(2);
  ASSERT_NE(it, test.end());
  test.erase(6);
  test.erase(2);
  ASSERT_EQ(test.empty(), true);
  test.insert(std::make_pair(10, 680));
  ASSERT_EQ(test.empty(), false);
  test.erase(10);
  ASSERT_EQ(test.empty(), true);
  ASSERT_EQ(test.begin(),test.end());
  test.erase(test.begin(), test.end());
  test[2] = 56;
  test[2] = 88;
  ASSERT_EQ(test.size(), 1);
  test.erase(2);
  ASSERT_EQ(test.empty(), true);
  test[3] = 53;
  test[29] = 56;
  ASSERT_EQ(1, test.erase(29));
  ASSERT_EQ(1, test.erase(3));
  ASSERT_EQ(test.size(), 0);
  ASSERT_EQ(test.begin(), test.end());
  std::cout << "Hello\n" << test;
}
TEST(UnorderedMapPool, Insert)
{
  typedef UnorderedMapPool<int,char> Map;
  typedef Map::iterator iterator;
  typedef std::pair<iterator, iterator> Range;
  Map map;
  map.insert(std::make_pair(1,'a'));
  map.insert(std::make_pair(1,'b'));
  map.insert(std::make_pair(1,'d'));
  map.insert(std::make_pair(2,'b'));
  Range range = map.equal_range(1);
  for (iterator it = range.first; it != range.second; ++it) {
      std::cout << it->first << ' ' << it->second << '\n';
  }
}
TEST(UnorderedMapPool, Swap)
{
  Pool test1, test2;
  test1.insert(std::make_pair(2, 20));
  test1.insert(std::make_pair(3, 30));
  test1[4] = 40;
  std::cout << "test1 = \n";
  for (Pool::iterator it = test1.begin(); it != test1.end(); ++it) {
    std::cout << it->first << " " << it->second << '\n';
  }
  test2.insert(std::make_pair(4, 44));
  test2.insert(std::make_pair(5, 55));
  test2[6] = 66;
  test1.swap(test2);
  std::cout << "test1 = \n";
  for (Pool::iterator it = test1.begin(); it != test1.end(); it++) {
    std::cout << it->first << " " << it->second << '\n';
  }
}
TEST(UnorderedMapPool, Copy)
{
  Pool test1;
  test1.insert(std::make_pair(5, 500));
  test1[6] = 600;
  test1.insert(std::make_pair(9, 900));
  std::cout << "test1 = \n";
  for (Pool::iterator it = test1.begin(); it != test1.end(); ++it) {
    std::cout << it->first << " " << it->second << '\n';
  }
  Pool test2(test1);
  std::cout << "test2 = \n";
  std::cout << test2;
  // Copy-assignment
  std::cout << "--------------\n";
  Pool test3(test2);
  Pool test4;
  test4 = test3;
  std::cout << test4 << '\n';
}
void Verify(Pool const& test) {
  std::map<Pool::key_type, Pool::mapped_type> it_list, bucket_list;
  for (Pool::const_iterator it = test.begin(); it != test.end(); ++it) {
    it_list.insert(*it);
  }
  for (std::size_t i = 0; i < test.bucket_count(); ++i) {
    for (Pool::node* p = test.buckets_[i]; p != NULL; p = p->next()) {
      bucket_list.insert(p->data());
    }
  }
  ASSERT_EQ(it_list, bucket_list);
}*/

Pool Init()
{
  Pool test;
  test[10] = 0;
  //for (int i = 0; i < 10; ++i) {
  //  test[i] = i ^ 0x35 + 5;
  //}
  return test;
}
/*
TEST(UnorderedMapPool, Verify) {
  Pool test = Init();
  Verify(test);
  test.rehash(534);
  Verify(test);
  test.clear();
  Verify(test);
  ASSERT_EQ(test.empty(), true);
  test.insert(test.begin(), std::make_pair(3, 544));
  test[1453258] = 234;
  test.swap(test);
  Verify(test);
}*/

TEST(UnorderedMapPool, Rehash) {
  Pool test = Init();
  //test.rehash(100);
  
  for (int i = 0; i < 3; ++i) {
    test[i * 4 >> 2 | 0x434314] = 54 & i << 0x124 + i;
    //test.rehash(i+1);
  }
}

TEST(UnorderedMapPool, Equality) {
  //Pool p1, p2;
  //p1.reserve(500);
  //p1.insert(std::make_pair(5, 25));
  //p1.insert(std::make_pair(7, 27));
  //p2 = p1;
  //ASSERT_EQ(p1, p2);
  //p2[578] = 2;
  //ASSERT_NE(p1, p2);
  //
  //Pool p3(p2);
  //ASSERT_NE(p1, p3);
  //ASSERT_EQ(p2, p3);
  //p2.insert(std::make_pair(35, 22));
  //ASSERT_NE(p2, p3);
  //p3.insert(std::make_pair(35, (*p2.find(35)).second));
  //ASSERT_EQ(p2, p3);
  //ASSERT_EQ(p3.at(35), 22);
  //ASSERT_EQ(p3.bucket_count(), p2.bucket_count());
  //p3.rehash(150);
  //ASSERT_NE(p3.bucket_count(), p2.bucket_count());
}
/*
TEST(UnorderedMapPool, RehashOnInsert) {
  Pool test;
  size_t kMaxSize = test.bucket_count() * test.max_load_factor()+2;
  for (size_t i = 0; i < kMaxSize; ++i) {
    test[50 * i] = i;
  }
  ASSERT_EQ(test.bucket_count(), 100);
}
TEST(UnorderedMapPool, EraseRange) {
  std::vector<Pool::iterator> v;
  Pool test;
  for (int i = 0; i < 10; ++i) {
    test[i] = i;
  }
  std::cout << test;
  int k = 0;
  for (Pool::iterator it = test.begin(); it != test.end(); ++it, ++k) {
    if (k > 3 && k < 7) {
      std::cout<<it->first << "\n";
      v.push_back(it);
    }
  }
  // erase 2, 3, and 4
  test.erase(v.front(), v.back() + 1);
  std::cout << test;
}
TEST(UnorderedMapPool, DeleteOne)
{
  Pool test;
  ASSERT_EQ(test.begin(), test.end());
  test[5] = 3;
  ASSERT_NE(test.begin(), test.end());
  test.erase(5);
  ASSERT_EQ(test.begin(), test.end());
  test[6] = 3;
  test[9] = 1;
  test.erase(9);
}
TEST(UnorderedMapPool, RehashInvalidation) {
  Pool test;
  test[3] = 5;
  std::cout << "Hash before: " << test.bucket_hash(3) <<'\n';
  int* p = &test.find(3)->second;
  test.rehash(187);
  std::cout << "Hash after: " << test.bucket_hash(3) << '\n';
  std::cout << *p;
}*/
} // bp_core_test_unordered_map_pool_test_hpp

#endif  // BP_CORE_TEST_UNORDERED_MAP_POOL_TEST_HPP
