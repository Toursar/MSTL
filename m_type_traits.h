#ifndef M_TYPE_TRAITS_H_
#define M_TYPE_TRAITS_H_

#include <type_traits>

namespace mstl {
    // 通过value辅助判断true和false
    template<typename T, T v>
    struct m_integral_constant {
        static constexpr T value = v;
        typedef T value_type;
        // 此处重载value_type类型，也就是T，这样的话
        // m_integral_constant对象可以直接转换为value_type类型
        constexpr value_type operator()() {
            return value;
        }
    };

    template<bool B>
    using m_bool_constant = m_integral_constant<bool, B>;

    // 这里的true_type，false_type在进行判定是否相同时需要与stl库中的其他traits配合使用
    // 本程序未实现复杂的traits，因此这里的两个定义会出现不能使用的情况，依旧需要std::true_type
    // true_type::value将会为true，反之亦然
    typedef m_bool_constant<true> true_type;
    typedef m_bool_constant<false> false_type;

    // 判断是否带const
    // 下面继承false_type仅为了提取value_type，不涉及判同, 所以mstl中的false_type也可以
    template<typename T>
    struct is_const : false_type {};
    // 偏特化版本，如果带const将会进入下面的偏特化形式
    template<typename T>
    struct is_const<const T> : true_type {};

    // 判断是否为pair
    template<typename T1, typename T2>
    // 提前声明一个mstl中的pair
    struct pair;

    template<typename T>
    struct is_pair : false_type {};
    
    template<typename T1, typename T2>
    struct is_pair<mstl::pair<T1, T2>> : true_type {};

}

#endif

// 简单的测试程序，需要在其他单独的cpp文件中运行
// int main() {
//     const int a = 1;
//     int b = 1;
//     mstl::pair<int, int> c(1,1);
//     std::cout << mstl::is_const<decltype(a)>::value;
//     return 0;
// }