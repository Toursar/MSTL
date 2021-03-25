#ifndef M_VECTOR_H_
#define M_VECTOR_H_

// 容器vector的实现

#include <initializer_list>
#include "m_iterator.h"
#include "m_memory.h"
#include "m_util.h"
#include "m_exceptdef.h"

namespace mstl {

#ifdef max
#pragma message("#undefing macro max")
#undef max
#endif

#ifdef min
#pragma message("#undefing macro min")
#undef min
#endif

template<typename T>
class vector {
public:
    typedef mstl::allocator<T>  allocator_type;
    typedef mstl::allocator<T>  data_allocator;

    typedef typename allocator_type::value_type         value_type;
    typedef typename allocator_type::pointer            pointer;
    typedef typename allocator_type::const_pointer      const_pointer;
    typedef typename allocator_type::reference          reference;
    typedef typename allocator_type::const_reference    const_reference;
    typedef typename allocator_type::size_type          size_type;
    typedef typename allocator_type::difference_type    difference_type;

    typedef value_type*                                 iterator;
    typedef const value_type*                           const_iterator;
    typedef mstl::reverse_iterator<iterator>            reverse_iterator;
    typedef mstl::reverse_iterator<const_iterator>      const_reverse_iterator;
private:
    iterator begin_; // 使用的空间的头部
    iterator end_;   // 使用的空间的尾部
    iterator cap_;   // 总空间的尾部
public:
    // vector的构造器
    vector() noexcept {
        try_init();
    }

    explicit vector(size_type n) {
        fill_init(n, value_type());
    }

    vector(size_type n, const value_type& value) {
        fill_init(n, value);
    }

    template<typename Iter, typename mstl::enable_if<
            mstl::is_input_iterator<Iter>::value, int>::type = 0>
    vector(Iter first, Iter last) {
        MSTL_DEBUG(!(last < first));
        range_init(first, last);
    }

    vector(const vector& rhs) {
        range_init(rhs.begin_, rhs.end_);
    }

    vector(vector&& rhs) noexcept 
        :begin_(rhs.begin_), end_(rhs.end_), cap_(rhs.cap_) {
        rhs.begin_ = nullptr;
        rhs.end_ = nullptr;
        rhs.cap_ = nullptr;
    }

    vector(std::initializer_list<value_type> ilist) {
        range_init(ilist.begin(), ilist.end());
    }

    vector& operator=(const vector& rhs);
    vector& operator=(vector&& rhs) noexcept;

    vector& operator=(std::initializer_list<value_type> ilist) {
        vector temp(ilist.begin(), ilist.end());
        swap(temp);
        return *this;
    }

    ~vector() {
        destory_and_recover(begin_, end_, cap_ - begin_);
        begin_ = end_ = cap_ = nullptr;
    }
public:
    // iterator操作
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

    // vector容量相关操作
    bool empty() const noexcept {
        return begin_ == end_;
    }

    size_type size() const noexcept {
        return static_cast<size_type>(end_ - begin_);
    }

    size_type max_size() const noexcept {
        return static_cast<size_type>(-1) / sizeof(T);
    }

    size_type capacity() const noexcept {
        return static_cast<size_type>(cap_ - begin_);
    }

    void reserve(size_type n);
    void shrink_to_fit();

    // 元素的访问
    reference operator[](size_type n) {
        MSTL_DEBUG(n < size());
        return *(begin_ + n);
    }

    const_reference operator[](size_type n) const {
        MSTL_DEBUG(n < size());
        return *(begin_ + n);
    }

    reference at(size_type n) {
        THROW_OUT_OF_RANGE_IF(!(n < size()),"vector<T>::at() subscript out of range");
        return (*this)[n];
    }

    const_reference at(size_type n) const {
        THROW_OUT_OF_RANGE_IF(!(n < size()),"vector<T>::at() subscript out of range");
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

    pointer data() noexcept {
        return begin_;
    }

    const_pointer data() const noexcept {
        return begin_;
    }

    // 修改容器操作
    void assign(size_type n, const value_type& value) {
        fill_assign(n, value);
    }

    template<typename Iter, typename mstl::enable_if<
            mstl::is_input_iterator<Iter>::value, int>::type = 0>
    void assign(Iter first, Iter last) {
        MSTL_DEBUG(!(last < first));
        copy_assign(first, last, mstl::iterator_category(first));
    }

    void assign(std::initializer_list<value_type> ilist) {
        copy_assign(ilist.begin(), ilist.end(), mstl::forward_iterator_tag());
    }

    template<typename ...Args>
    iterator emplace(const_iterator pos, Args&&... args);

    template<typename ...Args>
    void emplace_back(Args&&... args);

    void push_back(const value_type& value);
    void push_back(value_type&& value) {
        emplace_back(mstl::move(value));
    }

    void pop_back();

    iterator insert(const_iterator pos, const value_type& value);
    iterator insert(const_iterator pos, value_type&& value) {
        return emplace(pos, mstl::move(value));
    }
    iterator insert(const_iterator pos, size_type n, const value_type& value) {
        MSTL_DEBUG(pos >= begin() && pos <= end());
        return fill_insert(const_cast<iterator>(pos), n, value);
    }

    template<typename Iter, typename mstl::enable_if<
            mstl::is_input_iterator<Iter>::value, int>::type = 0>
    void insert(const_iterator pos, Iter first, Iter last) {
        MSTL_DEBUG(pos >= begin() && pos <= end() && !(last < first));
        copy_insert(const_cast<iterator>(pos), first, last);
    }

    iterator erase(const_iterator pos);
    iterator erase(const_iterator first, const_iterator last);

    void clear() {
        erase(begin(), end());
    }

    void resize(size_type new_size) {
        resize(new_size, value_type());
    }

    void resize(size_type new_size, const value_type& value);

    void reverse() {
        for (auto i = begin(), j = end(); i < j;) {
            mstl::iter_swap(i++, --j);
        }
    }

    void swap(vector& rhs) noexcept;
private:
    // 辅助函数
    void try_init() noexcept;

    void init_space(size_type size, size_type cap);

    void fill_init(size_type n, const value_type& value);

    template<typename Iter>
    void range_init(Iter first, Iter last);

    void destory_and_recover(iterator first, iterator last, size_type n);

    size_type get_new_cap(size_type add_size);

    void fill_assign(size_type n, const value_type& value);

    template<typename InputIterator>
    void copy_assign(InputIterator first, InputIterator last, input_iterator_tag);

    template<typename ForwardIterator>
    void copy_assign(ForwardIterator first, ForwardIterator last, forward_iterator_tag);

    template<typename... Args>
    void reallocate_emplace(iterator pos, Args&& ...args);
    void reallocate_insert(iterator pos, const value_type& value);

    iterator fill_insert(iterator pos, size_type n, const value_type& value);
    template<typename InputIterator>
    void copy_insert(iterator pos, InputIterator first, InputIterator last);

    void reinsert(size_type size);
};

template<typename T>
vector<T>& vector<T>::operator=(const vector& rhs) {
    if (this == &rhs) {
        return *this;
    }
    const size_type len = rhs.size();
    if (len > capacity()) {
        vector temp(rhs.begin(), rhs.end());
        swap(temp);
    } else if (len <= size()) {
        auto i = mstl::copy(rhs.begin(), rhs.end(), begin_);
        data_allocator::destory(i, end_);
        end_ = begin_ + len;
    } else {
        mstl::copy(rhs.begin(), rhs.begin() + size(), begin_);
        mstl::uninitialized_copy(rhs.begin() + size(), rhs.end(), end_);
        cap_ = end_ = begin_ + len;
    }
    return *this;
}

template<typename T>
vector<T>& vector<T>::operator=(vector&& rhs) noexcept {
    destory_and_recover(begin_, end_, cap_ - begin_);
    begin_ = rhs.begin_;
    end_ = rhs.end_;
    cap_ = rhs.cap_;
    rhs.begin_ = nullptr;
    rhs.end_ = nullptr;
    rhs.cap_ = nullptr;
    return *this;
}

// 设置vector的容量
template<typename T>
void vector<T>::reserve(size_type n) {
    if (capacity() > n) {
        return;
    }
    THROW_LENGTH_ERROR_IF(n > max_size(),
            "n can not larger than max_size() in vector<T>::reserve(n)");
    const size_type old_size = size();
    iterator temp = data_allocator::allocate(n);
    mstl::uninitialized_move(begin_, end_, temp);
    data_allocator::deallocate(begin_, cap_ - begin_);
    begin_ = temp;
    end_ = begin_ + old_size;
    cap_ = begin_ + n;
}

template<typename T>
void vector<T>::shrink_to_fit() {
    if (end_ < cap_) {
        reinsert(size());
    }
}

// 在pos位置构造元素，避免额外的复制或移动开销
template<typename T>
template<typename ...Args>
typename vector<T>::iterator
vector<T>::emplace(const_iterator pos, Args&& ...args) {
    MSTL_DEBUG(pos >= begin() && pos <= end());
    iterator xpos = const_cast<iterator>(pos);
    const size_type n = xpos - begin_;
    if (end_ != cap_ && xpos == end_) {
        data_allocator::construct(mstl::address_of(*end_), mstl::forward<Args>(args)...);
        ++end_;
    } else if (end_ != cap_) {
        iterator new_end = end_;
        data_allocator::construct(mstl::address_of(*end_), *(end_ - 1));
        ++new_end;
        mstl::copy_backward(xpos, end_ - 1, end_);
        *xpos = value_type(mstl::forward<Args>(args)...);
        end_ = new_end;
    } else {
        reallocate_emplace(xpos, mstl::forward<Args>(args)...);
    }
    return begin_ + n;
}

// 在尾部构造元素，避免额外的复制或移动开销
template<typename T>
template<typename ...Args>
void vector<T>::emplace_back(Args&& ...args) {
    if (end_ < cap_) {
        data_allocator::construct(mstl::address_of(*end_), mstl::forward<Args>(args)...);
        ++end_;
    } else {
        reallocate_emplace(end_,  mstl::forward<Args>(args)...);
    }
}

// 在尾部插入元素
template<typename T>
void vector<T>::push_back(const value_type& value) {
    if (end_ < cap_) {
        data_allocator::construct(mstl::address_of(*end_), value);
        ++end_;
    } else {
        reallocate_insert(end_,  value);
    }
}

// 弹出尾部元素
template<typename T>
void vector<T>::pop_back() {
    MSTL_DEBUG(!empty());
    data_allocator::destory(end_ - 1);
    --end_;
}

// 在pos位置插入元素value
template<typename T>
typename vector<T>::iterator
vector<T>::insert(const_iterator pos, const value_type& value) {
    MSTL_DEBUG(pos >= begin() && pos <= end());
    iterator xpos = const_cast<iterator>(pos);
    const size_type n = pos - begin_;
    if (end_ != cap_ && xpos == end_) {
        data_allocator::construct(mstl::address_of(*end_), value);
        ++end_;
    } else if (end_ != cap_) {
        iterator new_end = end_;
        data_allocator::construct(mstl::address_of(*end_), *(end_ - 1));
        ++new_end;
        value_type value_copy = value;
        mstl::move_backward(xpos, end_ - 1, end_);
        *xpos = mstl::move(value_copy);
        end_ = new_end;
    } else {
        reallocate_insert(xpos, value);
    }
    return begin_ + n;
}

// 删除pos位置的元素
template<typename T>
typename vector<T>::iterator
vector<T>::erase(const_iterator pos) {
    MSTL_DEBUG(pos >= begin_ && pos <= end_);
    iterator xpos = const_cast<iterator>(pos);
    mstl::move(xpos + 1, end_, xpos);
    data_allocator::destory(end_ - 1);
    --end_;
    return xpos;
}

// 删除[first, last)范围的元素
template<typename T>
typename vector<T>::iterator
vector<T>::erase(const_iterator first, const_iterator last) {
    MSTL_DEBUG(first >= begin() && last <= end() && !(last < first));
    const size_type dist = static_cast<size_type>(first - begin_);
    iterator r = begin_ + dist;
    const size_type n = static_cast<size_type>(last - first);
    data_allocator::destory(mstl::move(r + n, end_, r), end_);
    end_ = end_ - n;
    return begin_ + dist;
}

// 修改容器的size
template<typename T>
void vector<T>::resize(size_type new_size, const value_type& value) {
    if (new_size < size()) {
        erase(begin_ + new_size, end_);
    } else {
        insert(end_, new_size - size(), value);
    }
}

template<typename T>
void vector<T>::swap(vector& rhs) noexcept {
    if (this != &rhs) {
        mstl::swap(begin_, rhs.begin_);
        mstl::swap(end_, rhs.end_);
        mstl::swap(cap_, rhs.cap_);
    }
}

/********************************************************************/
// 辅助函数
template<typename T>
void vector<T>::try_init() noexcept {
    try {
        begin_ = data_allocator::allocate(16);
        end_ = begin_;
        cap_ = begin_ + 16;
    } catch (...) {
        begin_ = nullptr;
        end_ = nullptr;
        cap_ = nullptr;
    }
}

// 初始化vector，容量为cap，大小为size
template<typename T>
void vector<T>::init_space(size_type size, size_type cap) {
    try {
        begin_ = data_allocator::allocate(cap);
        end_ = begin_ + size;
        cap_ = begin_ + cap;
    } catch (...) {
        begin_ = nullptr;
        end_ = nullptr;
        cap_ = nullptr;
    }
}

// 初始化vector,大小为n，值为value，不足16则补至16
template<typename T>
void vector<T>::fill_init(size_type n, const value_type& value) {
    const size_type init_size = mstl::max(static_cast<size_type>(16), n);
    init_space(n, init_size);
    mstl::uninitialized_fill_n(begin_, n, value);
}

// 以[first，last)与初值初始化vector
template<typename T>
template<typename Iter>
void vector<T>::range_init(Iter first, Iter last) {
    const size_type init_size = mstl::max(static_cast<size_type>(16),
                                static_cast<size_type>(last - first));
    init_space(static_cast<size_type>(last - first), init_size);
    mstl::uninitialized_copy(first, last, begin_);
}

// 清空[first, last)范围的元素，释放n个元素的空间
template<typename T>
void vector<T>::destory_and_recover(iterator first, iterator last, size_type n) {
    data_allocator::destory(first, last);
    data_allocator::deallocate(first, n);
}

// 扩容
template<typename T>
typename vector<T>::size_type
vector<T>::get_new_cap(size_type add_size) {
    const size_type old_size = capacity();
    THROW_LENGTH_ERROR_IF(old_size > max_size() - add_size,
                        "vector<T>'s size too big");
    if (old_size > max_size() - old_size / 2) {
        return old_size + add_size > max_size() - 16 ?
            old_size + add_size : old_size + add_size + 16; 
    }
    const size_type new_size = old_size == 0 ?
            mstl::max(static_cast<size_type>(16), add_size) :
            mstl::max(old_size + old_size / 2, old_size + add_size);
    return new_size;
}

// 在vector末尾添加n个value元素
template<typename T>
void vector<T>::fill_assign(size_type n, const value_type& value) {
    if (n > capacity()) {
        vector temp(n, value);
        swap(temp);
    } else if (n > size()) {
        mstl::fill(begin_, end(), value);
        end_ = mstl::uninitialized_fill_n(end_, n - size(), value);
    } else {
        erase(mstl::fill_n(begin_, n, value), end_);
    }
}

// 拷贝[first, last)的元素到vector 
template<typename T>
template<typename InputIterator>
void vector<T>::copy_assign(InputIterator first, InputIterator last, input_iterator_tag) {
    iterator cur = begin_;
    for (; cur != end_ && first != last; ++cur, ++first) {
        *cur = *first;
    }
    if (first == last) {
        erase(cur, end_);
    } else {
        insert(end_, first, last);
    }
}

template<typename T>
template<typename ForwardIterator>
void vector<T>::copy_assign(ForwardIterator first, ForwardIterator last, forward_iterator_tag) {
    const size_type len = mstl::distance(first, last);
    if (len > capacity()) {
        vector temp(first, last);
        swap(temp);
    } else if (size() >= len) {
        iterator new_end = mstl::copy(first, last, begin_);
        data_allocator::destory(new_end, end_);
        end_ = new_end;
    } else {
        ForwardIterator mid = first;
        mstl::advance(mid, size());
        mstl::copy(first, mid, begin_);
        iterator new_end = mstl::uninitialized_copy(mid, last, end_);
        end_ = new_end;
    }
}

// 重新分配内存，在pos位置以右值的方式构造一个元素
template<typename T>
template<typename... Args>
void vector<T>::reallocate_emplace(iterator pos, Args&& ...args) {
    const size_type new_size = get_new_cap(1);
    iterator new_begin = data_allocator::allocate(new_size);
    iterator new_end = new_begin;
    try {
        new_end = mstl::uninitialized_move(begin_, pos, new_begin);
        data_allocator::construct(mstl::address_of(*new_end), mstl::forward<Args>(args)...);
        ++new_end;
        new_end = mstl::uninitialized_move(pos, end_, new_end);
    } catch (...) {
        data_allocator::deallocate(new_begin, new_size);
        throw;
    }
    destory_and_recover(begin_, end_, cap_ - begin_);
    begin_ = new_begin;
    end_ = new_end;
    cap_ = begin_ + new_size;
}

// 重新分配内存，并在pos位置插入value元素
template<typename T>
void vector<T>::reallocate_insert(iterator pos, const value_type& value) {
    const size_type new_size = get_new_cap(1);
    iterator new_begin = data_allocator::allocate(new_size);
    iterator new_end = new_begin;
    try {
        new_end = mstl::uninitialized_move(begin_, pos, new_begin);
        data_allocator::construct(mstl::address_of(*new_end), value);
        ++new_end;
        new_end = mstl::uninitialized_move(pos, end_, new_end);
    } catch (...) {
        data_allocator::deallocate(new_begin, new_size);
        throw;
    }
    destory_and_recover(begin_, end_, cap_ - begin_);
    begin_ = new_begin;
    end_ = new_end;
    cap_ = begin_ + new_size;
}

// pos位置插入n个value元素
template<typename T>
typename vector<T>::iterator
vector<T>::fill_insert(iterator pos, size_type n, const value_type& value) {
    if (n == 0) {
        return pos;
    }
    const size_type xpos = pos - begin_;
    const value_type value_copy = value;    //防止拷贝过程中value被覆盖
    if (static_cast<size_type>(cap_ - end_) >= n) {
        const size_type after_elems = end_ - pos;
        iterator old_end = end_;
        if (after_elems > n) {
            mstl::uninitialized_copy(end_ - n, end_ ,end_);
            end_ += n;
            mstl::move_backward(pos, old_end - n, old_end);
            mstl::uninitialized_fill_n(pos, n, value_copy);
        } else {
            end_ = mstl::uninitialized_fill_n(end_, n - after_elems, value_copy);
            end_ = mstl::uninitialized_move(pos, old_end, end_);
            mstl::uninitialized_fill_n(pos, after_elems, value_copy);
        }
    } else {
        const size_type new_size = get_new_cap(n);
        iterator new_begin = data_allocator::allocate(new_size);
        iterator new_end = new_begin;
        try {
            new_end = mstl::uninitialized_move(begin_, pos, new_begin);
            new_end = mstl::uninitialized_fill_n(new_end, n, value);
            new_end = mstl::uninitialized_move(pos, end_, new_end);
        } catch (...) {
            destory_and_recover(new_begin, new_end, new_size);
            throw;
        }
        data_allocator::deallocate(begin_, cap_ - begin_);
        begin_ = new_begin;
        end_ = new_end;
        cap_ = begin_ + new_size;
    }
    return begin_ + xpos;
}

// 将[first, last)的元素拷贝到pos位置
template<typename T>
template<typename InputIterator>
void vector<T>::copy_insert(iterator pos, InputIterator first, InputIterator last) {
    if (first == last) {
        return;
    }
    const size_type n = mstl::distance(first, last);
    if (static_cast<size_type>(cap_ - end_) >= n) {
        const size_type after_elems = end_ - pos;
        iterator old_end = end_;
        if (after_elems > n) {
            end_ = mstl::uninitialized_copy(end_ - n, end_ ,end_);
            mstl::move_backward(pos, old_end - n, old_end);
            mstl::uninitialized_copy(first, last, pos);
        } else {
            InputIterator mid = first;
            mstl::advance(mid, after_elems);
            end_ = mstl::uninitialized_copy(mid, last, end_);
            end_ = mstl::uninitialized_move(pos, old_end, end_);
            mstl::uninitialized_copy(first, mid, pos);
        }
    } else {
        const size_type new_size = get_new_cap(n);
        iterator new_begin = data_allocator::allocate(new_size);
        iterator new_end = new_begin;
        try {
            new_end = mstl::uninitialized_move(begin_, pos, new_begin);
            new_end = mstl::uninitialized_copy(first, last, new_end);
            new_end = mstl::uninitialized_move(pos, end_, new_end);
        } catch (...) {
            destory_and_recover(new_begin, new_end, new_size);
            throw;
        }
        data_allocator::deallocate(begin_, cap_ - begin_);
        begin_ = new_begin;
        end_ = new_end;
        cap_ = begin_ + new_size;
    }
}

// 修改vector容量为size个元素
template<typename T>
void vector<T>::reinsert(size_type size) {
    iterator new_begin = data_allocator::allocate(size);
    try {
        mstl::uninitialized_move(begin_, end_, new_begin);
    } catch (...) {
        data_allocator::deallocate(begin_, size);
        throw;
    }
    data_allocator::deallocate(begin_, cap_ - begin_);
    begin_ = new_begin;
    end_ = begin_ + size;
    cap_ = begin_ + size;
}

// 重载全局操作符
template<typename T>
bool operator==(const vector<T>& lhs, const vector<T>& rhs) {
    return lhs.size() == rhs.size() && 
        mstl::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template<typename T>
bool operator<(const vector<T>& lhs, const vector<T>& rhs) {
    return mstl::lexicographical_compare(lhs.begin(), lhs.end(),
            rhs.begin(), rhs.end());
}

template<typename T>
bool operator!=(const vector<T>& lhs, const vector<T>& rhs) {
    return !(lhs == rhs);
}

template<typename T>
bool operator>(const vector<T>& lhs, const vector<T>& rhs) {
    return rhs < lhs;
}

template<typename T>
bool operator<=(const vector<T>& lhs, const vector<T>& rhs) {
    return !(rhs < lhs);
}

template<typename T>
bool operator>=(const vector<T>& lhs, const vector<T>& rhs) {
    return !(lhs < rhs);
}

template<typename T>
void swap(vector<T>& lhs, vector<T>& rhs) {
    lhs.swap(rhs);
}

} //mstl
#endif