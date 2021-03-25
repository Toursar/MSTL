#ifndef M_UNINITIALIZED_H_
#define M_UNINITIALIZED_H_

// 用于对未初始化的空间进行构造

#include "m_algobase.h"
#include "m_construct.h"
#include "m_iterator.h"
#include "m_type_traits.h"
#include "m_util.h"

namespace mstl {

/********************************uninitialized_copy**************************************/
// 将[first，last)的元素复制到result起始的位置，返回复制的末尾
template<typename InputIterator, typename ForwardIterator>
ForwardIterator unchecked_uninit_copy(InputIterator first, InputIterator last,
                    ForwardIterator result, std::true_type) {
    return mstl::copy(first, last, result);
}

template<typename InputIterator, typename ForwardIterator>
ForwardIterator unchecked_uninit_copy(InputIterator first, InputIterator last,
                    ForwardIterator result, std::false_type) {
    ForwardIterator cur = result;
    try {
        for (; first != last; ++first, ++cur) {
            mstl::construct(&*cur, *first);
        }
    }
    catch (...) {
        for (; result != cur; ++result) {
            mstl::destory(&*result);
        }
    }
    return cur;
}

template<typename InputIterator, typename ForwardIterator>
ForwardIterator
uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result) {
    return unchecked_uninit_copy(first, last, result,
                std::is_trivially_copy_assignable<typename mstl::iterator_traits<
                ForwardIterator>::value_type>{});
}

/****************************************************************************************/

/********************************uninitialized_copy_n**************************************/
// 拷贝[first, first + n)的元素到以result为起始的位置，返回拷贝的结尾
template<typename InputIterator, typename Size, typename ForwardIterator>
ForwardIterator
unchecked_uninit_copy_n(InputIterator first, Size n, ForwardIterator result,
                        std::true_type) {
    return mstl::copy_n(first, n, result).second;
}

template<typename InputIterator, typename Size, typename ForwardIterator>
ForwardIterator
unchecked_uninit_copy_n(InputIterator first, Size n, ForwardIterator result,
                        std::false_type) {
    ForwardIterator cur = result;
    try {
        for (; n > 0; --n, ++cur, ++first) {
            mstl::construct(&*cur, *first);
        }
    }
    catch (...) {
        for (; result != cur; ++result) {
            mstl::destory(&*result);
        }
    }
    return cur;
}

template<typename InputIterator, typename Size, typename ForwardIterator>
ForwardIterator
uninitialized_copy_n(InputIterator first, Size n, ForwardIterator result) {
    return mstl::unchecked_uninit_copy_n(first, n, result,
                std::is_trivially_copy_assignable<typename mstl::iterator_traits<
                ForwardIterator>::value_type>{});
}

/****************************************************************************************/

/********************************uninitialized_fill**************************************/
// 填充[first，last)区间
template<typename ForwardIterator, typename T>
void unchecked_uninit_fill(ForwardIterator first, ForwardIterator last,
                            const T& value, std::true_type) {
    mstl::fill(first, last, value);
}

template<typename ForwardIterator, typename T>
void unchecked_uninit_fill(ForwardIterator first, ForwardIterator last,
                        const T& value, std::false_type) {
    ForwardIterator cur = first;
    try {
        for (; cur != last; ++cur) {
            mstl::construct(&*cur, value);
        }
    }
    catch (...) {
        for (; first != cur; ++first) {
            mstl::destory(&*first);
        }
    }
}

template<typename ForwardIterator, typename T>
void uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& value) {
    return unchecked_uninit_fill(first, last, value,
                std::is_trivially_copy_assignable<typename mstl::iterator_traits<
                ForwardIterator>::value_type>{});
}

/****************************************************************************************/

/********************************uninitialized_fill_n**************************************/
// 填充[first, first + n)的元素
template<typename ForwardIterator, typename Size, typename T>
ForwardIterator
unchecked_uninit_fill_n(ForwardIterator first, Size n, const T& value,
                        std::true_type) {
    return mstl::fill_n(first, n, value);
}

template<typename ForwardIterator, typename Size, typename T>
ForwardIterator
unchecked_uninit_fill_n(ForwardIterator first, Size n, const T& value,
                        std::false_type) {
    ForwardIterator cur = first;
    try {
        for (; n > 0; --n, ++cur) {
            mstl::construct(&*cur, value);
        }
    }
    catch (...) {
        for (; first != cur; ++first) {
            mstl::destory(&*first);
        }
    }
    return cur;
}

template<typename ForwardIterator, typename Size, typename T>
ForwardIterator
uninitialized_fill_n(ForwardIterator first, Size n, const T& value) {
    return unchecked_uninit_fill_n(first, n, value,
                std::is_trivially_copy_assignable<typename mstl::iterator_traits<
                ForwardIterator>::value_type>{});
}

/****************************************************************************************/

/********************************uninitialized_move**************************************/
// 移动[first，last)区间元素，到result位置，返回移动的末尾
template<typename InputIterator, typename ForwardIterator>
ForwardIterator unchecked_uninit_move(InputIterator first, InputIterator last,
                    ForwardIterator result, std::true_type) {
    return mstl::move(first, last, result);
}

template<typename InputIterator, typename ForwardIterator>
ForwardIterator unchecked_uninit_move(InputIterator first, InputIterator last,
                    ForwardIterator result, std::false_type) {
    ForwardIterator cur = result;
    try {
        for (; first != last; ++first, ++cur) {
            mstl::construct(&*cur, mstl::move(*first));
        }
    }
    catch (...) {
        mstl::destory(result, cur);
    }
    return cur;
}

template<typename InputIterator, typename ForwardIterator>
ForwardIterator
uninitialized_move(InputIterator first, InputIterator last, ForwardIterator result) {
    return unchecked_uninit_move(first, last, result,
                std::is_trivially_copy_assignable<typename mstl::iterator_traits<
                ForwardIterator>::value_type>{});
}

/****************************************************************************************/

/********************************uninitialized_move_n**************************************/
// 移动[first，first + n)区间元素，到result位置，返回移动的末尾
template<typename InputIterator, typename Size, typename ForwardIterator>
ForwardIterator
unchecked_uninit_move_n(InputIterator first, Size n, ForwardIterator result,
                        std::true_type) {
    return mstl::move(first, first + n, result);
}

template<typename InputIterator, typename Size, typename ForwardIterator>
ForwardIterator
unchecked_uninit_move_n(InputIterator first, Size n, ForwardIterator result,
                        std::false_type) {
    ForwardIterator cur = result;
    try {
        for (; n > 0; --n, ++cur, ++first) {
            mstl::construct(&*cur, mstl::move(*first));
        }
    }
    catch (...) {
        for (; result != cur; ++result) {
            mstl::destory(&*result);
        }
    }
    return cur;
}

template<typename InputIterator, typename Size, typename ForwardIterator>
ForwardIterator
uninitialized_move_n(InputIterator first, Size n, ForwardIterator result) {
    return unchecked_uninit_move_n(first, n, result,
                std::is_trivially_copy_assignable<typename mstl::iterator_traits<
                ForwardIterator>::value_type>{});
}


} //mstl


#endif