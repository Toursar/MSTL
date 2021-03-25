#ifndef M_RBTREE_H
#define M_RBTREE_H

#include <initializer_list>

#include "m_functional.h"
#include "m_exceptdef.h"
#include "m_memory.h"
#include "m_iterator.h"
#include "m_type_traits.h"

namespace mstl {

typedef bool rb_tree_color_type;

static constexpr rb_tree_color_type rb_tree_red = false;
static constexpr rb_tree_color_type rb_tree_black = true;

template<typename T>
struct rb_tree_node_base;
template<typename T>
struct rb_tree_node;
template<typename T>
struct rb_tree_iterator;
template<typename T>
struct rb_tree_const_iterator;

template<typename T, bool>
struct rb_tree_value_traits_imp {
    typedef T key_type;
    typedef T mapped_type;
    typedef T value_type;

    template<typename Ty>
    static const key_type& get_key(const Ty& value) const {
        return value;
    }

    template<typename Ty>
    static const key_type& get_value(const Ty& value) const {
        return value;
    }
};

template<typename T>
struct rb_tree_value_traits_imp<T, true> {
    typedef typename std::remove_cv<typename T::first_type>::type key_type;
    typedef typename T::second_type                               mapped_type;
    typedef T                                                     value_type;

    template<typename Ty>
    static const key_type& get_key(const Ty& value) const {
        return value.first;
    }

    template<typename Ty>
    static const key_type& get_value(const Ty& value) const {
        return value;
    }
};

template<typename T>
struct rb_tree_value_traits {
    static constexpr is_map = mstl::is_pair<T>::value;

    typedef rb_tree_value_traits_imp<T, is_map>     value_traits_type;
    typedef typename value_traits_type::key_type    key_type;
    typedef typename value_traits_type::mapped_type mapped_type;
    typedef typename value_traits_type::value_type  value_type;

    template<typename Ty>
    static const key_type& get_key(const Ty& value) const {
        return value_traits_type::get_key(value);
    }

    template<typename Ty>
    static const key_type& get_value(const Ty& value) const {
        return value_traits_type::get_value(value);
    }
};

template<typename T>
struct rb_tree_node_traits {
    typedef rb_tree_color_type                  color_type;
    typedef rb_tree_value_traits<T>             value_traits;
    typedef typename value_traits::key_type     key_type;
    typedef typename value_traits::mapped_type  mapped_type;
    typedef typename value_traits::value_type   value_type;

    typedef rb_tree_node_base<T>*               base_ptr;
    typedef rb_tree_node<T>*                    node_ptr;

};

template<typename T>
struct rb_tree_node_base {
    typedef rb_tree_color_type    color_type;
    typedef rb_tree_node_base<T>* base_ptr;
    typedef rb_tree_node<T>*      node_ptr;
    typedef rb_tree_node<T>&      node_ref;

    base_ptr parent;
    base_ptr left;
    base_ptr right;
    color_type color;

    base_ptr get_base_ptr() {
        return this;
    }

    node_ptr get_node_ptr() {
        return reinterpret_cast<node_ptr>(this);
    }

    node_ref get_node_ref() {
        return reinterpret_cast<node_ref>(*this);
    }
};

template<typename T>
struct rb_tree_node : public rb_tree_node_base<T> {
    typedef rb_tree_node_base<T>* base_ptr;
    typedef rb_tree_node<T>*      node_ptr;

    T value;

    base_ptr get_base_ptr() {
        return static_cast<base_ptr>(this);
    }

    node_ptr get_node_ptr() {
        return this;
    }
};



} // mstl

#endif