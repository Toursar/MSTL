#ifndef M_CONSTRUCT_H_
#define M_CONSTRUCT_H_

#include <new>

#include "m_type_traits.h"
#include "m_iterator.h"

// 包含两个函数construct和destory

namespace mstl{

template<typename Ty>
void construct(Ty* ptr) {
    new (ptr) Ty();
}

template <class T1, class T2>
inline void _construct(T1* p, const T2& value) {
    new(p) T1(value);
}

template<typename Ty, typename... Args>
void construct(Ty* ptr, Args&& ...args) {
    new (ptr) Ty(mstl::forward<Args>(args)...);
}

template<typename Ty>
void destory_one(Ty*, std::true_type) {}

template<typename Ty>
void destory_one(Ty* ptr, std::false_type) {
    if (ptr != nullptr) {
        ptr->~Ty();
    }
}

template<typename Ty>
void destory(Ty* ptr) {
    // 这里is_trivially_destructible<Ty>{}构建临时对象，由于此对象重载了value_type类型，
    // 因此这个对象可以转换为value_type类型stl源码中这个函数返回value值，也就等价于
    // std::is_trivially_destructible<Ty>::value
    destory_one(ptr, std::is_trivially_destructible<Ty>{});
}

template<typename ForwardIterator>
void destory_cat(ForwardIterator, ForwardIterator, std::true_type) {}

template<typename ForwardIterator>
void destory_cat(ForwardIterator first, ForwardIterator last, std::false_type) {
    for (;first != last; ++first) {
        destory(&*first);
    }
}

template<typename ForwardIterator>
void destory(ForwardIterator first, ForwardIterator last) {
    destory_cat(first, last, std::is_trivially_destructible<
                typename iterator_traits<ForwardIterator>::value_type>{});
}

}//mstl

#endif