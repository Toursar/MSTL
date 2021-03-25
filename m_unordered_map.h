#ifndef M_UNORDERED_MAP_H
#define M_UNORDERED_MAP_H

#include "m_hashtable.h"

namespace mstl {

// unordered_map模板类
// 第一参数为键的类型，第二参数为值的类型，第三参数为哈希函数类型，缺省时使用mstl::hash<>，第四参数为键比较大小的函数类型，缺省为mstl::equal_to<>
template<typename Key, typename T, typename Hash = mstl::hash<Key>, typename KeyEqual = mstl::equal_to<Key>>
class unordered_map {
private:
    // 以hashtable作为底层容器进行封装
    typedef hashtable<mstl::pair<const Key, T>, Hash, KeyEqual> base_type;
    base_type ht_;
public:
    typedef typename base_type::allocator_type       allocator_type;
    typedef typename base_type::key_type             key_type;
    typedef typename base_type::mapped_type          mapped_type;
    typedef typename base_type::value_type           value_type;
    typedef typename base_type::hasher               hasher;
    typedef typename base_type::key_equal            key_equal;

    typedef typename base_type::size_type            size_type;
    typedef typename base_type::difference_type      difference_type;
    typedef typename base_type::pointer              pointer;
    typedef typename base_type::const_pointer        const_pointer;
    typedef typename base_type::reference            reference;
    typedef typename base_type::const_reference      const_reference;

    typedef typename base_type::iterator             iterator;
    typedef typename base_type::const_iterator       const_iterator;
    typedef typename base_type::local_iterator       local_iterator;
    typedef typename base_type::const_local_iterator const_local_iterator;

    allocator_type get_allocator() const {
        return ht_.get_allocator();
    }

public:
    // 构造函数
    unordered_map() : ht_(100, Hash(), KeyEqual()) {}

    explicit unordered_map(size_type bucket_count,
                  const Hash& hash = Hash(),
                  const KeyEqual& equal = KeyEqual()) : ht_(bucket_count, hash, equal) {}

    template<typename InputIter>
    unordered_map(InputIter first, InputIter last,
                  const size_type bucket_count = 100,
                  const Hash& hash = Hash(),
                  const KeyEqual& equal = KeyEqual()) :
                  ht_(mstl::max(bucket_count, static_cast<size_type>(mstl::distance(first, last))), hash, equal) {
        for (; first != last; ++first) {
            ht_.insert_unique_noresize(*first);
        }
    }

    unordered_map(std::initializer_list<value_type> ilist,
                  const size_type bucket_count = 100,
                  const Hash& hash = Hash(),
                  const KeyEqual& equal = KeyEqual()) :
                  ht_(mstl::max(bucket_count, static_cast<size_type>(ilist.size())), hash, equal) {
        for (auto first = ilist.begin(); first != ilist.end(); ++first) {
            ht_.insert_unique_noresize(*first);
        }
    }

    unordered_map(const unordered_map& rhs) : ht_(rhs.ht_) {}

    unordered_map(unordered_map&& rhs) noexcept : ht_(mstl::move(rhs.ht_)) {}

    // 是否自我赋值交给底层容器判断即可
    unordered_map& operator=(const unordered_map& rhs) {
        ht_ = rhs.ht_;
        return *this;
    }

    unordered_map& operator=(unordered_map&& rhs) noexcept {
        ht_ = mstl::move(rhs.ht_);
        return *this;
    }

    unordered_map& operator=(std::initializer_list<value_type> ilist) {
        ht_.clear();
        ht_.reserve(static_cast<size_type>(ilist.size()));
        for (auto first = ilist.begin(); first != ilist.end(); ++first) {
            ht_.insert_unique_noresize(*first);
        }
        return *this;
    }

    ~unordered_map() = default;

    // 迭代器相关操作
    iterator begin() noexcept {
        return ht_.begin();
    }

    const_iterator begin() const noexcept {
        return ht_.begin();
    }

    iterator end() noexcept {
        return ht_.end();
    }

    const_iterator end() const noexcept {
        return ht_.end();
    }

    const_iterator cbegin() const noexcept {
        return ht_.cbegin();
    }

    const_iterator cend() const noexcept {
        return ht_.cend();
    }

    // 容器容量操作
    bool empty() const {
        return ht_.empty();
    }

    size_type size() const {
        return ht_.size();
    }

    size_type max_size() const {
        return ht_.max_size();
    }

    // 修改容器内容
    template<typename ...Args>
    pair<iterator, bool> emplace(Args&& ...args) {
        return ht_.emplace_unique(mstl::forward<Args>(args)...);
    }

    template<typename ...Args>
    iterator emplace_hint(const_iterator hint, Args&& ...args) {
        return ht_.emplace_unique_use_hint(hint, mstl::forward<Args>(args)...);
    }

    pair<iterator, bool> insert(const value_type& value) {
        return ht_.insert_unique(value);
    }

    pair<iterator, bool> insert(value_type&& value) {
        return ht_.emplace_unique(mstl::move(value));
    }

    iterator insert(const_iterator hint, const value_type& value) {
        return ht_.insert_unique_use_hint(hint, value);
    }

    iterator insert(const_iterator hint, value_type&& value) {
        return ht_.emplace_unique_use_hint(hint, mstl::move(value));
    }

    template<typename InputIter>
    void insert(InputIter first, InputIter last) {
        ht_.insert_unique(first, last);
    }

    void erase(iterator iter) {
        ht_.erase(iter);
    }

    void erase(iterator first, iterator last) {
        ht_.earse(first, last);
    }

    size_type erase(const key_type& key) {
        return ht_.erase_unique(key);
    }

    void clear() {
        ht_.clear();
    }

    void swap(unordered_map& rhs) noexcept {
        ht_.swap(rhs.ht_);
    }

    mapped_type& at(const key_type& key) {
        iterator iter = ht_.find(key);
        THROW_OUT_OF_RANGE_IF(iter == nullptr, "unordered_map<Key, T> no such element exists");
        return iter->second;
    }

    const mapped_type& at(const key_type& key) const {
        iterator iter = ht_.find(key);
        THROW_OUT_OF_RANGE_IF(iter == nullptr, "unordered_map<Key, T> no such element exists");
        return iter->second;
    }

    mapped_type& operator[](const key_type& key) {
        iterator iter = ht_.find(key);
        if (iter == nullptr) {
            iter = ht_.emplace_unique(key, T()).first;
        }
        return iter->second;
    }

    mapped_type& operator[](key_type&& key) {
        iterator iter = ht_.find(key);
        if (iter == nullptr) {
            iter = ht_.emplace_unique(mstl::move(key), T()).first;
        }
        return iter->second;
    }

    size_type count(const key_type key) const {
        return ht_.count(key);
    }

    iterator find(const key_type& key) {
        return ht_.find(key);
    }

    const_iterator find(const key_type& key) const {
        return ht_.find(key);
    }

    pair<iterator, iterator> equal_range(const key_type& key) {
        return ht_.equal_range_unique(key);
    }

    pair<iterator, iterator> equal_range(const key_type& key) const {
        return ht_.equal_range_unique(key);
    }

    local_iterator begin(const size_type n) noexcept {
        return ht_.begin(n);
    }

    const_local_iterator begin(const size_type n) const noexcept {
        return ht_.begin(n);
    }

    const_local_iterator cbegin(const size_type n) const noexcept {
        return ht_.cbegin(n);
    }

    local_iterator end(const size_type n) noexcept {
        return ht_.end(n);
    }

    const_local_iterator end(const size_type n) const noexcept {
        return ht_.end(n);
    }

    const_local_iterator cend(const size_type n) const noexcept {
        return ht_.cend(n);
    }

    size_type bucket_count() const noexcept {
        return ht_.bucket_count();
    }

    size_type max_bucket_count() const noexcept {
        return ht_.max_bucket_count();
    }

    size_type bucket_size(const size_type n) const noexcept {
        return ht_.bucket_size(n);
    }

    size_type bucket(const key_type& key) const {
        return ht_.bucket(key);
    }

    float load_factor() const noexcept {
        return ht_.load_factor();
    }

    float max_load_factor() const noexcept {
        return ht_.max_load_factor();
    }

    void max_load_factor(float ml) noexcept {
        ht_.max_load_factor(ml);
    }

    void rehash(const size_type count) {
        ht_.rehash(count);
    }

    void reserve(const size_type count) {
        ht_.reserve(count);
    }

    hasher hash_func() const {
        return ht_.hash_func();
    }

    key_equal key_eq() const {
        return ht_.key_eq();
    }

public:
    friend bool operator==(const unordered_map& lhs, const unordered_map& rhs) {
        return lhs.ht_.equal_to_unique(rhs.ht_);
    }

    friend bool operator!=(const unordered_map& lhs, const unordered_map& rhs) {
        return !lhs.ht_.equal_to_unique(rhs.ht_);
    }
};

template<typename Key, typename T, typename Hash, typename KeyEqual>
bool operator==(const unordered_map<Key, T, Hash, KeyEqual>& lhs,
                const unordered_map<Key, T, Hash, KeyEqual>& rhs) {
    return lhs == rhs;
}

template<typename Key, typename T, typename Hash, typename KeyEqual>
bool operator!=(const unordered_map<Key, T, Hash, KeyEqual>& lhs,
                const unordered_map<Key, T, Hash, KeyEqual>& rhs) {
    return lhs != rhs;
}

template<typename Key, typename T, typename Hash, typename KeyEqual>
void swap(unordered_map<Key, T, Hash, KeyEqual>& lhs,
          unordered_map<Key, T, Hash, KeyEqual>& rhs) {
    lhs.swap(rhs);
}


/****************************************************************************************************************************************/
// unordered_multimap模板类
// 第一参数为键的类型，第二参数为值的类型，第三参数为哈希函数类型，缺省时使用mstl::hash<>，第四参数为键比较大小的函数类型，缺省为mstl::equal_to<>
template<typename Key, typename T, typename Hash = mstl::hash<Key>, typename KeyEqual = mstl::equal_to<Key>>
class unordered_multimap {
private:
    // 以hashtable作为底层容器进行封装
    typedef hashtable<mstl::pair<const Key, T>, Hash, KeyEqual> base_type;
    base_type ht_;
public:
    typedef typename base_type::allocator_type       allocator_type;
    typedef typename base_type::key_type             key_type;
    typedef typename base_type::mapped_type          mapped_type;
    typedef typename base_type::value_type           value_type;
    typedef typename base_type::hasher               hasher;
    typedef typename base_type::key_equal            key_equal;

    typedef typename base_type::size_type            size_type;
    typedef typename base_type::difference_type      difference_type;
    typedef typename base_type::pointer              pointer;
    typedef typename base_type::const_pointer        const_pointer;
    typedef typename base_type::reference            reference;
    typedef typename base_type::const_reference      const_reference;

    typedef typename base_type::iterator             iterator;
    typedef typename base_type::const_iterator       const_iterator;
    typedef typename base_type::local_iterator       local_iterator;
    typedef typename base_type::const_local_iterator const_local_iterator;

    allocator_type get_allocator() const {
        return ht_.get_allocator();
    }

public:
    // 构造函数
    unordered_multimap() : ht_(100, Hash(), KeyEqual()) {}

    explicit unordered_multimap(size_type bucket_count,
                  const Hash& hash = Hash(),
                  const KeyEqual& equal = KeyEqual()) : ht_(bucket_count, hash, equal) {}

    template<typename InputIter>
    unordered_multimap(InputIter first, InputIter last,
                  const size_type bucket_count = 100,
                  const Hash& hash = Hash(),
                  const KeyEqual& equal = KeyEqual()) :
                  ht_(mstl::max(bucket_count, static_cast<size_type>(mstl::distance(first, last))), hash, equal) {
        for (; first != last; ++first) {
            ht_.insert_multi_noresize(*first);
        }
    }

    unordered_multimap(std::initializer_list<value_type> ilist,
                  const size_type bucket_count = 100,
                  const Hash& hash = Hash(),
                  const KeyEqual& equal = KeyEqual()) :
                  ht_(mstl::max(bucket_count, static_cast<size_type>(ilist.size())), hash, equal) {
        for (auto first = ilist.begin(); first != ilist.end(); ++first) {
            ht_.insert_multi_noresize(*first);
        }
    }

    unordered_multimap(const unordered_multimap& rhs) : ht_(rhs.ht_) {}

    unordered_multimap(unordered_multimap&& rhs) noexcept : ht_(mstl::move(rhs.ht_)) {}

    // 是否自我赋值交给底层容器判断即可
    unordered_multimap& operator=(const unordered_multimap& rhs) {
        ht_ = rhs.ht_;
        return *this;
    }

    unordered_multimap& operator=(unordered_multimap&& rhs) noexcept {
        ht_ = mstl::move(rhs.ht_);
        return *this;
    }

    unordered_multimap& operator=(std::initializer_list<value_type> ilist) {
        ht_.clear();
        ht_.reserve(static_cast<size_type>(ilist.size()));
        for (auto first = ilist.begin(); first != ilist.end(); ++first) {
            ht_.insert_multi_noresize(*first);
        }
        return *this;
    }

    ~unordered_multimap() = default;

    // 迭代器相关操作
    iterator begin() noexcept {
        return ht_.begin();
    }

    const_iterator begin() const noexcept {
        return ht_.begin();
    }

    iterator end() noexcept {
        return ht_.end();
    }

    const_iterator end() const noexcept {
        return ht_.end();
    }

    const_iterator cbegin() const noexcept {
        return ht_.cbegin();
    }

    const_iterator cend() const noexcept {
        return ht_.cend();
    }

    // 容器容量操作
    bool empty() const {
        return ht_.empty();
    }

    size_type size() const {
        return ht_.size();
    }

    size_type max_size() const {
        return ht_.max_size();
    }

    // 修改容器内容
    template<typename ...Args>
    iterator emplace(Args&& ...args) {
        return ht_.emplace_multi(mstl::forward<Args>(args)...);
    }

    template<typename ...Args>
    iterator emplace_hint(const_iterator hint, Args&& ...args) {
        return ht_.emplace_multi_use_hint(hint, mstl::forward<Args>(args)...);
    }

    iterator insert(const value_type& value) {
        return ht_.insert_multi(value);
    }

    iterator insert(value_type&& value) {
        return ht_.emplace_multi(mstl::move(value));
    }

    iterator insert(const_iterator hint, const value_type& value) {
        return ht_.insert_multi_use_hint(hint, value);
    }

    iterator insert(const_iterator hint, value_type&& value) {
        return ht_.emplace_multi_use_hint(hint, mstl::move(value));
    }

    template<typename InputIter>
    void insert(InputIter first, InputIter last) {
        ht_.insert_multi(first, last);
    }

    void erase(iterator iter) {
        ht_.erase(iter);
    }

    void erase(iterator first, iterator last) {
        ht_.earse(first, last);
    }

    size_type erase(const key_type& key) {
        return ht_.erase_multi(key);
    }

    void clear() {
        ht_.clear();
    }

    void swap(unordered_multimap& rhs) noexcept {
        ht_.swap(rhs.ht_);
    }

    size_type count(const key_type key) const {
        return ht_.count();
    }

    iterator find(const key_type& key) {
        return ht_.find();
    }

    const_iterator find(const key_type& key) const {
        return ht_.find(key);
    }

    pair<iterator, iterator> equal_range(const key_type& key) {
        return ht_.equal_range_multi(key);
    }

    pair<iterator, iterator> equal_range(const key_type& key) const {
        return ht_.equal_range_multi(key);
    }

    local_iterator begin(const size_type n) noexcept {
        return ht_.begin(n);
    }

    const_local_iterator begin(const size_type n) const noexcept {
        return ht_.begin(n);
    }

    const_local_iterator cbegin(const size_type n) const noexcept {
        return ht_.cbegin(n);
    }

    local_iterator end(const size_type n) noexcept {
        return ht_.end();
    }

    const_local_iterator end(const size_type n) const noexcept {
        return ht_.end();
    }

    const_local_iterator cend(const size_type n) const noexcept {
        return ht_.cend();
    }

    size_type bucket_count() const noexcept {
        return ht_.bucket_count();
    }

    size_type max_bucket_count() const noexcept {
        return ht_.max_bucket_count();
    }

    size_type bucket_size(const size_type n) const noexcept {
        return ht_.bucket_size(n);
    }

    size_type bucket(const key_type& key) const {
        return ht_.bucket(key);
    }

    float load_factor() const noexcept {
        return ht_.load_factor();
    }

    float max_load_factor() const noexcept {
        return ht_.max_load_factor();
    }

    void max_load_factor(float ml) noexcept {
        ht_.max_load_factor(ml);
    }

    void rehash(const size_type count) {
        ht_.rehash(count);
    }

    void reserve(const size_type count) {
        ht_.reserve(count);
    }

    hasher hash_func() const {
        return ht_.hash_func();
    }

    key_equal hash_eq() const {
        return ht_.hash_eq();
    }

public:
    friend bool operator==(const unordered_multimap& lhs, const unordered_multimap& rhs) {
        return lhs.ht_.equal_to_multi(rhs.ht_);
    }

    friend bool operator!=(const unordered_multimap& lhs, const unordered_multimap& rhs) {
        return !lhs.ht_.equal_to_multi(rhs.ht_);
    }
};

template<typename Key, typename T, typename Hash, typename KeyEqual>
bool operator==(const unordered_multimap<Key, T, Hash, KeyEqual>& lhs,
                const unordered_multimap<Key, T, Hash, KeyEqual>& rhs) {
    return lhs == rhs;
}

template<typename Key, typename T, typename Hash, typename KeyEqual>
bool operator!=(const unordered_multimap<Key, T, Hash, KeyEqual>& lhs,
                const unordered_multimap<Key, T, Hash, KeyEqual>& rhs) {
    return lhs != rhs;
}

template<typename Key, typename T, typename Hash, typename KeyEqual>
void swap(unordered_multimap<Key, T, Hash, KeyEqual>& lhs,
          unordered_multimap<Key, T, Hash, KeyEqual>& rhs) {
    lhs.swap(rhs);
}



} // mstl


#endif