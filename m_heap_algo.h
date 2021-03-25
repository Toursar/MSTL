#ifndef M_HEAP_ALGO_H
#define M_HEAP_ALGO_H

// 构造堆算法，也是priority queue数据结构的基础算法
// 需要注意这里实现堆结构并没有从像堆排序那样，忽略0号位置，从1开始进行堆排序
// 这里实现堆结构使用的是迭代器[first, last)范围，相当于我们使用迭代器的距离表示
// 数组的下标，那么迭代器的距离是从0开始的那么父节点，左右子节点的关系也发生了相应的变化
// parent = (i - 1) / 2，left = 2 * parent + 1, right = 2 * parent + 2
#include "m_iterator.h"

namespace mstl {

/**************************************************************************************************/
// push_heap实现了堆的插入操作，此时last - 1位置为新插入的元素，待平衡
template<typename RandomIter, typename Distance, typename T>
void push_heap_aux(RandomIter first, Distance holeIndex, Distance topIndex, T value) {
    auto parent = (holeIndex - 1) / 2;
    // 这里判断的是*(first + parent) < value也就表明此为最大堆，堆顶元素最大
    while (holeIndex > topIndex && *(first + parent) < value) {
        *(first + holeIndex) = *(first + parent);
        holeIndex = parent;
        parent = (holeIndex - 1) / 2;
    }
    *(first + holeIndex) = value;
}

template<typename RandomIter, typename Distance>
void push_heap_p(RandomIter first, RandomIter last, Distance*) {
    // 注意此处holeIndex传入的是last - first - 1，
    // 原因是last迭代器位置并没有元素，新加入的元素实际为last - 1位置的元素
    return push_heap_aux(first, (last - first) - 1, static_cast<Distance>(0), *(last - 1));
}

template<typename RandomIter, typename Distance>
void push_heap(RandomIter first, RandomIter last) {
    push_heap_p(first, last, difference_type(first));
}

// 自定义比较方法
template<typename RandomIter, typename Distance, typename T, typename Compare>
void push_heap_aux(RandomIter first, Distance holeIndex, Distance topIndex, T value, Compare cmp) {
    auto parent = (holeIndex - 1) / 2;
    while (holeIndex > topIndex && cmp(*(first + parent), value)) {
        *(first + holeIndex) = *(first + parent);
        holeIndex = parent;
        parent = (holeIndex - 1) / 2;
    }
    *(first + holeIndex) = value;
}

template<typename RandomIter, typename Distance, typename Compare>
void push_heap_p(RandomIter first, RandomIter last, Distance*, Compare cmp) {
    return push_heap_aux(first, (last - first) - 1, static_cast<Distance>(0), *(last - 1), cmp);
}

template<typename RandomIter, typename Compare>
void push_heap(RandomIter first, RandomIter last, Compare cmp) {
    push_heap_p(first, last, difference_type(first), cmp);
}

/**************************************************************************************************/
// pop_heap删除堆顶的元素，首先将顶部元素放在容器尾部，然后平衡堆即可。

template<typename RandomIter, typename T, typename Distance>
void adjust_heap(RandomIter first, Distance holeIndex, Distance len, T value) {
    // 从holeIndex位置开始进行"下沉"操作
    Distance topIndex = holeIndex;
    Distance right = 2 * holeIndex + 2;
    while (right < len) {
        if (*(first + right) < *(first + right - 1)) {
            --right;
        }
        *(first + holeIndex) = *(first + right);
        holeIndex = right;
        right = 2 * holeIndex + 2;
    }
    // 此为边界条件，如果刚好没有右节点，那么只能与左子节点交换位置
    if (right == len) {
        *(first + holeIndex) = *(first + right - 1);
        holeIndex = right - 1;
    }
    // 这里还需要进行上浮操作，因为上述并非交换节点的值，仅是将堆顶的元素沉到底部而已
    // 并没有在下沉过程中将每个节点与value比较，因此，value不一定是在叶子节点可能还需要上浮。
    mstl::push_heap_aux(first, holeIndex, topIndex, value);
}


template<typename RandomIter, typename T, typename Distance>
void pop_heap_aux(RandomIter first, RandomIter last, RandomIter result, T value, Distance*) {
    *result = *first;
    mstl::adjust_heap(first, static_cast<Distance>(0), last - first, value);
}

template<typename RandomIter>
void pop_heap(RandomIter first, RandomIter last) {
    mstl::pop_heap_aux(first, last - 1, last - 1, *(last - 1), difference_type(first));
}

// 自定义比较大小方法
template<typename RandomIter, typename T, typename Distance, typename Compare>
void adjust_heap(RandomIter first, Distance holeIndex, Distance len, T value, Compare cmp) {
    Distance topIndex = holeIndex;
    Distance right = 2 * holeIndex + 2;
    while (right < len) {
        if (cmp(*(first + right), *(first + right - 1))) {
            --right;
        }
        *(first + holeIndex) = *(first + right);
        holeIndex = right;
        right = 2 * holeIndex + 2;
    }
    if (right == len) {
        *(first + holeIndex) = *(first + right - 1);
        holeIndex = right - 1;
    }
    mstl::push_heap_aux(first, holeIndex, topIndex, value, cmp);
}


template<typename RandomIter, typename T, typename Distance, typename Compare>
void pop_heap_aux(RandomIter first, RandomIter last, RandomIter result, T value, Distance*, Compare cmp) {
    *result = *first;
    mstl::adjust_heap(first, static_cast<Distance>(0), last - first, value, cmp);
}

template<typename RandomIter, typename Compare>
void pop_heap(RandomIter first, RandomIter last, Compare cmp) {
    mstl::pop_heap_aux(first, last - 1, last - 1, *(last - 1), difference_type(first), cmp);
}

/**************************************************************************************************/
// sort_heap, 将堆顶不断的弹出，从而构造由于的数据

template<typename RandomIter>
void sort_heap(RandomIter first, RandomIter last) {
    while (last - first > 1) {
        mstl::pop_heap(first, last--);
    }
}

template<typename RandomIter, typename Compare>
void sort_heap(RandomIter first, RandomIter last, Compare cmp) {
    while (last - first > 1) {
        mstl::pop_heap(first, last--, cmp);
    }
}

/**************************************************************************************************/
// make_heap将[first, last)的数据构造为堆结构

template<typename RandomIter, typename Distance>
void make_heap_aux(RandomIter first, RandomIter last, Distance*) {
    if (last - first < 2) {
        return;
    }
    Distance len = last - first;
    // 注意这里没有-1而是-2原因是last迭代器指向的位置为空，需要额外-1
    Distance holeIndex = (len - 2) / 2;
    // 接下来从第一个根节点holeIndex开始依次平衡堆结构
    // 直到平衡到0位置，这样[first, last)范围的元素就已经形成堆结构
    while (true) {
        mstl::adjust_heap(first, holeIndex, len, *(first + holeIndex));
        if (holeIndex == 0) {
            return;
        }
        --holeIndex;
    }
}

// 自定义比较大小的方法
template<typename RandomIter>
void make_heap(RandomIter first, RandomIter last) {
    mstl::make_heap_aux(first, last, difference_type(first));
}

template<typename RandomIter, typename Distance, typename Compare>
void make_heap_aux(RandomIter first, RandomIter last, Distance*, Compare cmp) {
    if (last - first < 2) {
        return;
    }
    Distance len = last - first;
    Distance holeIndex = (len - 2) / 2;
    while (true) {
        mstl::adjust_heap(first, holeIndex, len, *(first + holeIndex), cmp);
        if (holeIndex == 0) {
            return;
        }
        --holeIndex;
    }
}

template<typename RandomIter, typename Compare>
void make_heap(RandomIter first, RandomIter last, Compare cmp) {
    mstl::make_heap_aux(first, last, difference_type(first), cmp);
}

} // mstl

#endif