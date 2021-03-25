#ifndef M_HASHTABLE_H
#define M_HASHTABLE_H

#include <initializer_list>

#include "m_vector.h"
#include "m_util.h"
#include "m_functional.h"
#include "m_memory.h"
#include "m_exceptdef.h"
#include "m_algo.h"

namespace mstl {

template<typename T>
struct hashtable_node {
    hashtable_node* next;
    T value;

    hashtable_node() = default;

    hashtable_node(const T& v) : next(nullptr), value(v) {}

    hashtable_node(const hashtable_node& rhs) : next(rhs.next), value(rhs.value) {}

    hashtable_node(hashtable_node&& rhs) : next(rhs.next), value(mstl::move(rhs.value)) {
        rhs.next = nullptr;
    }
};

template<typename T, bool>
struct ht_value_traits_imp {
    typedef T   key_type;
    typedef T   mapped_type;
    typedef T   value_type;

    // 返回const引用表明返回的key值不能修改
    template<typename Ty>
    static const key_type& get_key(const Ty& value) {
        return value;
    }

    template<typename Ty>
    static const value_type& get_value(const Ty& value) {
        return value;
    }
};

template<typename T>
struct ht_value_traits_imp<T, true> {
    typedef typename std::remove_cv<typename T::first_type>::type key_type;
    typedef typename T::second_type                               mapped_type;
    typedef T                                                     value_type;

    // 返回const引用表明返回的key值不能修改
    template<typename Ty>
    static const key_type& get_key(const Ty& value) {
        return value.first;
    }

    template<typename Ty>
    static const value_type& get_value(const Ty& value) {
        return value;
    }
};

template<typename T>
struct ht_value_traits {
    static constexpr bool is_map = mstl::is_pair<T>::value;

    typedef ht_value_traits_imp<T, is_map> value_traits_type;

    typedef typename value_traits_type::key_type    key_type;
    typedef typename value_traits_type::mapped_type mapped_type;
    typedef typename value_traits_type::value_type  value_type;

    template<typename Ty>
    static const key_type& get_key(const Ty& value) {
        return value_traits_type::get_key(value);
    }

    template<typename Ty>
    static const value_type& get_value(const Ty& value) {
        return value_traits_type::get_value(value);
    }
};

template<typename T, typename HashFun, typename KeyEqual>
struct hashtable;

template<typename T, typename HashFun, typename KeyEqual>
struct ht_iterator;

template<typename T, typename HashFun, typename KeyEqual>
struct ht_const_iterator;

template<typename T>
struct ht_local_iterator;

template<typename T>
struct ht_const_local_iterator;

template<typename T, typename Hash, typename KeyEqual>
struct ht_iterator_base : public mstl::iterator<mstl::forward_iterator_tag, T>{
    typedef mstl::hashtable<T, Hash, KeyEqual>         hashtable;
    typedef ht_iterator_base<T, Hash, KeyEqual>        base;
    typedef mstl::ht_iterator<T, Hash, KeyEqual>       iterator;
    typedef mstl::ht_const_iterator<T, Hash, KeyEqual> const_iterator;
    typedef hashtable_node<T>*                         node_ptr;
    typedef hashtable*                                 contain_ptr;
    typedef const node_ptr                             const_node_ptr;
    typedef const contain_ptr                          const_contain_ptr;

    typedef size_t                                     size_type;
    typedef ptrdiff_t                                  difference_type;

    node_ptr    node;
    contain_ptr ht;

    ht_iterator_base() = default;

    bool operator==(const base& rhs) {
        return node == rhs.node;
    }

    bool operator!=(const base& rhs) {
        return node != rhs.node;
    }
};

template<typename T, typename Hash, typename KeyEqual>
struct ht_iterator : public ht_iterator_base<T, Hash, KeyEqual> {
    typedef ht_iterator_base<T, Hash, KeyEqual> base;
    typedef typename base::hashtable            hashtable;
    typedef typename base::iterator             iterator;
    typedef typename base::const_iterator       const_iterator;
    typedef typename base::node_ptr             node_ptr;
    typedef typename base::contain_ptr          contain_ptr;

    typedef ht_value_traits<T>                  value_traits;
    typedef T                                   value_type;
    typedef value_type*                         pointer;
    typedef value_type&                         reference;






    // 此处using的作用是什么
    using base::node;
    using base::ht;











    ht_iterator() = default;

    ht_iterator(node_ptr n, contain_ptr c) {
        node = n;
        ht = c;
    }

    ht_iterator(const iterator& rhs) {
        node = rhs.node;
        ht = rhs.ht;
    }

    ht_iterator(const const_iterator& rhs) {
        node = rhs.node;
        ht = rhs.ht;
    }

    iterator& operator=(const iterator& rhs) {
        if (this != &rhs) {
            node = rhs.node;
            ht = rhs.ht;
        }
        return *this;
    }

    iterator& operator=(const const_iterator& rhs) {
        if (this != &rhs) {
            node = rhs.node;
            ht = rhs.ht;
        }
        return *this;
    }

    reference operator*() const {
        return node->value;
    }

    pointer operator->() const {
        return &(operator*());
    }

    iterator& operator++() {
        MSTL_DEBUG(node != nullptr);
        node_ptr old = node;
        // 指向下一个节点
        node = node->next;
        // 下一个节点不存在，则跳转到下一个篮子bucket中
        if (node == nullptr) {
            size_t index = ht->hash(value_traits::get_key(old->value));
            while (!node && ++index < ht->bucket_size_) {
                node = ht->buckets_[index];
            }
        }
        return *this;
    }

    iterator operator++(int) {
        iterator temp = *this;
        ++*this;
        return temp;
    }
};

template<typename T, typename Hash, typename KeyEqual>
struct ht_const_iterator : public ht_iterator_base<T, Hash, KeyEqual> {
    typedef ht_iterator_base<T, Hash, KeyEqual> base;
    typedef typename base::hashtable            hashtable;
    typedef typename base::iterator             iterator;
    typedef typename base::const_iterator       const_iterator;
    typedef typename base::const_node_ptr       node_ptr;
    typedef typename base::const_contain_ptr    contain_ptr;

    typedef ht_value_traits<T>                  value_traits;
    typedef T                                   value_type;
    typedef value_type*                         pointer;
    typedef value_type&                         reference;

    using base::node;
    using base::ht;

    ht_const_iterator() = default;

    ht_const_iterator(node_ptr n, contain_ptr c) {
        node = n;
        ht = c;
    }

    ht_const_iterator(const iterator& rhs) {
        node = rhs.node;
        ht = rhs.ht;
    }

    ht_const_iterator(const const_iterator& rhs) {
        node = rhs.node;
        ht = rhs.ht;
    }

    iterator& operator=(const iterator& rhs) {
        if (this != &rhs) {
            node = rhs.node;
            ht = rhs.ht;
        }
        return *this;
    }

    iterator& operator=(const const_iterator& rhs) {
        if (this != &rhs) {
            node = rhs.node;
            ht = rhs.ht;
        }
        return *this;
    }

    reference operator*() const {
        return node->value;
    }

    pointer operator->() const {
        return &(operator*());
    }

    iterator& operator++() {
        MSTL_DEBUG(node != nullptr);
        const node_ptr old = node;
        node = node->next;
        if (node == nullptr) {
            size_t index = ht->hash(value_traits::get_key(old->value));
            while (!node && ++index < ht->bucket_size_) {
                node = ht->buckets_[index];
            }
        }
        return *this;
    }

    iterator operator++(int) {
        iterator temp = *this;
        ++*this;
        return temp;
    }
};

template<typename T>
struct ht_local_iterator : public mstl::iterator<mstl::forward_iterator_tag, T> {
    typedef T                          value_type;
    typedef value_type*                pointer;
    typedef value_type&                reference;
    typedef size_t                     size_type;
    typedef ptrdiff_t                  difference_type;
    typedef hashtable_node<T>*         node_ptr;

    typedef ht_local_iterator<T>       self;
    typedef ht_local_iterator<T>       local_iterator;
    typedef ht_const_local_iterator<T> const_local_iterator;

    node_ptr node; 

    ht_local_iterator(node_ptr n) : node(n) {}

    ht_local_iterator(const local_iterator& rhs) : node(rhs.node) {}

    ht_local_iterator(const const_local_iterator& rhs) : node(rhs.node) {}

    reference operator*() const {
        return node->value;
    }

    pointer operator->() const {
        return &(operator*());
    }

    self& operator++() {
        MSTL_DEBUG(node != nullptr);
        node = node->next;
        return *this;
    }

    self operator++(int) {
        self temp(*this);
        ++*this;
        return temp;
    }

    bool operator==(const self& other) const {
        return node == other.node;
    }

    bool operator!=(const self& other) const {
        return node != other.node;
    }
};

template<typename T>
struct ht_const_local_iterator : public mstl::iterator<mstl::forward_iterator_tag, T> {
    typedef T                          value_type;
    typedef const value_type*          pointer;
    typedef const value_type&          reference;
    typedef size_t                     size_type;
    typedef ptrdiff_t                  difference_type;
    typedef const hashtable_node<T>*   node_ptr;

    typedef ht_const_local_iterator<T> self;
    typedef ht_local_iterator<T>       local_iterator;
    typedef ht_const_local_iterator<T> const_local_iterator;

    node_ptr node; 

    ht_const_local_iterator(node_ptr n) : node(n) {}

    ht_const_local_iterator(const local_iterator& rhs) : node(rhs.node) {}

    ht_const_local_iterator(const const_local_iterator& rhs) : node(rhs.node) {}

    reference operator*() const {
        return node->value;
    }

    pointer operator->() const {
        return &(operator*());
    }

    self& operator++() {
        MSTL_DEBUG(node != nullptr);
        node = node->next;
        return *this;
    }

    self operator++(int) {
        self temp(*this);
        ++*this;
        return temp;
    }

    bool operator==(const self& other) const {
        return node == other.node;
    }

    bool operator!=(const self& other) const {
        return node != other.node;
    }
};

// bucket 使用的大小

#if (_MSC_VER && _WIN64) || ((__GNUC__ || __clang__) &&__SIZEOF_POINTER__ == 8)
#define SYSTEM_64 1
#else
#define SYSTEM_32 1
#endif

#ifdef SYSTEM_64

#define PRIME_NUM 99

// 1. start with p = 101
// 2. p = next_prime(p * 1.7)
// 3. if p < (2 << 63), go to step 2, otherwise, go to step 4
// 4. end with p = prev_prime(2 << 63 - 1)
static constexpr size_t ht_prime_list[] = {
  101ull, 173ull, 263ull, 397ull, 599ull, 907ull, 1361ull, 2053ull, 3083ull,
  4637ull, 6959ull, 10453ull, 15683ull, 23531ull, 35311ull, 52967ull, 79451ull,
  119179ull, 178781ull, 268189ull, 402299ull, 603457ull, 905189ull, 1357787ull,
  2036687ull, 3055043ull, 4582577ull, 6873871ull, 10310819ull, 15466229ull,
  23199347ull, 34799021ull, 52198537ull, 78297827ull, 117446801ull, 176170229ull,
  264255353ull, 396383041ull, 594574583ull, 891861923ull, 1337792887ull,
  2006689337ull, 3010034021ull, 4515051137ull, 6772576709ull, 10158865069ull,
  15238297621ull, 22857446471ull, 34286169707ull, 51429254599ull, 77143881917ull,
  115715822899ull, 173573734363ull, 260360601547ull, 390540902329ull, 
  585811353559ull, 878717030339ull, 1318075545511ull, 1977113318311ull, 
  2965669977497ull, 4448504966249ull, 6672757449409ull, 10009136174239ull,
  15013704261371ull, 22520556392057ull, 33780834588157ull, 50671251882247ull,
  76006877823377ull, 114010316735089ull, 171015475102649ull, 256523212653977ull,
  384784818980971ull, 577177228471507ull, 865765842707309ull, 1298648764060979ull,
  1947973146091477ull, 2921959719137273ull, 4382939578705967ull, 6574409368058969ull,
  9861614052088471ull, 14792421078132871ull, 22188631617199337ull, 33282947425799017ull,
  49924421138698549ull, 74886631708047827ull, 112329947562071807ull, 168494921343107851ull,
  252742382014661767ull, 379113573021992729ull, 568670359532989111ull, 853005539299483657ull,
  1279508308949225477ull, 1919262463423838231ull, 2878893695135757317ull, 4318340542703636011ull,
  6477510814055453699ull, 9716266221083181299ull, 14574399331624771603ull, 18446744073709551557ull
};

#else

#define PRIME_NUM 44

// 1. start with p = 101
// 2. p = next_prime(p * 1.7)
// 3. if p < (2 << 31), go to step 2, otherwise, go to step 4
// 4. end with p = prev_prime(2 << 31 - 1)
static constexpr size_t ht_prime_list[] = {
  101u, 173u, 263u, 397u, 599u, 907u, 1361u, 2053u, 3083u, 4637u, 6959u, 
  10453u, 15683u, 23531u, 35311u, 52967u, 79451u, 119179u, 178781u, 268189u,
  402299u, 603457u, 905189u, 1357787u, 2036687u, 3055043u, 4582577u, 6873871u,
  10310819u, 15466229u, 23199347u, 34799021u, 52198537u, 78297827u, 117446801u,
  176170229u, 264255353u, 396383041u, 594574583u, 891861923u, 1337792887u,
  2006689337u, 3010034021u, 4294967291u,
};

#endif

inline size_t ht_next_prime(size_t n) {
    const size_t* first = ht_prime_list;
    const size_t* last = ht_prime_list + PRIME_NUM;
    const size_t* pos = mstl::lower_bound(first, last, n);
    return pos == last ? *(last - 1) : *pos;
}

template<typename T, typename Hash, typename KeyEqual>
class hashtable {
    // 允许两个iterator类访问自身的私有成员
    friend struct mstl::ht_iterator<T, Hash, KeyEqual>;
    friend struct mstl::ht_const_iterator<T, Hash, KeyEqual>;

public:
    typedef ht_value_traits<T>                               value_traits;
    typedef typename value_traits::key_type                  key_type;
    typedef typename value_traits::mapped_type               mapped_type;
    typedef typename value_traits::value_type                value_type;
    typedef Hash                                             hasher;
    typedef KeyEqual                                         key_equal;

    typedef hashtable_node<T>                                node_type;
    typedef node_type*                                       node_ptr;
    typedef mstl::vector<node_ptr>                           bucket_type;

    typedef mstl::allocator<T>                               allocator_type;
    typedef mstl::allocator<T>                               data_allocator;
    typedef mstl::allocator<node_type>                       node_allocator;

    typedef typename allocator_type::pointer                 pointer;
    typedef typename allocator_type::const_pointer           const_pointer;
    typedef typename allocator_type::reference               reference;
    typedef typename allocator_type::const_reference         const_reference;
    typedef typename allocator_type::size_type               size_type;
    typedef typename allocator_type::difference_type         difference_type;

    typedef mstl::ht_iterator<T, Hash, KeyEqual>             iterator;
    typedef mstl::ht_const_iterator<T, Hash, KeyEqual>       const_iterator;
    typedef mstl::ht_local_iterator<T>                       local_iterator;
    typedef mstl::ht_const_local_iterator<T>                 const_local_iterator;

    allocator_type get_allocator() {
        return allocator_type();
    }

private:
    bucket_type buckets_;
    size_type   bucket_size_;
    size_type   size_;
    // 负载因子
    float       mlf_;
    hasher      hash_;
    key_equal   equal_;

private:
    bool is_equal(const key_type& k1, const key_type k2) {
        return equal_(k1, k2);
    }

    bool is_equal(const key_type& k1, const key_type k2) const {
        return equal_(k1, k2);
    }
    
    // 生成const迭代器
    const_iterator M_cit(node_ptr node) const noexcept {
        return const_iterator(node, const_cast<hashtable*>(this));
    }

    iterator M_begin() noexcept {
        for (size_type n = 0; n < bucket_size_; ++n) {
            if (buckets_[n]) {
                return iterator(buckets_[n], this);
            }
        }
        return iterator(nullptr, this);
    }

    iterator M_begin() const noexcept {
        for (size_type n = 0; n < bucket_size_; ++n) {
            if (buckets_[n]) {
                return M_cit(buckets_[n]);
            }
        }
        return M_cit(nullptr);
    }

public:
    // 各类构造函数
    explicit hashtable(size_type bucket_count, const Hash& hash = Hash(),
        const KeyEqual& equal = KeyEqual())
        : size_(0), mlf_(1.0f), hash_(hash), equal_(equal) {
        init(bucket_count);
    }

    template<typename InputIter, typename mstl::enable_if<
        mstl::is_input_iterator<InputIter>::value, int>::type = 0>
    hashtable(InputIter first, InputIter last,
              size_type bucket_count, const Hash& hash = Hash(),
              const KeyEqual& equal = KeyEqual())
    : size_(mstl::distance(first, last)), mlf_(1.0f), hash_(hash), equal_(equal) {
        init(mstl::max(bucket_count, static_cast<size_type>(mstl::distance(first, last))));
    }

    hashtable(const hashtable& rhs) : hash_(rhs.hash_), equal_(rhs.equal_) {
        copy_init(rhs);
    }

    hashtable(hashtable&& rhs) noexcept 
        : bucket_size_(rhs.bucket_size_), size_(rhs.size_),
        mlf_(rhs.mlf_), hash_(rhs.hash_), equal_(rhs.equal_) {
        buckets_ = mstl::move(rhs.buckets_);
        rhs.bucket_size_ = 0;
        rhs.size_ = 0;
        rhs.mlf_ = 0.0f;
    }

    hashtable& operator=(const hashtable& rhs);
    hashtable& operator=(hashtable&& rhs) noexcept;

    ~hashtable() {
        clear();
    }

    iterator begin() noexcept {
        return M_begin();
    }

    const_iterator begin() const noexcept {
        return M_begin();
    }

    iterator end() noexcept {
        return iterator(nullptr, this);
    }

    const_iterator end() const noexcept {
        return M_cit(nullptr);
    }

    const_iterator cbegin() const noexcept {
        return begin();
    }

    const_iterator cend() const noexcept {
        return end();
    }

    bool empty() const noexcept {
        return size_ == 0;
    }

    size_type size() const noexcept {
        return size_;
    }

    size_type max_size() const noexcept {
        return static_cast<size_type>(-1);
    }

    template<typename... Args>
    iterator emplace_multi(Args&&... args);

    template<typename... Args>
    pair<iterator, bool> emplace_unique(Args&&... args);

    // 插入元素，同时给予 hint 位置，告诉编译器寻找可插入位置的起始处用以优化，编译器可能不采用
    // 这里 hint 对于 hashtable 其实没有意义，因为即使提供了 hint，也要做一次 hash()，
    // 来确保 hashtable 的性质，所以这里我们选择忽略它，直接调用无 hint 版本
    template<typename... Args>
    iterator emplace_multi_use_hint(const_iterator /*hint*/, Args&&... args) {
        return emplace_multi(mstl::forward<Args>(args)...);
    }

    template<typename... Args>
    iterator emplace_unique_use_hint(const_iterator /*hint*/, Args&&... args) {
        return emplace_unique(mstl::forward<Args>(args)...).first;
    }

    iterator insert_multi_noresize(const value_type& value);
    pair<iterator, bool> insert_unique_noresize(const value_type& value);

    iterator insert_multi(const value_type& value) {
        rehash_if_need(1);
        return insert_multi_noresize(value);
    }
    iterator insert_multi(value_type&& value) {
        return emplace_multi(mstl::move(value));
    }

    pair<iterator, bool> insert_unique(const value_type& value) {
        rehash_if_need(1);
        return insert_unique_noresize(value);
    }

    pair<iterator, bool> insert_unique(value_traits&& value) {
        return emplace_unique(mstl::move(value));
    }

    // 同样，我们对于 hint 方法不做理会
    iterator insert_multi_use_hint(const_iterator /*hint*/, const value_type& value) {
        return insert_multi(value);
    }

    iterator insert_multi_use_hint(const_iterator /*hint*/, value_type&& value) {
        return emplace_multi(mstl::move(value));
    }

    iterator insert_unique_use_hint(const_iterator /*hint*/, const value_type& value) {
        return insert_unique(value).first;
    }

    iterator insert_unique_use_hint(const_iterator /*hint*/, value_type&& value) {
        return emplace_unique(mstl::move(value));
    }

    template <class InputIter>
    void insert_multi(InputIter first, InputIter last) {
        copy_insert_multi(first, last, iterator_category(first));
    }

    template <class InputIter>
    void insert_unique(InputIter first, InputIter last) {
        copy_insert_unique(first, last, iterator_category(first));
    }

    void erase(const_iterator pos);
    void erase(const_iterator first, const_iterator last);

    size_type erase_multi(const key_type& key);
    size_type erase_unique(const key_type& key);

    void clear();

    void swap(hashtable& rhs) noexcept;

    // hashtable查找相关
    size_type count(const key_type& key) const;

    iterator find(const key_type& key);
    const_iterator find(const key_type& key) const;

    pair<iterator, iterator> equal_range_multi(const key_type& key);
    pair<const_iterator, const_iterator> equal_range_multi(const key_type& key) const;
    pair<iterator, iterator> equal_range_unique(const key_type& key);
    pair<const_iterator, const_iterator> equal_range_unique(const key_type& key) const;

    // 每个篮子中链表的迭代器相关
    local_iterator begin(size_type n) noexcept {
        MSTL_DEBUG(n < bucket_size_);
        return buckets_[n];
    }

    const_local_iterator begin(size_type n) const noexcept {
        MSTL_DEBUG(n < bucket_size_);
        return buckets_[n];
    }

    const_local_iterator cbegin(size_type n) const noexcept {
        MSTL_DEBUG(n < bucket_size_);
        return buckets_[n];
    }

    local_iterator end(size_type n) noexcept {
        MSTL_DEBUG(n < bucket_size_);
        return nullptr;
    }

    const_local_iterator end(size_type n) const noexcept {
        MSTL_DEBUG(n < bucket_size_);
        return nullptr;
    }

    const_local_iterator cend(size_type n) const noexcept {
        MSTL_DEBUG(n < bucket_size_);
        return nullptr;
    }

    size_type bucket_count() const noexcept {
        return bucket_size_;
    }

    size_type max_bucket_count() const noexcept {
        return ht_prime_list[PRIME_NUM - 1];
    }

    size_type bucket_size(size_type n) const noexcept;
    size_type bucket(const key_type& key) const {
        return hash(key);
    }

    float load_factor() const noexcept {
        return bucket_size_ != 0 ? (float)size_ / bucket_size_ : 0.0f; 
    }
    
    float max_load_factor() const noexcept {
        return mlf_;
    }

    void max_load_factor(float ml) noexcept {

        
        
        // ml != ml意义不明


        
        THROW_OUT_OF_RANGE_IF(ml != ml || ml < 0, "invalid hash load factor!");
        mlf_ = ml;
    }

    void rehash(size_type count);
    void reserve(size_type count) {
        return rehash(static_cast<size_type>((float)count / max_load_factor() + 0.5f));
    }

    hasher hash_func() const {
        return hash_;
    }

    key_equal key_eq() const {
        return equal_;
    }

    // 判断hashtable是否相同
    bool equal_to_multi(const hashtable& other) const;
    bool equal_to_unique(const hashtable& other) const;

private:
    void init(size_type n);
    void copy_init(const hashtable& ht);

    template<typename ...Args>
    node_ptr create_node(Args&& ...args);
    void destory_node(node_ptr n);

    size_type next_size(size_type n) const;

    size_type hash(const key_type& key) const;
    size_type hash(const key_type& key, size_type n) const;
    void rehash_if_need(size_type n);






    // 区分InputIter和ForwardIter的原因是什么？
    template<typename InputIter>
    void copy_insert_multi(InputIter first, InputIter last, mstl::input_iterator_tag);
    template<typename forwardIter>
    void copy_insert_multi(forwardIter first, forwardIter last, mstl::forward_iterator_tag);
    template<typename InputIter>
    void copy_insert_unqiue(InputIter first, InputIter last, mstl::input_iterator_tag);
    template<typename forwardIter>
    void copy_insert_unqiue(forwardIter first, forwardIter last, mstl::forward_iterator_tag);






    iterator insert_node_multi(node_ptr np);
    pair<iterator, bool> insert_node_unique(node_ptr np);


    void replace_bucket(size_type bucket_count);
    void erase_bucket(size_type n, node_ptr first, node_ptr last);
    void erase_bucket(size_type n, node_ptr last);

};

template<typename T, typename Hash, typename KeyEqual>
hashtable<T, Hash, KeyEqual>&
hashtable<T, Hash, KeyEqual>::operator=(const hashtable& rhs) {
    if (this != &rhs) {
        hashtable temp(rhs);
        swap(rhs);
    }
    return *this;
}

template<typename T, typename Hash, typename KeyEqual>
hashtable<T, Hash, KeyEqual>&
hashtable<T, Hash, KeyEqual>::operator=(hashtable&& rhs) noexcept {
    if (this != &rhs) {
        hashtable temp(mstl::move(rhs));
        swap(rhs);
    }
    return *this;
}

// 插入元素可重复
template<typename T, typename Hash, typename KeyEqual>
template<typename... Args>
typename hashtable<T, Hash, KeyEqual>::iterator 
hashtable<T, Hash, KeyEqual>::emplace_multi(Args&&... args) {
    node_ptr np = create_node(mstl::forward<Args>(args)...);
    try {
        if ((float)(size_ + 1) > (float)bucket_size_ * max_load_factor()) {
            rehash(size_ + 1);
        }
    } catch (...) {
        destory_node(np);
        throw;
    }
    return insert_node_multi(np);
}

// 插入元素不可重复
template<typename T, typename Hash, typename KeyEqual>
template<typename... Args>
pair<typename hashtable<T, Hash, KeyEqual>::iterator, bool>
hashtable<T, Hash, KeyEqual>::emplace_unique(Args&&... args) {
    node_ptr np = create_node(mstl::forward<Args>(args)...);
    try {
        if ((float)(size_ + 1) > (float)bucket_size_ * max_load_factor()) {
            rehash(size_ + 1);
        }
    } catch (...) {
        destory_node(np);
        throw;
    }
    return insert_node_unique(np);
}

// 可重复插入相同元素
template<typename T, typename Hash, typename KeyEqual>
typename hashtable<T, Hash, KeyEqual>::iterator
hashtable<T, Hash, KeyEqual>::insert_multi_noresize(const value_type& value) {
    const size_type n = hash(value_traits::get_key(value));
    node_ptr first = buckets_[n];
    node_ptr temp = create_node(value);
    for (node_ptr cur = first; cur != nullptr; cur = cur->next) {
        if (is_equal(value_traits::get_key(value), value_traits::get_key(cur->value))) {
            temp->next = cur->next;
            cur->next = temp;
            ++size_;
            return iterator(temp, this);
        }
    }
    temp->next = first;
    buckets_[n] = temp;
    ++size_;
    return iterator(temp, this);
}

// 不可重复插入相同元素
template<typename T, typename Hash, typename KeyEqual>
pair<typename hashtable<T, Hash, KeyEqual>::iterator, bool>
hashtable<T, Hash, KeyEqual>::insert_unique_noresize(const value_type& value) {
    const size_type n = hash(value_traits::get_key(value));
    node_ptr first = buckets_[n];
    for (node_ptr cur = first; cur != nullptr; cur = cur->next) {
        if (is_equal(value_traits::get_key(value), value_traits::get_key(cur->value))) {
            return mstl::make_pair(iterator(cur, this), false);
        }
    }
    node_ptr temp = create_node(value);
    temp->next = first;
    buckets_[n] = temp;
    ++size_;
    return mstl::make_pair(iterator(temp, this), true);
}

template<typename T, typename Hash, typename KeyEqual>
void hashtable<T, Hash, KeyEqual>::erase(const_iterator pos) {
    node_ptr p = pos.node;
    if (p == nullptr) {
        return;
    }
    const size_type n = hash(value_traits::get_key(p->value));
    node_ptr cur = buckets_[n];
    if (cur == p) {
        buckets_[n] = cur->next;
        destory_node(p);
        --size_;
    } else {
        node_ptr next = cur->next;
        while (next != nullptr) {
            if (next == p) {
                cur->next = next->next;
                destory_node(next);
                --size_;
                break;
            } else {
                cur = next;
                next = cur->next;
            }
        }
    }
}

template<typename T, typename Hash, typename KeyEqual>
void hashtable<T, Hash, KeyEqual>::erase(const_iterator first, const_iterator last) {
    if (first.node == last.node) {
        return;
    }
    const size_type first_bucket = first.node != nullptr ? hash(value_traits::get_key(first.node->value))
                                    : bucket_size_;
    const size_type last_bucket = last.node != nullptr ? hash(value_traits::get_key(last.node->value))
                                    : bucket_size_;
    if (first_bucket == last_bucket) {
        erase_bucket(first_bucket, first.node, last.node);
    } else {
        erase_bucket(first_bucket, first.node, nullptr);
        for (size_type n = first_bucket + 1; n < last_bucket; ++n) {
            if (buckets_[n] != nullptr) {
                erase_bucket(n, nullptr);
            }
        }
        if (last_bucket != bucket_size_) {
            erase_bucket(last_bucket, last.node);
        }
    }
}

template<typename T, typename Hash, typename KeyEqual>
typename hashtable<T, Hash, KeyEqual>::size_type
hashtable<T, Hash, KeyEqual>::erase_multi(const key_type& key) {
    auto p = equal_range_multi(key);
    if (p.first.node != nullptr) {
        erase(p.first, p.second);
        return mstl::distance(p.first, p.second);
    }
    return 0;
}

template<typename T, typename Hash, typename KeyEqual>
typename hashtable<T, Hash, KeyEqual>::size_type
hashtable<T, Hash, KeyEqual>::erase_unique(const key_type& key) {
    const size_type n = hash(key);
    node_ptr first = buckets_[n];
    if (first == nullptr) {
        return 0;
    }
    if (is_equal(value_traits::get_key(first.value), key)) {
        buckets_[n] = first->next;
        destory_node(first);
        --size_;
        return 1;
    } else {
        node_ptr next = first->next;
        while (next != nullptr) {
            if (is_equal(value_traits::get_key(next->value), key)) {
                first->next = next->next;
                destory_node(next);
                --size_;
                return 1;
            }
            first = next;
            next = first->next;
        }
    }
    return 0;
}

template<typename T, typename Hash, typename KeyEqual>
void hashtable<T, Hash, KeyEqual>::clear() {
    if (size_ != 0) {
        for (size_type i = 0; i < bucket_size_; ++i) {
            node_ptr cur = buckets_[i];
            while (cur != nullptr) {
                node_ptr next = cur->next;
                destory_node(cur);
                cur = next;
            }
            buckets_[i] = nullptr;
        }
        size_ = 0;
    }
}

template<typename T, typename Hash, typename KeyEqual>
void hashtable<T, Hash, KeyEqual>::swap(hashtable& rhs) noexcept {
    if (this != &rhs) {
        buckets_.swap(rhs.buckets_);
        mstl::swap(bucket_size_, rhs.bucket_size_);
        mstl::swap(size_, rhs.size_);
        mstl::swap(mlf_, rhs.mlf_);
        mstl::swap(hash_, rhs.hash_);
        mstl::swap(equal_, rhs.equal_);
    }
}

template<typename T, typename Hash, typename KeyEqual>
typename hashtable<T, Hash, KeyEqual>::size_type
hashtable<T, Hash, KeyEqual>::count(const key_type& key) const {
    const size_type n = hash(key);
    size_type count = 0;
    for (node_ptr cur = buckets_[n]; cur != nullptr; cur = cur->next) {
        if (is_equal(value_traits::get_key(cur->value), key)) {
            ++count;
        }
    }
    return count;
}

template<typename T, typename Hash, typename KeyEqual>
typename hashtable<T, Hash, KeyEqual>::iterator
hashtable<T, Hash, KeyEqual>::find(const key_type& key) {
    const size_type n = hash(key);
    node_ptr cur = buckets_[n];
    while (cur != nullptr) {
        if (is_equal(value_traits::get_key(cur->value), key)) {
            break;
        }
        cur = cur->next;
    }
    return iterator(cur, this);
}

template<typename T, typename Hash, typename KeyEqual>
typename hashtable<T, Hash, KeyEqual>::const_iterator
hashtable<T, Hash, KeyEqual>::find(const key_type& key) const {
    const size_type n = hash(key);
    node_ptr cur = buckets_[n];
    while (cur != nullptr) {
        if (is_equal(value_traits::get_key(cur->value), key)) {
            break;
        }
        cur = cur->next;
    }
    return M_cit(cur);
}

template<typename T, typename Hash, typename KeyEqual>
pair<typename hashtable<T, Hash, KeyEqual>::iterator, typename hashtable<T, Hash, KeyEqual>::iterator>
hashtable<T, Hash, KeyEqual>::equal_range_multi(const key_type& key) {
    const size_type n = hash(key);
    for (node_ptr first = buckets_[n]; first != nullptr; first = first->next) {
        // 找到第一个相等的位置
        if (is_equal(value_traits::get_key(first->value), key)) {
            // 开始寻找最后一个相等的位置
            for (node_ptr second = first->next; second != nullptr; second = second->next) {
                if (!is_equal(value_traits::get_key(second->value), key)) {
                    return mstl::make_pair(iterator(first, this), iterator(second, this));
                }
            }
            // 如果first开始的整个链表全都相等，那么寻找下一个非空的bucket作为尾节点last
            for (size_type m = n + 1; m < bucket_size_; ++m) {
                if (buckets_[m] != nullptr) {
                    return mstl::make_pair(iterator(first, this), iterator(buckets_[m], this));
                }
            }
            return mstl::make_pair(iterator(first, this), end());
        }
    }
    return mstl::make_pair(end(), end());
}

// 寻找键值为key的所有节点，并返回pair表示起止位置
template<typename T, typename Hash, typename KeyEqual>
pair<typename hashtable<T, Hash, KeyEqual>::const_iterator, typename hashtable<T, Hash, KeyEqual>::const_iterator>
hashtable<T, Hash, KeyEqual>::equal_range_multi(const key_type& key) const {
    const size_type n = hash(key);
    for (node_ptr first = buckets_[n]; first != nullptr; first = first->next) {
        // 找到第一个相等的位置
        if (is_equal(value_traits::get_key(first->value), key)) {
            // 开始寻找最后一个相等的位置
            for (node_ptr second = first->next; second != nullptr; second = second->next) {
                if (!is_equal(value_traits::get_key(second->value), key)) {
                    return mstl::make_pair(M_cit(first), M_cit(second));
                }
            }
            // 如果first开始的整个链表全都相等，那么寻找下一个非空的bucket作为尾节点last
            for (size_type m = n + 1; m < bucket_size_; ++m) {
                if (buckets_[m] != nullptr) {
                    return mstl::make_pair(M_cit(first), M_cit(buckets_[m]));
                }
            }
            return mstl::make_pair(M_cit(first), cend());
        }
    }
    return mstl::make_pair(cend(), cend());
}

template<typename T, typename Hash, typename KeyEqual>
pair<typename hashtable<T, Hash, KeyEqual>::iterator, typename hashtable<T, Hash, KeyEqual>::iterator>
hashtable<T, Hash, KeyEqual>::equal_range_unique(const key_type& key) {
    const size_type n = hash(key);
    for (node_ptr first = buckets_[n]; first != nullptr; first = first->next) {
        if (is_equal(value_traits::get_key(first->value), key)) {
            if (first->next != nullptr) {
                return mstl::make_pair(iterator(first, this), iterator(first->next, this));
            }
            // 如果相等的节点是链表的最后一个节点，那么需要找到下一个非空的bucket作为结尾
            for (size_type m = n + 1; m < bucket_size_; ++m) {
                if (buckets_[m] != nullptr) {
                    return mstl::make_pair(iterator(first, this), iterator(buckets_[m], this));
                }
            }
            return mstl::make_pair(iterator(first, this), end());
        }
    }
    return mstl::make_pair(end(), end());
}

template<typename T, typename Hash, typename KeyEqual>
pair<typename hashtable<T, Hash, KeyEqual>::const_iterator, typename hashtable<T, Hash, KeyEqual>::const_iterator>
hashtable<T, Hash, KeyEqual>::equal_range_unique(const key_type& key) const {
    const size_type n = hash(key);
    for (node_ptr first = buckets_[n]; first != nullptr; first = first->next) {
        if (is_equal(value_traits::get_key(first->value), key)) {
            if (first->next != nullptr) {
                return mstl::make_pair(M_cit(first), M_cit(first->next));
            }
            // 如果相等的节点是链表的最后一个节点，那么需要找到下一个非空的bucket作为结尾
            for (size_type m = n + 1; m < bucket_size_; ++m) {
                if (buckets_[m] != nullptr) {
                    return mstl::make_pair(M_cit(first), M_cit(buckets_[m]));
                }
            }
            return mstl::make_pair(M_cit(first), cend());
        }
    }
    return mstl::make_pair(cend(), cend());
}

// 返回一个篮子中有多少节点
template<typename T, typename Hash, typename KeyEqual>
typename hashtable<T, Hash, KeyEqual>::size_type
hashtable<T, Hash, KeyEqual>::bucket_size(size_type n) const noexcept {
    size_type result = 0;
    for (node_ptr cur = buckets_[n]; cur != nullptr; cur = cur->next) {
        ++result;
    }
    return result;
}

// rehash分为两种情况，扩容和缩容
template<typename T, typename Hash, typename KeyEqual>
void hashtable<T, Hash, KeyEqual>::rehash(size_type count) {
    size_type n = ht_next_prime(count);
    // n > bucket_size_需要扩容
    if (n > bucket_size_) {
        replace_bucket(n);
    } else {
        // n <= bucket_size_判断一下是否值得缩容
        // 如果本来元素已经够多，缩容后会导致容量马上不够需要扩容，
        // 这样的缩容没有意义
        if ((float)size_ / (float)n < max_load_factor() - 0.25f &&
            (float)n < (float)bucket_size_ * 0.75f) {
            replace_bucket(n);
        }
    }
}

template<typename T, typename Hash, typename KeyEqual>
void hashtable<T, Hash, KeyEqual>::init(size_type n) {
    const size_type bucket_nums = next_size(n);
    try {
        // 注意vector扩容后大小不一定为bucket_nums
        buckets_.reserve(bucket_nums);
        buckets_.assign(bucket_nums, nullptr);
    } catch(...) {
        bucket_size_ = 0;
        size_ = 0;
        throw;
    }
    bucket_size_ = buckets_.size();
}

template<typename T, typename Hash, typename KeyEqual>
void hashtable<T, Hash, KeyEqual>::copy_init(const hashtable& ht) {
    bucket_size_ = 0;
    buckets_.reserve(ht.bucket_size_);
    buckets_.assign(ht.bucket_size_, nullptr);
    try {
        for (int i = 0; i < ht.bucket_size_; ++i) {
            node_ptr cur = ht.buckets_[i];
            if (cur != nullptr) {
                node_ptr copy = create_node(cur->value);
                buckets_[i] = copy;
                for (node_ptr next = cur->next; next != nullptr; cur = next, next = cur->next) {
                    copy->next = create_node(next->value);
                    copy = copy->next;
                }
                copy->next = nullptr;
            }
        }
        bucket_size_ = ht.bucket_size_;
        mlf_ = ht.mlf_;
        size_ = ht.size_;
    } catch (...) {
        clear();
    }
}

template<typename T, typename Hash, typename KeyEqual>
template<typename ...Args>
typename hashtable<T, Hash, KeyEqual>::node_ptr 
hashtable<T, Hash, KeyEqual>::create_node(Args&& ...args) {
    node_ptr temp = node_allocator::allocate(1);
    try {
        data_allocator::construct(mstl::address_of(temp->value), mstl::forward<Args>(args)...);
        temp->next = nullptr;

    } catch(...) {
        node_allocator::deallocate(temp);
        throw;
    }
    return temp;
}

template<typename T, typename Hash, typename KeyEqual>
void hashtable<T, Hash, KeyEqual>::destory_node(node_ptr node) {
    data_allocator::destory(mstl::address_of(node->value));
    node_allocator::deallocate(node);
    node = nullptr;
}

// 找到大于n的下一个bucket大小
template<typename T, typename Hash, typename KeyEqual>
typename hashtable<T, Hash, KeyEqual>::size_type
hashtable<T, Hash, KeyEqual>::next_size(size_type n) const {
    return ht_next_prime(n);
}

template<typename T, typename Hash, typename KeyEqual>
typename hashtable<T, Hash, KeyEqual>::size_type
hashtable<T, Hash, KeyEqual>::hash(const key_type& key) const {
    return hash_(key) % bucket_size_;
}

template<typename T, typename Hash, typename KeyEqual>
typename hashtable<T, Hash, KeyEqual>::size_type
hashtable<T, Hash, KeyEqual>::hash(const key_type& key, size_type n) const {
    return hash_(key) % n;
}

template<typename T, typename Hash, typename KeyEqual>
void hashtable<T, Hash, KeyEqual>::rehash_if_need(size_type n) {
    if (static_cast<float>(size_ + n) > (float)bucket_size_ * max_load_factor()) {
        rehash(size_ + n);
    }
}

template<typename T, typename Hash, typename KeyEqual>
template<typename InputIter>
void hashtable<T, Hash, KeyEqual>::copy_insert_multi(InputIter first, InputIter last, mstl::input_iterator_tag) {
    rehash_if_need(mstl::distance(first, last));
    for (; first != last; ++first) {
        insert_multi_noresize(*first);
    }
}

template<typename T, typename Hash, typename KeyEqual>
template<typename forwardIter>
void hashtable<T, Hash, KeyEqual>::copy_insert_multi(forwardIter first, forwardIter last, mstl::forward_iterator_tag) {
    const size_type n = mstl::distance(first, last);
    rehash_if_need(n);
    for (; n > 0; --n, ++first) {
        insert_multi_noresize(*first);
    }
}

template<typename T, typename Hash, typename KeyEqual>
template<typename InputIter>
void hashtable<T, Hash, KeyEqual>::copy_insert_unqiue(InputIter first, InputIter last, mstl::input_iterator_tag) {
    rehash_if_need(mstl::distance(first, last));
    for (; first != last; ++first) {
        insert_unique_noresize(*first);
    }
}

template<typename T, typename Hash, typename KeyEqual>
template<typename forwardIter>
void hashtable<T, Hash, KeyEqual>::copy_insert_unqiue(forwardIter first, forwardIter last, mstl::forward_iterator_tag) {
    const size_type n = mstl::distance(first, last);
    rehash_if_need(n);
    for (; n > 0; --n, ++first) {
        insert_unique_noresize(*first);
    }
}

template<typename T, typename Hash, typename KeyEqual>
typename hashtable<T, Hash, KeyEqual>::iterator
hashtable<T, Hash, KeyEqual>::insert_node_multi(node_ptr np) {
    const size_type n = hash(value_traits::get_key(np->value));
    node_ptr cur = buckets_[n];
    if (cur == nullptr) {
        buckets_[n] = np;
        ++size_;
        return iterator(np, this);
    }
    for (; cur != nullptr; cur = cur->next) {
        if (is_equal(value_traits::get_key(cur->value), value_traits::get_key(np->value))) {
            np->next = cur->next;
            cur->next = np;
            ++size_;
            return iterator(np, this);
        }
    }
    np->next = buckets_[n];
    buckets_[n] = np;
    ++size_;
    return iterator(np, this);
}

template<typename T, typename Hash, typename KeyEqual>
pair<typename hashtable<T, Hash, KeyEqual>::iterator, bool>
hashtable<T, Hash, KeyEqual>::insert_node_unique(node_ptr np) {
    const size_type n = hash(value_traits::get_key(np->value));
    node_ptr cur = buckets_[n];
    if (cur == nullptr) {
        buckets_[n] = np;
        ++size_;
        return mstl::make_pair(iterator(np, this), true);
    }
    for (; cur != nullptr; cur = cur->next) {
        if (is_equal(value_traits::get_key(cur->value), value_traits::get_key(np->value))) {
            // 重复则不插入
            return mstl::make_pair(iterator(np, this), false);
        }
    }
    np->next = buckets_[n];
    buckets_[n] = np;
    ++size_;
    return mstl::make_pair(iterator(np, this), true);
}

template<typename T, typename Hash, typename KeyEqual>
void hashtable<T, Hash, KeyEqual>::replace_bucket(size_type bucket_count) {
    bucket_type bucket(bucket_count);
    if (size_ != 0) {
        for (size_type i = 0; i < bucket_size_; ++i) {
            for (node_ptr first = buckets_[i]; first != nullptr; first = first->next) {
                node_ptr temp = create_node(first->value);
                const size_type n = hash(value_traits::get_key(first->value), bucket_count);
                node_ptr f = bucket[n];
                bool is_insert = false;
                for (node_ptr cur = f; cur != nullptr; cur = cur->next) {
                    // 键值相同则插入到相同节点后面
                    if (is_equal(value_traits::get_key(cur->value), value_traits::get_key(first->value))) {
                        temp->next = cur->next;
                        cur->next = temp;
                        is_insert = true;
                        break;
                    }
                }
                // 没有重复则头插法
                if (!is_insert) {
                    temp->next = f;
                    bucket[n] = temp;
                }
            }
        }
    }
    buckets_.swap(bucket);
    bucket_size_ = buckets_.size();
}

// 删除第n个bucket(篮子)中[first, last)位置的节点
template<typename T, typename Hash, typename KeyEqual>
void hashtable<T, Hash, KeyEqual>::erase_bucket(size_type n, node_ptr first, node_ptr last) {
    node_ptr cur = buckets_[n];
    if (cur == first) {
        erase_bucket(n, last);
    } else {
        node_ptr next = cur->next;
        while (next != first) {
            cur = next;
            next = cur->next;
        }
        while (next != last) {
            cur->next = next->next;
            destory_node(next);
            next = cur->next;
            --size_;
        }
    }
}

// 删除第n个bucket(篮子)中从开始到last位置的节点
template<typename T, typename Hash, typename KeyEqual>
void hashtable<T, Hash, KeyEqual>::erase_bucket(size_type n, node_ptr last) {
    node_ptr cur = buckets_[n];
    while (cur != last) {
        node_ptr next = cur->next;
        destory_node(cur);
        cur = next;
        --size_;
    }
    buckets_[n] = last;
}

// 判断两个hashtable是否相同
template<typename T, typename Hash, typename KeyEqual>
bool hashtable<T, Hash, KeyEqual>::equal_to_multi(const hashtable& other) const {
    if (size_ != other.size_) {
        return false;
    }
    for (iterator f = begin(); f != end();) {
        pair<iterator, iterator> p1 = equal_range_multi(value_traits::get_key(*f));
        pair<iterator, iterator> p2 = other.equal_range_multi(value_traits::get_key(*f));
        // 判断键值是否相同，如果value为pair类型，那么键值相等仅为pair::first相同，
        // 还需要进一步判断pair::second是否相同is_permutation.
        if (mstl::distance(p1.first, p1.second) != mstl::distance(p2.first, p2.second) ||
            !mstl::is_permutation(p1.first, p1.second, p2.first, p2.second)) {
            return false;
        }
        f = p1.second;
    }
    return true;
}

template<typename T, typename Hash, typename KeyEqual>
bool hashtable<T, Hash, KeyEqual>::equal_to_unique(const hashtable& other) const {
    if (size_ != other.size_) {
        return false;
    }
    for (iterator f = begin(); f != end(); ++f) {
        iterator res = other.find(value_traits::get_key(*f));
        if (res.node == nullptr || *f != *res) {
            return false;
        }
    }
    return true;
}

template<typename T, typename Hash, typename KeyEqual>
void swap(hashtable<T, Hash, KeyEqual>& lhs, hashtable<T, Hash, KeyEqual>& rhs) noexcept {
    lhs.swap(rhs);
}

} // mstl

#endif