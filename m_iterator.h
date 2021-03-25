#ifndef M_ITERATOR_H_
#define M_ITERATOR_H_

#include <cstddef>
#include "m_type_traits.h"

// 迭代器的实现，以及反向迭代器

namespace mstl{

    // 五种迭代器的类型
    // 输入迭代器，只读
    struct input_iterator_tag{};
    // 输出迭代器，只写
    struct output_iterator_tag{};
    // 前向迭代器，只能向前即++
    struct forward_iterator_tag : public input_iterator_tag {};
    // 双向迭代器，即可++也可--
    struct bidirectional_iterator_tag : public forward_iterator_tag {};
    // 随机存储迭代器，可以++，--也可以跳转至任意位置
    struct random_access_iterator_tag : public bidirectional_iterator_tag {};
    
    // iterator的定义
    template<typename Category, typename T, typename Distance = ptrdiff_t,
            typename Pointer = T*, typename Reference = T&>
    struct iterator {
        typedef Category    iterator_category;
        typedef T           value_type;
        typedef Pointer     pointer;
        typedef Reference   reference;
        typedef Distance    difference_type;
    };
    
    // 判断传入的T是否存在iterator_category
    template<typename T>
    struct has_iterator_cat {
    private:
        struct twoChar {char a; char b;};

        // 此函数接受任意参数，返回为twoChar，2字节
        template<typename U>
        static twoChar test(...);

        // 只有对象U存在iterator_category才有char返回值，1字节
        template<typename U>
        static char test(typename U::iterator_category* = 0);
    public:
        static const bool value = sizeof(test<T>(0)) == sizeof(char); 
    };

    template<typename Iterator, bool>
    struct iterator_traits_impl {};

    template<typename Iterator>
    struct iterator_traits_impl<Iterator, true> {
        typedef typename Iterator::iterator_category  iterator_category;
        typedef typename Iterator::value_type         value_type;
        typedef typename Iterator::pointer            pointer;
        typedef typename Iterator::reference          reference;
        typedef typename Iterator::difference_type    difference_type;
    };

    template<typename Iterator, bool>
    struct iterator_traits_helper {};

    template<typename Iterator>
    struct iterator_traits_helper<Iterator, true> 
        : public iterator_traits_impl<Iterator,
        std::is_convertible<typename Iterator::iterator_category, input_iterator_tag>::value ||
        std::is_convertible<typename Iterator::iterator_category, output_iterator_tag>::value>{
    };

    // iterator_traits
    template<typename Iterator>
    struct iterator_traits : public iterator_traits_helper<Iterator, 
                                    has_iterator_cat<Iterator>::value> {};
    
    // 对于指针类型的偏特化
    template<typename T>
    struct iterator_traits<T*> {
        typedef random_access_iterator_tag  iterator_category;
        typedef T                           value_type;
        typedef T*                          pointer;
        typedef T&                          reference;
        typedef ptrdiff_t                   difference_type;
    };

    template<typename T>
    struct iterator_traits<const T*> {
        typedef random_access_iterator_tag  iterator_category;
        typedef T                           value_type;
        typedef const T*                    pointer;
        typedef const T&                    reference;
        typedef ptrdiff_t                   difference_type;
    };
    
    // 判断T的iterator_category与U能否转换，即判断T的iterator性质
    template<typename T, typename U, bool = has_iterator_cat<iterator_traits<T>>::value>
    struct has_iterator_cat_of : public m_bool_constant<std::is_convertible<
                    typename iterator_traits<T>::iterator_category, U>::value> {};


    // 判断迭代器类型
    // 下面继承false_type仅为了提取value_type，不涉及判同, 所以mstl中的false_type也可以
    template<typename T, typename U>
    struct has_iterator_cat_of<T, U, false> : public false_type {};

    template<typename Iter>
    struct is_input_iterator : public has_iterator_cat_of<Iter, input_iterator_tag> {};

    template<typename Iter>
    struct is_output_iterator : public has_iterator_cat_of<Iter, output_iterator_tag> {};

    template<typename Iter>
    struct is_forward_iterator : public has_iterator_cat_of<Iter, forward_iterator_tag> {};

    template<typename Iter>
    struct is_bidirectional_iterator : public has_iterator_cat_of<Iter, bidirectional_iterator_tag> {};

    template<typename Iter>
    struct is_random_access_iterator : public has_iterator_cat_of<Iter, random_access_iterator_tag> {};
    
    template<typename Iter>
    struct is_iterator : public m_bool_constant<is_input_iterator<Iter>::value ||
                                                is_output_iterator<Iter>::value> {};

    // 萃取迭代器的category
    template<typename Iterator>
    typename iterator_traits<Iterator>::iterator_category
    iterator_category(const Iterator&) {
        typedef typename iterator_traits<Iterator>::iterator_category Category;
        return Category();
    }

    // 萃取迭代器的distance_type
    template<typename Iterator>
    typename iterator_traits<Iterator>::difference_type*
    difference_type(const Iterator&) {
        return static_cast<typename iterator_traits<Iterator>::difference_type*>(0);
    }

    // 萃取迭代器的value_type
    template<typename Iterator>
    typename iterator_traits<Iterator>::value_type*
    value_type(const Iterator&) {
        return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
    }

    // 计算迭代器的距离函数

    // input_iterator版本
    template<typename InputIterator>
    typename iterator_traits<InputIterator>::difference_type
    distance_dispatch(InputIterator first, InputIterator last, input_iterator_tag) {
        typename iterator_traits<InputIterator>::difference_type n = 0;
        while (first != last) {
            ++first;
            ++n;
        }
        return n;
    }

    template<typename RandomIter>
    typename iterator_traits<RandomIter>::difference_type
    distance_dispatch(RandomIter first, RandomIter last, random_access_iterator_tag) {
        return last - first;
    }

    // 对外使用的接口
    template<typename InputIterator>
    typename iterator_traits<InputIterator>::difference_type
    distance(InputIterator first, InputIterator last) {
        return distance_dispatch(first, last, iterator_category(first));
    }

    // 让迭代器前进的函数实现

    // input_iterator版本实现
    template<typename InputIterator, typename Distance>
    void advance_dispatch(InputIterator& iter, Distance n, input_iterator_tag) {
        while (n > 0) {
            --n;
            ++iter;
        }
    }

    // bidirectional_iterator版本实现
    template<typename BidirectionalIterator, typename Distance>
    void advance_dispatch(BidirectionalIterator& iter, Distance n, bidirectional_iterator_tag) {
        if (n >= 0) {
            while (n--) ++iter;
        } else {
            while (n++) --iter;
        }
    }

    // bidirectional_iterator版本实现
    template<typename randomIter, typename Distance>
    void advance_dispatch(randomIter& iter, Distance n, random_access_iterator_tag) {
        iter += n;
    }

    // 对外的advance接口
    template<typename InputIterator, typename Distance>
    void  advance(InputIterator& iter, Distance n) {
        advance_dispatch(iter, n, iterator_category(iter));
    }

    //--------------------------------------------------------------------
    // reverse_iterator类
    template<typename Iterator>
    class reverse_iterator {
    private:
        Iterator current;
    public:
        typedef typename iterator_traits<Iterator>::iterator_category  iterator_category;
        typedef typename iterator_traits<Iterator>::value_type         value_type;
        typedef typename iterator_traits<Iterator>::pointer            pointer;
        typedef typename iterator_traits<Iterator>::reference          reference;
        typedef typename iterator_traits<Iterator>::difference_type    difference_type;
        typedef Iterator                                               iterator_type;
        typedef reverse_iterator<Iterator>                             self;

        reverse_iterator() {}
        explicit reverse_iterator(const iterator_type iter) : current(iter) {}
        reverse_iterator(const self& rev_iter) : current(rev_iter.current) {}

        iterator_type base() const {
            return current;
        }

        // 实际对应着正向迭代器的前一个位置
        reference operator*() const {
            Iterator temp = current;
            return *(--temp);
        }

        pointer operator->() const {
            return &(operator*());
        }

        self& operator++() {
            --current;
            return *this;
        }

        self operator++(int) {
            self temp = this;
            --current;
            return temp;
        }

        self& operator--() {
            ++current;
            return *this;
        }

        self operator--(int) {
            self temp = this;
            ++current;
            return temp;
        }

        self& operator+=(difference_type n) {
            current -= n;
            return *this;
        }

        self operator+(difference_type n) const {
            return self(current - n);
        }

        self& operator-=(difference_type n) {
            current += n;
            return *this;
        }

        self operator-(difference_type n) const {
            return self(current + n);
        }

        reference operator[](difference_type n) {
            return *(*this + n);
        }

    };

    // 重载全局运算符
    template<typename Iterator>
    typename reverse_iterator<Iterator>::difference_type
    operator-(const reverse_iterator<Iterator>& first,
              const reverse_iterator<Iterator>& second) {
        return second.base() - first.base();
    }

    template<typename Iterator>
    bool operator==(const reverse_iterator<Iterator>& first,
                    const reverse_iterator<Iterator>& second) {
        return first.base() == second.base();
    }

    template<typename Iterator>
    bool operator<(const reverse_iterator<Iterator>& first,
                    const reverse_iterator<Iterator>& second) {
        // 与iterator相反
        return second.base() < first.base();
    }

    template<typename Iterator>
    bool operator!=(const reverse_iterator<Iterator>& first,
                    const reverse_iterator<Iterator>& second) {
        return !(first == second);
    }

    template<typename Iterator>
    bool operator>(const reverse_iterator<Iterator>& first,
                    const reverse_iterator<Iterator>& second) {
        return second < first;
    }

    template<typename Iterator>
    bool operator<=(const reverse_iterator<Iterator>& first,
                    const reverse_iterator<Iterator>& second) {
        return !(second < first);
    }

    template<typename Iterator>
    bool operator>=(const reverse_iterator<Iterator>& first,
                    const reverse_iterator<Iterator>& second) {
        return !(first < second);
    }

} //mstl


#endif