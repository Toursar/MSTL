#ifndef M_FUNCTIONAL_H_
#define M_FUNCTIONAL_H_

// 包含一些仿函数以及hash函数
#include <functional>
#include <cstddef>

namespace mstl{

// 一元仿函数的参数与返回
template<typename Arg, typename Result>
struct unarg_function {
    typedef Arg      argument_type;
    typedef Result   result_type;
};

// 二元仿函数的参数与返回
template<typename Arg1, typename Arg2, typename Result>
struct binary_function {
    typedef Arg1      first_argument_type;
    typedef Arg2      second_argument_type;
    typedef Result    result_type;
};


template<typename T>
struct plus : public binary_function<T, T, T> {
    T operator()(const T& first, const T& last) const {
        return first + last;
    } 
};

template<typename T>
struct minus : public binary_function<T, T, T> {
    T operator()(const T& first, const T& last) const {
        return first - last;
    } 
};

template<typename T>
struct multiplies : public binary_function<T, T, T> {
    T operator()(const T& first, const T& last) const {
        return first * last;
    } 
};

template<typename T>
struct divides : public binary_function<T, T, T> {
    T operator()(const T& first, const T& last) const {
        return first / last;
    } 
};

template<typename T>
struct modulus : public binary_function<T, T, T> {
    T operator()(const T& first, const T& last) const {
        return first % last;
    } 
};

template<typename T>
struct nagate : public unarg_function<T, T> {
    T operator()(const T& first) const {
        return -first;
    } 
};

template<typename T>
T identity_element(plus<T>) {
    return T(0);
}

template<typename T>
T identity_element(multiplies<T>) {
    return T(1);
}

template<typename T>
struct equal_to : public binary_function<T, T, bool> {
    bool operator()(const T& first, const T& last) const {
        return first == last;
    } 
};

template<typename T>
struct not_equal_to : public binary_function<T, T, bool> {
    bool operator()(const T& first, const T& last) const {
        return first != last;
    } 
};

template<typename T>
struct greater : public binary_function<T, T, bool> {
    bool operator()(const T& first, const T& last) const {
        return first > last;
    } 
};

template<typename T>
struct less : public binary_function<T, T, bool> {
    bool operator()(const T& first, const T& last) const {
        return first < last;
    } 
};

template<typename T>
struct greater_equal : public binary_function<T, T, bool> {
    bool operator()(const T& first, const T& last) const {
        return first >= last;
    } 
};

template<typename T>
struct less_equal : public binary_function<T, T, bool> {
    bool operator()(const T& first, const T& last) const {
        return first <= last;
    } 
};

template<typename T>
struct logical_and : public binary_function<T, T, bool> {
    bool operator()(const T& first, const T& last) const {
        return first && last;
    } 
};

template<typename T>
struct logical_or : public binary_function<T, T, bool> {
    bool operator()(const T& first, const T& last) const {
        return first || last;
    } 
};

template<typename T>
struct logical_not : public unarg_function<T, bool> {
    bool operator()(const T& first) const {
        return !first;
    } 
};

// 不会改变传入元素，返回const的自身
template<typename T>
struct identity : public unarg_function<T, T> {
    const T& operator()(const T& first) const {
        return first;
    } 
};

template<typename Pair>
struct select_first : public unarg_function<Pair, typename Pair::first_type> {
    const typename Pair::first_type& operator()(const Pair& x) const {
        return x.first;
    } 
};

template<typename Pair>
struct select_second : public unarg_function<Pair, typename Pair::second_type> {
    const typename Pair::second_type& operator()(const Pair& x) const {
        return x.second;
    } 
};

template<typename Arg1, typename Arg2>
struct projectfirst : public binary_function<Arg1, Arg2, Arg1> {
    Arg1 operator()(const Arg1& arg1, const Arg2& arg2) const {
        return arg1;
    }
};

template<typename Arg1, typename Arg2>
struct projectsecond : public binary_function<Arg1, Arg2, Arg2> {
    Arg2 operator()(const Arg1& arg1, const Arg2& arg2) const {
        return arg2;
    }
};

/********************************hash函数************************************/
// 哈希函数的仿函数
template<typename Key>
struct hash {};

// 对于指针的偏特化
template<typename Key>
struct hash<Key*> : public unarg_function<Key*, size_t> {
    size_t operator()(Key* key) const noexcept {
        return reinterpret_cast<size_t>(key);
    }
};

// 整型的哈希函数
#define MSTL_TRIVIAL_HASH_FUNC(Type)                        \
template<>                                                  \
struct hash<Type> : public unarg_function<Type, size_t>  {  \
    size_t operator()(Type val) const noexcept {            \
        return static_cast<size_t>(val);                    \
    }                                                       \
};

MSTL_TRIVIAL_HASH_FUNC(bool)

MSTL_TRIVIAL_HASH_FUNC(char)

MSTL_TRIVIAL_HASH_FUNC(signed char)

MSTL_TRIVIAL_HASH_FUNC(unsigned char)

MSTL_TRIVIAL_HASH_FUNC(wchar_t)

MSTL_TRIVIAL_HASH_FUNC(char16_t)

MSTL_TRIVIAL_HASH_FUNC(char32_t)

MSTL_TRIVIAL_HASH_FUNC(short)

MSTL_TRIVIAL_HASH_FUNC(unsigned short)

MSTL_TRIVIAL_HASH_FUNC(int)

MSTL_TRIVIAL_HASH_FUNC(unsigned int)

MSTL_TRIVIAL_HASH_FUNC(long)

MSTL_TRIVIAL_HASH_FUNC(unsigned long)

MSTL_TRIVIAL_HASH_FUNC(long long)

MSTL_TRIVIAL_HASH_FUNC(unsigned long long)

#undef MSTL_TRIVIAL_HASH_FUNC

/****************************************************************************/

template<>
struct hash<float> : public unarg_function<float, size_t> {
    size_t operator()(float val) const noexcept {
        return val != 0.0f ? std::_Hash_impl::hash(val) : 0;
    }
};

template<>
struct hash<double> : public unarg_function<double, size_t> {
    size_t operator()(double val) const noexcept {
        return val != 0.0 ? std::_Hash_impl::hash(val) : 0;
    }
};

template<>
struct hash<long double> : public unarg_function<long double, size_t> {
    size_t operator()(long double val) const noexcept {
        return val != 0.0 ? std::_Hash_impl::hash(val) : 0;
    }
};

}//mstl

#endif