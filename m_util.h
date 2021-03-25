#ifndef M_UTIL_H_
#define M_UTIL_H_

#include "m_type_traits.h"
#include <iostream>
#include <cstddef>

namespace mstl{

// 实现了一些通用性的函数，move forward swap等

    template<typename T>
    struct remove_reference {
        using type = T;
    };

    // 两个偏特化版本
    template<typename T>
    struct remove_reference<T&> {
        using type = T;
    };

    template<typename T>
    struct remove_reference<T&&> {
        using type = T;
    };

    // move函数的实现
    template<typename T>
    typename mstl::remove_reference<T>::type&& move(T&& arg) noexcept {
        using return_type = typename mstl::remove_reference<T>::type&&;
        return static_cast<return_type>(arg);
    }

    // forward实现
    // 在实现转发的情况下只会调用第一种函数，因为函数的参数均为左值
    // 只有在直接只用forward而不是函数内的转发时才会调用第二种函数
    template<typename T>
    T&& forward(typename mstl::remove_reference<T>::type& arg) noexcept {
        return static_cast<T&&>(arg);
    }

    template<typename T>
    T&& forward(typename mstl::remove_reference<T>::type&& arg) noexcept {
        static_assert(!std::is_lvalue_reference<T>::value, "bad forward");
        return static_cast<T&&>(arg);
    }

    // swap实现
    template<typename T>
    void swap(T& lhs, T& rhs) {
        T temp = mstl::move(lhs);
        lhs = mstl::move(rhs);
        rhs = mstl::move(temp);
    }

    template<typename forwardIter1, typename forwardIter2>
    forwardIter2 swap_ranges(forwardIter1 first1, forwardIter1 last1,
                            forwardIter2 first2) {
        while (first1 != last1) {
            mstl::swap(*first1, *first2);
            ++first1;
            ++first2;
        }
        return first2;
    }

    template<typename Tp, size_t N>
    void swap(Tp (&first1)[N], Tp (&first2)[N]) {
        mstl::swap_ranges(first1, first1 + N, first2);
    }

    // enable_if
    template<bool b, typename T = void>
    struct enable_if {};

    template<typename T>
    struct enable_if<true, T>
    {
        using type = T;
    };
    

//-------------------------pair--------------------------
    template<typename Ty1, typename Ty2>
    struct pair {
        // 为pair的两个类型命名
        typedef Ty1 first_type;
        typedef Ty2 second_type;

        // 保存pair的两个数据
        first_type first;
        second_type second;

        // 使用enable_if创建构造函数
        template<typename Other1 = Ty1, typename Other2 = Ty2,
                typename = typename mstl::enable_if<std::is_default_constructible<Other1>::value
                && std::is_default_constructible<Other2>::value, void>::type>
        constexpr pair() : first(), second() {
            std::cout << "1\n";
        }


        // 可进行隐式构造的构造函数
        template<typename U1 = Ty1, typename U2 = Ty2,
                typename mstl::enable_if<std::is_copy_constructible<U1>::value &&
                std::is_copy_constructible<U2>::value &&
                std::is_convertible<const U1&, Ty1>::value &&
                std::is_convertible<const U2&, Ty2>::value, bool>::type = false>
        constexpr pair(const Ty1& _first, const Ty2& _second) 
                                : first(_first), second(_second) {}

        // 进行显式构造的构造函数
        template<typename U1 = Ty1, typename U2 = Ty2,
                typename mstl::enable_if<std::is_copy_constructible<U1>::value &&
                std::is_copy_constructible<U2>::value &&
                (!std::is_convertible<const U1&, Ty1>::value ||
                !std::is_convertible<const U2&, Ty2>::value), bool>::type = false>
        explicit constexpr  pair(const Ty1& _first, const Ty2& _second) 
                                : first(_first), second(_second) {}

        pair(const pair& lhs) = default;
        pair(pair&& rhs) = default;

        // 可进行隐式构造的右值构造函数
        template<typename U1 = Ty1, typename U2 = Ty2,
                typename mstl::enable_if<std::is_copy_constructible<U1>::value &&
                std::is_copy_constructible<U2>::value &&
                std::is_convertible<const U1&, Ty1>::value &&
                std::is_convertible<const U2&, Ty2>::value, bool>::type = false>
        constexpr pair(U1&& _first, U2&& _second) 
                        : first(mstl::forward<U1>(_first)), second(mstl::forward<U2>(_second)) {}

        // 进行显式构造的右值构造函数
        template<typename U1 = Ty1, typename U2 = Ty2,
                typename mstl::enable_if<std::is_copy_constructible<U1>::value &&
                std::is_copy_constructible<U2>::value &&
                (!std::is_convertible<const U1&, Ty1>::value ||
                !std::is_convertible<const U2&, Ty2>::value), bool>::type = false>
        explicit constexpr pair(U1&& _first, U2&& _second) 
                        : first(mstl::forward<U1>(_first)), second(mstl::forward<U2>(_second))  {}

        // 可进行隐式构造的拷贝构造函数
        template<typename U1 = Ty1, typename U2 = Ty2,
                typename mstl::enable_if<std::is_copy_constructible<U1>::value &&
                std::is_copy_constructible<U2>::value &&
                std::is_convertible<const U1&, Ty1>::value &&
                std::is_convertible<const U2&, Ty2>::value, bool>::type = false>
        constexpr pair(const pair<U1, U2>& other) 
                            : first(other.first), second(other.second) {}

        // 进行显式构造的拷贝构造函数
        template<typename U1 = Ty1, typename U2 = Ty2,
                typename mstl::enable_if<std::is_copy_constructible<U1>::value &&
                std::is_copy_constructible<U2>::value &&
                (!std::is_convertible<const U1&, Ty1>::value ||
                !std::is_convertible<const U2&, Ty2>::value), bool>::type = false>
        explicit constexpr pair(const pair<U1, U2>& other) 
                    : first(other.first), second(other.second) {}

        // 可进行隐式构造的右值拷贝构造函数
        template<typename U1 = Ty1, typename U2 = Ty2,
                typename mstl::enable_if<std::is_copy_constructible<U1>::value &&
                std::is_copy_constructible<U2>::value &&
                std::is_convertible<const U1&, Ty1>::value &&
                std::is_convertible<const U2&, Ty2>::value, bool>::type = false>
        constexpr pair(pair<U1, U2>&& other) 
                    : first(mstl::forward<U1>(other.first)), second(mstl::forward<U2>(other.second)) {}

        // 进行显式构造的右值拷贝构造函数
        template<typename U1 = Ty1, typename U2 = Ty2,
                typename mstl::enable_if<std::is_copy_constructible<U1>::value &&
                std::is_copy_constructible<U2>::value &&
                (!std::is_convertible<const U1&, Ty1>::value ||
                !std::is_convertible<const U2&, Ty2>::value), bool>::type = false>
        explicit constexpr pair(pair<U1, U2>&& other) 
                    : first(mstl::forward<U1>(other.first)), second(mstl::forward<U2>(other.second))  {}

        // 赋值构造函数
        pair& operator=(const pair& lhs) {
            if (this != &lhs) {
                first = lhs.first;
                second = lhs.second;
            }
            return *this;
        }

        pair& operator=(const pair&& rhs) {
            if (this != &rhs) {
                first = mstl::move(rhs.first);
                second = mstl::move(rhs.second);
            }
            return *this;
        }

        // 其他类型pair的拷贝构造函数
        template<typename U1, typename U2>
        pair& operator=(const pair<U1, U2>& lhs) {
            first = lhs.first;
            second = lhs.second;
            return *this;
        }

        template<typename U1, typename U2>
        pair& operator=(const pair<U1, U2>&& rhs) {
            first = mstl::move(rhs.first);
            second = mstl::move(rhs.second);
            return *this;
        }

        ~pair() = default;

        void swap(pair& other) {
            if (this != &other) {
                mstl::swap(first, other.first);
                mstl::swap(second, other.second);
            }
        }

    };

    // 重载一部分pair相关的全局操作符
    template<typename Ty1, typename Ty2>
    bool operator==(const pair<Ty1, Ty2>& pair1, const pair<Ty1, Ty2>& pair2) {
        return pair1.first == pair2.first && pair1.second == pair2.second;
    }

    template<typename Ty1, typename Ty2>
    bool operator<(const pair<Ty1, Ty2>& pair1, const pair<Ty1, Ty2>& pair2) {
        return pair1.first == pair2.first ? pair1.second < pair2.second : pair1.first < pair2.first;
    }

    template<typename Ty1, typename Ty2>
    bool operator!=(const pair<Ty1, Ty2>& pair1, const pair<Ty1, Ty2>& pair2) {
        return !(pair1 == pair2);
    }

    template<typename Ty1, typename Ty2>
    bool operator>(const pair<Ty1, Ty2>& pair1, const pair<Ty1, Ty2>& pair2) {
        return pair2 < pair1;
    }

    template<typename Ty1, typename Ty2>
    bool operator<=(const pair<Ty1, Ty2>& pair1, const pair<Ty1, Ty2>& pair2) {
        return !(pair1 > pair2);
    }

    template<typename Ty1, typename Ty2>
    bool operator>=(const pair<Ty1, Ty2>& pair1, const pair<Ty1, Ty2>& pair2) {
        return !(pair1 < pair2);
    }

    template<typename Ty1, typename Ty2>
    void swap(const pair<Ty1, Ty2>& pair1, const pair<Ty1, Ty2>& pair2) {
        pair1.swap(pair2);
    }

    // 定义一个将两变量绑定为pair的函数
    template<typename Ty1, typename Ty2>
    pair<Ty1, Ty2> make_pair(Ty1&& _first, Ty2&& _second) {
        return pair<Ty1, Ty2>(forward<Ty1>(_first), forward<Ty2>(_second));
    }

}//mstl

#endif

// 简单的测试，需要在其他单独的cpp文件中运行
// move，forward测试类--------------------------
// void func(int& a) {
//     std::cout << "Lref\n";
// }

// void func(int&& a) {
//     std::cout << "Rref\n";
// }

// template<typename T>
// void test(T&& a) {
//     func(mstl::forward<T>(a));
// }

// //--------------------------------------------

// //swap测试类-----------------------------------
// template<typename T>
// class A {
//     T m_data;
// public:
//     A(T data) : m_data(data) {}
//     friend std::ostream& operator<<(std::ostream& os, const A<T>& temp) {
//         os << temp.m_data;
//         return os;
//     }
// };

// //--------------------------------------------

// int main() {
//     // move，forward测试--------------------------

//     int num = 1;
//     test(mstl::move(num));
//     func(mstl::forward<int>(mstl::move(num)));

//     // --------------------------------------------

//     // swap测试------------------------------------

//     A<double> a(1.1);
//     A<double> b(2.2);
//     std::cout << a << " " << b;
//     mstl::swap(a, b);
//     std::cout << a << " " << b;

//     int a1[5] = {1,2,3,4,5};
//     int b1[5] = {5,4,3,2,1};
//     mstl::swap(a1, b1);
//     for (int i = 0; i < 5; ++i) {
//         std::cout << a1[i] << " " << b1[i] << "\n";
//     }

//     A<int> x(1);
//     A<int> y(2);
//     mstl::pair<A<int>, int> c(x, 1);
//     mstl::pair<A<int>, A<int>> d(x, y);
//     d = mstl::make_pair(2.3, 4);
//     std::cout << c.first << " " << c.second;

//     // --------------------------------------------
//     return 0;
// }