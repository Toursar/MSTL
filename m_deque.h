#ifndef M_DEQUE_H_
#define M_DEQUE_H_

// 容器deque，以及deque_iterator

#include <initializer_list>

#include "m_iterator.h"
#include "m_memory.h"
#include "m_exceptdef.h"
#include "m_util.h"

namespace mstl {

#ifdef max
#pragma message("#undefing macro max")
#undef max
#endif

#ifdef min
#pragma message("#undefing macro min")
#undef min
#endif

#ifndef DEQUE_MAP_INIT_SIZE
#define DEQUE_MAP_INIT_SIZE 8
#endif

template<typename T>
struct deque_buf_size {
    static constexpr size_t value = sizeof(T) < 256 ? 4096 / sizeof(T) : 16;
};

// deque的迭代器定义
// 继承iterator类，并且给定iterator的类型为random_access_iterator
template<typename T, typename Ref, typename Ptr>
class deque_iterator : public iterator<mstl::random_access_iterator_tag, T> {
public:
    typedef deque_iterator<T, T&, T*>               iterator;
    typedef deque_iterator<T, const T&, const T*>   const_iterator;
    typedef deque_iterator                          self;

    typedef T           value_type;
    typedef Ref         reference;
    typedef Ptr         pointer;
    typedef size_t      size_type;
    typedef ptrdiff_t   difference_type;
    typedef T*          value_pointer;
    typedef T**         map_pointer;

    static const size_type buffer_size = mstl::deque_buf_size<T>::value;

    // 迭代器的数据
    value_pointer cur;      // 指向当前缓冲区元素的位置
    value_pointer first;    // 指向当前缓冲区的头
    value_pointer last;     // 指向当前缓冲区的尾
    map_pointer   node;     // 指向当前的缓冲区

    // 构造，拷贝赋值函数
    deque_iterator() noexcept
        : cur(nullptr), first(nullptr), last(nullptr), node(nullptr) {}
    deque_iterator(value_pointer v, map_pointer n) 
        : cur(v), first(*n), last(*n + buffer_size), node(n) {}
    deque_iterator(const iterator& rhs) 
        : cur(rhs.cur), first(rhs.first), last(rhs.last), node(rhs.node) {}
    deque_iterator(iterator&& rhs) noexcept
        : cur(rhs.cur), first(rhs.first), last(rhs.last), node(rhs.node) {
        rhs.cur = nullptr;
        rhs.first = nullptr;
        rhs.last = nullptr;
        rhs.node = nullptr;
    }
    deque_iterator(const const_iterator& rhs) 
        : cur(rhs.cur), first(rhs.first), last(rhs.last), node(rhs.node) {}

    self& operator=(const iterator& rhs) {
        if (this == &rhs) {
            return *this;
        }
        cur = rhs.cur;
        first = rhs.first;
        last = rhs.last;
        node = rhs.node;
        return *this;
    }
    // 转换所在的缓存区
    void set_node(map_pointer new_node) {
        node = new_node;
        first = *new_node;
        last = first + buffer_size;
    }
    // 重载*,->运算符，构建pointer-like class
    reference operator*() const {
        return *cur;
    }
    pointer operator->() const {
        return cur;
    }

    // 重载运算符
    difference_type operator-(const self& x) const {
        size_type node_num = node - x.node;
        size_type before = cur - first;
        size_type after = x.cur - x.first;
        return static_cast<difference_type>(buffer_size) * (node - x.node)
                + (cur - first) - (x.cur - x.first);
    }
    // 前++
    self& operator++() {
        ++cur;
        if (cur == last) {
            set_node(node + 1);
            cur = first;
        }
        return *this;
    }
    // 后++
    self& operator++(int) {
        self temp = *this;
        ++(*this);
        return temp;
    }
    // 前--
    self& operator--() {
        if (cur == first) {
            set_node(node - 1);
            cur = last;
        }
        --cur;
        return *this;
    }
    // 后--
    self& operator--(int) {
        self temp = *this;
        --(*this);
        return temp;
    }

    self& operator+=(difference_type n) {
        difference_type offset = n + (cur - first);
        // 先判断是否会超出当前缓存区
        if (offset >= 0 && offset < static_cast<difference_type>(buffer_size)) {
            // 没有超出直接修改即可
            cur += n;
        } else {
            // 超出缓存区，则需要判断下一个缓存区位置
            const auto node_offset = offset > 0 ?
                offset / static_cast<difference_type>(buffer_size):
                -static_cast<difference_type>((-offset - 1) / buffer_size) - 1;
            set_node(node + node_offset);
            cur = first + (offset - node_offset * static_cast<difference_type>(buffer_size));
        }
        return *this;
    }

    self operator+(difference_type n) const {
        self temp = *this;
        return temp += n;
    }
    self& operator-=(difference_type n) {
        return *this += -n;
    }
    self operator-(difference_type n) const {
        self temp = *this;
        return temp -= n;
    }

    reference operator[](size_type n) const {
        return *(*this + n);
    }
    // 重载比较运算符
    bool operator==(const self& rhs) const {
        return cur == rhs.cur;
    }
    bool operator<(const self& rhs) const{
        return node == rhs.node ? (cur < rhs.cur) : (node < rhs.node);
    }
    bool operator!=(const self& rhs) const {
        return !(*this == rhs);
    }
    bool operator>(const self& rhs) const{
        return rhs < *this;
    }
    bool operator<=(const self& rhs) const{
        return !(rhs < *this);
    }
    bool operator>=(const self& rhs) const{
        return !(*this < rhs);
    }

};

template<typename T>
class deque {
public:
    typedef mstl::allocator<T>                    allocator_type;
    typedef mstl::allocator<T>                    data_allocator;
    typedef mstl::allocator<T*>                   map_allocator;

    typedef typename allocator_type::value_type            value_type;
    typedef typename allocator_type::pointer               pointer;
    typedef typename allocator_type::const_pointer         const_pointer;
    typedef typename allocator_type::reference             reference;
    typedef typename allocator_type::const_reference       const_reference;
    typedef typename allocator_type::size_type             size_type;
    typedef typename allocator_type::difference_type       difference_type;
    typedef pointer*                              map_pointer;
    typedef const_pointer*                        const_map_pointer;

    typedef deque_iterator<T, T&, T*>             iterator;
    typedef deque_iterator<T, const T&, const T*> const_iterator;
    typedef mstl::reverse_iterator<iterator>            reverse_iterator;
    typedef mstl::reverse_iterator<const_iterator>      const_reverse_iterator;

    allocator_type get_allocator() {
        return allocator_type();
    }
    static const size_type buffer_size = deque_buf_size<T>::value;

// deque中的实际数据
private:
    iterator begin_;        // 指向第一个节点
    iterator end_;          // 指向末尾节点
    map_pointer map_;       // 指向map数组，map数组为一个指针数组，其中每一个指针指向一块缓冲区
    size_type map_size_;    // map数组的大小
public:
    // 构造赋值移动析构函数
    deque() {
        fill_init(0, value_type());
    }
    explicit deque(size_type n) {
        fill_init(n, value_type());
    }
    deque(size_type n, const value_type& value) {
        fill_init(n, value);
    }
    template<typename InputIterator, typename mstl::enable_if<
            mstl::is_input_iterator<InputIterator>::value, int>::type = 0>
    deque(InputIterator first, InputIterator last) {
        copy_init(first, last, iterator_category(first));
    }
    deque(std::initializer_list<value_type> ilist) {
        copy_init(ilist.begin(), ilist.end(), mstl::forward_iterator_tag());
    }
    deque(const deque& rhs) {
        copy_init(rhs.begin(), rhs.end(), mstl::forward_iterator_tag());
    }
    deque(deque&& rhs) noexcept
        : begin_(mstl::move(rhs.begin_)), end_(mstl::move(rhs.end_)), 
          map_(rhs.map_), map_size_(rhs.map_size_) {
        rhs.map_ = nullptr;
        rhs.map_size_ = 0;
    }

    deque& operator=(const deque& rhs);
    deque& operator=(deque&& rhs);
    deque& operator=(std::initializer_list<value_type> ilist) {
        deque temp(ilist);
        swap(temp);
        return *this;
    }

    ~deque() {
        if (map_ != nullptr) {
            clear();
            // clear函数已经将除了头部节点外的其他节点全部释放
            // 下面的操作仅为释放头部元素
            data_allocator::deallocate(*begin_.node, buffer_size);
            *begin_.node = nullptr;
            map_allocator::deallocate(map_, map_size_);
            map_ = nullptr;
        }
    }
// iterator的操作
public:
    iterator begin() noexcept {
        return begin_;
    }
    const_iterator begin() const noexcept {
        return begin_;
    }
    iterator end() noexcept {
        return end_;
    }
    const_iterator end() const noexcept {
        return end_;
    }
    reverse_iterator rbegin() noexcept {
        return reverse_iterator(end());
    }
    const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(end());
    }
    reverse_iterator rend() noexcept {
        return reverse_iterator(begin());
    }
    const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(begin());
    }
    const_iterator cbegin() const noexcept {
        return begin();
    }
    const_iterator cend() const noexcept {
        return end();
    }
    const_reverse_iterator crbegin() const noexcept {
        return rbegin();
    }
    const_reverse_iterator crend() const noexcept {
        return rend();
    }
    // 容器容量相关操作
    bool empty() const {
        return begin_ == end_;
    }
    size_type size() const {
        return end_ - begin_;
    }
    size_type max_size() const {
        return static_cast<size_type>(-1);
    }
    void resize(size_type new_size) {
        resize(new_size, value_type());
    }
    void resize(size_type new_size, const value_type& value);
    void shrink_to_fit() noexcept;

    reference operator[](size_type n) {
        MSTL_DEBUG(n < size());
        return begin_[n];
    }
    const_reference operator[](size_type n) const {
        MSTL_DEBUG(n < size());
        return begin_[n];
    }
    reference at(size_type n) {
        THROW_OUT_OF_RANGE_IF(n >= size(), "deque<T>::at() subscript out of range");
        return (*this)[n];
    }
    const_reference at(size_type n) const {
        THROW_OUT_OF_RANGE_IF(n >= size(), "deque<T>::at() subscript out of range");
        return (*this)[n];
    }
    reference front() {
        MSTL_DEBUG(!empty());
        return *begin_;
    }
    const_reference front() const {
        MSTL_DEBUG(!empty());
        return *begin_;
    }
    reference back() {
        MSTL_DEBUG(!empty());
        return *(end_ - 1);
    }
    const_reference back() const {
        MSTL_DEBUG(!empty());
        return *(end_ - 1);
    }

    // 修改容器操作
    void assign(size_type n, const value_type& value) {
        fill_assign(n, value);
    }

    template<typename InputIterator, typename mstl::enable_if<
            mstl::is_input_iterator<InputIterator>::value, int>::type = 0>
    void assign(InputIterator first, InputIterator last) {
        copy_assign(first, last, mstl::iterator_category(first));
    }
    void assign(std::initializer_list<value_type> ilist) {
        copy_assign(ilist.begin(), ilist.end(), mstl::forward_iterator_tag());
    }

    // emplace方法
    template<typename ...Args>
    void emplace_front(Args&& ...args);

    template<typename ...Args>
    void emplace_back(Args&& ...args);

    template<typename ...Args>
    iterator emplace(iterator postion ,Args&& ...args);

    // push方法
    void push_back(const value_type& value);
    void push_front(const value_type& value);
    void push_front(value_type&& value) {
        emplace_front(mstl::move(value));
    }
    void push_back(value_type&& value) {
        emplace_back(mstl::move(value));
    }
    void pop_back();
    void pop_front();

    // insert方法
    iterator insert(iterator position, const value_type& value);
    iterator insert(iterator position, value_type&& value);
    void insert(iterator postion, size_type n, const value_type& value);

    template<typename InputIterator, typename mstl::enable_if<
            mstl::is_input_iterator<InputIterator>::value, int>::type = 0>
    void insert(iterator position, InputIterator first, InputIterator last) {
        insert_dispatch(position, first, last, mstl::iterator_category(first));
    }

    iterator erase(iterator position);
    iterator erase(iterator first, iterator last);

    void clear();

    void swap(deque& rhs) noexcept;

// 辅助函数
private:
    map_pointer create_map(size_type n);
    void create_buffer(map_pointer nstart, map_pointer nfinish);
    void destory_buffer(map_pointer nstart, map_pointer nfinish);

    void map_init(size_type nElem);
    void fill_init(size_type n, const value_type& value);
    template<typename InputIterator>
    void copy_init(InputIterator first, InputIterator last, mstl::input_iterator_tag);
    template<typename ForwardIterator>
    void copy_init(ForwardIterator first, ForwardIterator last, mstl::forward_iterator_tag);

    void fill_assign(size_type n, const value_type& value);
    template<typename InputIterator>
    void copy_assign(InputIterator first, InputIterator last, mstl::input_iterator_tag);
    template<typename forwardIterator>
    void copy_assign(forwardIterator first, forwardIterator last, mstl::forward_iterator_tag);

    template<typename... Args>
    iterator insert_aux(iterator position, Args&& ...args);
    void fill_insert(iterator position, size_type n, const value_type& x);
    template<typename ForwardIterator>
    void copy_insert(iterator pos, ForwardIterator first, ForwardIterator last, size_type n);
    template<typename InputIterator>
    void insert_dispatch(iterator pos, InputIterator first, InputIterator last,
                        mstl::input_iterator_tag);
    template<typename ForwardIterator>
    void insert_dispatch(iterator pos, ForwardIterator first, ForwardIterator last,
                        mstl::forward_iterator_tag);

    void require_capacity(size_type n, bool front);
    void reallocate_map_at_front(size_type need_size);
    void reallocate_map_at_back(size_type need_size);
};

template<typename T>
deque<T>& deque<T>::operator=(const deque& rhs) {
    if (this != &rhs) {
        const size_type len = size();
        if (len >= rhs.size()) {
            erase(mstl::copy(rhs.begin_, rhs.end_, begin_), end_);
        } else {
            iterator mid = rhs.begin_ + static_cast<difference_type>(len);
            mstl::copy(rhs.begin_, mid, begin_);
            insert(end_, mid, rhs.end_);
        }
    }
    return *this;
}

template<typename T>
deque<T>& deque<T>::operator=(deque&& rhs) {
    clear();
    begin_ = mstl::move(rhs.begin_);
    end_ = mstl::move(rhs.end_);
    map_ = rhs.map_;
    map_size_ = rhs.map_size_;
    rhs.map_ = nullptr;
    rhs.map_size_ = 0;
    return *this;
}

template<typename T>
void deque<T>::resize(size_type new_size, const value_type& value) {
    const size_type len = size();
    if (new_size < size()) {
        erase(begin_ + new_size, end_);
    } else {
        insert(end_, new_size - len, value);
    }
}

template<typename T>
void deque<T>::shrink_to_fit() noexcept {
    for (map_pointer cur = map_; cur < begin_.node; ++cur) {
        data_allocator::deallocate(*cur, buffer_size);
        *cur = nullptr;
    }
    for (map_pointer cur = end_.node + 1; cur < map_ + map_size_; ++cur) {
        data_allocator::deallocate(*cur, buffer_size);
        *cur = nullptr;
    }
}

template<typename T>
template<typename ...Args>
void deque<T>::emplace_front(Args&& ...args) {
    if (begin_.cur != begin_.first) {
        data_allocator::construct(begin_.cur - 1, mstl::forward<Args>(args)...);
        --begin_.cur;
    } else {
        require_capacity(1, true);
        try {
            --begin_;
            data_allocator::construct(begin_.cur, mstl::forward<Args>(args)...);
        } catch (...) {
            ++begin_;
            throw;
        }
    }
}

template<typename T>
template<typename ...Args>
void deque<T>::emplace_back(Args&& ...args) {
    if (end_.cur != end_.last - 1) {
        data_allocator::construct(end_.cur, mstl::forward<Args>(args)...);
        ++end_.cur;
    } else {
        require_capacity(1, false);
        data_allocator::construct(end_.cur, mstl::forward<Args>(args)...);
        ++end_;

    }
}

template<typename T>
template<typename ...Args>
typename deque<T>::iterator
deque<T>::emplace(iterator position, Args&& ...args) {
    if (position.cur == begin_.cur) {
        emplace_front(mstl::forward<Args>(args)...);
        return begin_;
    } else if (position.cur == end_.cur) {
        emplace_back(mstl::forward<Args>(args)...);
        return end_;
    }
    return insert_aux(position, mstl::forward<Args>(args)...);
}

template<typename T>
void deque<T>::push_front(const value_type& value) {
    if (begin_.cur != begin_.first) {
        data_allocator::construct(begin_.cur - 1, value);
        --begin_.cur;
    } else {
        require_capacity(1, true);
        try {
            --begin_;
            data_allocator::construct(begin_.cur, value);
        } catch (...) {
            ++begin_;
            throw;
        }
    }
}

template<typename T>
void deque<T>::push_back(const value_type& value) {
    if (end_.cur != end_.last - 1) {
        data_allocator::construct(end_.cur, value);
        ++end_.cur;
    } else {
        require_capacity(1, false);
        data_allocator::construct(end_.cur, value);
        ++end_;
    }
}

template<typename T>
void deque<T>::pop_front() {
    MSTL_DEBUG(!empty());
    if (begin_.cur != begin_.last - 1) {
        data_allocator::destory(begin_.cur);
        ++begin_.cur;
    } else {
        data_allocator::destory(begin_.cur);
        ++begin_;
        destory_buffer(begin_.node - 1, begin_.node - 1);
    }
}

template<typename T>
void deque<T>::pop_back() {
    MSTL_DEBUG(!empty());
    if (end_.cur != end_.first) {
        --end_.cur;
        data_allocator::destory(end_.cur);
    } else {
        --end_;
        data_allocator::destory(end_.cur);
        destory_buffer(end_.node + 1, end_.node + 1);
    }
}

template<typename T>
typename deque<T>::iterator
deque<T>::insert(iterator position, const value_type& value) {
    if (position.cur == begin_.cur) {
        push_front(value);
        return begin_;
    } else if (position.cur == end_.cur) {
        push_back(value);
        return end_ - 1;
    } else {
        insert_aux(position, value);
    }
}

template<typename T>
typename deque<T>::iterator
deque<T>::insert(iterator position, value_type&& value) {
    if (position.cur == begin_.cur) {
        push_front(mstl::move(value));
        return begin_;
    } else if (position.cur == end_.cur) {
        push_back(mstl::move(value));
        return end_ - 1;
    } else {
        return insert_aux(position, mstl::move(value));
    }
}

template<typename T>
void deque<T>::insert(iterator position, size_type n, const value_type& value) {
    if (position.cur == begin_.cur) {
        require_capacity(n, true);
        iterator new_begin = begin_ - n;
        mstl::uninitialized_fill_n(new_begin, n, value);
        begin_ = new_begin;
    } else if (position.cur == end_.cur) {
        require_capacity(n, false);
        iterator new_end = end_ + n;
        mstl::uninitialized_fill_n(end_, n, value);
        end_ = new_end;
    } else {
        fill_insert(position, n, value);
    }
}

template<typename T>
typename deque<T>::iterator
deque<T>::erase(iterator position) {
    iterator next = position;
    ++next;
    const size_type elem_before = position - begin_;
    if (elem_before < size() / 2) {
        mstl::copy_backward(begin_, position, next);
        pop_front();
    } else {
        mstl::copy(next, end_, position);
        pop_back();
    }
    return begin_ + elem_before;
}

template<typename T>
typename deque<T>::iterator
deque<T>::erase(iterator first, iterator last) {
    if (first == begin_ && last == end_) {
        clear();
        return end_;
    } else {
        const size_type len = mstl::distance(first, last);
        const size_type elem_before = first - begin_;
        if (elem_before < ((size() - len) / 2)) {
            mstl::copy_backward(begin_, first, last);
            iterator new_begin = begin_ + len;
            data_allocator::destory(begin_.cur, new_begin.cur);
            begin_ = new_begin;
        } else {
            mstl::copy(last, end_, first);
            iterator new_end = end_ - len;
            data_allocator::destory(new_end.cur, end_.cur);
            end_ = new_end;
        }
        return begin_ + elem_before;
    }
}

template<typename T>
void deque<T>::clear() {
    // 保留了头部缓冲区
    for (map_pointer cur = begin_.node + 1; cur < end_.node; ++cur) {
        data_allocator::destory(*cur, *cur + buffer_size);
    }
    if (begin_.node != end_.node) {
        data_allocator::destory(begin_.cur, begin_.last);
        data_allocator::destory(end_.first, end_.cur);
    } else {
        data_allocator::destory(begin_.cur, end_.cur);
    }
    shrink_to_fit();
    end_ = begin_;
}

template<typename T>
void deque<T>::swap(deque& rhs) noexcept {
    mstl::swap(begin_, rhs.begin_);
    mstl::swap(end_, rhs.end_);
    mstl::swap(map_, rhs.map_);
    mstl::swap(map_size_, rhs.map_size_);
}

/********************************************************************************************/
// 辅助函数
// 创建size个元素的缓冲区指针数组
template<typename T>
typename deque<T>::map_pointer
deque<T>::create_map(size_type size) {
    map_pointer map = nullptr;
    map = map_allocator::allocate(size);
    for (size_type i = 0; i < size; ++i) {
        *(map + i) = nullptr;
    }
    return map;
}

// nstart-nfinish范围的缓冲区分配内存
template<typename T>
void deque<T>::create_buffer(map_pointer nstart, map_pointer nfinish) {
    map_pointer cur;
    try {
        for (cur = nstart; cur <= nfinish; ++cur) {
            *cur = data_allocator::allocate(buffer_size);
        }
    } catch (...) {
        while (cur != nstart) {
            --cur;
            data_allocator::deallocate(*cur, buffer_size);
            cur = nullptr;
        }
        throw;
    }
}

// 回收nstart-nfinish范围缓冲区的内存
template<typename T>
void deque<T>::destory_buffer(map_pointer nstart, map_pointer nfinish) {
    for (map_pointer p = nstart; p <= nfinish; ++p) {
        data_allocator::deallocate(*p, buffer_size);
        *p = nullptr;
    }
}

// 创建可以容纳nElen个元素的deque容器
template<typename T>
void deque<T>::map_init(size_type nElem) {
    const size_type nNode = nElem / buffer_size + 1;
    map_size_ = mstl::max(static_cast<size_type>(DEQUE_MAP_INIT_SIZE), nNode + 2);
    try {
        map_ = create_map(map_size_);
    } catch (...) {
        map_ = nullptr;
        map_size_ = 0;
        throw;
    }

    // 优先从中间开始分配内存，方便向两边扩展
    map_pointer nstart = map_ + (map_size_ - nNode) / 2;
    map_pointer nfinish = nstart + nNode - 1;
    try {
        create_buffer(nstart, nfinish);
    } catch (...) {
        map_allocator::deallocate(map_, map_size_);
        map_ = nullptr;
        map_size_ = 0;
        throw;
    }
    begin_.set_node(nstart);
    end_.set_node(nfinish);
    begin_.cur = begin_.first;
    end_.cur = end_.first + (nElem % buffer_size);
}

template<typename T>
void deque<T>::fill_init(size_type n, const value_type& value) {
    map_init(n);
    if (n == 0) {
        return;
    }
    for (auto cur = begin_.node; cur < end_.node; ++cur) {
        uninitialized_fill(*cur, *cur + buffer_size, value);
    }
    uninitialized_fill(end_.first, end_.cur, value);
}

template<typename T>
template<typename InputIterator>
void deque<T>::copy_init(InputIterator first, InputIterator last, mstl::input_iterator_tag) {
    const size_type n = mstl::distance(first, last);
    map_init(n);
    for (;first != last; ++first) {
        emplace_back(*first);
    }
}

template<typename T>
template<typename ForwardIterator>
void deque<T>::copy_init(ForwardIterator first, ForwardIterator last,
                mstl::forward_iterator_tag) {
    const size_type n = mstl::distance(first, last);
    map_init(n);
    for (map_pointer cur = begin_.node; cur < end_.node; ++cur) {
        ForwardIterator next = first;
        mstl::advance(next, buffer_size);
        mstl::uninitialized_copy(first, next, *cur);
        first = next;
    }
    mstl::uninitialized_copy(first, last, end_.first);
}

template<typename T>
void deque<T>::fill_assign(size_type n, const value_type& value) {
    if (n > size()) {
        mstl::fill(begin_, end_, value);
        insert(end_, n - size(), value);
    } else {
        erase(begin_ + n, end_);
        mstl::fill(begin_, end_, value);
    }
}

template<typename T>
template<typename InputIterator>
void deque<T>::copy_assign(InputIterator first, InputIterator last, mstl::input_iterator_tag) {
    iterator first1 = begin_;
    iterator last1 = end_;
    for (; first != last && first1 != last1; ++first1, ++first) {
        *first1 = first;
    }
    if (first1 != last1) {
        erase(first1, last1);
    } else {
        insert_dispatch(end_, first, last, input_iterator_tag());
    }
}

template<typename T>
template<typename forwardIterator>
void deque<T>::copy_assign(forwardIterator first, forwardIterator last, mstl::forward_iterator_tag) {
    const size_type len1 = size();
    const size_type len2 = mstl::distance(first, last);
    if (len1 < len2) {
        forwardIterator next = first;
        mstl::advance(next, len1);
        mstl::uninitialized_copy(first, next, begin_);
        insert_dispatch(end_, next, last, mstl::forward_iterator_tag());
    } else {
        erase(mstl::copy(first, last, begin_), end_);
    }
}

// position位置(非头尾)插入一个元素
template<typename T>
template<typename... Args>
typename deque<T>::iterator deque<T>::insert_aux(iterator position, Args&& ...args) {
    const size_type elem_before = position - begin_;
    value_type value_copy = value_type(mstl::forward<Args>(args)...);
    if (elem_before < (size() / 2)) {
        emplace_front(front());
        // 如果上一步的emplace_front导致扩容，那么map数组的原内存失效
        // 之前传递的position位置已经无效, position的位置需要重新确定
        position = begin_ + elem_before;
        iterator pos = position;
        ++pos;
        mstl::copy(begin_ + 2, pos, begin_ + 1);
    } else {
        emplace_back(back());
        // 此步骤同理
        position = begin_ + elem_before;
        mstl::copy_backward(position, end_ - 2, end_ - 1);
    }
    *position = mstl::move(value_copy);
    return position;
}

// position位置插入n个元素
template<typename T>
void deque<T>::fill_insert(iterator position, size_type n, const value_type& x) {
    const size_type elem_before = position - begin_;
    const size_type len = size();
    const value_type value_copy = x;
    if (elem_before < len / 2) {
        require_capacity(n, true);
        iterator old_begin = begin_;
        iterator new_begin = begin_ - n;
        // 同样position很可能失效，需要重新确定位置
        position = begin_ + elem_before;
        try {
            if (elem_before >= n) {
                mstl::uninitialized_copy(begin_, begin_ + n, new_begin);
                mstl::copy(begin_ + n, position, old_begin);
                mstl::fill(position - n, position, value_copy);
                begin_ = new_begin;
            } else {
                mstl::uninitialized_fill(
                mstl::uninitialized_copy(old_begin, position, new_begin), begin_, value_copy);
                begin_ = new_begin;
                mstl::fill(old_begin, position, value_copy);
            }
        } catch (...) {
            if (new_begin.node != begin_.node) {
                destory_buffer(new_begin.node, begin_.node - 1);
            }
            throw;
        }
    } else {
        require_capacity(n, false);
        iterator old_end = end_;
        iterator new_end = end_ + n;
        const size_type elem_after = len - elem_before;
        position = end_ - elem_after;
        try {
            // 此处判断为>而非>=，原因是end位置不会存放元素，这点与begin位置不同
            if (elem_after > n) {
                iterator end_n =  end_ - n;
                mstl::uninitialized_copy(end_n, end_, end_);
                end_ = new_end;
                mstl::copy_backward(position, end_n, old_end);
                mstl::fill(position, position + n, value_copy);
            } else {
                mstl::uninitialized_fill(end_, position + n, value_copy);
                mstl::uninitialized_copy(position, end_, position + n);
                end_ = new_end;
                mstl::fill(position, old_end, value_copy);
            }
        } catch (...) {
            if (new_end != end_) {
                destory_buffer(end_.node + 1, new_end.node);
            }
            throw;
        }
    }
}

// 拷贝[first, last)中的n个元素到postion位置
template<typename T>
template<typename ForwardIterator>
void deque<T>::copy_insert(iterator position, ForwardIterator first, ForwardIterator last, size_type n) {
        const size_type elem_before = position - begin_;
    const size_type len = size();
    if (elem_before < len / 2) {
        require_capacity(n, true);
        iterator old_begin = begin_;
        iterator new_begin = begin_ - n;
        // 同样position很可能失效，需要重新确定位置
        position = begin_ + elem_before;
        try {
            if (elem_before >= n) {
                mstl::uninitialized_copy(begin_, begin_ + n, new_begin);
                mstl::copy(begin_ + n, position, old_begin);
                mstl::copy(first, last, position - n);
                begin_ = new_begin;
            } else {
                ForwardIterator mid = first;
                mstl::advance(mid, n - elem_before);
                mstl::uninitialized_copy(first, mid,
                    mstl::uninitialized_copy(begin_, position, new_begin));
                begin_ = new_begin;
                mstl::copy(mid, last, old_begin);
            }
        } catch (...) {
            if (new_begin.node != begin_.node) {
                destory_buffer(new_begin.node, begin_.node - 1);
            }
            throw;
        }
    } else {
        require_capacity(n, false);
        iterator old_end = end_;
        iterator new_end = end_ + n;
        const size_type elem_after = len - elem_before;
        position = end_ - elem_after;
        try {
            // 此处判断为>而非>=，原因是end位置不会存放元素，这点与begin位置不同
            if (elem_after > n) {
                iterator end_n =  end_ - n;
                mstl::uninitialized_copy(end_n, end_, end_);
                end_ = new_end;
                mstl::copy_backward(position, end_n, old_end);
                mstl::copy(first, last, position);
            } else {
                ForwardIterator mid = first;
                mstl::advance(mid, elem_after);
                mstl::uninitialized_copy(position, end_, 
                    mstl::uninitialized_copy(mid, last, end_));
                end_ = new_end;
                mstl::copy(first, mid, position);
            }
        } catch (...) {
            if (new_end != end_) {
                destory_buffer(end_.node + 1, new_end.node);
            }
            throw;
        }
    }
}

template<typename T>
template<typename InputIterator>
void deque<T>::insert_dispatch(iterator position, InputIterator first, InputIterator last,
                    mstl::input_iterator_tag) {
    if (last <= first) {
        return;
    }
    const size_type n = mstl::distance(first, last);
    const size_type elem_before = position - begin_;
    if (elem_before < (size() / 2)) {
        require_capacity(n, true);
    } else {
        require_capacity(n, false);
    }
    position = begin_ + elem_before;
    InputIterator cur = --last;
    for (size_type i = 0; i < n; ++i, --cur) {
        insert(position, *cur);
    }
}

template<typename T>
template<typename ForwardIterator>
void deque<T>::insert_dispatch(iterator position, ForwardIterator first, ForwardIterator last,
                    mstl::forward_iterator_tag) {
    if (last <= first) {
        return;
    }
    const size_type n = mstl::distance(first, last);
    if (position.cur == begin_.cur) {
        require_capacity(n, true);
        iterator new_begin = begin_ - n;
        try {
            mstl::uninitialized_copy(first, last, new_begin);
            begin_ = new_begin;
        } catch (...) {
            if (new_begin.node != begin_.node) {
                destory_buffer(new_begin.node, begin_.node - 1);
            }
            throw;
        }
    } else if (position.cur == end_.cur) {
        require_capacity(n, false);
        iterator new_end = end_ + n;
        try {
            mstl::uninitialized_copy(first, last, end_);
            end_ = new_end;
        } catch (...) {
            if (new_end.node != end_.node) {
                destory_buffer(end_.node + 1, new_end.node);
            }
        }
    } else {
        copy_insert(position, first, last, n);
    }
}

// 在front或back位置申请n的空间
template<typename T>
void deque<T>::require_capacity(size_type n, bool front) {
    if (front && static_cast<size_type>(begin_.cur - begin_.first) < n) {
        const size_type need_buffer = (n - (begin_.cur - begin_.first)) / buffer_size + 1;
        if (need_buffer > static_cast<size_type>(begin_.node - map_)) {
            reallocate_map_at_front(need_buffer);
            return;
        }
        create_buffer(begin_.node - need_buffer, begin_.node - 1);
    } else if (!front && static_cast<size_type>(end_.last - end_.cur - 1) < n) {
        const size_type need_buffer = (n - (end_.last - end_.cur - 1)) / buffer_size + 1;
        if (need_buffer > static_cast<size_type>((map_ + map_size_) - end_.node - 1)) {
            reallocate_map_at_back(need_buffer);
            return;
        }
        create_buffer(end_.node + 1, end_.node + need_buffer);
    }
}

// 为deque在前部扩容
template<typename T>
void deque<T>::reallocate_map_at_front(size_type need_buffer) {
    const size_type new_map_size = mstl::max(map_size_ << 1, 
                    map_size_ + need_buffer + DEQUE_MAP_INIT_SIZE);
    map_pointer new_map = create_map(new_map_size);
    const size_type old_buffer = end_.node - begin_.node + 1;
    const size_type new_buffer = old_buffer + need_buffer;
    map_pointer begin = new_map + (new_map_size - new_buffer) / 2;
    map_pointer mid = begin + need_buffer;
    map_pointer end = mid + old_buffer;
    create_buffer(begin, mid - 1);
    for (map_pointer begin1 = mid, begin2 = begin_.node; begin1 != end; ++begin1, ++begin2) {
        *begin1 = *begin2;
    }
    map_allocator::deallocate(map_, map_size_);
    map_ = new_map;
    map_size_ = new_map_size;
    // 通过map_pointer构造新的头尾迭代器
    begin_ = iterator(*mid + (begin_.cur - begin_.first), mid);
    end_ = iterator(*(end - 1) + (end_.cur - end_.first), end - 1);
}

// 为deque在尾部扩容
template<typename T>
void deque<T>::reallocate_map_at_back(size_type need_buffer) {
        const size_type new_map_size = mstl::max(map_size_ << 1, 
                    map_size_ + need_buffer + DEQUE_MAP_INIT_SIZE);
    map_pointer new_map = create_map(new_map_size);
    const size_type old_buffer = end_.node - begin_.node + 1;
    const size_type new_buffer = old_buffer + need_buffer;
    map_pointer begin = new_map + (new_map_size - new_buffer) / 2;
    map_pointer mid = begin + old_buffer;
    map_pointer end = mid + need_buffer;
    create_buffer(mid, end - 1);
    for (map_pointer begin1 = begin, begin2 = begin_.node; begin1 != mid; ++begin1, ++begin2) {
        *begin1 = *begin2;
    }
    create_buffer(mid, end - 1);
    map_allocator::deallocate(map_, map_size_);
    map_ = new_map;
    map_size_ = new_map_size;
    // 通过map_pointer构造新的头尾迭代器
    begin_ = iterator(*begin + (begin_.cur - begin_.first), begin);
    end_ = iterator(*(mid - 1) + (end_.cur - end_.first), mid - 1);
}

template<typename T>
bool operator==(const deque<T>& lhs, const deque<T>& rhs) {
    return lhs.size() == rhs.size() && mstl::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template<typename T>
bool operator<(const deque<T>& lhs, const deque<T>& rhs) {
    return mstl::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template<typename T>
bool operator!=(const deque<T>& lhs, const deque<T>& rhs) {
    return !(lhs == rhs);
}

template<typename T>
bool operator>(const deque<T>& lhs, const deque<T>& rhs) {
    return rhs < lhs;
}

template<typename T>
bool operator<=(const deque<T>& lhs, const deque<T>& rhs) {
    return !(rhs < lhs);
}

template<typename T>
bool operator>=(const deque<T>& lhs, const deque<T>& rhs) {
    return !(lhs < rhs);
}

template<typename T>
void swap(deque<T>& lhs, deque<T>& rhs) {
    lhs.swap(rhs);
}

} // mstl

#endif