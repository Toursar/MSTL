#ifndef M_BASIC_STRING_H_
#define M_BASIC_STRING_H_

//包含一个模板类basic_string，字符串类
#include "m_iterator.h"
#include "m_memory.h"
#include "m_functional.h"
#include "m_exceptdef.h"

namespace mstl {
// char_traits
template<typename CharType>
struct char_traits {
    typedef CharType char_type;

    static size_t length(const char_type* str) {
        size_t len = 0;
        for (; *str != char_type(0) ; ++str) {
            ++len;
        }
        return len;
    }

    static int compare(const char_type* str1, const char_type* str2, size_t n) {
        for (; n > 0; --n, ++str1, ++str2) {
            if (*str1 < *str2) {
                return -1;
            } else if (*str2 < *str1) {
                return 1;
            }
        }
        return 0;
    }

    static char_type* copy(char_type* dst, const char_type* src, size_t n) {
        MSTL_DEBUG(dst + n <= src || src + n <= dst);
        char_type* c = dst;
        for (; n > 0; --n, ++dst, ++src) {
            *dst = *src;
        }
        return c;
    }

    static char_type* move(char_type* dst, const char_type* src, size_t n) {
        char_type* c = dst;
        if (dst < src) {
            for (; n != 0; --n, ++src, ++dst) {
                *dst = *src;
            }
        } else if (src < dst) {
            dst += n;
            src += n;
            for (; n != 0; --n) {
                *(--dst) = *(--src);
            }
        }
        return c;
    }

    static char_type* fill(char_type* dst, char_type ch, size_t count) {
        char_type* c = dst;
        for (; count > 0; --count, ++dst) {
            *dst = ch;
        }
        return c;
    }
};

// char的偏特化
template<>
struct char_traits<char> {
    typedef char char_type;

    static size_t length(const char_type* str) noexcept {
        return std::strlen(str);
    }
    static int compare(const char_type* str1, const char_type* str2, size_t n) noexcept {
        return std::memcmp(str1, str2, n);
    }

    static char_type* copy(char_type* dst, const char_type* src, size_t n) noexcept {
        MSTL_DEBUG(dst + n <= src || src + n <= dst);
        return static_cast<char_type*>(std::memcpy(dst, src, n));
    }

    static char_type* move(char_type* dst, const char_type* src, size_t n) noexcept {
        return static_cast<char_type*>(std::memmove(dst, src, n));
    }

    static char_type* fill(char_type* dst, char_type ch, size_t count) {
        return static_cast<char_type*>(std::memset(dst, ch, count));
    }
};

// char的偏特化
template<>
struct char_traits<wchar_t> {
    typedef wchar_t char_type;

    static size_t length(const char_type* str) noexcept {
        return std::wcslen(str);
    }
    static int compare(const char_type* str1, const char_type* str2, size_t n) noexcept {
        return std::wmemcmp(str1, str2, n);
    }

    static char_type* copy(char_type* dst, const char_type* src, size_t n) noexcept {
        MSTL_DEBUG(dst + n <= src || src + n <= dst);
        return static_cast<char_type*>(std::wmemcpy(dst, src, n));
    }

    static char_type* move(char_type* dst, const char_type* src, size_t n) noexcept {
        return static_cast<char_type*>(std::wmemmove(dst, src, n));
    }

    static char_type* fill(char_type* dst, char_type ch, size_t count) {
        return static_cast<char_type*>(std::wmemset(dst, ch, count));
    }
};

template<>
struct char_traits<char16_t> {
    typedef char16_t char_type;

    static size_t length(const char_type* str) {
        size_t len = 0;
        for (; *str != char_type(0) ; ++str) {
            ++len;
        }
        return len;
    }

    static int compare(const char_type* str1, const char_type* str2, size_t n) {
        for (; n != 0; --n, ++str1, ++str2) {
            if (*str1 < *str2) {
                return -1;
            } else if (*str2 < *str1) {
                return 1;
            }
        }
        return 0;
    }

    static char_type* copy(char_type* dst, const char_type* src, size_t n) {
        MSTL_DEBUG(dst + n <= src || src + n <= dst);
        char_type* c = dst;
        for (; n != 0; --n, ++dst, ++src) {
            *dst = *src;
        }
        return c;
    }

    static char_type* move(char_type* dst, const char_type* src, size_t n) {
        char_type* c = dst;
        if (dst < src) {
            for (; n != 0; --n, ++src, ++dst) {
                *dst = *src;
            }
        } else if (src < dst) {
            dst += n;
            src += n;
            for (; n != 0; --n) {
                *(--dst) = *(--src);
            }
        }
        return c;
    }

    static char_type* fill(char_type* dst, char_type ch, size_t count) {
        char_type* c = dst;
        for (; count > 0; --count, ++dst) {
            *dst = ch;
        }
        return c;
    }
};

template<>
struct char_traits<char32_t> {
    typedef char32_t char_type;

    static size_t length(const char_type* str) {
        size_t len = 0;
        for (; *str != char_type(0) ; ++str) {
            ++len;
        }
        return len;
    }

    static int compare(const char_type* str1, const char_type* str2, size_t n) {
        for (; n != 0; --n, ++str1, ++str2) {
            if (*str1 < *str2) {
                return -1;
            } else if (*str2 < *str1) {
                return 1;
            }
        }
        return 0;
    }

    static char_type* copy(char_type* dst, const char_type* src, size_t n) {
        MSTL_DEBUG(dst + n <= src || src + n <= dst);
        char_type* c = dst;
        for (; n != 0; --n, ++dst, ++src) {
            *dst = *src;
        }
        return c;
    }

    static char_type* move(char_type* dst, const char_type* src, size_t n) {
        char_type* c = dst;
        if (dst < src) {
            for (; n != 0; --n, ++src, ++dst) {
                *dst = *src;
            }
        } else if (src < dst) {
            dst += n;
            src += n;
            for (; n != 0; --n) {
                *(--dst) = *(--src);
            }
        }
        return c;
    }

    static char_type* fill(char_type* dst, char_type ch, size_t count) {
        char_type* c = dst;
        for (; count > 0; --count, ++dst) {
            *dst = ch;
        }
        return c;
    }
};


#define STRING_INIT_SIZE 32
// 第一参数为字符类型，第二参数为萃取字符类型类
template <typename CharType, typename CharTraits = mstl::char_traits<CharType>>
class basic_string {
public:
    typedef CharTraits                                  traits_type;
    typedef CharTraits                                  char_traits;

    typedef mstl::allocator<CharType>                   allocator_type;
    typedef mstl::allocator<CharType>                   data_allocator;

    typedef typename allocator_type::value_type         value_type;
    typedef typename allocator_type::pointer            pointer;
    typedef typename allocator_type::const_pointer      const_pointer;
    typedef typename allocator_type::reference          reference;
    typedef typename allocator_type::const_reference    const_reference;
    typedef typename allocator_type::size_type          size_type;
    typedef typename allocator_type::difference_type    difference_type;

    // 这里iterator定义为CharType*，那么将来使用iterator_traits萃取迭代器性质时
    // 将使用T*的偏特化版本，偏特化中定义的iterator_category为random_access_iterator_tag
    typedef value_type*                                 iterator;
    typedef const value_type*                           const_iterator;
    typedef mstl::reverse_iterator<iterator>            reverse_iterator;
    typedef const mstl::reverse_iterator<iterator>      const_reverse_iterator;

    allocator_type get_allocator() {
        return allocator_type();
    }

    static_assert(std::is_pod<CharType>::value, "Character type of basic_string must be POD");
    static_assert(std::is_same<CharType, typename traits_type::char_type>::value,
                "CharType must be same as traits_type::char_type");
public:
    // 函数作用是返回字符串的末尾，一般用于判断字符串是否匹配成功
    // size_type为allocator中的size_t，为无符号整型，-1转为size_t
    // 也就是size_t的最大值，用来代表一个不存在的位置
    static constexpr size_type npos = static_cast<size_type>(-1);
private:
    iterator buffer_;   // 字符串起始位置
    size_type size_;    // 字符串大小
    size_type cap_;     // 字符串容量

public:
    basic_string() noexcept {
        try_init();
    }

    basic_string(size_type n, value_type ch) 
        : buffer_(nullptr), size_(0), cap_(0) {
        fill_init(n, ch);
    }

    basic_string(const basic_string& other, size_type pos)
        : buffer_(nullptr), size_(0), cap_(0) {
        init_from(other.buffer_, pos, other.size_ - pos);
    }

    basic_string(const basic_string& other, size_type pos, size_type count)
        : buffer_(nullptr), size_(0), cap_(0) {
        init_from(other.buffer_, pos, count);
    }

    basic_string(const_pointer str)
        : buffer_(nullptr), size_(0), cap_(0) {
        init_from(str, 0, char_traits::length(str));
    }

    basic_string(const_pointer str, size_type count)
        : buffer_(nullptr), size_(0), cap_(0) {
        init_from(str, 0, count);
    }

    template<typename Iter, typename mstl::enable_if<
            mstl::is_input_iterator<Iter>::value, int>::type = 0>
    basic_string(Iter first, Iter last) {
        copy_init(first, last, mstl::iterator_category(first));
    }

    basic_string(const basic_string& rhs)
        : buffer_(nullptr), size_(0), cap_(0) {
        init_from(rhs.buffer_, 0, rhs.size_);
    }

    basic_string(basic_string&& rhs) noexcept
        : buffer_(rhs.buffer_), size_(rhs.size_), cap_(rhs.cap_) {
            rhs.buffer_ = nullptr;
            rhs.size_ = 0;
            rhs.cap_ = 0;
    }

    basic_string& operator=(const basic_string& rhs);
    basic_string& operator=(basic_string&& rhs) noexcept;
    basic_string& operator=(const_pointer str);
    basic_string& operator=(value_type ch);

    ~basic_string() {
        destory_buffer();
    }
// 迭代器的操作
public:
    iterator begin() noexcept {
        return buffer_;
    }

    const_iterator begin() const noexcept {
        return buffer_;
    }

    iterator end() noexcept {
        return buffer_ + size_;
    }

    const_iterator end() const noexcept {
        return buffer_ + size_;
    }

    reverse_iterator rbegin() noexcept {
        return reverse_iterator(end());
    }

    const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(end());
    }

    reverse_iterator rend() noexcept {
        return reverse_iterator(begin());
    }

    const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(begin());
    }

    // 返回const迭代器
    const_iterator cbegin() const noexcept {
        return begin();
    }

    const_iterator cend() const noexcept {
        return end();
    }

    const_reverse_iterator crbegin() const noexcept {
        return rbegin();
    }

    const_reverse_iterator crend() const noexcept {
        return rend();
    }

    // 容器的操作
    bool empty() const noexcept {
        return size_ == 0;
    }

    size_type size() const noexcept {
        return size_;
    }

    size_type length() const noexcept {
        return size_;
    }

    size_type capacity() const noexcept {
        return cap_;
    }

    size_type max_size() const noexcept {
        return static_cast<size_type>(-1);
    }

    void reserve(size_type n);

    void shrink_to_fit();

    // 元素的访问
    reference operator[](size_type n) {
        MSTL_DEBUG(n <= size_);
        if (n == size_) {
            *(buffer_ + n) = value_type();
        }
        return *(buffer_ + n);
    } 

    const_reference operator[](size_type n) const {
        MSTL_DEBUG(n <= size_);
        if (n == size_) {
            *(buffer_ + n) = value_type();
        }
        return *(buffer_ + n);
    }

    reference at(size_type n) {
        THROW_OUT_OF_RANGE_IF(n >= size_, "basic_string<Char, Traits>::at()"
                          "subscript out of range");
        return (*this)[n];
    }

    const_reference at(size_type n) const {
        THROW_OUT_OF_RANGE_IF(n >= size_, "basic_string<Char, Traits>::at()"
                          "subscript out of range");
        return (*this)[n];
    }

    reference front() {
        MSTL_DEBUG(!empty());
        return *begin();
    }

    const_reference front() const {
        MSTL_DEBUG(!empty());
        return *begin();
    }

    reference back() {
        MSTL_DEBUG(!empty());
        return *(end() - 1);
    }

    const_reference back() const {
        MSTL_DEBUG(!empty());
        return *(end() - 1);
    }

    const_pointer data() const noexcept {
        return to_raw_pointer();
    }

    const_pointer c_str() const noexcept {
        return to_raw_pointer();
    }

    // 添加删除
    iterator insert(const_iterator pos, value_type ch);
    iterator insert(const_iterator pos, size_type count, value_type ch);

    template<typename Iter>
    iterator insert(const_iterator pos, Iter first, Iter last);

    void push_back(value_type ch) {
        append(1, ch);
    }

    void pop_back() {
        MSTL_DEBUG(!empty());
        --size_;
    }

    basic_string& append(size_type count, value_type ch);

    basic_string& append(const basic_string& str) {
        return append(str, 0, str.size_);
    }
    basic_string& append(const basic_string& str, size_type pos) {
        return append(str, pos, str.size_ - pos);
    }
    basic_string& append(const basic_string& str, size_type pos, size_type count);

    basic_string& append(pointer s) {
        return append(s, char_traits::length(s));
    }
    basic_string& append(const_pointer a, size_type count);

    template<typename Iter,typename mstl::enable_if<
        mstl::is_input_iterator<Iter>::value, int>::type = 0>
    basic_string& append(Iter first, Iter last) {
        return append_range(first, last);
    }

    iterator erase(const_iterator pos);
    iterator erase(const_iterator first, const_iterator last);

    // resize
    void resize(size_type count) {
        return resize(count, value_type());
    }
    void resize(size_type count, value_type ch);

    void clear() noexcept {
        size_ = 0;
    }

    // compare
    int compare(const basic_string& other) const;
    int compare(size_type pos1, size_type count1, const basic_string& other) const;
    int compare(size_type pos1, size_type count1, const basic_string& other,
                size_type pos2, size_type count2 = npos) const;
    int compare(const_pointer s) const;
    int compare(size_type pos1, size_type count1, const_pointer s) const;
    int compare(size_type pos1, size_type count1, const_pointer s, size_type count2) const;

    basic_string substr(size_type index, size_type count = npos) {
        count = mstl::min(count, size_ - index);
        return basic_string(buffer_ + index, buffer_ + index + count);
    }

    // replace
    basic_string& replace(size_type pos, size_type count, const basic_string& str) {
        THROW_OUT_OF_RANGE_IF(pos > size_, "basic_string<Char, Traits>::replace's pos out of range");
        return replace_cstr(buffer_ + pos, count, str.buffer_, str.size_);
    }

    basic_string& replace(const_iterator first, const_iterator last, const basic_string& str) {
        MSTL_DEBUG(begin() <= first && last <= end() && first < last);
        return replace_cstr(first, static_cast<size_type>(last - first), str.buffer_, str.size_);
    }

    basic_string& replace(size_type pos, size_type count, const_pointer str) {
        THROW_OUT_OF_RANGE_IF(pos > size_, "basic_string<Char, Traits>::replace's pos out of range");
        return replace_cstr(buffer_ + pos, count, str, char_traits::length(str));
    }

    basic_string& replace(const_iterator first, const_iterator last, const_pointer str) {
        MSTL_DEBUG(begin() <= first && last <= end() && first < last);
        return replace_cstr(first, static_cast<size_type>(last - first), str, char_traits::length(str));
    }

    basic_string& replace(size_type pos, size_type count, const_pointer str, size_type count2) {
        THROW_OUT_OF_RANGE_IF(pos > size_, "basic_string<Char, Traits>::replace's pos out of range");
        return replace_cstr(buffer_ + pos, count, str, count2);
    }

    basic_string& replace(const_iterator first, const_iterator last, const_pointer str,
                            size_type count2) {
        MSTL_DEBUG(begin() <= first && last <= end() && first < last);
        return replace_cstr(first, static_cast<size_type>(last - first), str, count2);
    }

    basic_string& replace(size_type pos, size_type count, size_type count2, value_type ch) {
        THROW_OUT_OF_RANGE_IF(pos > size_, "basic_string<Char, Traits>::replace's pos out of range");
        return replace_fill(buffer_ + pos, count, count2, ch);
    }

    basic_string& replace(const_iterator first, const_iterator last, size_type count2,
                            value_type ch) {
        MSTL_DEBUG(begin() <= first && last <= end() && first < last);
        return replace_fill(first, static_cast<size_type>(last - first), count2, ch);
    }

    basic_string& replace(size_type pos1, size_type count1, const basic_string& str,
                            size_type pos2, size_type count2 = npos) {
        THROW_OUT_OF_RANGE_IF(pos1 > size_ || pos2 > str.size_, 
                            "basic_string<Char, Traits>::replace's pos out of range");
        return replace_cstr(buffer_ + pos1, count1, str.buffer_, count2);
    }

    template<typename Iter, typename mstl::enable_if<
        mstl::is_input_iterator<Iter>::value, int>::type = 0>
    basic_string& replace(const_iterator first, const_iterator last,
                        Iter first2, Iter last2) {
        MSTL_DEBUG(begin() < first || last < end() || first < last);
        return replace_copy(first, last, first2, last2);
    }

    void reverse() noexcept;

    void swap(basic_string& other) noexcept;

    // 查找
    // find
    size_type find(value_type ch, size_type pos = 0)                             const noexcept;
    size_type find(const_pointer str, size_type pos = 0)                         const noexcept;
    size_type find(const_pointer str, size_type pos, size_type count)            const noexcept;
    size_type find(const basic_string& str, size_type pos = 0)                   const noexcept;

    // rfind
    size_type rfind(value_type ch, size_type pos = npos)                         const noexcept;
    size_type rfind(const_pointer str, size_type pos = npos)                     const noexcept;
    size_type rfind(const_pointer str, size_type pos, size_type count)           const noexcept;
    size_type rfind(const basic_string& str, size_type pos = npos)               const noexcept;

    // find_first_of
    size_type find_first_of(value_type ch, size_type pos = 0)                    const noexcept;
    size_type find_first_of(const_pointer s, size_type pos = 0)                  const noexcept;
    size_type find_first_of(const_pointer s, size_type pos, size_type count)     const noexcept;
    size_type find_first_of(const basic_string& str, size_type pos = 0)          const noexcept;

    // find_first_not_of
    size_type find_first_not_of(value_type ch, size_type pos = 0)                const noexcept;
    size_type find_first_not_of(const_pointer s, size_type pos = 0)              const noexcept;
    size_type find_first_not_of(const_pointer s, size_type pos, size_type count) const noexcept;
    size_type find_first_not_of(const basic_string& str, size_type pos = 0)      const noexcept;

    // find_last_of
    size_type find_last_of(value_type ch, size_type pos = 0)                     const noexcept;
    size_type find_last_of(const_pointer s, size_type pos = 0)                   const noexcept;
    size_type find_last_of(const_pointer s, size_type pos, size_type count)      const noexcept;
    size_type find_last_of(const basic_string& str, size_type pos = 0)           const noexcept;

    // find_last_not_of
    size_type find_last_not_of(value_type ch, size_type pos = 0)                 const noexcept;
    size_type find_last_not_of(const_pointer s, size_type pos = 0)               const noexcept;
    size_type find_last_not_of(const_pointer s, size_type pos, size_type count)  const noexcept;
    size_type find_last_not_of(const basic_string& str, size_type pos = 0)       const noexcept;

    // count
    size_type count(value_type ch, size_type pos) const noexcept;

public:
    // 重载+=
    basic_string& operator+=(const basic_string& str) {
        return append(str);
    }
    basic_string& operator+=(value_type ch) {
        return append(1, ch);
    }
    basic_string& operator+=(const_pointer& str) {
        return append(str, str + char_traits::length(str));
    }

    // 重载>>, <<运算符
    friend std::istream& operator>>(std::istream& is, const basic_string& str) {
        value_type* buf = new value_type[4096];
        is >> buf;
        basic_string temp(buf);
        str = mstl::move(temp);
        delete[] buf;
        return is;
    }
    friend std::ostream& operator<<(std::ostream& os, const basic_string& str) {
        for (size_type i = 0; i < str.size_; ++i) {
            os << *(str.buffer_ + i);
        }
        return os;
    }

private:
    // 辅助函数
    void try_init() noexcept;

    void fill_init(size_type n, value_type ch);

    template <class Iter>
    void copy_init(Iter first, Iter last, mstl::input_iterator_tag);

    template <class Iter>
    void copy_init(Iter first, Iter last, mstl::forward_iterator_tag);

    void init_from(const_pointer src, size_type pos, size_type n);

    void destory_buffer();

    const_pointer to_raw_pointer() const;

    void reinsert(size_type size);

    template<typename Iter>
    basic_string& append_range(Iter first, Iter last);

    int compare_cstr(const_pointer s1, size_type n1, const_pointer s2, size_type n2) const;

    basic_string& replace_cstr(const_iterator first, size_type count1, const_pointer str, size_type count2);
    basic_string& replace_fill(const_iterator first, size_type count1, size_type count2, value_type ch);

    template<typename Iter>
    basic_string& replace_copy(const_iterator first1, const_iterator last1, Iter first2, Iter last2);

    void reallocate(size_type need);
    iterator reallocate_and_fill(iterator pos, size_type n, value_type ch);
    iterator reallocate_and_copy(iterator pos, const_iterator first, const_iterator last);
};


using string    = mstl::basic_string<char>;
using wstring   = mstl::basic_string<wchar_t>;
using u16string = mstl::basic_string<char16_t>;
using u32string = mstl::basic_string<char32_t>;


template <typename CharType, typename CharTraits>
basic_string<CharType, CharTraits>&
basic_string<CharType, CharTraits>::operator=(const basic_string& str) {
    if (this != &str) {
        basic_string temp(str);
        swap(temp);
    }
    return *this;
}

template <typename CharType, typename CharTraits>
basic_string<CharType, CharTraits>&
basic_string<CharType, CharTraits>::operator=(basic_string&& str) noexcept {
    destory_buffer();
    buffer_ = str.buffer_;
    size_ = str.size_;
    cap_ = str.cap_;
    str.buffer_ = nullptr;
    str.size_ = 0;
    str.cap_ = 0;
    return *this;
}

template <typename CharType, typename CharTraits>
basic_string<CharType, CharTraits>&
basic_string<CharType, CharTraits>::operator=(const_pointer str) {
    const size_type len = char_traits::length(str);
    if (len > cap_) {
        auto new_buffer = data_allocator::allocate(len + 1);
        data_allocator::deallocate(buffer_);
        buffer_ = new_buffer;
        cap_ = len + 1;
    }
    char_traits::copy(buffer_, str, len);
    size_ = len;
    return *this;
}

template <typename CharType, typename CharTraits>
basic_string<CharType, CharTraits>&
basic_string<CharType, CharTraits>::operator=(value_type ch) {
    if (cap_ < 1) {
        auto new_buffer = data_allocator::allocate(2);
        data_allocator::deallocate(buffer_);
        buffer_ = new_buffer;
        cap_ = 2;
    }
    *buffer_ = ch;
    size_ = 1;
    return *this;
}

template <typename CharType, typename CharTraits>
void basic_string<CharType, CharTraits>::reserve(size_type n) {
    if (cap_ > n) {
        return;
    }
    THROW_LENGTH_ERROR_IF(n > max_size(), "n can not larger than max_size()"
                          "in basic_string<Char,Traits>::reserve(n)");
    auto new_buffer = data_allocator::allocate(n);
    char_traits::move(new_buffer, buffer_, size_);
    buffer_ = new_buffer;
    cap_ = n;
}

// string缩容操作
template <typename CharType, typename CharTraits>
void basic_string<CharType, CharTraits>::shrink_to_fit() {
    if (size_ == cap_) {
        return;
    }
    reinsert(size_);
}

// pos位置插入一个字符ch
template <typename CharType, typename CharTraits>
typename basic_string<CharType, CharTraits>::iterator
basic_string<CharType, CharTraits>::insert(const_iterator pos, value_type ch) {
    iterator r = const_cast<iterator>(pos);
    if (size_ == cap_) {
        return reallocate_and_fill(r, 1, ch);
    }
    char_traits::move(r + 1, r, end() - r);
    ++size_;
    *r = ch;
    return r;
}

// pos位置插入n个ch字符
template <typename CharType, typename CharTraits>
typename basic_string<CharType, CharTraits>::iterator
basic_string<CharType, CharTraits>::insert(const_iterator pos, size_type count, value_type ch) {
    iterator r = const_cast<iterator>(pos);
    if (count == 0) {
        return r;
    }
    if (cap_ - size_ < count) {
        return reallocate_and_fill(r, count, ch);
    }
    if (pos == end()) {
        return char_traits::fill(end(), ch, count);
        size_ += count;
        return r;
    }
    char_traits::move(r + count, r, count);
    char_traits::fill(r, ch, count);
    size_ += count;
    return r;
}

// 在pos位置插入[first, last)的元素
template <typename CharType, typename CharTraits>
template <typename Iter>
typename basic_string<CharType, CharTraits>::iterator
basic_string<CharType, CharTraits>::insert(const_iterator pos, Iter first, Iter last) {
    iterator r = const_cast<iterator>(pos);
    size_type len = mstl::distance(first, last);
    if (len == 0) {
        return r;
    }
    if (cap_ - size_ < len) {
        return reallocate_and_copy(r, first, last);
    }
    if (pos == end()) {
        mstl::uninitialized_copy(first, last, end());
        size_ += len;
        return r;
    }
    char_traits::move(r + len, r, len);
    mstl::uninitialized_copy(first, last, r);
    size_ += len;
    return r;
}

// 末尾添加count个ch字符
template <typename CharType, typename CharTraits>
basic_string<CharType, CharTraits>&
basic_string<CharType, CharTraits>::append(size_type count, value_type ch) {
    THROW_LENGTH_ERROR_IF(size_ > max_size() - count,
                        "basic_string<Char, Tratis>'s size too big");
    if (cap_ - size_ < count) {
        reallocate(count);
    }
    char_traits::fill(buffer_ + size_, ch, count);
    size_ += count;
    return *this;
}

// 末尾添加str[pos]到str[pos + count]的字符
template <typename CharType, typename CharTraits>
basic_string<CharType, CharTraits>&
basic_string<CharType, CharTraits>::
append(const basic_string& str, size_type pos, size_type count) {
    THROW_LENGTH_ERROR_IF(size_ > max_size() - count,
                        "basic_string<Char, Tratis>'s size too big");
    if (count == 0) {
        return *this;
    }
    if (cap_ - size_ < count) {
        reallocate(count);
    }
    char_traits::copy(buffer_ + size_, str.buffer_ + pos, count);
    size_ += count;
    return *this;
}

// 在末尾添加s,s+count的字符
template <typename CharType, typename CharTraits>
basic_string<CharType, CharTraits>&
basic_string<CharType, CharTraits>::append(const_pointer s, size_type count) {
    THROW_LENGTH_ERROR_IF(size_ > max_size() - count,
                        "basic_string<Char, Tratis>'s size too big");
    if (cap_ - size_ < count) {
        reallocate(count);
    }
    char_traits::copy(buffer_ + size_, s, count);
    size_ += count;
    return *this;
}

// 删除pos位置的字符
template <typename CharType, typename CharTraits>
typename basic_string<CharType, CharTraits>::iterator
basic_string<CharType, CharTraits>::erase(const_iterator pos) {
    MSTL_DEBUG(pos != end());
    iterator r = const_cast<iterator>(pos);
    char_traits::move(r, pos + 1, end() - pos - 1);
    --size_;
    return r;
}

// 删除[first,last)的字符
template <typename CharType, typename CharTraits>
typename basic_string<CharType, CharTraits>::iterator
basic_string<CharType, CharTraits>::erase(const_iterator first, const_iterator last) {
    if (first == begin() && last == end()) {
        clear();
        return end();
    }
    const size_type n = end() - last;
    iterator r = const_cast<iterator>(first);
    char_traits::move(r, last, n);
    size_ -= (last - first);
    return r;
}

// 重置容器大小
template <typename CharType, typename CharTraits>
void basic_string<CharType, CharTraits>::resize(size_type count, value_type ch) {
    if (count < size_) {
        erase(buffer_ + count, buffer_ + size_);
    } else {
        append(count - size_, ch);
    }
}

// 比较字符串的大小，大于返回1，小于返回-1等于返回0
template<typename CharType, typename CharTraits>
int basic_string<CharType, CharTraits>::compare(const basic_string& other) const {
    return compare_cstr(buffer_, size_, other.buffer_, other.size_);
}

// 从pos1位置开始的count1个字符与字符串str比较
template<typename CharType, typename CharTraits>
int basic_string<CharType, CharTraits>::
compare(size_type pos1, size_type count1, const basic_string& other) const {
    size_type n = mstl::min(count1, size_ - pos1);
    return compare_cstr(buffer_ + pos1, n, other.buffer_, other.size_);
}

// 从pos1位置开始的count1个字符与字符串str的pos2开始的count2个字符比较
template<typename CharType, typename CharTraits>
int basic_string<CharType, CharTraits>::
compare(size_type pos1, size_type count1, const basic_string& other,
        size_type pos2, size_type count2) const {
    size_type n1 = mstl::min(count1, size_ - pos1);   
    size_type n2 = mstl::min(count2, other.size_ - pos2);
    return compare_cstr(buffer_ + pos1, n1, other.buffer_ + pos2, n2);
}

// 与一个字符串指针比较
template<typename CharType, typename CharTraits>
int basic_string<CharType, CharTraits>::compare(const_pointer s) const {
    size_type n = char_traits::length(s);
    return compare_cstr(buffer_, size_, s, n);
}

// 从pos1位置开始的count1个字符与字符串指针other比较
template<typename CharType, typename CharTraits>
int basic_string<CharType, CharTraits>::
compare(size_type pos1, size_type count1, const_pointer s) const {
    size_type n1 = mstl::min(count1, size_ - pos1);
    size_type n2 = char_traits::length(s);
    return compare_cstr(buffer_ + pos1, n1, s, n2);
}

// 从pos1位置开始的count1个字符与字符串指针other的前count2个字符比较
template<typename CharType, typename CharTraits>
int basic_string<CharType, CharTraits>::
compare(size_type pos1, size_type count1, const_pointer s, size_type count2) const {
    size_type n1 = mstl::min(count1, size_ - pos1);
    return compare_cstr(buffer_ + pos1, n1, s, count2);
}

// 反转字符串
template<typename CharType, typename CharTraits>
void basic_string<CharType, CharTraits>::reverse() noexcept {
    for (auto i = begin(), j = end(); i < j;) {
        mstl::iter_swap(i++, --j);
    }
}

// 交换两个字符串
template<typename CharType, typename CharTraits>
void basic_string<CharType, CharTraits>::swap(basic_string& rhs) noexcept {
    if (this != &rhs) {
        mstl::swap(buffer_, rhs.buffer_);
        mstl::swap(size_, rhs.size_);
        mstl::swap(cap_, rhs.cap_);
    }
}

// 从下标pos开始查找字符串中的第一个ch字符位置
template<typename CharType, typename CharTraits>
typename basic_string<CharType, CharTraits>::size_type
basic_string<CharType, CharTraits>::
find(value_type ch, size_type pos) const noexcept {
    for (auto i = pos; i < size_; ++i) {
        if (*(buffer_ + i) == ch) {
            return i;
        }
    }
    return npos;
}

// 从下标pos开始查找匹配字符串str
template<typename CharType, typename CharTraits>
typename basic_string<CharType, CharTraits>::size_type
basic_string<CharType, CharTraits>::
find(const_pointer str, size_type pos) const noexcept {
    size_type len = char_traits::length(str);
    if (len == 0) {
        return pos;
    }
    if (size_ - pos < len) {
        return npos;
    }
    const size_type left = size_ - len;
    for (auto i = pos; i <= left; ++i) {
        if (*(buffer_ + i) == *str) {
            size_type j = 1;
            for (; j < len; ++j) {
                if (*(buffer_ + i + j) != *(str + j)) {
                    break;
                }
            }
            if (j == len) {
                return i;
            }
        }
    }
    return npos;
}

// 从下标pos开始查找匹配字符串str的前count个字符
template<typename CharType, typename CharTraits>
typename basic_string<CharType, CharTraits>::size_type
basic_string<CharType, CharTraits>::
find(const_pointer str, size_type pos, size_type count) const noexcept {
    if (count == 0) {
        return pos;
    }
    if (size_ - pos < count) {
        return npos;
    }
    const size_type left = size_ - count;
    for (auto i = pos; i <= left; ++i) {
        if (*(buffer_ + i) == *str) {
            size_type j = 1;
            for (; j < count; ++j) {
                if (*(buffer_ + i + j) != *(str + j)) {
                    break;
                }
            }
            if (j == count) {
                return i;
            }
        }
    }
    return npos;
}

// 从下标pos开始查找匹配字符串str
template<typename CharType, typename CharTraits>
typename basic_string<CharType, CharTraits>::size_type
basic_string<CharType, CharTraits>::
find(const basic_string& str, size_type pos) const noexcept {
    size_type len = str.size_;
    if (len == 0) {
        return pos;
    }
    if (size_ - pos < len) {
        return npos;
    }
    const size_type left = size_ - len;
    for (auto i = pos; i <= left; ++i) {
        if (*(buffer_ + i) == str.front()) {
            size_type j = 1;
            for (; j < len; ++j) {
                if (*(buffer_ + i + j) != str[j]) {
                    break;
                }
            }
            if (j == len) {
                return i;
            }
        }
    }
    return npos;
}

// 从下标pos开始反向查找匹配字符ch
template<typename CharType, typename CharTraits>
typename basic_string<CharType, CharTraits>::size_type
basic_string<CharType, CharTraits>::
rfind(value_type ch, size_type pos) const noexcept {
    if (pos >= size_) {
        pos = size_ - 1;
    }
    for (auto i = pos; i != 0; --i) {
        if (*(buffer_ + i) == ch) {
            return i;
        }
    }
    return front() == ch ? 0 : npos;
}

// 从下标pos开始反向查找匹配字符串str
template<typename CharType, typename CharTraits>
typename basic_string<CharType, CharTraits>::size_type
basic_string<CharType, CharTraits>::
rfind(const_pointer str, size_type pos) const noexcept {
    if (pos >= size_) {
        pos = size_ - 1;
    }
    size_type len = char_traits::length(str);
    switch (len) {
        case 0:
            return pos;
        case 1: {
            for (auto i = pos; i != 0; --i) {
                if (*(buffer_ + i) == *str) {
                    return i;
                }
            }
            return front() == *str ? 0 : npos;
        }
        default: {
            for (auto i = pos; i >= len - 1; --i) {
                if (*(buffer_ + i) == *(str + len - 1)) {
                    size_type j = 1;
                    for (; j < len; ++j) {
                        if (*(buffer_ + i - j) != *(str + len - j - 1)) {
                            break;
                        }
                    }
                    if (j == len) {
                        return i - len + 1;
                    }
                }
            }
            return npos;
        }
    }
}

// 从下标pos开始反向查找匹配字符串str的前count个字符
template<typename CharType, typename CharTraits>
typename basic_string<CharType, CharTraits>::size_type
basic_string<CharType, CharTraits>::
rfind(const_pointer str, size_type pos, size_type count) const noexcept {
    if (count == 0) {
        return pos;
    }
    if (pos >= size_) {
        pos = size_ - 1;
    }
    if (pos < count - 1) {
        return npos;
    }
    for (auto i = pos; i >= count - 1; --i) {
        if (*(buffer_ + i) == *(str + count - 1)) {
            size_type j = 1;
            for (; j < count; ++j) {
                if (*(buffer_ + i - j) != *(str + count - j - 1)) {
                    break;
                }
            }
            if (j == count) {
                return i - count + 1;
            }
        }
    }
    return npos;
}

// 从下标pos开始反向查找匹配字符串str
template<typename CharType, typename CharTraits>
typename basic_string<CharType, CharTraits>::size_type
basic_string<CharType, CharTraits>::
rfind(const basic_string& str, size_type pos) const noexcept {
    if (count == 0) {
        return pos;
    }
    if (pos >= size_) {
        pos = size_ - 1;
    }
    if (pos < count - 1) {
        return npos;
    }
    for (auto i = pos; i >= count - 1; --i) {
        if (*(buffer_ + i) == str[count - 1]) {
            size_type j = 1;
            for (; j < count; ++j) {
                if (*(buffer_ + i - j) != str[count - j - 1]) {
                    break;
                }
            }
            if (j == count) {
                return i - count + 1;
            }
        }
    }
    return npos;
}

// 从下标pos开始查找匹配的第一个字符ch
template<typename CharType, typename CharTraits>
typename basic_string<CharType, CharTraits>::size_type
basic_string<CharType, CharTraits>::
find_first_of(value_type ch, size_type pos) const noexcept {
    for (auto i = pos; i < size_; ++i) {
        if (*(buffer_ + i)  == ch) {
            return i;
        }
    }
    return npos;
}

// 从下标pos开始查找字符串中第一次出现字符串指针s的字符的位置
template<typename CharType, typename CharTraits>
typename basic_string<CharType, CharTraits>::size_type
basic_string<CharType, CharTraits>::
find_first_of(const_pointer s, size_type pos) const noexcept {
    const size_type len = char_traits::length(s);
    for (size_type i = pos; i < size_; ++i) {
        value_type c = *(buffer_ + i);
        for (size_type j = 0; j < len; ++j) {
            if (c == *(s + j)) {
                return i;
            }
        }
    }
    return npos;
}

// 从下标pos开始查找字符串中第一次出现字符串指针s的前count个字符的位置
template<typename CharType, typename CharTraits>
typename basic_string<CharType, CharTraits>::size_type
basic_string<CharType, CharTraits>::
find_first_of(const_pointer s, size_type pos, size_type count) const noexcept {
    for (size_type i = pos; i < size_; ++i) {
        value_type c = *(buffer_ + i);
        for (size_type j = 0; j < count; ++j) {
            if (c == *(s + j)) {
                return i;
            }
        }
    }
    return npos;
}

// 从下标pos开始查找字符串中第一次出现字符串指针s的字符的位置
template<typename CharType, typename CharTraits>
typename basic_string<CharType, CharTraits>::size_type
basic_string<CharType, CharTraits>::
find_first_of(const basic_string& str, size_type pos) const noexcept {
    for (size_type i = pos; i < size_; ++i) {
        value_type c = *(buffer_ + i);
        for (size_type j = 0; j < str.size_; ++j) {
            if (c == str[j]) {
                return i;
            }
        }
    }
    return npos;
}

// 从pos开始查找第一个不等于ch的位置
template<typename CharType, typename CharTraits>
typename basic_string<CharType, CharTraits>::size_type
basic_string<CharType, CharTraits>::
find_first_not_of(value_type ch, size_type pos) const noexcept {
    for (size_type i = pos; i < size_; ++i) {
        if (*(buffer_ + i) != ch) {
            return i;
        }
    }
    return npos;
}

// 从pos开始查找第一个不存在于str中的字符的位置
template<typename CharType, typename CharTraits>
typename basic_string<CharType, CharTraits>::size_type
basic_string<CharType, CharTraits>::
find_first_not_of(const_pointer s, size_type pos) const noexcept {
    size_type len = char_traits::length(s);
    for (size_type i = pos; i < size_; ++i) {
        value_type ch = *(buffer_ + i);
        size_type j = 0;
        for (; j < len; ++j) {
            if (ch == *(s + j)) {
                break;
            }
        }
        if (j == len) {
            return i;
        }
    }
    return npos;
}

// 从pos开始查找第一个不存在于str中前count个字符的位置
template<typename CharType, typename CharTraits>
typename basic_string<CharType, CharTraits>::size_type
basic_string<CharType, CharTraits>::
find_first_not_of(const_pointer s, size_type pos, size_type count) const noexcept {
    for (size_type i = pos; i < size_; ++i) {
        value_type ch = *(buffer_ + i);
        size_type j = 0;
        for (; j < count; ++j) {
            if (ch == *(s + j)) {
                break;
            }
        }
        if (j == count) {
            return i;
        }
    }
    return npos;
}

// 从pos开始查找第一个不存在于str中的字符的位置
template<typename CharType, typename CharTraits>
typename basic_string<CharType, CharTraits>::size_type
basic_string<CharType, CharTraits>::
find_first_not_of(const basic_string& str, size_type pos) const noexcept {
    for (size_type i = pos; i < size_; ++i) {
        value_type ch = *(buffer_ + i);
        size_type j = 0;
        for (; j < str.size_; ++j) {
            if (ch == str[j]) {
                break;
            }
        }
        if (j == str.size_) {
            return i;
        }
    }
    return npos;
}

// 从下标pos开始查找最后一个与ch相等的位置
template<typename CharType, typename CharTraits>
typename basic_string<CharType, CharTraits>::size_type
basic_string<CharType, CharTraits>::
find_last_of(value_type ch, size_type pos) const noexcept {
    for (auto i = pos; i < size_; ++i) {
        if (*(buffer_ + i) == ch) {
            return i;
        }
    }
    return npos;
}

// 从下标pos开始查找最后一个与s中的任意一个字符相等的位置
template<typename CharType, typename CharTraits>
typename basic_string<CharType, CharTraits>::size_type
basic_string<CharType, CharTraits>::
find_last_of(const_pointer s, size_type pos) const noexcept {
    size_type len = char_traits::length(s);
    for (size_type i = size_ - 1; i >= pos; --i) {
        value_type ch = *(buffer_ + i);
        for (size_type j = 0; j < len; ++j) {
            if (ch == *(s + j)) {
                return i;
            }
        }
    }
    return npos;
}

// 从下标pos开始查找最后一个与s中的前count个字符相等的位置
template<typename CharType, typename CharTraits>
typename basic_string<CharType, CharTraits>::size_type
basic_string<CharType, CharTraits>::
find_last_of(const_pointer s, size_type pos, size_type count) const noexcept {
    for (size_type i = size_ - 1; i >= pos; --i) {
        value_type ch = *(buffer_ + i);
        for (size_type j = 0; j < count; ++j) {
            if (ch == *(s + j)) {
                return i;
            }
        }
    }
    return npos;
}

// 从下标pos开始查找最后一个与s中的任意一个字符相等的位置
template<typename CharType, typename CharTraits>
typename basic_string<CharType, CharTraits>::size_type
basic_string<CharType, CharTraits>::
find_last_of(const basic_string& str, size_type pos) const noexcept {
    for (size_type i = size_ - 1; i >= pos; --i) {
        value_type ch = *(buffer_ + i);
        for (size_type j = 0; j < str.size_; ++j) {
            if (ch == str[j]) {
                return i;
            }
        }
    }
    return npos;
}

// 从下标pos开始查找最后一个与ch不相等的位置
template<typename CharType, typename CharTraits>
typename basic_string<CharType, CharTraits>::size_type
basic_string<CharType, CharTraits>::
find_last_not_of(value_type ch, size_type pos) const noexcept {
    for (auto i = pos; i < size_; ++i) {
        if (*(buffer_ + i) != ch) {
            return i;
        }
    }
    return npos;
}

// 从下标pos开始查找最后一个与s中的任意一个字符相等的位置
template<typename CharType, typename CharTraits>
typename basic_string<CharType, CharTraits>::size_type
basic_string<CharType, CharTraits>::
find_last_not_of(const_pointer s, size_type pos) const noexcept {
    size_type len = char_traits::length(s);
    for (size_type i = size_ - 1; i >= pos; --i) {
        value_type ch = *(buffer_ + i);
        size_type j = 0;
        for (; j < len; ++j) {
            if (ch == *(s + j)) {
                break;
            }
        }
        if (j == len) {
            return i;
        }
    }
    return npos;
}

// 从下标pos开始查找最后一个与s中的前count个字符相等的位置
template<typename CharType, typename CharTraits>
typename basic_string<CharType, CharTraits>::size_type
basic_string<CharType, CharTraits>::
find_last_not_of(const_pointer s, size_type pos, size_type count) const noexcept {
    for (size_type i = size_ - 1; i >= pos; --i) {
        value_type ch = *(buffer_ + i);
        size_type j = 0;
        for (; j < count; ++j) {
            if (ch == *(s + j)) {
                break;
            }
        }
        if (j == count) {
            return i;
        }
    }
    return npos;
}

// 从下标pos开始查找最后一个与s中的任意一个字符相等的位置
template<typename CharType, typename CharTraits>
typename basic_string<CharType, CharTraits>::size_type
basic_string<CharType, CharTraits>::
find_last_not_of(const basic_string& str, size_type pos) const noexcept {
    for (size_type i = size_ - 1; i >= pos; --i) {
        value_type ch = *(buffer_ + i);
        size_type j = 0;
        for (; j < str.size_; ++j) {
            if (ch == str[j]) {
                break;
            }
        }
        if (j == str.size_) {
            return i;
        }
    }
    return npos;
}

// 从下标pos开始ch出现的次数
template<typename CharType, typename CharTraits>
typename basic_string<CharType, CharTraits>::size_type
basic_string<CharType, CharTraits>::
count(value_type ch, size_type pos) const noexcept {
    size_type n = 0;
    for (size_type i = pos; i < size_; ++i) {
        if (*(buffer_ + i) == ch) {
            ++n;
        }
    }
    return n;
}

/********************************************************************************/
// 辅助函数
// 尝试分配一段内存，如果失败不会抛出异常
template <typename CharType, typename CharTraits>
void basic_string<CharType, CharTraits>::try_init() noexcept {
    try {
        buffer_ = data_allocator::allocate(static_cast<size_type>(STRING_INIT_SIZE));
        size_ = 0;
        cap_ = 0;
    }
    catch (...) {
        buffer_ = nullptr;
        size_ = 0;
        cap_ = 0;
        // no throw
    }
}

template <typename CharType, typename CharTraits>
void basic_string<CharType, CharTraits>::fill_init(size_type n, value_type ch) {
    const size_type init_size = mstl::max(static_cast<size_type>(STRING_INIT_SIZE), n + 1);
    buffer_ = data_allocator::allocate(init_size);
    char_traits::fill(buffer_, ch, n);
    size_ = n;
    cap_ = init_size;
}

template <typename CharType, typename CharTraits>
template <typename Iter>
void basic_string<CharType, CharTraits>::copy_init(Iter first, Iter last, mstl::input_iterator_tag) {
    size_type n = mstl::distance(first, last);
    const size_type init_size = mstl::max(static_cast<size_type>(STRING_INIT_SIZE), n + 1);
    try {
        buffer_ = data_allocator::allocate(init_size);
        size_ = n;
        cap_ = init_size;
    }
    catch (...) {
        buffer_ = nullptr;
        size_ = 0;
        cap_ = 0;
        throw;
    }
    for (; n > 0; --n, ++first) {
        append(*first);
    }
}

template <typename CharType, typename CharTraits>
template <typename Iter>
void basic_string<CharType, CharTraits>::copy_init(Iter first, Iter last, mstl::forward_iterator_tag) {
    size_type n = mstl::distance(first, last);
    const size_type init_size = mstl::max(static_cast<size_type>(STRING_INIT_SIZE), n + 1);
    try {
        buffer_ = data_allocator::allocate(init_size);
        size_ = n;
        cap_ = init_size;
        mstl::uninitialized_copy(first, last, buffer_);
    }
    catch (...) {
        buffer_ = nullptr;
        size_ = 0;
        cap_ = 0;
        throw;
    }
}

template <typename CharType, typename CharTraits>
void basic_string<CharType, CharTraits>::init_from(const_pointer src, size_type pos, size_type count) {
    const size_type init_size = mstl::max(static_cast<size_type>(STRING_INIT_SIZE), count + 1);
    buffer_ = data_allocator::allocate(init_size);
    char_traits::copy(buffer_, src + pos, count);
    size_ = count;
    cap_ = init_size;
}

template <typename CharType, typename CharTraits>
void basic_string<CharType, CharTraits>::destory_buffer() {
    if (buffer_ != nullptr) {
        data_allocator::deallocate(buffer_);
        buffer_ = nullptr;
        size_ = 0;
        cap_ = 0;
    }
}

template <typename CharType, typename CharTraits>
typename basic_string<CharType, CharTraits>::const_pointer
basic_string<CharType, CharTraits>::to_raw_pointer() const {
    *(buffer_ + size_) = value_type();
    return buffer_;
}

template <typename CharType, typename CharTraits>
void basic_string<CharType, CharTraits>::reinsert(size_type size) {
    auto new_buffer = data_allocator::allocate(size);
    try {
        char_traits::move(new_buffer, buffer_, size_);
    }
    catch (...) {
        data_allocator::deallcate(new_buffer);
    }
    buffer_ = new_buffer;
    size_ = size;
    cap_ = size;
}

// 在末尾追加一段[first，last)的内容
template <class CharType, class CharTraits>
template <class Iter>
basic_string<CharType, CharTraits>&
basic_string<CharType, CharTraits>::append_range(Iter first, Iter last) {
    const size_type n = mstl::distance(first, last);
    THROW_LENGTH_ERROR_IF(size_ > max_size() - n, "basic_string<Char, Tratis>'s size too big");
    if (cap_ - size_ < n) {
        reallocate(n);
    }
    mstl::uninitialized_copy_n(first, n, buffer_ + size_);
    size_ += n;
    return *this;
}

template <class CharType, class CharTraits>
int basic_string<CharType, CharTraits>::
    compare_cstr(const_pointer s1, size_type n1, const_pointer s2, size_type n2) const {
    auto len = mstl::min(n1, n2);
    auto res = char_traits::compare(s1, s2, len);
    if (res != 0) {
        return res;
    }
    if (n1 < n2) {
        return -1;
    }
    if (n2 < n1) {
        return 1;
    }
    return 0;
}

// 把从s1开始的count1个字符替换为以str开始的count2个字符
template <class CharType, class CharTraits>
basic_string<CharType, CharTraits>& 
basic_string<CharType, CharTraits>::
replace_cstr(const_iterator first, size_type count1, const_pointer str, size_type count2) {
    if (static_cast<size_t>(cend() - first) < count1) {
        count1 = cend() - first;
    }
    if (count1 < count2) {
        const size_type add = count2 - count1;
        THROW_LENGTH_ERROR_IF(size_ > max_size() - add, 
                    "basic_string<Char, Traits>'s size too big");
        if (size_ > cap_ - add) {
            reallocate(add);
        }
        pointer r = const_cast<pointer>(first);
        char_traits::move(r + count2, first + count1, end() - (first + count1));
        char_traits::copy(r, str, count2);
        size_ += add;
    } else {
        pointer r = const_cast<pointer>(first);
        char_traits::move(r + count2, first + count1, end() - (first + count1));
        char_traits::copy(r, str, count2);
        size_ -= (count1 - count2);
    }
    return *this;
}

// 把以first开头的count1个字符替换为count2个ch字符
template <class CharType, class CharTraits>
basic_string<CharType, CharTraits>& 
basic_string<CharType, CharTraits>::
replace_fill(const_iterator first, size_type count1, size_type count2, value_type ch) {
    if (static_cast<size_t>(cend() - first) < count1) {
        count1 = cend() - first;
    }
    if (count1 < count2) {
        const size_type add = count2 - count1;
        THROW_LENGTH_ERROR_IF(size_ > max_size() - add, 
                    "basic_string<Char, Traits>'s size too big");
        if (size_ > cap_ - add) {
            reallocate(add);
        }
        pointer r = const_cast<pointer>(first);
        char_traits::move(r + count2, first + count1, end() - (first + count1));
        char_traits::fill(r, ch, count2);
        size_ += add;
    } else {
        pointer r = const_cast<pointer>(first);
        char_traits::move(r + count2, first + count1, end() - (first + count1));
        char_traits::copy(r, ch, count2);
        size_ -= (count1 - count2);
    }
    return *this;
}

template <class CharType, class CharTraits>
template<typename Iter>
basic_string<CharType, CharTraits>& 
basic_string<CharType, CharTraits>::
replace_copy(const_iterator first1, const_iterator last1, Iter first2, Iter last2) {
    size_type count1 = last1 - first1;
    size_type count2 = last2 - first2;
    if (count1 < count2) {
        const size_type add = count2 - count1;
        THROW_LENGTH_ERROR_IF(size_ > max_size() - add, 
                    "basic_string<Char, Traits>'s size too big");
        if (size_ > cap_ - add) {
            reallocate(add);
        }
        pointer r = const_cast<pointer>(first1);
        char_traits::move(r + count2, first1 + count1, end() - (first1 + count1));
        char_traits::copy(r, first2, count2);
        size_ += add;
    } else {
        pointer r = const_cast<pointer>(first1);
        char_traits::move(r + count2, first1 + count1, end() - (first1 + count1));
        char_traits::copy(r, first2, count2);
        size_ -= (count1 - count2);
    }
    return *this;
}

template <class CharType, class CharTraits>
void basic_string<CharType, CharTraits>::reallocate(size_type need) {
    const size_type new_cap = mstl::max(cap_ + need, cap_ + (cap_ >> 1));
    auto new_buffer = data_allocator::allocate(new_cap);
    char_traits::move(new_buffer, buffer_, size_);
    data_allocator::deallocate(buffer_);
    buffer_ = new_buffer;
    cap_ = new_cap;
}

// 重新分配空间，并在pos位置插入n的ch字符，返回原pos的位置
template <class CharType, class CharTraits>
typename basic_string<CharType, CharTraits>::iterator
basic_string<CharType, CharTraits>::
reallocate_and_fill(iterator pos, size_type n, value_type ch) {
    const auto r = pos - buffer_;
    const size_type old_cap = cap_;
    const auto new_cap = mstl::max(old_cap + n, old_cap + (old_cap >> 1));
    auto new_buffer = data_allocator::allocate(new_cap);
    auto p1 = char_traits::move(new_buffer, buffer_ , r) + r;
    auto p2 = char_traits::fill(p1, ch, n) + n;
    char_traits::move(p2, buffer_ + r , size_ - r);
    data_allocator::deallocate(buffer_, old_cap);
    buffer_ = new_buffer;
    size_ += n;
    cap_ = new_cap;
    return buffer_ + r;
}

// 重新分配空间，并在pos位置插入[first, last)的字符，返回原pos的位置
template <class CharType, class CharTraits>
typename basic_string<CharType, CharTraits>::iterator
basic_string<CharType, CharTraits>::
reallocate_and_copy(iterator pos, const_iterator first, const_iterator last) {
    const auto r = pos - buffer_;
    const size_type old_cap = cap_;
    const size_type n = mstl::distance(first, last);
    const auto new_cap = mstl::max(old_cap + n, old_cap + (old_cap >> 1));
    auto new_buffer = data_allocator::allocate(new_cap);
    auto p1 = char_traits::move(new_buffer, buffer_ , r) + r;
    auto p2 = mstl::uninitialized_copy_n(first, n, p1) + n;
    char_traits::move(p2, buffer_ + r , size_ - r);
    data_allocator::deallocate(buffer_, old_cap);
    buffer_ = new_buffer;
    size_ += n;
    cap_ = new_cap;
    return buffer_ + r;
}

/***********************************************************************************/
// 重载关于string的全局操作符
template<typename CharType, typename CharTraits>
basic_string<CharType, CharTraits>
operator+(const basic_string<CharType, CharTraits>& lhs,
          const basic_string<CharType, CharTraits>& rhs) {
    basic_string<CharType, CharTraits> temp(lhs);
    temp.append(rhs);
    return temp;
}

template<typename CharType, typename CharTraits>
basic_string<CharType, CharTraits>
operator+(const CharType* lhs, const basic_string<CharType, CharTraits>& rhs) {
    basic_string<CharType, CharTraits> temp(lhs);
    temp.append(rhs);
    return temp;
}

template<typename CharType, typename CharTraits>
basic_string<CharType, CharTraits>
operator+(CharType ch, const basic_string<CharType, CharTraits>& rhs) {
    basic_string<CharType, CharTraits> temp(1, ch);
    temp.append(rhs);
    return temp;
}

template<typename CharType, typename CharTraits>
basic_string<CharType, CharTraits>
operator+(const basic_string<CharType, CharTraits>& lhs, const CharType* rhs) {
    basic_string<CharType, CharTraits> temp(lhs);
    temp.append(rhs);
    return temp;
}

template<typename CharType, typename CharTraits>
basic_string<CharType, CharTraits>
operator+(const basic_string<CharType, CharTraits>& lhs, CharType ch) {
    basic_string<CharType, CharTraits> temp(lhs);
    temp.append(1, ch);
    return temp;
}

template<typename CharType, typename CharTraits>
basic_string<CharType, CharTraits>
operator+(basic_string<CharType, CharTraits>&& lhs,
          const basic_string<CharType, CharTraits>& rhs) {
    basic_string<CharType, CharTraits> temp(mstl::move(lhs));
    temp.append(rhs);
    return temp;
}

template<typename CharType, typename CharTraits>
basic_string<CharType, CharTraits>
operator+(const basic_string<CharType, CharTraits>& lhs,
          basic_string<CharType, CharTraits>&& rhs) {
    basic_string<CharType, CharTraits> temp(mstl::move(rhs));
    temp.insert(temp.begin(), lhs.begin(), lhs.end());
    return temp;
}

template<typename CharType, typename CharTraits>
basic_string<CharType, CharTraits>
operator+(basic_string<CharType, CharTraits>&& lhs,
          basic_string<CharType, CharTraits>&& rhs) {
    basic_string<CharType, CharTraits> temp(mstl::move(lhs));
    temp.append(rhs);
    return temp;
}

template<typename CharType, typename CharTraits>
basic_string<CharType, CharTraits>
operator+(const CharType* lhs, basic_string<CharType, CharTraits>&& rhs) {
    basic_string<CharType, CharTraits> temp(mstl::move(rhs));
    temp.insert(temp.begin(), lhs, lhs + CharTraits::length(lhs));
    return temp;
}

template<typename CharType, typename CharTraits>
basic_string<CharType, CharTraits>
operator+(CharType ch, basic_string<CharType, CharTraits>&& rhs) {
    basic_string<CharType, CharTraits> temp(mstl::move(rhs));
    temp.insert(temp.begin(), ch);
    return temp;
}

template<typename CharType, typename CharTraits>
basic_string<CharType, CharTraits>
operator+(basic_string<CharType, CharTraits>&& lhs, const CharType* rhs) {
    basic_string<CharType, CharTraits> temp(mstl::move(lhs));
    temp.append(rhs);
    return temp;
}

template<typename CharType, typename CharTraits>
basic_string<CharType, CharTraits>
operator+(basic_string<CharType, CharTraits>&& lhs, CharType ch) {
    basic_string<CharType, CharTraits> temp(mstl::move(lhs));
    temp.append(1, ch);
    return temp;
}

// 重载比较操作符
template<typename CharType, typename CharTraits>
bool operator==(const basic_string<CharType, CharTraits>& lhs,
                const basic_string<CharType, CharTraits>& rhs) {
    return lhs.size_ == rhs.size_ && lhs.compare(rhs) == 0;
}

template<typename CharType, typename CharTraits>
bool operator!=(const basic_string<CharType, CharTraits>& lhs,
                const basic_string<CharType, CharTraits>& rhs) {
    return lhs.size_ != rhs.size_ || lhs.compare(rhs) != 0;
}

template<typename CharType, typename CharTraits>
bool operator<(const basic_string<CharType, CharTraits>& lhs,
                const basic_string<CharType, CharTraits>& rhs) {
    return lhs.compare(rhs) < 0;
}

template<typename CharType, typename CharTraits>
bool operator<=(const basic_string<CharType, CharTraits>& lhs,
                const basic_string<CharType, CharTraits>& rhs) {
    return lhs.compare(rhs) <= 0;
}

template<typename CharType, typename CharTraits>
bool operator>(const basic_string<CharType, CharTraits>& lhs,
                const basic_string<CharType, CharTraits>& rhs) {
    return lhs.compare(rhs) > 0;
}

template<typename CharType, typename CharTraits>
bool operator>=(const basic_string<CharType, CharTraits>& lhs,
                const basic_string<CharType, CharTraits>& rhs) {
    return lhs.compare(rhs) >= 0;
}

// 重载全局的swap函数
template<typename CharType, typename CharTraits>
void swap(const basic_string<CharType, CharTraits>& lhs,
          const basic_string<CharType, CharTraits>& rhs) {
    lhs.swap(rhs);
}

// basic_string的hash仿函数
template<typename CharType, typename CharTraits>
struct hash<basic_string<CharType, CharTraits>> {
    size_t operator()(const basic_string<CharType, CharTraits>& str) const noexcept {
        return std::_Hash_impl::hash(str.data(), str.length());
    }
};

} // mstl

#endif