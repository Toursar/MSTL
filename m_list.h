#ifndef M_LIST_H
#define M_LIST_H

// list双向链表的实现

#include <initializer_list>

#include "m_iterator.h"
#include "m_functional.h"
#include "m_exceptdef.h"
#include "m_util.h"
#include "m_memory.h"

namespace mstl {

// 使用了继承的方式base_ptr保存指针，node_ptr保存值，在需要时可以相互转化获取结果
template<typename T>
struct list_node_base;

template<typename T>
struct list_node;

template<typename T>
struct list_traits {
    typedef list_node_base<T>* base_ptr;
    typedef list_node<T>*      node_ptr;
};


template<typename T>
struct list_node_base {
    typedef typename list_traits<T>::base_ptr base_ptr;
    typedef typename list_traits<T>::node_ptr node_ptr;

    list_node_base() = default;
    // 前后两个指针
    base_ptr prev;
    base_ptr next;

    node_ptr as_node() {
        return static_cast<node_ptr>(self());
    }

    // 这里unlink作用时在初始化时，将初始的标志结尾的节点node_首先组成一个链表
    // 即此时只有一个标志结尾的节点，prev和next都是自己
    void unlink() {
        prev = next = self();
    }

    base_ptr self() {
        return static_cast<base_ptr>(this);
    }
};

template<typename T>
struct list_node : public list_node_base<T> {
    typedef typename list_traits<T>::base_ptr base_ptr;
    typedef typename list_traits<T>::node_ptr node_ptr;
    // node的值
    T value;

    list_node() = default;

    list_node(const T& v) : value(v) {}

    list_node(T&& v) : value(mstl::move(v)) {}

    base_ptr as_base() {
        return static_cast<base_ptr>(self());
    }

    node_ptr self() {
        return static_cast<node_ptr>(this);
    }
};

// list的迭代器定义
template<typename T>
class list_iterator : public iterator<mstl::bidirectional_iterator_tag, T> {
public:
    typedef T                                 value_type;
    typedef T*                                pointer;
    typedef T&                                reference;
    typedef typename list_traits<T>::base_ptr base_ptr;
    typedef typename list_traits<T>::node_ptr node_ptr;
    typedef list_iterator<T>                  self;

    base_ptr node_;

    list_iterator() = default;

    list_iterator(base_ptr ptr) : node_(ptr) {}

    list_iterator(node_ptr ptr) : node_(ptr->as_base()) {}

    list_iterator(const list_iterator& rhs) : node_(rhs.node_) {}

    reference operator*() const {
        return node_->as_node()->value;
    }

    pointer operator->() const {
        return &(operator*());
    }

    self& operator++() {
        MSTL_DEBUG(node_ != nullptr);
        node_ = node_->next;
        return *this;
    }

    self operator++(int) {
        self temp = *this;
        ++*(this);
        return temp;
    }

    self& operator--() {
        MSTL_DEBUG(node_ != nullptr);
        node_ = node_->prev;
        return *this;
    }

    self operator--(int) {
        self temp = *this;
        --*(this);
        return temp;
    }

    bool operator==(const self& rhs) const {
        return node_ == rhs.node_;
    }

    bool operator!=(const self& rhs) const {
        return node_ != rhs.node_;
    }
};

template<typename T>
class list_const_iterator : public iterator<mstl::bidirectional_iterator_tag, T> {
public:
    typedef T                                 value_type;
    typedef const T*                          pointer;
    typedef const T&                          reference;
    typedef typename list_traits<T>::base_ptr base_ptr;
    typedef typename list_traits<T>::node_ptr node_ptr;
    typedef list_const_iterator<T>            self;

    base_ptr node_;

    list_const_iterator() = default;

    list_const_iterator(base_ptr ptr) : node_(ptr) {}

    list_const_iterator(node_ptr ptr) : node_(ptr->as_base()) {}

    list_const_iterator(const list_iterator<T>& rhs) : node_(rhs.node_) {}

    list_const_iterator(const list_const_iterator& rhs) : node_(rhs.node_) {}

    reference operator*() const {
        return node_->as_node()->value;
    }

    pointer operator->() const {
        return &(operator*());
    }

    self& operator++() {
        MSTL_DEBUG(node_ != nullptr);
        node_ = node_->next;
        return *this;
    }

    self operator++(int) {
        self temp = *this;
        ++*(this);
        return temp;
    }

    self& operator--() {
        MSTL_DEBUG(node_ != nullptr);
        node_ = node_->prev;
        return *this;
    }

    self operator--(int) {
        self temp = *this;
        --*(this);
        return temp;
    }

    bool operator==(const self& rhs) const {
        return node_ == rhs.node_;
    }

    bool operator!=(const self& rhs) const {
        return node_ != rhs.node_;
    }
};

template<typename T>
class list {
public:
    typedef mstl::allocator<T>                       allocator_type;
    typedef mstl::allocator<T>                       data_allocator;
    typedef mstl::allocator<list_node_base<T>>       base_allocator;
    typedef mstl::allocator<list_node<T>>            node_allocator;

    typedef typename allocator_type::value_type      value_type;
    typedef typename allocator_type::pointer         pointer;
    typedef typename allocator_type::const_pointer   const_pointer;
    typedef typename allocator_type::reference       reference;
    typedef typename allocator_type::const_reference const_reference;
    typedef typename allocator_type::size_type       size_type;
    typedef typename allocator_type::difference_type difference_type;

    typedef list_iterator<T>                         iterator;
    typedef list_const_iterator<T>                   const_iterator;
    typedef mstl::reverse_iterator<iterator>         reverse_iterator;
    typedef mstl::reverse_iterator<const_iterator>   const_reverse_iterator;

    typedef typename list_traits<T>::base_ptr        base_ptr;
    typedef typename list_traits<T>::node_ptr        node_ptr;

    node_allocator get_allocator() {
        return node_allocator();
    }

private:
    // 这里只用一个虚拟节点node_就能表征整个链表
    // 当node->next == node_时，此节点为最后一个节点
    // 一般此时node_的next为nullptr，而这里将node_->next
    // 指向链表的头部，形成一个环维护整个链表，因此只需要一个节点就可以辅助维护整个链表
    base_ptr node_;     // 指向末尾节点
    size_type size_;    // 存放的数据个数

public:
    // 一系列构造与赋值函数
    list(){
        fill_init(0, value_type());
    }

    explicit list(size_type n) {
        fill_init(0, value_type());
    }

    list(size_type n, const value_type& value) {
        fill_init(n, value);
    }

    template<typename Iter, typename mstl::enable_if<
            mstl::is_input_iterator<Iter>::value, int>::type = 0>
    list(Iter first, Iter last) {
        copy_init(first, last);
    }

    list(std::initializer_list<T> ilist) {
        copy_init(ilist.begin(), ilist.end());
    }

    list(const list& rhs) {
        copy_init(rhs.cbegin(), rhs.cend());
    }

    list(list&& rhs) noexcept : node_(rhs.node_), size_(rhs.size_) {
        rhs.node_ = nullptr;
        rhs.size_ = 0;
    }

    list& operator=(const list& rhs) {
        if (this != &rhs) {
            assign(rhs.begin(), rhs.end());
        }
        return *this;
    }

    list& operator=(list&& rhs) noexcept {
        clear();
        splice(end(), rhs);
        return *this;
    }

    list& operator=(std::initializer_list<T> ilist) {
        list temp(ilist.begin(), ilist.end());
        swap(temp);
        return *this;
    }

    ~list() {
        if (node_) {
            clear();
            base_allocator::deallocate(node_);
            node_ = nullptr;
            size_ = 0;
        }
    }
public:
    // 迭代器相关操作
    iterator begin() noexcept {
        return node_->next;
    }

    const_iterator begin() const noexcept {
        return node_->next;
    }

    iterator end() noexcept {
        return node_;
    }

    const_iterator end() const noexcept {
        return node_;
    }

    reverse_iterator rbegin() noexcept {
        return reverse_iterator(end());
    }

    const_reverse_iterator rbegin() const noexcept {
        return reverse_iterator(end()); 
    }

    reverse_iterator rend() noexcept {
        return reverse_iterator(begin());
    }

    const_reverse_iterator rend() const noexcept {
        return reverse_iterator(begin());
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

    // 容量相关方法
    bool empty() const noexcept {
        return node_ == node_->next;
    }

    size_type size() const noexcept{
        return size_;
    }

    size_type max_size() const noexcept {
        return static_cast<size_type>(-1);
    }

    // 访问元素
    reference front() {
        MSTL_DEBUG(!empty());
        return *begin();
    }

    const_reference front() const {
        MSTL_DEBUG(!empty());
        return *begin();
    }

    reference back() {
        MSTL_DEBUG(!empty());
        return *(--end());
    }

    const_reference back() const {
        MSTL_DEBUG(!empty());
        return *(--end());
    }

    void assign(size_type n, const value_type& value) {
        fill_assign(n, value);
    }

    template<typename Iter, typename mstl::enable_if<
            mstl::is_input_iterator<Iter>::value, int>::type = 0>
    void assign(Iter first, Iter last) {
        copy_assign(first, last);
    }

    void assign(std::initializer_list<T> ilist) {
        copy_assign(ilist.begin(), ilist.end());
    }

    template<typename... Args>
    void emplace_front(Args&& ...args) {
        THROW_LENGTH_ERROR_IF(size_ > max_size() - 1 ,"this size of list<T> is too large");
        node_ptr node = create_node(mstl::forward<Args>(args)...);
        link_nodes_at_front(node->as_base(), node->as_base());
        ++size_;
    }

    template<typename... Args>
    void emplace_back(Args&& ...args) {
        THROW_LENGTH_ERROR_IF(size_ > max_size() - 1 ,"this size of list<T> is too large");
        node_ptr node = create_node(mstl::forward<Args>(args)...);
        link_nodes_at_back(node->as_base(), node->as_base());
        ++size_;
    }

    template<typename... Args>
    iterator emplace(const_iterator pos, Args&& ...args) {
        THROW_LENGTH_ERROR_IF(size_ > max_size() - 1 ,"this size of list<T> is too large");
        node_ptr node = create_node(mstl::forward<Args>(args)...);
        link_nodes(pos.node_, node->as_base(), node->as_base());
        ++size_;
        return iterator(node);
    }

    iterator insert(const_iterator pos, const value_type& value) {
        THROW_LENGTH_ERROR_IF(size_ > max_size() - 1 ,"this size of list<T> is too large");
        node_ptr node = create_node(value);
        ++size_;
        return link_iter_node(pos, node->as_base());
    }

    iterator insert(const_iterator pos, value_type&& value) {
        THROW_LENGTH_ERROR_IF(size_ > max_size() - 1 ,"this size of list<T> is too large");
        node_ptr node = create_node(mstl::move(value));
        ++size_;
        return link_iter_node(pos, node->as_base());
    }

    iterator insert(const_iterator pos, size_type n, const value_type& value) {
        THROW_LENGTH_ERROR_IF(size_ > max_size() - n ,"this size of list<T> is too large");
        return fill_insert(pos, n, value);
    }

    template<typename Iter, typename mstl::enable_if<
            mstl::is_input_iterator<Iter>::value, int>::type = 0>
    iterator insert(const_iterator pos, Iter first, Iter last) {
        const size_type n = mstl::distance(first, last);
        THROW_LENGTH_ERROR_IF(size_ > max_size() - n ,"this size of list<T> is too large");
        return copy_insert(pos, n, first);
    }

    void push_front(const value_type& value) {
        THROW_LENGTH_ERROR_IF(size_ > max_size() - 1 ,"this size of list<T> is too large");
        node_ptr node = create_node(value);
        link_nodes_at_front(node->as_base(), node->as_base());
        ++size_;
    }

    void push_front(value_type&& value) {
        emplace_front(mstl::move(value));
    }

    void push_back(const value_type& value) {
        THROW_LENGTH_ERROR_IF(size_ > max_size() - 1 ,"this size of list<T> is too large");
        node_ptr node = create_node(value);
        link_nodes_at_back(node->as_base(), node->as_base());
        ++size_;
    }

    void push_back(value_type&& value) {
        emplace_back(mstl::move(value));
    }

    void pop_front() {
        MSTL_DEBUG(!empty());
        base_ptr n = node_->next;
        unlink_nodes(n, n);
        destory_node(n->as_node());
        --size_;
    }

    void pop_back() {
        MSTL_DEBUG(!empty());
        base_ptr n = node_->prev;
        unlink_nodes(n, n);
        destory_node(n->as_node());
        --size_;
    }

    iterator erase(const_iterator pos);
    iterator erase(const_iterator first, const_iterator last);

    void clear();

    void resize(size_type new_size) {
        resize(new_size, value_type());
    }

    void resize(size_type new_size, const value_type& value);

    void swap(list& rhs) noexcept {
        mstl::swap(node_, rhs.node_);
        mstl::swap(size_, rhs.size_);
    }

    void splice(const_iterator pos, list& other);
    void splice(const_iterator pos, list& other, const_iterator it);
    void splice(const_iterator pos, list& other, const_iterator first, const_iterator last);

    void remove(const value_type& value) {
        remove_if([&](const value_type& v) {
            return v == value;
        });
    }

    template<typename UnaryPredicate>
    void remove_if(UnaryPredicate pred);

    void unique() {
        unique(mstl::equal_to<T>());
    }

    template<typename BinaryPredicate>
    void unique(BinaryPredicate pred);

    void merge(list& x) {
        merge(x, mstl::less<T>());
    }

    template<typename Compare>
    void merge(list& x, Compare cmp);

    void sort() {
        list_sort(begin(), end(), size_, mstl::less<T>());
    }

    template<typename Compare>
    void sort(Compare cmp) {
        list_sort(begin(), end(), size_, cmp);
    }

    void reverse();

private:
  // 辅助函数

template <typename ...Args>
node_ptr create_node(Args&& ...agrs);
void destory_node(node_ptr p);

void fill_init(size_type n, const value_type& value);
template <typename Iter>
void copy_init(Iter first, Iter last);

iterator link_iter_node(const_iterator pos, base_ptr link_node);
void link_nodes(base_ptr p, base_ptr first, base_ptr last);
void link_nodes_at_front(base_ptr first, base_ptr last);
void link_nodes_at_back(base_ptr first, base_ptr last);
void unlink_nodes(base_ptr first, base_ptr last);

void fill_assign(size_type n, const value_type& value);
template <typename Iter>
void copy_assign(Iter first2, Iter last2);

iterator fill_insert(const_iterator pos, size_type n, const value_type& value);
template <typename Iter>
iterator copy_insert(const_iterator pos, size_type n, Iter first);

template <typename Compared>
iterator list_sort(iterator first, iterator last, size_type n, Compared comp);

};


template<typename T>
typename list<T>::iterator
list<T>::erase(const_iterator pos) {
    MSTL_DEBUG(pos != cend());
    base_ptr node = pos.node_;
    base_ptr next = node->next;
    unlink_nodes(node, node);
    destory_node(node->as_node());
    --size_;
    return iterator(next);
}

template<typename T>
typename list<T>::iterator
list<T>::erase(const_iterator first, const_iterator last) {
    if (first == last) {
        return iterator(last.node_);
    }
    unlink_nodes(first.node_, last.node_->prev);
    while (first != last) {
        base_ptr cur = first.node_;
        ++first;
        destory_node(cur->as_node());
        --size_;
    }
    return iterator(last.node_);
}

template<typename T>
void list<T>::clear() {
    if (size_ != 0) {
        base_ptr cur = node_->next;
        base_ptr next = cur->next;
        while (cur != node_) {
            destory_node(cur->as_node());
            cur = next;
            next = cur->next;
        }
        node_->unlink();
        size_ = 0;
    }
}

template<typename T>
void list<T>::resize(size_type new_size, const value_type& value) {
    iterator b = begin();
    size_type len = 0;
    while (b != end() && len < new_size) {
        ++b;
        ++len;
    }
    if (b == end()) {
        insert(end(), new_size - len, value);
    } else {
        erase(b, end());
    }
}

// 将list x的节点都插入到pos位置之前
template<typename T>
void list<T>::splice(const_iterator pos, list& x) {
    MSTL_DEBUG(this != &x);
    if (!x.empty()) {
        THROW_LENGTH_ERROR_IF(size_ + x.size_ > max_size(), "this size of list<T> is to large");
        base_ptr f = x.node_->next;
        base_ptr l = x.node_->prev;
        x.unlink_nodes(f, l);
        link_nodes(pos.node_, f, l);
        size_ += x.size_;
        x.size_ = 0;
    }
}

// 将list x的it迭代器所指节点插入到pos位置之前
template<typename T>
void list<T>::splice(const_iterator pos, list& x, const_iterator it) {
    // 两个条件，一个是插入的节点不能是自己，另一个是此节点如果已经是pos的前节点，无需再修改
    if (pos.node_ != it.node_ && pos.node_ != it.node_->next) {
        THROW_LENGTH_ERROR_IF(size_ > max_size() - 1, "this size of list<T> is to large");
        base_ptr f = it.node_;
        x.unlink_nodes(f, f);
        link_nodes(pos.node_, f, f);
        ++size_;
        --x.size_;
    }
}

// 将list x的[first, last)迭代器所指节点插入到pos位置之前
template<typename T>
void list<T>::splice(const_iterator pos, list& x, const_iterator first, const_iterator last) {
    if (first != last && this != &x) {
        size_type n = mstl::distance(first, last);
        THROW_LENGTH_ERROR_IF(size_ > max_size() - n, "this size of list<T> is to large");
        base_ptr f = first.node_;
        base_ptr l = last.node_->prev;

        x.unlink_nodes(f, l);
        link_nodes(pos.node_, f, l);

        size_ += n;
        x.size_ -= n;
    }
}

template<typename T>
template<typename UnaryPredicate>
void list<T>::remove_if(UnaryPredicate pred) {
    iterator f = begin();
    iterator l = end();
    for (iterator next = f; next != l; f = next) {
        ++next;
        if (pred(*f)) {
            erase(f);
        }
    }
}

template<typename T>
template<typename BinaryPredicate>
void list<T>::unique(BinaryPredicate pred) {
    iterator f = begin();
    iterator l = end();
    iterator next = f;
    ++next;
    while (next != l) {
        if (pred(*f, *next)) {
            // 注意erase导致迭代器失效的问题
            erase(next);
        } else {
            // 为了配合if中的失效问题，
            // 和下一条语句合起来相当于什么也没做（只有f被改变了）
            f = next;
        }
        // 重新给定next迭代器的指向
        next = f;
        ++next;
    }
}

// 将链表x以cmp的比较方式合并
template<typename T>
template<typename Compare>
void list<T>::merge(list& x, Compare cmp) {
    if (this != &x) {
        THROW_LENGTH_ERROR_IF(size_ > max_size() - x.size_, "this size of list<T> is to large");
        iterator first1 = begin();
        iterator last1 = end();
        iterator first2 = x.begin();
        iterator last2 = x.end();

        while (first1 != last1 && first2 != last2) {
            if (cmp(*first2, *first1)) {
                iterator m = first2;
                ++m;
                while (m != last2 && cmp(*m, *first1)) {
                    ++m;
                }
                base_ptr first = first2.node_;
                base_ptr last = m.node_->prev;
                first2 = m;
                
                unlink_nodes(first, last);
                link_nodes(first1.node_, first, last);

                ++first1;
            } else {
                ++first1;
            }
            // 如果x还有剩余
            if (first2 != last2) {
                base_ptr first = first2.node_;
                base_ptr last = last2.node_->prev;
                unlink_nodes(first, last);
                link_nodes(last1.node_, first, last);
            }
            size_ += x.size_;
            x.size_ = 0;
        }
    }
}

// 反转链表
template<typename T>
void list<T>::reverse() {
    if (size_ <= 1) {
        return;
    }
    iterator f = begin();
    iterator l = end();
    while (f.node_ != l.node_) {
        mstl::swap(f.node_->prev, f.node_->next);
        f.node_ = f.node_->prev;
    }
    mstl::swap(l.node_->prev, l.node_->next);
}

/***************************************************************************************************/
// 辅助函数的实现

template <typename T>
template <typename ...Args>
typename list<T>::node_ptr
list<T>::create_node(Args&& ...args) {
    node_ptr p = node_allocator::allocate(1);
    try {
        data_allocator::construct(mstl::address_of(p->value), mstl::forward<Args>(args)...);
        p->prev = nullptr;
        p->next = nullptr;
    } catch(...) {
        node_allocator::deallocate(p);
        throw;
    }
    return p;
}

template <typename T>
void list<T>::destory_node(node_ptr p) {
    data_allocator::destory(mstl::address_of(p->value));
    node_allocator::deallocate(p);
}

// 这里，调用初始的unlink方法，插入节点后就可以使此时的结构变为环状链表
// node_->next指向链表第一个元素
template<typename T>
void list<T>::fill_init(size_type n, const value_type& value) {
    node_ = base_allocator::allocate(1);
    node_->unlink();
    size_ = n;
    try {
        for (; n > 0; --n) {
            node_ptr node = create_node(value);
            link_nodes_at_back(node->as_base(), node->as_base());
        }
    } catch (...) {
        clear();
        base_allocator::deallocate(node_);
        node_ = nullptr;
        throw;
    }
}

template<typename T>
template <typename Iter>
void list<T>::copy_init(Iter first, Iter last) {
    node_ = base_allocator::allocate(1);
    node_->unlink();
    size_type n = mstl::distance(first, last);
    size_ = n;
    try {
        for (; n > 0; --n, ++first) {
            node_ptr node = create_node(*first);
            link_nodes_at_back(node->as_base(), node->as_base());
        }
    } catch (...) {
        clear();
        base_allocator::deallocate(node_);
        node_ = nullptr;
        throw;
    }
}

template<typename T>
typename list<T>::iterator
list<T>::link_iter_node(const_iterator pos, base_ptr link_node) {
    // 如果插入的位置pos是node_->next说明插入的位置为链表的头
    if (pos == node_->next) {
        link_nodes_at_front(link_node, link_node);
    // 如果插入的位置pos是node_-说明插入的位置为链表的尾
    } else if (pos == node_) {
        link_nodes_at_back(link_node, link_node);
    } else {
        link_nodes(pos.node_, link_node, link_node);
    }
    return iterator(link_node);
}

// 在链表中间位置pos插入节点
template<typename T>
void list<T>::link_nodes(base_ptr pos, base_ptr first, base_ptr last) {
    pos->prev->next = first;
    first->prev = pos->prev;
    pos->prev = last;
    last->next = pos;
}

// 在链表头插入节点
template<typename T>
void list<T>::link_nodes_at_front(base_ptr first, base_ptr last) {
    first->prev = node_;
    last->next = node_->next;
    last->next->prev = last;
    node_->next = first;
}

// 在链表尾部插入节点
template<typename T>
void list<T>::link_nodes_at_back(base_ptr first, base_ptr last) {
    last->next = node_;
    first->prev = node_->prev;
    first->prev->next = first;
    node_->prev = last;
}

// 将[first, last)范围的节点剔除 
template<typename T>
void list<T>::unlink_nodes(base_ptr first, base_ptr last) {
    first->prev->next = last->next;
    last->next->prev = first->prev;
}

// 将n个value赋值给当前list
template<typename T>
void list<T>::fill_assign(size_type n, const value_type& value) {
    iterator i = begin();
    iterator e = end();
    for (; n > 0 && i != e; --n, ++i) {
        *i = value;
    }
    if (n > 0) {
        insert(e, n, value);
    } else {
        erase(i, e);
    }
}

// 将[first, last)赋值给当前list
template<typename T>
template<typename Iter>
void list<T>::copy_assign(Iter first2, Iter last2) {
    iterator first1 = begin();
    iterator last1 = end();
    for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
        *first1 = *first2;
    }
    if (first2 == last2) {
        erase(first1, last1);
    } else {
        insert(last1, first2, last2);
    }
}

// 在pos位置插入n个value值
template<typename T>
typename list<T>::iterator
list<T>::fill_insert(const_iterator pos, size_type n, const value_type& value) {
    iterator r(pos.node_);
    if (n > 0) {
        const size_type add_size = n;
        node_ptr node = create_node(value);
        node->prev = nullptr;
        r = iterator(node);
        iterator end = r;
        try {
            for (--n; n > 0; --n, ++end) {
                node_ptr next = create_node(value);
                end.node_->next = next->as_base();
                next->prev = end.node_;
            }
            size_ += add_size;
        } catch (...) {
            base_ptr endNode = end.node_;
            while (true) {
                base_ptr prev = endNode->prev;
                destory_node(endNode->as_node());
                if (prev == nullptr) {
                    break;
                }
                endNode = prev;
            }
            throw;
        }
        link_nodes(pos.node_, r.node_, end.node_);
    }
    return r;
}

// 拷贝first开始的n个值插入到pos位置
template<typename T>
template<typename Iter>
typename list<T>::iterator
list<T>::copy_insert(const_iterator pos, size_type n, Iter first) {
    iterator r(pos.node_);
    if (n > 0) {
        const size_type add_size = n;
        node_ptr node = create_node(*first);
        node->prev = nullptr;
        r = iterator(node);
        iterator end = r;
        try {
            for (--n, ++first; n > 0; --n, ++first, ++end) {
                node_ptr next = create_node(*first);
                end.node_->next = next->as_base();
                next->prev = end.node_;
            }
            size_ += add_size;
        } catch (...) {
            base_ptr endNode = end.node_;
            while (true) {
                base_ptr prev = endNode->prev;
                destory_node(endNode->as_node());
                if (prev == nullptr) {
                    break;
                }
                endNode = prev;
            }
            throw;
        }
        link_nodes(pos.node_, r.node_, end.node_);
    }
    return r;
}

// 对list进行归并排序，并返回最小的迭代器
template<typename T>
template <typename Compared>
typename list<T>::iterator
list<T>::list_sort(iterator first, iterator last, size_type n, Compared comp) {
    if (n < 2) {
        return first;
    }

    if (n == 2) {
        if (comp(*--last, *first)) {
            base_ptr cur = last.node_;
            unlink_nodes(cur, cur);
            link_nodes(first.node_, cur, cur);
            return last;
        }
        return first;
    }

    size_type n1 = n / 2;
    iterator last2 = first;
    mstl::advance(last2, n1);
    iterator result = first = list_sort(first, last2, n1, comp);
    iterator first2 = last2 = list_sort(last2, last, n - n1, comp);

    // 先判断归并的两部分头结点哪个大，首先处理头部
    if (comp(*first2, *first)) {
        iterator m = first2;
        ++m;
        while (m != last && comp(*m, *first)) {
            ++m;
        }
        base_ptr f = first2.node_;
        base_ptr l = m.node_->prev;
        result = first2;
        first2 = last2 = m;
        unlink_nodes(f, l);
        link_nodes(first.node_, f, l);
        ++first;
    } else {
        ++first;
    }

    // 然后将其余节点按照大小顺序merger起来即可
    while (first != last2 && first2 != last) {
        if (comp(*first2, *first)) {
            iterator m = first2;
            ++m;
            while (m != last && comp(*m, *first)) {
                ++m;
            }
            base_ptr f = first2.node_;
            base_ptr l = m.node_->prev;
            if (last2 == first2) {
                last2 = m;
            }
            first2 = m;
            unlink_nodes(f, l);
            link_nodes(first.node_, f, l);
            ++first;
        } else {
            ++first;
        }
    }
    return result;
}

template<typename T>
bool operator==(const list<T>& lhs, const list<T>& rhs) {
    auto first1 = lhs.cbegin();
    auto first2 = rhs.cbegin();
    auto last1 = lhs.cend();
    auto last2 = rhs.cend();
    while (first1 != last1 && first2 != last2 && *first1 == *first2) {
        ++first1;
        ++first2;
    }
    return first1 == last1 && first2 == last2;
}

template<typename T>
bool operator<(const list<T>& lhs, const list<T>& rhs) {
    return mstl::lexicographical_compare(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
}

template<typename T>
bool operator!=(const list<T>& lhs, const list<T>& rhs) {
    return !(lhs == rhs);
}

template<typename T>
bool operator>(const list<T>& lhs, const list<T>& rhs) {
    return rhs < lhs;
}

template<typename T>
bool operator<=(const list<T>& lhs, const list<T>& rhs) {
    return !(rhs < lhs);
}

template<typename T>
bool operator>=(const list<T>& lhs, const list<T>& rhs) {
    return !(lhs < rhs);
}

template<typename T>
void swap(list<T>& lhs, list<T>& rhs) noexcept {
    lhs.swap(rhs);
}

} // mstl

#endif