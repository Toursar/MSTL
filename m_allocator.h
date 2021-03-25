#ifndef M_ALLOCATOR_H_
#define M_ALLOCATOR_H_

#include "m_construct.h"
#include "m_util.h"

// 包含一个类allocator用于管理内存的分配以及释放，对象的构造与析构

namespace mstl {

template<typename T>
class allocator {
public:
    typedef T           value_type;
    typedef T*          pointer;
    typedef const T*    const_pointer;
    typedef T&          reference;
    typedef const T&    const_reference;
    typedef size_t      size_type;
    typedef ptrdiff_t   difference_type;

    static pointer allocate();
    static pointer allocate(size_type n);

    static void deallocate(pointer ptr);
    static void deallocate(pointer ptr, size_type n);

    static void construct(pointer ptr);
    static void construct(pointer ptr, const_reference value);
    static void construct(pointer ptr, T&& value);

    template<typename... Args>
    static void construct(pointer ptr, Args&& ...args);

    static void destory(pointer ptr);
    static void destory(pointer first, pointer last);
};

template<typename T>
T* allocator<T>::allocate() {
    return static_cast<T*>(::operator new(sizeof(T)));
}

template<typename T>
T* allocator<T>::allocate(size_t n) {
    if (n == 0) {
        return nullptr;
    }
    return static_cast<T*>(::operator new(n * sizeof(T)));
}

template<typename T>
void allocator<T>::deallocate(T* ptr) {
    if (ptr == nullptr) {
        return;
    }
    ::operator delete(ptr);
}

template<typename T>
void allocator<T>::deallocate(T* ptr, size_t) {
    if (ptr == nullptr) {
        return;
    }
    ::operator delete(ptr);
}

template<typename T>
void allocator<T>::construct(T* ptr) {
    mstl::construct(ptr);
}

template<typename T>
void allocator<T>::construct(T* ptr, const T& value) {
    mstl::construct(ptr, value);
}

template<typename T>
void allocator<T>::construct(T* ptr, T&& value) {
    mstl::construct(ptr, mstl::move(value));
}

template<typename T>
template<typename... Args>
void allocator<T>::construct(T* ptr, Args&& ...args) {
    mstl::construct(ptr, mstl::forward<Args>(args)...);
}

template<typename T>
void allocator<T>::destory(T* ptr) {
    mstl::destory(ptr);
}

template<typename T>
void allocator<T>::destory(T* first, T* last) {
    mstl::destory(first, last);
}

} //mstl

#endif


//allocator测试类-----------------------------------

// template<typename T>
// class A {
//     T m_data;
// public:
//     A() = default;
//     A(const T& data) : m_data(data) {}
//     A(const A& temp) {
//         this->m_data = temp.m_data;
//     };
//     friend std::ostream& operator<<(std::ostream& os, const A<T>& temp) {
//         os << temp.m_data;
//         return os;
//     }
// };

// //--------------------------------------------

// int main() {
//     A<int>* ptr;
//     int a = 1;
//     A<int> ptr1(a);
//     // 先使用allocate分配内存，然后调用construct构建对象
//     ptr = mstl::allocator<A<int>>::allocate(1);
//     mstl::allocator<A<int>>::construct(ptr, 5);
//     //mstl::allocator<A<int>>::construct(ptr, ptr1);
//     std::cout << *ptr;
//     return 0;
// }