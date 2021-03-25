#ifndef M_ALGOBASE_H_
#define M_ALGOBASE_H_

// 包含部分基本算法

#include <cstring>

#include "m_iterator.h"
#include "m_util.h"

namespace mstl {

// 如果定义了max和min宏，就删除该宏，防止后面定义的max，min函数产生问题
#ifdef max
#pragma message("#undefing macro max")
#undef max
#endif

#ifdef min
#pragma message("#undefing macro min")
#undef min
#endif

/******************************max min***************************************/
// max，min定义，相等返回第一参数
template<typename T>
T max(const T& lhs, const T& rhs) {
    return lhs < rhs ? rhs : lhs;
}

template<typename T, typename Compare>
T max(const T& lhs, const T& rhs, Compare cmp) {
    return cmp(lhs, rhs) ? rhs : lhs;
}

template<typename T>
T min(const T& lhs, const T& rhs) {
    return rhs < lhs ? rhs : lhs;
}

template<typename T, typename Compare>
T min(const T& lhs, const T& rhs, Compare cmp) {
    return cmp(rhs, lhs) ? rhs : lhs;
}

/****************************************************************************/

/******************************iter_swap*************************************/
// 交换迭代器指向的对象
template<typename Iter1, typename Iter2>
void iter_swap(Iter1 iter1, Iter2 iter2) {
    mstl::swap(*iter1, *iter2);
}
/****************************************************************************/

/*******************************copy*************************************/
// 把区间[first, last)拷贝到result为起点的位置
template<typename InputIterator, typename OutputIterator>
OutputIterator unchecked_copy_cat(InputIterator first, InputIterator last,
                OutputIterator result, mstl::input_iterator_tag) {
                    for (; first != last; ++first, ++result) {
                        *result = *first;
                    }
                    return result;
                }

template<typename InputIterator, typename OutputIterator>
OutputIterator unchecked_copy_cat(InputIterator first, InputIterator last,
                OutputIterator result, mstl::random_access_iterator_tag) {
                    for (auto n = last - first; n > 0; --n, ++first, ++result) {
                        *result = *first;
                    }
                    return result;
                }

template<typename InputIterator, typename OutputIterator>
OutputIterator unchecked_copy(InputIterator first, InputIterator last, OutputIterator result) {
    return unchecked_copy_cat(first, last, result, iterator_category(first));
}

// 偏特化版本，如果U的拷贝构造是trivial的，直接进行内存拷贝即可
template<typename T, typename U>
typename mstl::enable_if<std::is_same<typename std::remove_const<T>::type, U>::value &&
        std::is_trivially_copy_assignable<U>::value, U*>::type
unchecked_copy(T* first, T* last, U* result) {
    const size_t n = static_cast<size_t>(last - first);
    if (n != 0) {
        std::memmove(result, first, n * sizeof(U));
    }
    return result + n;
}

template<typename T, typename U>
U copy(T first, T last, U result) {
    return unchecked_copy(first, last, result);
}

/*******************************************************************************/

/*******************************copy_backward*************************************/
// 把区间[first, last)拷贝到result为终点的位置
template<typename BidirectionalIter1, typename BidirectionalIter2>
BidirectionalIter2
unchecked_copy_backward_cat(BidirectionalIter1 first, BidirectionalIter1 last,
                BidirectionalIter2 result, mstl::bidirectional_iterator_tag) {
    while (first != last) {
        // iterator的last位置指向空，应该先--再赋值
        *--result = *--last;
    }
    return result;
}

template<typename BidirectionalIter1, typename BidirectionalIter2>
BidirectionalIter2
unchecked_copy_backward_cat(BidirectionalIter1 first, BidirectionalIter1 last,
                BidirectionalIter2 result, mstl::random_access_iterator_tag) {
    for (auto n = last - first; n > 0; --n) {
        // iterator的last位置指向空，应该先--再赋值
        *--result = *--last;
    }
    return result;
}

template<typename BidirectionalIter1, typename BidirectionalIter2>
BidirectionalIter2
unchecked_copy_backward(BidirectionalIter1 first, BidirectionalIter1 last,
                BidirectionalIter2 result) {
    return unchecked_copy_backward_cat(first, last, result, mstl::iterator_category(first));
}

// 偏特化版本，如果U的拷贝构造是trivial的，直接进行内存拷贝即可
template<typename T, typename U>
typename mstl::enable_if<std::is_same<typename std::remove_const<T>::type, U>::value &&
        std::is_trivially_copy_assignable<U>::value, U*>::type
unchecked_copy_backward(T* first, T* last, U* result) {
    const size_t n = static_cast<size_t>(last - first);
    if (n != 0) {
        result -= n;
        std::memmove(result, first, n * sizeof(U));
    }
    return result;
}

template<typename T, typename U>
U copy_backward(T first, T last, U result) {
    return unchecked_copy_backward(first, last, result);
}

/*******************************************************************************/

/*******************************copy_if*************************************/
// 把[first，last)中满足一元操作符unaryop的元素拷贝到result起始的位置
template<typename InputIterator, typename OutputIterator, typename UnaryPredicate>
OutputIterator copy_if(InputIterator first, InputIterator last,
                    OutputIterator result, UnaryPredicate unaryop) {
    for (; first != last; ++first) {
        if (unaryop(*first)) {
            *result++ = *first;
        }
    }
    return result;
}

/*******************************************************************************/

/*******************************copy_n*************************************/
// 把[first，first + n)的元素拷贝到[result，result + n)的位置，并返回pair其中
// 包含两个拷贝位置的结束点
template<typename InputIterator, typename OutputIterator>
mstl::pair<InputIterator, OutputIterator>
unchecked_copy_n(InputIterator first, size_t n, OutputIterator result, mstl::input_iterator_tag) {
    for (; n > 0; --n, ++first, ++result) {
        *result = *first;
    }
    return mstl::pair<InputIterator, OutputIterator>(first, result);
}

template<typename InputIterator, typename OutputIterator>
mstl::pair<InputIterator, OutputIterator>
unchecked_copy_n(InputIterator first, size_t n, OutputIterator result,
                mstl::random_access_iterator_tag) {
    InputIterator last = first + n;
    return mstl::pair<InputIterator, OutputIterator>(last, mstl::copy(first, last, result));
}

template<typename InputIterator, typename OutputIterator>
mstl::pair<InputIterator, OutputIterator>
copy_n(InputIterator first, size_t n, OutputIterator result) {
    return unchecked_copy_n(first, n, result, mstl::iterator_category(first));
}

/*******************************************************************************/

/*******************************move*************************************/
// 把 [first, last)区间内的元素移动到 [result, result + (last - first))内
template<typename InputIterator, typename OutputIterator>
OutputIterator unchecked_move_cat(InputIterator first, InputIterator last,
                    OutputIterator result, mstl::input_iterator_tag) {
    for (; first != last; ++first, ++result) {
        *result = mstl::move(*first);
    }
    return result;
}

template<typename InputIterator, typename OutputIterator>
OutputIterator unchecked_move_cat(InputIterator first, InputIterator last,
                    OutputIterator result, mstl::random_access_iterator_tag) {
    for (auto n = last - first; n > 0; --n, ++first, ++result) {
        *result = mstl::move(*first);
    }
    return result;
}

template<typename InputIterator, typename OutputIterator>
OutputIterator
unchecked_move(InputIterator first, InputIterator last, OutputIterator result) {
    return unchecked_move_cat(first, last, result, mstl::iterator_category(first));
}

// 偏特化版本，如果U的移动构造是trivial的，直接进行内存拷贝即可
template<typename T, typename U>
typename mstl::enable_if<std::is_same<typename std::remove_const<T>::type, U>::value &&
        std::is_trivially_move_assignable<U>::value, U*>::type
unchecked_move(T* first, T* last, U* result) {
    const size_t n = static_cast<size_t>(last - first);
    if (n != 0) {
        std::memmove(result, first, n * sizeof(U));
    }
    return result + n;
}

template<typename InputIterator, typename OutputIterator>
OutputIterator
move(InputIterator first, InputIterator last, OutputIterator result) {
    return unchecked_move(first, last, result);
}

/*******************************************************************************/

/*******************************move_backward*************************************/
// 把区间[first, last)移动到result为终点的位置
template<typename BidirectionalIter1, typename BidirectionalIter2>
BidirectionalIter2
unchecked_move_backward_cat(BidirectionalIter1 first, BidirectionalIter1 last,
                BidirectionalIter2 result, mstl::bidirectional_iterator_tag) {
    while (first != last) {
        // iterator的last位置指向空，应该先--再赋值
        *--result = mstl::move(*--last);
    }
    return result;
}

template<typename BidirectionalIter1, typename BidirectionalIter2>
BidirectionalIter2
unchecked_move_backward_cat(BidirectionalIter1 first, BidirectionalIter1 last,
                BidirectionalIter2 result, mstl::random_access_iterator_tag) {
    for (auto n = last - first; n > 0; --n) {
        // iterator的last位置指向空，应该先--再赋值
        *--result = mstl::move(*--last);
    }
    return result;
}

template<typename BidirectionalIter1, typename BidirectionalIter2>
BidirectionalIter2
unchecked_move_backward(BidirectionalIter1 first, BidirectionalIter1 last,
                BidirectionalIter2 result) {
    return unchecked_move_backward_cat(first, last, result, mstl::iterator_category(first));
}

// 偏特化版本，如果U的拷贝构造是trivial的，直接进行内存拷贝即可
template<typename T, typename U>
typename mstl::enable_if<std::is_same<typename std::remove_const<T>::type, U>::value &&
        std::is_trivially_move_assignable<U>::value, U*>::type
unchecked_move_backward(T* first, T* last, U* result) {
    const size_t n = static_cast<size_t>(last - first);
    if (n != 0) {
        result -= n;
        std::memmove(result, first, n * sizeof(U));
    }
    return result;
}

template<typename T, typename U>
U move_backward(T first, T last, U result) {
    return unchecked_move_backward(first, last, result);
}

/*******************************************************************************/

/*******************************equal*************************************/
// 比较[first1，last)中的每个元素是否与first2开头的元素相同
template<typename InputIter1, typename InputIter2>
bool equal(InputIter1 first1, InputIter1 last, InputIter2 first2) {
    for (; first1 != last; ++first1, ++first2) {
        if (*first1 != *first2) {
            return false;
        }
    }
    return true;
}

// 自定义比较函数的版本
template<typename InputIter1, typename InputIter2, typename Compare>
bool equal(InputIter1 first1, InputIter1 last, InputIter2 first2, Compare cmp) {
    for (; first1 != last; ++first1, ++first2) {
        if (!cmp(*first1, *first2)) {
            return false;
        }
    }
    return true;
}

/*******************************************************************************/

/*******************************fill_n*************************************/
// 从first开始，填充n个值
template<typename OutputIter, typename Size, typename T>
OutputIter unchecked_fill_n(OutputIter first, Size n, const T& value) {
    for (; n > 0; --n, ++first) {
        *first = value;
    }
    return first;
}

// 1 byte的偏特化版本
template<typename T, typename Size, typename U>
typename mstl::enable_if<std::is_integral<T>::value && sizeof(T) == 1 &&
!std::is_same<T, bool>::value && std::is_integral<U>::value &&
sizeof(U) == 1, T*>::type
unchecked_fill_n(T* first, Size n, U value) {
    if (n > 0) {
        std::memset(first, (unsigned char)value, (size_t)(n));
    }
    return first + n;
}

template<typename OutputIter, typename Size, typename T>
OutputIter fill_n(OutputIter first, Size n, const T& value) {
    return unchecked_fill_n(first, n, value);
}

/*******************************************************************************/

/*******************************fill*************************************/
// 为[first，last)区间的元素赋新值
template<typename ForwardIter, typename T>
void fill_cat(ForwardIter first, ForwardIter last,
                const T& value, mstl::forward_iterator_tag) {
    for (; first != last; ++first) {
        *first = value;
    }
}

template<typename ForwardIter, typename T>
void fill_cat(ForwardIter first, ForwardIter last,
                const T& value, mstl::random_access_iterator_tag) {
    fill_n(first, last - first, value);
}

template<typename ForwardIter, typename T>
void fill(ForwardIter first, ForwardIter last, const T& value) {
    fill_cat(first, last, value, mstl::iterator_category(first));
}

/*****************************************************************************************/
// lexicographical_compare, 返回较小
// 以字典序排列对两个序列进行比较，当在某个位置发现第一组不相等元素时，有下列几种情况：
// (1)如果第一序列的元素较小，返回 true ，否则返回 false
// (2)如果到达 last1 而尚未到达 last2 返回 true
// (3)如果到达 last2 而尚未到达 last1 返回 false
// (4)如果同时到达 last1 和 last2 返回 false
/*****************************************************************************************/
template<typename InputIter1, typename InputIter2>
bool lexicographical_compare(InputIter1 first1, InputIter1 last1,
                             InputIter2 first2, InputIter2 last2) {
    for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
        if (*first1 < *first2) {
            return true;
        }
        if (*first2 < *first1) {
            return false;
        }
    }
    return first1 == last1 && first2 != last2;
}

template<typename InputIter1, typename InputIter2, typename Compare>
bool lexicographical_compare(InputIter1 first1, InputIter1 last1,
                    InputIter2 first2, InputIter2 last2, Compare cmp) {
    for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
        if (cmp(*first1, *first2)) {
            return true;
        }
        if (cmp(*first2, *first1)) {
            return false;
        }
    }
    return first1 == last1 && first2 != last2;
}

// 为const unsigned char提供偏特化版本
bool lexicographical_compare(const unsigned char* first1,
                             const unsigned char* last1,
                             const unsigned char* first2,
                             const unsigned char* last2) {
    const auto len1 = last1 - first1;
    const auto len2 = last2 - first2;
    const int result = std::memcmp(first1, first2, mstl::min(len1, len2));
    return result != 0 ? result < 0 : len1 < len2;
}

/*******************************************************************************/

/*******************************mismatch*************************************/
// 比较两个序列，寻找第一个不相等的位置，并返回一个pair包含两个序列位置的迭代器
template<typename InputIter1, typename InputIter2>
mstl::pair<InputIter1, InputIter2>
mismatch(InputIter1 first1, InputIter1 last, InputIter2 first2) {
    while (first1 != last && *first1 == *first2) {
        ++first1;
        ++first2;
    }
    return mstl::pair<InputIter1, InputIter2>(first1, first2);
}

// 自定义相等操作
template<typename InputIter1, typename InputIter2, typename Compare>
mstl::pair<InputIter1, InputIter2>
mismatch(InputIter1 first1, InputIter1 last, InputIter2 first2, Compare cmp) {
    while (first1 != last && cmp(*first1, *first2)) {
        ++first1;
        ++first2;
    }
    return mstl::pair<InputIter1, InputIter2>(first1, first2);
}

}//mstl

#endif