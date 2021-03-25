#ifndef M_MEMORY_H_
#define M_MEMORY_H_

// 空间配置器，未初始化的空间管理，auto_ptr

#include <cstdlib>
#include <cstddef>
#include <climits>

#include "m_algobase.h"
#include "m_construct.h"
#include "m_allocator.h"
#include "m_uninitialized.h"

namespace mstl {
// 返回变量地址
template<typename T>
constexpr T* address_of(T& value) noexcept {
    return &value;
}

// 获取/释放临时缓冲区
template<typename T>
mstl::pair<T*, ptrdiff_t> get_buffer_helper(ptrdiff_t len, T*) {
    if (len > static_cast<ptrdiff_t>(INT_MAX / sizeof(T))) {
        len = INT_MAX / sizeof(T);
    }
    while (len > 0) {
        T* temp = static_cast<T*>(malloc(static_cast<size_t>(len) * sizeof(T)));
        if (temp != nullptr) {
            return mstl::pair<T*, ptrdiff_t>(temp, len);
        }
        len /= 2;   // 申请失败就缩小空间再试一次
    }
    return mstl::pair<T*, ptrdiff_t>(nullptr, len);
}

template<typename T>
mstl::pair<T*, ptrdiff_t> get_temporary_buffer(ptrdiff_t len) {
    return get_buffer_helper(len, static_cast<T*>(0));
}

template<typename T>
mstl::pair<T*, ptrdiff_t> get_temporary_buffer(ptrdiff_t len, T*) {
    return get_buffer_helper(len, static_cast<T*>(0));
}

template<typename T>
void release_temporary_buffer(T* ptr) {
    free(ptr);
}


// 缓冲区类，进行临时缓冲区的申请和释放
template<typename ForwardIterator, typename T>
class temporary_buffer {
private:
    ptrdiff_t   original_len;    // 想要申请的缓冲区大小
    ptrdiff_t   len;             // 实际申请的缓冲区大小
    T*          buffer;          // 指向缓冲区的指针
public:
    temporary_buffer(ForwardIterator first, ForwardIterator last);

    ~temporary_buffer() {
        mstl::destory(buffer, buffer + len);
    }

    ptrdiff_t size() const noexcept {
        return len;
    }

    ptrdiff_t requested_size() const noexcept {
        return original_len;
    }

    T* begin() noexcept {
        return buffer;
    }

    T* end() noexcept {
        return buffer + len;
    }
private:
    void allocate_buffer();
    void initialize_buffer(const T&, std::true_type) {}
    void initialize_buffer(const T& value, std::false_type) {
        mstl::uninitialized_fill_n(buffer, len, value);
    }
private:
    temporary_buffer(const temporary_buffer&);
    void operator=(const temporary_buffer&);
};

template<typename ForwardIterator, typename T>
temporary_buffer<ForwardIterator, T>::
temporary_buffer(ForwardIterator first, ForwardIterator last) {
    try {
        len = mstl::distance(first, last);
        allocate_buffer();
        if (len > 0) {
            initialize_buffer(*first, std::is_trivially_default_constructible<T>());
        }
    }
    catch (...) {
        free(buffer);
        buffer = nullptr;
        len = 0;
    }
}

template<typename ForwardIterator, typename T>
void temporary_buffer<ForwardIterator, T>::allocate_buffer() {
    original_len = len;
    if (len > static_cast<ptrdiff_t>(INT_MAX / sizeof(T))) {
        len = INT_MAX / sizeof(T);
    }
    while (len > 0) {
        buffer = static_cast<T*>(malloc(len * sizeof(T)));
        if (buffer != nullptr) {
            break;
        }
        len /= 2;
    }
}

// auto_ptr, 小型智能指针，不建议使用
template<typename T>
class auto_ptr {
public:
    typedef T elem_type;
private:
    T* m_ptr;
public:
    explicit auto_ptr(T* p = nullptr) : m_ptr(p) {}
    // 拷贝构造的参数不是const修饰的，使用了release方法释放了传入的对象
    // 从而保证资源的独占，导致拷贝过程中被拷贝对象被修改也就是拷贝对象与被拷贝对象不等价
    // 这种拷贝行为虽然保证了资源的独占性，但是却导致原有数据不可用，存在较大问题
    auto_ptr(auto_ptr& p) : m_ptr(p.release()) {}
    template<typename U>
    auto_ptr(auto_ptr<U>& p) : m_ptr(p.release()) {}

    // 同样赋值函数参数也未被const修饰，调用了release方法
    auto_ptr& operator=(auto_ptr& p) {
        if (&p != this) {
            delete m_ptr;
            m_ptr = p.release();
        }
    }
    template<typename U>
    auto_ptr& operator=(auto_ptr<U>& p) {
        if (&p != this) {
            delete m_ptr;
            m_ptr = p.release();
        }
    }

    T* release() {
        T* temp = m_ptr;
        m_ptr = nullptr;
        return temp;
    }
    T* get() {
        return m_ptr;
    }

    void reset(T* p = nullptr) {
        if (m_ptr != p) {
            delete m_ptr;
            m_ptr = p;
        }
    }
public:
    // 重载符号，构造pointer-like class
    T& operator*() const {
        return *m_ptr;
    }
    T* operator->() const {
        return m_ptr;
    }
};




} //mstl



#endif