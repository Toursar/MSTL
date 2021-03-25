#ifndef M_ALGO_H
#define M_ALGO_H

#include <cstddef>
#include <ctime>

#include "m_algobase.h"
#include "m_functional.h"
#include "m_memory.h"
#include "m_heap_algo.h"

namespace mstl {

/************************************************************************************************/
// lower_bound在[first, last)的范围内寻找第一个不小于value的元素并返回，没有则返回last

template<typename ForwardIter, typename T>
ForwardIter
lbound_dispatch(ForwardIter first, ForwardIter last,
                const T& value, mstl::forward_iterator_tag) {
    size_t len = mstl::distance(first, last);
    size_t half = len;
    ForwardIter mid;  
    while (len > 0) {
        half = len >> 1;
        mid = first;
        mstl::advance(mid, half);
        if (*mid < value) {
            first = mid;
            ++first;
            len = len - half - 1;
        } else {
            len = half;
        }
    }
    return first;
}

template<typename RandomIter, typename T>
RandomIter
lbound_dispatch(RandomIter first, RandomIter last,
                const T& value, mstl::random_access_iterator_tag) {
    size_t len = last - first;
    size_t half = len;
    RandomIter mid;  
    while (len > 0) {
        half = len >> 1;
        mid = first + half;
        if (*mid < value) {
            first = mid + 1;
            len = len - half - 1;
        } else {
            len = half;
        }
    }
    return first;
}

template<typename Iter, typename T>
Iter lower_bound(Iter first, Iter last, const T& value) {
    return mstl::lbound_dispatch(first, last, value, mstl::iterator_category(first));
}

// 自定义比较算法的情况
template<typename ForwardIter, typename T, typename Compare>
ForwardIter
lbound_dispatch(ForwardIter first, ForwardIter last,
                const T& value, mstl::forward_iterator_tag, Compare cmp) {
    size_t len = mstl::distance(first, last);
    size_t half = len;
    ForwardIter mid;  
    while (len > 0) {
        half = len >> 1;
        mid = first;
        mstl::advance(mid, half);
        if (cmp(*mid, value)) {
            first = mid;
            ++first;
            len = len - half - 1;
        } else {
            len = half;
        }
    }
    return first;
}

template<typename RandomIter, typename T, typename Compare>
RandomIter
lbound_dispatch(RandomIter first, RandomIter last, const T& value,
                mstl::random_access_iterator_tag, Compare cmp) {
    size_t len = last - first;
    size_t half = len;
    RandomIter mid;  
    while (len > 0) {
        half = len >> 1;
        mid = first + half;
        if (cmp(*mid, value)) {
            first = mid + 1;
            len = len - half - 1;
        } else {
            len = half;
        }
    }
    return first;
}

template<typename Iter, typename T, typename Compare>
Iter lower_bound(Iter first, Iter last, const T& value, Compare cmp) {
    return mstl::lbound_dispatch(first, last, value, mstl::iterator_category(first), cmp);
}

/************************************************************************************************/










// is_permutation判断[first1, last1)是否为[first2, last2)的排列组合
template<typename ForwardIter1, typename ForwardIter2, typename BinaryPred>
bool is_permutation_aux(ForwardIter1 first1, ForwardIter1 last1,
                        ForwardIter2 first2, ForwardIter2 last2, BinaryPred pred) {
    constexpr bool is_random_iter = (mstl::is_random_access_iterator<ForwardIter1>::value
                        && mstl::is_random_access_iterator<ForwardIter2>::value);
    if (is_random_iter) {
        size_t len1 = mstl::distance(first1, last1);
        size_t len2 = mstl::distance(first2, last2);
        if (len1 != len2) {
            return false;
        }
    }
    // 先去掉公共前缀
    for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
        if (!pred(*first1, *first2)) {
            break;
        }
    }
    if (is_random_iter) {
        if (first1 == last1) {
            return true;
        }
    } else {
        size_t len1 = mstl::distance(first1, last1);
        size_t len2 = mstl::distance(first2, last2);
        if (len1 == 0 && len2 == 0) {
            return true;
        }
        if (len1 != len2) {
            return false;
        }
    }
    for (ForwardIter1 i = first1; i != last1; ++i) {
        // 记录当前位置的值是否已经判断过了
        bool is_repeated = false;
        for (ForwardIter1 j = first1; j != i; ++j) {
            if (pred(*i, *j)) {
                is_repeated = true;
                break;
            }
        }
        // 如果尚未判断则进行判断
        if (!is_repeated) {
            size_t freq2 = 0;
            for (ForwardIter2 j = first2; j != last2; ++j) {
                if (pred(*j, *i)) {
                    ++freq2;
                }
            }
            if (freq2 == 0) {
                return false;
            }
            size_t freq1 = 1;
            for (ForwardIter1 j = i; j != last1; ++j) {
                if (pred(*j, *i)) {
                    ++freq1;
                }
            }
            if (freq1 != freq2) {
                return false;
            }
        }
    }
    return true;
}

template<typename ForwardIter1, typename ForwardIter2, typename BinaryPred>
bool is_permutation(ForwardIter1 first1, ForwardIter1 last1,
                    ForwardIter2 first2, ForwardIter2 last2, BinaryPred pred) {
    return is_permutation_aux(first1, last1, first2, last2, pred);
}

template<typename ForwardIter1, typename ForwardIter2>
bool is_permutation(ForwardIter1 first1, ForwardIter1 last1,
                    ForwardIter2 first2, ForwardIter2 last2) {
    typedef typename mstl::iterator_traits<ForwardIter1>::value_type v1;
    typedef typename mstl::iterator_traits<ForwardIter2>::value_type v2;
    static_assert(std::is_same<v1, v2>::value, "the type should be same in mystl::is_permutation");
    return is_permutation_aux(first1, last1, first2, last2, mstl::equal_to<v1>());
}
/************************************************************************************************/


} // mstl

#endif