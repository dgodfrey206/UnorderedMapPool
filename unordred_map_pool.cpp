// UnorderedMap.cpp : Defines the entry point for the console application.
//
#include <iostream>
#include <limits>
#include <stdexcept>

namespace detail {
struct Node {};
template<class T>
class PreallocMemoryPool {
public:
  PreallocMemoryPool() {}
private:
  Node* free_;

};
}

template<typename T>
class PreallocMemoryPool {
public:
  PreallocMemoryPool() : nodes_(0), pos_(0), size_(0) { }
  ~PreallocMemoryPool() {
    clear();
  }

  PreallocMemoryPool(PreallocMemoryPool const& other);
  PreallocMemoryPool& operator=(const PreallocMemoryPool& other);

  struct do_default_construction { };

  template<class U>
  T* direct_construction(U const& arg) { return new T(arg); }
  T* direct_construction(do_default_construction) { return new T(); }

  template<class U>
  T* placement_construction(void* n, U const& arg) { return new(n) T(arg); }
  T* placement_construction(void* n, do_default_construction) { return new(n) T(); }

  template<typename U>
  T* Create(U const& arg) {
    if (pos_ == 0) {
      return direct_construction(arg);
    }

    void* n = nodes_[--pos_];
    return placement_construction(n, arg);
  }

  T* Create() {
    return Create(do_default_construction());
  }

  void clear() {
    delete[] nodes_;
  }

  void Release(T* node) {
#ifdef _DEBUG
    char buff[sizeof(T)];
    memset(buff, 0xCDCDCDCD, sizeof(buff));
    if (memcmp(node, buff, sizeof(T)) == 0)  {
      bool oops = true;
    }
#endif
    if (pos_ < size_) {
      node->~T();
      nodes_[pos_++] = static_cast<void*>(node);
    }
#ifdef _DEBUG
    memset(node, 0xCDCDCDCD, sizeof(T));
#endif
  }

  void Preallocate(std::size_t sz) {
    if (sz > size_) {
      clear();
      nodes_ = new void*[size_ = pos_ = sz]();
      allocate(sz);
    }
  }
private:
  void** nodes_;
  std::size_t pos_;
  std::size_t size_;

  void allocate(std::size_t sz) {
    for (std::size_t i = 0; i < sz; ++i) {
      nodes_[i] = ::operator new(sizeof(T));
    }
  }
};

template<class T>
PreallocMemoryPool<T>::PreallocMemoryPool(PreallocMemoryPool const& other)
  : nodes_(new void*[other.size_]()),
    pos_(other.size_),
    size_(other.size_)
{
  allocate(other.size_);
}

template<class T>
PreallocMemoryPool<T>& PreallocMemoryPool<T>::operator=(const PreallocMemoryPool& other)
{
  void** tmp = new void*[other.size_]();
  for (std::size_t i = 0; i < other.size_; ++i) {
    tmp[i] = ::operator new(sizeof(T));
  }
  delete[] tmp;
  nodes_ = tmp;
  pos_ = other.pos_;
  size_ = other.size_;
  return *this;
}

template<class Key,
         class Value,
         class Hash = std::hash<Key>,
         class KeyEqual = std::equal_to<Key>,
         class Allocator = std::allocator<std::pair<Key const, Value> >
>
class UnorderedMapPool;

namespace bp_core_test_unordered_map_pool_test_hpp { void Verify(UnorderedMapPool<int,int> const&); }

template< class Key, class T, class Hash, class KeyEqual, class Allocator >
bool operator==( const UnorderedMapPool<Key,T,Hash,KeyEqual,Allocator>& lhs,
                 const UnorderedMapPool<Key,T,Hash,KeyEqual,Allocator>& rhs );

template< class Key, class T, class Hash, class KeyEqual, class Allocator >
bool operator!=( const UnorderedMapPool<Key,T,Hash,KeyEqual,Allocator>& lhs,
                 const UnorderedMapPool<Key,T,Hash,KeyEqual,Allocator>& rhs );

template<class Key,
         class Value,
         class Hash,
         class KeyEqual,
         class Allocator
>
class UnorderedMapPool {
  struct node;
public:
  friend void bp_core_test_unordered_map_pool_test_hpp::Verify(UnorderedMapPool<int,int> const&);

  typedef Key                               key_type;
  typedef Value                             mapped_type;
  typedef std::pair<Key const, Value>       value_type;
  typedef std::size_t                       size_type;
  typedef std::ptrdiff_t                    difference_type;
  typedef Hash                              hasher;
  typedef KeyEqual                          key_equal;
  typedef Allocator                         allocator_type;
  typedef value_type&                       reference;
  typedef const value_type&                 const_reference;
  typedef typename Allocator::pointer       pointer;
  typedef typename Allocator::const_pointer const_pointer;
  class                                     iterator;
  class                                     const_iterator;
  class                                     local_iterator;
  class                                     const_local_iterator;

  UnorderedMapPool(size_type bucket_count = 50, // random number
                   const Hash& hash       = Hash(),
                   const KeyEqual& equal  = KeyEqual(),
                   Allocator* alloc       = NULL);//

  template< class InputIt >
  UnorderedMapPool(InputIt first, InputIt last,
                   size_type bucket_count = 50, // random number
                   const Hash& hash       = Hash(),
                   const KeyEqual& equal  = KeyEqual(),
                   Allocator* alloc       = Allocator());//

  UnorderedMapPool(const UnorderedMapPool& other);//
  UnorderedMapPool(const UnorderedMapPool& other, Allocator* alloc);//

  UnorderedMapPool& operator=(const UnorderedMapPool& other );//
  ~UnorderedMapPool() { clear(); delete[] buckets_; }//

  allocator_type* get_allocator();//
  Value& at(const Key& key);//
  const Value& at(const Key& key) const;//

  iterator begin();//
  const_iterator begin() const;//
  const_iterator cbegin() const;//

  iterator end();//
  const_iterator end() const;//
  const_iterator cend() const;//

  bool empty() const;//
  size_type size() const;//
  size_type max_size() const;//

  void clear();//
  std::pair<iterator, bool> insert(const value_type& value);//
  iterator insert(const_iterator hint, const value_type& value);//

  float max_load_factor() const;//
  void max_load_factor(float ml);
  float load_factor() const;//

  iterator erase(const_iterator pos);//
  iterator erase(const_iterator first, const_iterator last); //
  size_type erase(const key_type& key);//

  void swap(UnorderedMapPool& other);//
  Value& operator[](const Key& key);//
  size_type count(const Key& key) const;//
  iterator find(const Key& key);//
  const_iterator find(const Key& key) const;//
  std::pair<iterator, iterator> equal_range(const Key& key);//
  std::pair<const_iterator, const_iterator> equal_range(const Key& key) const;//

  void rehash(size_type count);//
  void reserve(size_type count);//
  hasher hash_function() const;//
  key_equal key_eq() const;///

  friend bool operator==<Key, Value, Hash, KeyEqual, Allocator>( const UnorderedMapPool<Key,Value,Hash,KeyEqual,Allocator>& lhs,
                                                                 const UnorderedMapPool<Key,Value,Hash,KeyEqual,Allocator>& rhs );//

  friend bool operator!=<Key, Value, Hash, KeyEqual, Allocator>( const UnorderedMapPool<Key,Value,Hash,KeyEqual,Allocator>& lhs,
                                                                 const UnorderedMapPool<Key,Value,Hash,KeyEqual,Allocator>& rhs );//
  size_type bucket_count() const;//
  size_type bucket_size(size_type n) const;//
public: // Set back to private
  node** buckets_;
  hasher hash_;
  key_equal pred_;
  allocator_type* alloc_;
  PreallocMemoryPool<node> buffer_;
  size_type bucket_count_;
  size_type size_;
  node* begin_node_;
  float mlf_; // max load factor

  size_type bucket_hash(Key const& key) const {
    return hash_function()(key) % bucket_count();
  }
private:
  void init(size_type bucket_count, const Hash& hash, const KeyEqual& equal, Allocator* alloc) {
    buckets_ = new node*[bucket_count]();
    hash_ = hash;
    pred_ = equal;
    alloc_ = alloc;
    bucket_count_ = bucket_count;
    size_ = 0;
    begin_node_ = NULL;
    mlf_ = 0.0f;
  }

  void init(const UnorderedMapPool& other) {
    init(other.bucket_count_, other.hash_, other.pred_, other.alloc_);
    size_ = other.size_;
    begin_node_ = other.begin_node_;
    mlf_ = other.mlf_;
    buffer_ = other.buffer_;
    for (size_type i = 0; i < other.bucket_count(); ++i) {
      buckets_[i] = other.buckets_[i];
    }
  }

  float threshold() const { return bucket_count() * max_load_factor(); }

  void push_front(node*& head, node* newNode, node* node::*next) {
    newNode->*next = head;
    head = newNode;
  }

  void push_front_bucket_node(node*& head, node* newNode)
  { push_front(head, newNode, &node::next_); }
  void push_front_iterator_node(node*& head, node* newNode)
  { push_front(head, newNode, &node::iter_next_); }

  node*& bucket_at(Key const& key) const {
    return buckets_[bucket_hash(key)];
  }

  iterator InternalInsert(node** bucket, size_type pos, node* n) {
    node*& head = bucket[pos];
    push_front_bucket_node(head, n);
    push_front_iterator_node(begin_node_, head);
    return iterator(head);
  }

  iterator find_hint(size_type idx, Key const& key) {
    node* dest = buckets_[idx];
    for (; dest != NULL; dest = dest->next_) {
      if (key_eq()(dest->key(), key)) {
        break;
      }
    }
    return iterator(dest);
  }
  const_iterator find_hint(size_type idx, Key const& key) const {
      return const_cast<UnorderedMapPool&>(*this).find_hint(idx, key);
  }

  node** Detach(node** start, node* node::*next, const key_type& key)
  {
    node** current = start;
    node* previous = NULL;
    for (; *current != NULL && !key_eq()(current[0]->key(), key); current = &(current[0]->*next)) {
      previous = *current;
    }

    if (*current) {
      if (previous) {
        previous->*next = current[0]->*next;
      }
    }
    return current;
  }
};

template<class Key, class Value, class Hash, class KeyEqual, class Allocator>
struct UnorderedMapPool<Key, Value, Hash, KeyEqual, Allocator>::node {
  node(value_type const& data = value_type(), node* next = NULL, node* iter_next = NULL)
    : data_(data),
      next_(next),
      iter_next_(iter_next)
  { }

  Key   const& key  () const { return data_.first;  }
  Value      & value()       { return data_.second; }
  Value const& value() const { return data_.second; }

  value_type data() { return data_; }
  node* next()      { return next_; }

  friend class UnorderedMapPool;
private:
  value_type data_;
  node* next_;
  node* iter_next_;
};

template<class Key, class Value, class Hash, class KeyEqual, class Allocator>
void UnorderedMapPool<Key, Value, Hash, KeyEqual, Allocator>::reserve(size_type sz) {
  buffer_.Preallocate(sz);
}

template<class Key, class Value, class Hash, class KeyEqual, class Allocator>
UnorderedMapPool<Key,Value,Hash,KeyEqual,Allocator>::UnorderedMapPool(size_type bucket_count,
                                                                      const Hash& hash,
                                                                      const KeyEqual& equal,
                                                                      Allocator* alloc)
{ init(bucket_count, hash, equal, alloc); }

template<class Key, class Value, class Hash, class KeyEqual, class Allocator>
UnorderedMapPool<Key,Value,Hash,KeyEqual,Allocator>& UnorderedMapPool<Key,Value,Hash,KeyEqual,Allocator>::operator=(const UnorderedMapPool& other ) { // copy
  rehash(other.bucket_count());

  //delete[] buckets_;
  //buckets_ = new node*[other.buckets_
  //
  //for (const_iterator it = other.begin(); it != other.end(); ++it) {
  //  insert(std::make_pair(3,2));
  //}
  //bucket_count_ = other.bucket_count_;
  //size_ = other.size_;
  //begin_node_ = other.begin_node_;
  //mlf_ = other.mlf_;
  //alloc_ = other.alloc_;
  //pred_ = other.pred_;
  //hash_ = other.hash_;

  return *this;
}

template<class Key, class Value, class Hash, class KeyEqual, class Allocator>
template< class InputIt >
UnorderedMapPool<Key,Value,Hash,KeyEqual,Allocator>::UnorderedMapPool(InputIt first, InputIt last,
                                                                      size_type bucket_count,
                                                                      const Hash& hash,
                                                                      const KeyEqual& equal,
                                                                      Allocator* alloc)
{
  init(bucket_count, hash, equal, alloc);
  for (size_type i = 0, len = std::distance(first, last); i < len; ++i) {
    buckets_[i] = buffer_.Create(*first++);
    ++size_;
  }
}

template<class Key, class Value, class Hash, class KeyEqual, class Allocator>
UnorderedMapPool<Key,Value,Hash,KeyEqual,Allocator>::UnorderedMapPool(const UnorderedMapPool& other)
  : buckets_(new node*[other.bucket_count()]()),
    hash_(other.hash_),
    pred_(other.pred_),
    alloc_(other.alloc_),
    bucket_count_(other.bucket_count_),
    size_(other.size_),
    begin_node_(other.begin_node_),
    mlf_(other.mlf_)
{
  for (const_iterator it = other.begin(); it != other.end(); ++it) {
    insert(*it);
  }
}

template<class Key, class Value, class Hash, class KeyEqual, class Allocator>
UnorderedMapPool<Key,Value,Hash,KeyEqual,Allocator>::UnorderedMapPool(const UnorderedMapPool& other, Allocator* alloc) {
  init(other);
  alloc_ = alloc;
}

template<class Key, class Value, class Hash, class KeyEqual, class Allocator>
Value& UnorderedMapPool<Key, Value, Hash, KeyEqual, Allocator>::operator[](const Key& key) {
  size_type bucket_pos = bucket_hash(key);
  iterator it = find_hint(bucket_pos, key);

  if (it != end()) {
    return it->second;
  }
  
  if (size()+1 > threshold()) {
    rehash(bucket_count() * 2);
    bucket_pos = bucket_hash(key);
  }
  
  // not there -- insert a new one
  node* n = buffer_.Create(std::make_pair(key, Value()));
  InternalInsert(buckets_, bucket_pos, n);

  ++size_;
  return n->value(); // n->data_.second
}

template<class Key, class Value, class Hash, class KeyEqual, class Allocator>
typename UnorderedMapPool<Key, Value, Hash, KeyEqual, Allocator>::size_type UnorderedMapPool<Key, Value, Hash, KeyEqual, Allocator>::bucket_size(size_type n) const {
  size_type count = 0;
  for (node* pos = buckets_[n]; pos != NULL; pos = pos->next()) {
    ++count;
  }
  return count;
}

template<class Key, class Value, class Hash, class KeyEqual, class Allocator>
bool UnorderedMapPool<Key, Value, Hash, KeyEqual, Allocator>::empty() const {
  return size_ == 0;
}

template<class Key, class Value, class Hash, class KeyEqual, class Allocator>
float UnorderedMapPool<Key, Value, Hash, KeyEqual, Allocator>::load_factor() const
{ return .75; }

template<class Key, class Value, class Hash, class KeyEqual, class Allocator>
typename UnorderedMapPool<Key, Value, Hash, KeyEqual, Allocator>::hasher UnorderedMapPool<Key, Value, Hash, KeyEqual, Allocator>::hash_function() const
{ return hash_; }

template<class Key, class Value, class Hash, class KeyEqual, class Allocator>
typename UnorderedMapPool<Key, Value, Hash, KeyEqual, Allocator>::size_type UnorderedMapPool<Key, Value, Hash, KeyEqual, Allocator>::size() const
{ return size_; }

template<class Key, class Value, class Hash, class KeyEqual, class Allocator>
typename UnorderedMapPool<Key, Value, Hash, KeyEqual, Allocator>::size_type UnorderedMapPool<Key, Value, Hash, KeyEqual, Allocator>::bucket_count() const
{ return bucket_count_; }

template<class Key, class Value, class Hash, class KeyEqual, class Allocator>
typename UnorderedMapPool<Key, Value, Hash, KeyEqual, Allocator>::size_type UnorderedMapPool<Key, Value, Hash, KeyEqual, Allocator>::max_size() const
{ return std::numeric_limits<size_type>::max(); }

template<class Key, class Value, class Hash, class KeyEqual, class Allocator>
typename UnorderedMapPool<Key, Value, Hash, KeyEqual, Allocator>::key_equal UnorderedMapPool<Key, Value, Hash, KeyEqual, Allocator>::key_eq() const
{ return pred_; }

template<class Key, class Value, class Hash, class KeyEqual, class Allocator>
typename UnorderedMapPool<Key, Value, Hash, KeyEqual, Allocator>::allocator_type* UnorderedMapPool<Key, Value, Hash, KeyEqual, Allocator>::get_allocator()
{ return alloc_; }

template<class Key, class Value, class Hash, class KeyEqual, class Allocator>
void UnorderedMapPool<Key, Value, Hash, KeyEqual, Allocator>::clear() {
  for (size_type i = 0; i < bucket_count(); ++i) {
    for (node* p = buckets_[i]; p != NULL; ) {
      node* p_next = p->next();
      buffer_.Release(p);
      p = p_next;
    }
    buckets_[i] = NULL;
  }
  begin_node_ = NULL;
  size_ = 0;
}

template<class Key, class Value, class Hash, class KeyEqual, class Allocator>
typename UnorderedMapPool<Key, Value, Hash, KeyEqual, Allocator>::size_type UnorderedMapPool<Key, Value, Hash, KeyEqual, Allocator>::count(const Key& key) const {
  node* pos = bucket_at(key);

  if (!pos) {
    return 0;
  }

  size_type c = 0;
  for (; pos; pos = pos->next()) {
    if (key_eq()(pos->key(), key)) {
      ++c;
    }
  }
  return c;
}

template<class Key, class Value, class Hash, class KeyEqual, class Allocator>
void UnorderedMapPool<Key, Value, Hash, KeyEqual, Allocator>::rehash(size_type count) {
  node** tmp = new node*[count]();

  // iterate and rehash
  for (iterator it = begin(), last = end(); it != last; ++it) {
    // insert into tmp
    size_type hash = hash_function()(it->first) % count;
    push_front_bucket_node(tmp[hash], buffer_.Create(*it));
  }
  buckets_ = tmp;
  bucket_count_ = count;
}

// Const-version
template<class Key, class Value, class Hash, class KeyEqual, class Allocator>
class UnorderedMapPool<Key, Value, Hash, KeyEqual, Allocator>::const_iterator : public std::iterator<std::forward_iterator_tag, std::pair<Key const, Value>> {
public:
  explicit const_iterator(node* begin) : node_(begin) { }

  const_iterator& operator++() {
    node_ = node_->iter_next_;
    return *this;
  }

  const_iterator operator++(int) {
    const_iterator copy(*this);
    ++*this;
    return copy;
  }

  const_iterator operator+(size_type c) const {
    return const_iterator(*this) += c;
  }

  const_iterator& operator+=(size_type c) {
    while (c--) {
      ++*this;
    }
    return *this;
  }

  value_type const& operator*() const
  { return node_->data_; }

  value_type const* operator->() const
  { return &**this; }

  friend class UnorderedMapPool;
  friend class iterator;
  friend bool operator==(const const_iterator& lhs, const const_iterator& rhs)
  { return lhs.node_ == rhs.node_; }
  friend bool operator!=(const const_iterator& lhs, const const_iterator& rhs)
  { return !(lhs == rhs); }
private:
  node* node_;
};

// Non-const iterator
template<class Key, class Value, class Hash, class KeyEqual, class Allocator>
class UnorderedMapPool<Key, Value, Hash, KeyEqual, Allocator>::iterator : public std::iterator<std::forward_iterator_tag, std::pair<Key const, Value>> {
public:
  explicit iterator(node* begin) : node_(begin) { }
  iterator() : node_(NULL) { }

  iterator& operator++() {
    node_ = node_->iter_next_;
    return *this;
  }

  iterator operator++(int) {
    iterator copy(*this);
    ++*this;
    return copy;
  }

  iterator operator+(size_type c) const {
    return iterator(*this) += c;
  }

  iterator& operator+=(size_type c) {
    while (c--) {
      ++*this;
    }
    return *this;
  }

  value_type const& operator*() const
  { return node_->data_; }

  value_type& operator*()
  { return node_->data_; }

  value_type const* operator->() const
  { return &**this; }

  value_type* operator->()
  { return &**this; }

  operator const_iterator() { return const_iterator(node_); }

  friend class UnorderedMapPool;
  friend class const_iterator;
  friend bool operator==(const const_iterator& lhs, const const_iterator& rhs);
  friend bool operator!=(const const_iterator& lhs, const const_iterator& rhs);
private:
  node* node_;
};

template<class Key, class Value, class Hash, class KeyEqual, class Allocator>
typename UnorderedMapPool<Key, Value, Hash, KeyEqual, Allocator>::iterator UnorderedMapPool<Key, Value, Hash, KeyEqual, Allocator>::erase(const_iterator pos) {
  // If pos == end() the behavior is undefined
  node* next = pos.node_->iter_next_;
  erase(pos->first);
  return iterator(next);
}

template<class Key, class Value, class Hash, class KeyEqual, class Allocator>
typename UnorderedMapPool<Key, Value, Hash, KeyEqual, Allocator>::iterator UnorderedMapPool<Key, Value, Hash, KeyEqual, Allocator>::erase(const_iterator first, const_iterator last) {
  const_iterator temp = first;
  while (first != last) {
    temp = erase(first++);
  }
  return (first != last) ? iterator(bucket_at(temp->first)) : end();
}

template<class Key, class Value, class Hash, class KeyEqual, class Allocator>
typename UnorderedMapPool<Key, Value, Hash, KeyEqual, Allocator>::size_type UnorderedMapPool<Key, Value, Hash, KeyEqual, Allocator>::erase(const key_type& key) {
  node** result = (Detach(&begin_node_,    &node::iter_next_, key),
                   Detach(&bucket_at(key), &node::next_,      key));

  if (*result) {
    buffer_.Release(*result);
    *result = NULL;
    --size_;
    if (empty()) {
      begin_node_ = NULL;
    }
    return 1;
  }
  return 0;
}

template<class Key, class Value, class Hash, class KeyEqual, class Allocator>
std::pair<typename UnorderedMapPool<Key, Value, Hash, KeyEqual, Allocator>::iterator, bool>
UnorderedMapPool<Key, Value, Hash, KeyEqual, Allocator>::insert(const value_type& value) {
  size_type bucket_pos = bucket_hash(value.first);
  iterator it = find_hint(bucket_pos, value.first);

  if (it != end()) {
    return std::make_pair(it, false);
  }

  it = InternalInsert(buckets_, bucket_pos, buffer_.Create(value));
  ++size_;
  return std::make_pair(it, it != end());
}

template<class Key, class Value, class Hash, class KeyEqual, class Allocator>
typename UnorderedMapPool<Key, Value, Hash, KeyEqual, Allocator>::iterator
UnorderedMapPool<Key, Value, Hash, KeyEqual, Allocator>::insert(const_iterator /* hint */, const value_type& value) {
  return insert(value).first;
}

template<class Key, class Value, class Hash, class KeyEqual, class Allocator>
typename UnorderedMapPool<Key, Value, Hash, KeyEqual, Allocator>::iterator UnorderedMapPool<Key, Value, Hash, KeyEqual, Allocator>::find(Key const& key) {
  return find_hint(bucket_hash(key), key);
}

template<class Key, class Value, class Hash, class KeyEqual, class Allocator>
typename UnorderedMapPool<Key, Value, Hash, KeyEqual, Allocator>::const_iterator UnorderedMapPool<Key, Value, Hash, KeyEqual, Allocator>::find(Key const& key) const
{ return const_cast<UnorderedMapPool&>(*this).find(key); }

template<class Key, class Value, class Hash, class KeyEqual, class Allocator>
typename UnorderedMapPool<Key, Value, Hash, KeyEqual, Allocator>::const_iterator UnorderedMapPool<Key, Value, Hash, KeyEqual, Allocator>::begin() const {
  // Find the first bucket that is non-null. Return an iterator to that node. It's that simple!
  return const_iterator(begin_node_);
}

template<class Key, class Value, class Hash, class KeyEqual, class Allocator>
typename UnorderedMapPool<Key, Value, Hash, KeyEqual, Allocator>::iterator UnorderedMapPool<Key, Value, Hash, KeyEqual, Allocator>::begin() {
  return iterator(begin_node_);
}

template<class Key, class Value, class Hash, class KeyEqual, class Allocator>
typename UnorderedMapPool<Key, Value, Hash, KeyEqual, Allocator>::const_iterator UnorderedMapPool<Key, Value, Hash, KeyEqual, Allocator>::cbegin() const
{ return begin(); }

template<class Key, class Value, class Hash, class KeyEqual, class Allocator>
typename UnorderedMapPool<Key, Value, Hash, KeyEqual, Allocator>::iterator UnorderedMapPool<Key, Value, Hash, KeyEqual, Allocator>::end()
{ return iterator(NULL); }

template<class Key, class Value, class Hash, class KeyEqual, class Allocator>
typename UnorderedMapPool<Key, Value, Hash, KeyEqual, Allocator>::const_iterator UnorderedMapPool<Key, Value, Hash, KeyEqual, Allocator>::end() const
{ return const_iterator(NULL); }

template<class Key, class Value, class Hash, class KeyEqual, class Allocator>
typename UnorderedMapPool<Key, Value, Hash, KeyEqual, Allocator>::const_iterator UnorderedMapPool<Key, Value, Hash, KeyEqual, Allocator>::cend() const
{ return end(); }

template<class Key, class Value, class Hash, class KeyEqual, class Allocator>
std::pair<typename UnorderedMapPool<Key, Value, Hash, KeyEqual, Allocator>::iterator,
          typename UnorderedMapPool<Key, Value, Hash, KeyEqual, Allocator>::iterator>
UnorderedMapPool<Key, Value, Hash, KeyEqual, Allocator>::equal_range(const Key& key) {
  iterator it = find(key);
  return std::make_pair(it, it == end() ? it : it + 1);
}

template<class Key, class Value, class Hash, class KeyEqual, class Allocator>
std::pair<typename UnorderedMapPool<Key, Value, Hash, KeyEqual, Allocator>::const_iterator,
          typename UnorderedMapPool<Key, Value, Hash, KeyEqual, Allocator>::const_iterator>
UnorderedMapPool<Key, Value, Hash, KeyEqual, Allocator>::equal_range(const Key& key) const {
  return const_cast<UnorderedMapPool&>(*this).equal_range(key);
}

template<class Key, class Value, class Hash, class KeyEqual, class Allocator>
void UnorderedMapPool<Key, Value, Hash, KeyEqual, Allocator>::swap(UnorderedMapPool& other) {
  using std::swap;
  for (size_type i = 0; i < std::min(bucket_count(), other.bucket_count()); ++i) {
    swap(buckets_[i], other.buckets_[i]);
  }
  swap(buckets_, other.buckets_);
  swap(size_, other.size_);
  swap(mlf_, other.mlf_);
  swap(buffer_, other.buffer_);
  swap(hash_, other.hash_);
  swap(begin_node_, other.begin_node_);
}

template< class Key, class T, class Hash, class KeyEqual, class Allocator >
bool operator==( const UnorderedMapPool<Key,T,Hash,KeyEqual,Allocator>& lhs,
                 const UnorderedMapPool<Key,T,Hash,KeyEqual,Allocator>& rhs )
{
  if (lhs.size() != rhs.size()) return false;
  return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template< class Key, class T, class Hash, class KeyEqual, class Allocator >
bool operator!=( const UnorderedMapPool<Key,T,Hash,KeyEqual,Allocator>& lhs,
                 const UnorderedMapPool<Key,T,Hash,KeyEqual,Allocator>& rhs )
{
  return !(lhs == rhs);
}

template<class Key, class Value, class Hash, class KeyEqual, class Allocator>
float UnorderedMapPool<Key, Value, Hash, KeyEqual, Allocator>::max_load_factor() const {
  if (size() == 0) return 1.0f;
  return (float)bucket_count() / (float)size();
}

template<class Key, class Value, class Hash, class KeyEqual, class Allocator>
void UnorderedMapPool<Key, Value, Hash, KeyEqual, Allocator>::max_load_factor(float ml) {
  mlf_ = ml;
}

template<class Key, class Value, class Hash, class KeyEqual, class Allocator>
Value& UnorderedMapPool<Key, Value, Hash, KeyEqual, Allocator>::at(const Key& key) {
  size_type bucket_pos = bucket_hash(key);
  iterator it = find_hint(bucket_pos, key);
  if (it == end()) {
    throw std::invalid_argument("Could not find key");
  }
  return it->second;
}

template<class Key, class Value, class Hash, class KeyEqual, class Allocator>
Value const& UnorderedMapPool<Key, Value, Hash, KeyEqual, Allocator>::at(const Key& key) const {
  return const_cast<UnorderedMapPool&>(*this).at(key);
}

int main()
{
    
}
