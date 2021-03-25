#ifndef M_STACK_H_
#define M_STACK_H_

#include "m_deque.h"

// stack的实现是通过stl中的其他容器deque作为底层容器实现
// 利用has-a的关系，通过deque的方法实现自己的接口, 
// 只暴露部分stack所需的接口，其余deque接口stack不可用

namespace mstl {

template<typename T, typename Container = mstl::deque<T>>
class stack {
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
    stack() = default;
    explicit stack(size_type n) : c_(n) {}
    stack(size_type n, const value_type& value) : c_(n, value) {}

    template<typename InputIterator>
    stack(InputIterator first, InputIterator last) : c_(first, last) {}

    stack(std::initializer_list<T> ilist) : c_(ilist.begin(), ilist.end()) {}

    stack(const stack& rhs) : c_(rhs.c_) {}

    stack(stack&& rhs) noexcept(std::is_nothrow_move_assignable<Container>::value)
        : c_(mstl::move(rhs.c_)) {}
    
    stack(const Container& c) : c_(c) {}

    stack(Container&& c) noexcept(std::is_nothrow_move_assignable<Container>::value)
        : c_(mstl::move(c)) {}
    
    stack& operator=(const stack& rhs) {
        // 这里无需判定自我赋值，因为底层容器会进行判定
        c_ = rhs.c_;
        return *this;
    }

    stack& operator=(stack&& rhs) noexcept(std::is_nothrow_move_assignable<Container>::value) {
        c_ = mstl::move(rhs.c_);
        return *this;
    }

    stack& operator=(std::initializer_list<T> ilist) {
        c_ = ilist;
        return *this;
    }

    ~stack() = default;

    // 元素的访问，委托底层容器
    reference top() {
        return c_.back();
    }
    const_reference top() const {
        return c_.back();
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
        c_.pop_back();
    }
    void clear() {
        while(empty()) {
            pop();
        }
    }
    void swap(stack& rhs) noexcept(noexcept(mstl::swap(c_, rhs.c_))) {
        mstl::swap(c_, rhs.c_);
    }

    bool operator==(const stack& rhs) const {
        return c_ == rhs.c_;
    }
    bool operator<(const stack& rhs) const {
        return c_ < rhs.c_;
    }
};

template<typename T, typename Container>
bool operator==(const stack<T, Container>& lhs, const stack<T, Container>& rhs) {
    return lhs == rhs;
}

template<typename T, typename Container>
bool operator<(const stack<T, Container>& lhs, const stack<T, Container>& rhs) {
    return lhs < rhs;
}

template<typename T, typename Container>
bool operator!=(const stack<T, Container>& lhs, const stack<T, Container>& rhs) {
    return !(lhs == rhs);
}

template<typename T, typename Container>
bool operator>(const stack<T, Container>& lhs, const stack<T, Container>& rhs) {
    return rhs < lhs;
}

template<typename T, typename Container>
bool operator<=(const stack<T, Container>& lhs, const stack<T, Container>& rhs) {
    return !(rhs < lhs);
}

template<typename T, typename Container>
bool operator>=(const stack<T, Container>& lhs, const stack<T, Container>& rhs) {
    return !(lhs < rhs);
}

template<typename T, typename Container>
void swap(stack<T, Container>& lhs, stack<T, Container>& rhs) {
    lhs.swap(rhs);
}

} // mstl

#endif