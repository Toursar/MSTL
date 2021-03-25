#ifndef M_QUEUE_H_
#define M_QUEUE_H_

#include "m_deque.h"
#include "m_vector.h"
#include "m_functional.h"
#include "m_heap_algo.h"

namespace mstl {

template<typename T, typename Container = mstl::deque<T>>
class queue {
public:
    typedef Container                           container_type;
    typedef typename Container::value_type      value_type;
    typedef typename Container::size_type       size_type;
    typedef typename Container::reference       reference;
    typedef typename Container::const_reference const_reference;
    static_assert(std::is_same<T, value_type>::value,
                    "the value_type of Container should be same with template T");
private:
    container_type c_;
public:
    queue() = default;
    explicit queue(size_type n) : c_(n) {}
    queue(size_type n, const value_type& value) : c_(n, value) {}

    template<typename InputIterator>
    queue(InputIterator first, InputIterator last) : c_(first, last) {}

    queue(std::initializer_list<T> ilist) : c_(ilist.begin(), ilist.end()) {}

    queue(const queue& rhs) : c_(rhs.c_) {}

    queue(queue&& rhs) noexcept(std::is_nothrow_move_assignable<Container>::value)
        : c_(mstl::move(rhs.c_)) {}
    
    queue(const Container& c) : c_(c) {}

    queue(Container&& c) noexcept(std::is_nothrow_move_assignable<Container>::value)
        : c_(mstl::move(c)) {}
    
    queue& operator=(const queue& rhs) {
        // 这里无需判定自我赋值，因为底层容器会进行判定
        c_ = rhs.c_;
        return *this;
    }

    queue& operator=(queue&& rhs) noexcept(std::is_nothrow_move_assignable<Container>::value) {
        c_ = mstl::move(rhs.c_);
        return *this;
    }

    queue& operator=(std::initializer_list<T> ilist) {
        c_ = ilist;
        return *this;
    }

    ~queue() = default;

    // 元素的访问，委托底层容器
    reference front() {
        return c_.front();
    }
    const_reference front() const {
        return c_.front();
    }
    // 容器容量相关
    bool empty() const {
        return c_.empty();
    }
    size_type size() const {
        return c_.size();
    }

    // 容器容量修改操作
    template<typename ...Args>
    void emplace(Args&& ...args) {
        c_.emplace_back(mstl::forward<Args>(args)...);
    }

    void push(const value_type& value) {
        c_.push_back(value);
    }
    void push(value_type&& value) {
        c_.emplace_back(mstl::move(value));
    }
    void pop() {
        c_.pop_front();
    }
    void clear() {
        while(empty()) {
            pop();
        }
    }
    void swap(queue& rhs) noexcept(noexcept(mstl::swap(c_, rhs.c_))) {
        mstl::swap(c_, rhs.c_);
    }

    bool operator==(const queue& rhs) const {
        return c_ == rhs.c_;
    }
    bool operator<(const queue& rhs) const {
        return c_ < rhs.c_;
    }
};

template<typename T, typename Container>
bool operator==(const queue<T, Container>& lhs, const queue<T, Container>& rhs) {
    return lhs == rhs;
}

template<typename T, typename Container>
bool operator<(const queue<T, Container>& lhs, const queue<T, Container>& rhs) {
    return lhs < rhs;
}

template<typename T, typename Container>
bool operator!=(const queue<T, Container>& lhs, const queue<T, Container>& rhs) {
    return !(lhs == rhs);
}

template<typename T, typename Container>
bool operator>(const queue<T, Container>& lhs, const queue<T, Container>& rhs) {
    return rhs < lhs;
}

template<typename T, typename Container>
bool operator<=(const queue<T, Container>& lhs, const queue<T, Container>& rhs) {
    return !(rhs < lhs);
}

template<typename T, typename Container>
bool operator>=(const queue<T, Container>& lhs, const queue<T, Container>& rhs) {
    return !(lhs < rhs);
}

template<typename T, typename Container>
void swap(queue<T, Container>& lhs, queue<T, Container>& rhs) {
    lhs.swap(rhs);
}

/***********************************************************************************************/
// 优先队列的实现

template<typename T, typename Container = mstl::vector<T>, typename Compare = 
            mstl::less<typename Container::value_type>>
class priority_queue {
public:
    typedef Container                           container_type;
    typedef Compare                             value_compare;
    typedef typename Container::value_type      value_type;
    typedef typename Container::size_type       size_type;
    typedef typename Container::reference       reference;
    typedef typename Container::const_reference const_reference;
    static_assert(std::is_same<T, value_type>::value, "the container value_type should be same with T");

private:
    container_type  c_;
    value_compare    cmp_;

public:
    priority_queue() = default;

    priority_queue(value_compare cmp) : c_(), cmp_(cmp) {}

    explicit priority_queue(size_type n) : c_(n) {
        make_heap(c_.beign(), c_.end(), cmp_);
    }

    priority_queue(size_type n, const value_type value) : c_(n, value) {
        make_heap(c_.begin(), c_.end(), cmp_);
    }

    template<typename InputIter>
    priority_queue(InputIter first, InputIter last) : c_(first, last) {
        make_heap(c_.begin(), c_.end(), cmp_);
    }

    priority_queue(std::initializer_list<T> ilist) : c_(ilist) {
        make_heap(c_.begin(), c_.end(), cmp_);
    }

    priority_queue(const container_type& s) : c_(s) {
        make_heap(c_.begin(), c_.end(), cmp_);
    }

    priority_queue(container_type&& s) : c_(mstl::move(s)) {
        make_heap(c_.begin(), c_.end(), cmp_);
    }

    priority_queue(const priority_queue& rhs) : c_(rhs.c_), cmp_(rhs.cmp_) {
        make_heap(c_.begin(), c_.end(), cmp_);
    }

    priority_queue(priority_queue &&rhs) : c_(mstl::move(rhs.c_)), cmp_(rhs.cmp_) {
        make_heap(c_.begin(), c_.end(), cmp_);
    }

    priority_queue& operator=(const priority_queue& rhs) {
        c_ = rhs.c_;
        cmp_ = rhs.cmp_;
        make_heap(c_.begin(), c_.end(), cmp_);
        return *this;
    }

    priority_queue& operator=(priority_queue&& rhs) {
        c_ = mstl::move(rhs.c_);
        cmp_ = rhs.cmp_;
        make_heap(c_.begin(), c_.end(), cmp_);
        return *this;
    }

    priority_queue& operator=(std::initializer_list<T> ilist) {
        c_ = ilist;
        cmp_ = value_compare();
        make_heap(c_.begin(), c_.end(), cmp_);
        return *this;
    }

    ~priority_queue() = default;

public:
    const_reference top() const {
        return c_.front();
    }

    bool empty() const noexcept {
        return c_.empty();
    }

    size_type size() const noexcept {
        return c_.size();
    }

    template<typename... Args>
    void emplace(Args&&... args) {
        c_.emplace_back(mstl::forward<Args>(args)...);
        mstl::push_heap(c_.begin(), c_.end(), cmp_);
    }

    void push(const value_type& value) {
        c_.push_back(value);
        mstl::push_heap(c_.begin(), c_.end(), cmp_);
    }

    void pop() {
        pop_heap(c_.begin(), c_.end(), cmp_);
        c_.pop_back();
    }

    void clear() {
        while (!empty()) {
            pop();
        }
    }

    void swap(priority_queue& rhs) noexcept(noexcept(mstl::swap(c_, rhs.c_)) &&
                                            noexcept(mstl::swap(cmp_, rhs.cmp_))) {
        mstl::swap(c_, rhs.c_);
        mstl::swap(cmp_, rhs.cmp_);
    }
public:
    friend bool operator==(const priority_queue& lhs, const priority_queue& rhs) {
        return lhs.c_ == rhs.c_;
    }
    friend bool operator!=(const priority_queue& lhs, const priority_queue& rhs) {
        return lhs.c_ != rhs.c_;
    }
};

// 是否应该加const修饰符
template<typename T, typename Container, typename Compare>
bool operator==(const priority_queue<T, Container, Compare>& lhs,
                const priority_queue<T, Container, Compare>& rhs) {
    return lhs == rhs;
}

template<typename T, typename Container, typename Compare>
bool operator!=(const priority_queue<T, Container, Compare>& lhs,
                const priority_queue<T, Container, Compare>& rhs) {
    return lhs != rhs;
}

template<typename T, typename Container, typename Compare>
void swap(priority_queue<T, Container, Compare>& lhs, priority_queue<T, Container, Compare>& rhs) 
                                                                    noexcept(noexcept(lhs.swap(rhs))) {
    lhs.swap(rhs);
}

} //mstl

#endif