#ifndef M_NUMERIC_H_
#define M_NUMERIC_H_

// 一些常用的数值算法

#include "m_iterator.h"
#include <iterator>

namespace mstl {

/*********************************accumulate******************************************/

// init为初值，累加first到last的值
template<typename InputIterator, typename T>
T accumulate(InputIterator first, InputIterator last, T init) {
    for (; first != last; ++first) {
        init += *first;
    }
    return init;
}

// 以init为初值，累加first到last的值，累加方式为BinaryOp
template<typename InputIterator, typename T, typename BinaryOp>
T accumulate(InputIterator first, InputIterator last, T init, BinaryOp binary_op) {
    for (; first != last; ++first) {
        init = binary_op(init, *first);
    }
    return init;
}

/*******************************************************************************************/

/*********************************adjacent_difference***************************************/

// 计算相邻元素的差值，结果保存到result起的区间上
template<typename InputIterator, typename OutputIterator>
OutputIterator adjacent_difference(InputIterator first, InputIterator last, OutputIterator result) {
    if (first == last) {
        return result;
    }
    typedef typename mstl::iterator_traits<InputIterator>::value_type  value_type;
    *result = *first;
    value_type value = *first;
    while (++first != last) {
        value_type temp = *first;
        *++result = temp - value;
        value = temp;
    }
    return ++result;
}

// 计算相邻元素的差值，结果保存到result起的区间上，差值计算使用自定义函数
template<typename InputIterator, typename OutputIterator, typename BinaryOp>
OutputIterator adjacent_difference(InputIterator first, InputIterator last,
                            OutputIterator result, BinaryOp binary_op) {
    typedef typename mstl::iterator_traits<InputIterator>::value_type  value_type;
    if (first == last) {
        return result;
    }
    *result = *first;
    value_type value = *first;
    while (++first != last) {
        value_type temp = *first;
        *++result = binary_op(temp, value);
        value = temp;
    }
    return ++result;
}

/*******************************************************************************************/

/*********************************inner_product***************************************/

// 以init为初值计算first到last范围的内积
template<typename InputIterator1, typename InputIterator2, typename T>
T inner_product(InputIterator1 first1, InputIterator1 last, InputIterator1 first2, T init) {
    for (;first1 != last; ++first1, ++first2) {
        init = init + (*first1 * *first2);
    }
    return init;
}

template<typename InputIterator1, typename InputIterator2, typename T,
                     typename BinaryOp1, typename BinaryOp2>
T inner_product(InputIterator1 first1, InputIterator1 last, InputIterator1 first2,
                T init, BinaryOp1 binary_op1, BinaryOp2 binary_op2) {
    for (;first1 != last; ++first1, ++first2) {
        init = binary_op1(init, binary_op2(*first1, *first2));
    }
    return init;
}

/*******************************************************************************************/

/**************************************iota********************************************/

// 填充[first, last)，以value开始递增
template<typename InputIterator, typename T>
void iota(InputIterator first, InputIterator last, T value) {
    for (; first != last; ++first) {
        *first = value;
        ++value;
    }
}

/*******************************************************************************************/

/**************************************partial_sum********************************************/

// 局部求和，结果保存到result为起始的位置，求前缀和
template<typename InputIterator, typename OutputIterator>
OutputIterator partial_sum(InputIterator first, InputIterator last, OutputIterator result) {
    typedef typename mstl::iterator_traits<InputIterator>::value_type  value_type;
    if (first == last) {
        return result;
    }
    *result = *first;
    value_type value = *first;
    while (++first != last) {
        value = value + *first;
        *++result = value;
    }
    return result;
}

// 局部求和，结果保存到result为起始的位置，求和方式为自定义
template<typename InputIterator, typename OutputIterator, typename BinaryOp>
OutputIterator partial_sum(InputIterator first, InputIterator last,
                            OutputIterator result, BinaryOp binary_op) {
    typedef typename mstl::iterator_traits<InputIterator>::value_type  value_type;
    if (first == last) {
        return result;
    }
    *result = *first;
    value_type value = *first;
    while (++first != last) {
        value = binary_op(value, *first);
        *++result = value;
    }
    return result;
}

}// mstl

#endif