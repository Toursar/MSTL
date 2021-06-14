#ifndef M_RBTREE_H
#define M_RBTREE_H

#include <initializer_list>

#include "m_functional.h"
#include "m_exceptdef.h"
#include "m_memory.h"
#include "m_iterator.h"
#include "m_type_traits.h"


/* 红黑树的节点性质与要求
 * 1. 每个结点要么是红的要么是黑的。  
 * 2. 根结点是黑的。  
 * 3. 每个叶结点（叶结点即指树尾端NIL指针或NULL结点）都是黑的。  
 * 4. 任何两个红色的节点不能相连，即如果一个结点是红的，那么它的两个子节点以及父节点都是黑的。  
 * 5. 对于任意结点而言，其到叶结点树尾端NIL指针的每条路径都包含相同数目的黑结点。 
*/


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
    static const key_type& get_key(const Ty& value) {
        return value;
    }

    template<typename Ty>
    static const key_type& get_value(const Ty& value) {
        return value;
    }
};

template<typename T>
struct rb_tree_value_traits_imp<T, true> {
    typedef typename std::remove_cv<typename T::first_type>::type key_type;
    typedef typename T::second_type                               mapped_type;
    typedef T                                                     value_type;

    template<typename Ty>
    static const key_type& get_key(const Ty& value) {
        return value.first;
    }

    template<typename Ty>
    static const key_type& get_value(const Ty& value) {
        return value;
    }
};

template<typename T>
struct rb_tree_value_traits {
    static constexpr bool is_map = mstl::is_pair<T>::value;

    typedef rb_tree_value_traits_imp<T, is_map>     value_traits_type;
    typedef typename value_traits_type::key_type    key_type;
    typedef typename value_traits_type::mapped_type mapped_type;
    typedef typename value_traits_type::value_type  value_type;

    template<typename Ty>
    static const key_type& get_key(const Ty& value) {
        return value_traits_type::get_key(value);
    }

    template<typename Ty>
    static const key_type& get_value(const Ty& value) {
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

template<typename T>
struct rb_tree_traits {
    typedef rb_tree_value_traits<T>              value_traits;

    typedef typename value_traits::key_type      key_type;
    typedef typename value_traits::mapped_type   mapped_type;
    typedef typename value_traits::value_type    value_type;

    typedef value_type*                          pointer;
    typedef value_type&                          reference;
    typedef const value_type*                    const_pointer;
    typedef const value_type&                    const_reference;

    typedef rb_tree_node_base<T>                 base_type;
    typedef rb_tree_node<T>                      node_type;

    typedef base_type*                           base_ptr;
    typedef node_type*                           node_ptr;

};

// rb_tree迭代器的基类
template<typename T>
struct rb_tree_iterator_base : public mstl::iterator<mstl::bidirectional_iterator_tag, T> {
    typedef typename rb_tree_traits<T>::base_ptr    base_ptr;

    base_ptr node;

    rb_tree_iterator_base() : node(nullptr) {}

    void inc() {
        if (node->right != nullptr) {
            node = rb_tree_min(node->right);
        } else {
            base_ptr p_node = node->parent;
            // 如果此时node是父节点的右子节点，那么就意味以node父节点的整个子树全部遍历完成
            // 那么就继续向上寻找父节点
            while (p_node->right == node) {
                node = p_node;
                p_node = p_node->parent;
            }
            /* 这里对应着当前node为根节点，同时根节点的右节点不存在的特殊情况
             * 在我们rb_tree的实现中，采用一个虚拟的节点，其左子节点为树的
             * 最小节点（即最左子节点），其右子节点为最大节点（即最右子节点）
             * 其父节点为根节点。如果根节点没有右子节点，那么此虚拟节点的parent
             * 以及right都将是根节点。
             * 在此时的状况下上面的操作就会导致，此时p_node为根节点，node为
             * 虚拟节点，这时我们就不应该对node进行修改了，因此下面的node赋值
             * 操作增加了一个判断条件。
             */
            if (node->right != p_node) {
                node = p_node;
            }
        }
    }

    void dec() {
        // 这里对应着end节点的迭代器后退操作，end节点的后退就是指向树的最大节点
        if (node->parent->parent == node && rb_tree_is_red(node)) {
            node = node->right;
        } else if (node->left != nullptr) {
            node = rb_tree_max(node->left);
        } else {
            base_ptr p_node = node->parent;
            while (p_node->left == node) {
                node = p_node;
                p_node = p_node->parent;
            }
            node = p_node;
        }
    }

    bool operator==(const rb_tree_iterator_base& rhs) {
        return node == rhs.node;
    }

    bool operator!=(const rb_tree_iterator_base& rhs) {
        return node != rhs.node;
    }
};

// 两种rb_tree的迭代器
template<typename T>
struct rb_tree_iterator : public rb_tree_iterator_base<T> {
    typedef rb_tree_traits<T>   tree_traits;

    typedef typename tree_traits::value_type value_type; 
    typedef typename tree_traits::pointer    pointer; 
    typedef typename tree_traits::reference  reference; 
    typedef typename tree_traits::base_ptr   base_ptr; 
    typedef typename tree_traits::node_ptr   node_ptr;

    typedef rb_tree_iterator<T>              iterator; 
    typedef rb_tree_const_iterator<T>        const_iterator; 
    typedef iterator                         self;

    using rb_tree_iterator_base<T>::node;

    rb_tree_iterator() {}

    rb_tree_iterator(base_ptr x) {
        node = x;
    }

    rb_tree_iterator(node_ptr x) {
        node = x;
    }

    rb_tree_iterator(const iterator& rhs) {
        node = rhs.node;
    }

    rb_tree_iterator(const const_iterator& rhs) {
        node = rhs.node;
    }

    reference operator*() const {
        return node->get_node_ptr()->value;
    }

    pointer operator->() const {
        return &(operator*());
    }
    
    self& operator++() {
        this->inc();
        return *this;
    }

    self operator++(int) {
        self temp(*this);
        this->inc();
        return temp;
    }

    self& operator--() {
        this->dec();
        return *this;
    }

    self operator--(int) {
        self temp(*this);
        this->dec();
        return temp;
    }
};

template<typename T>
struct rb_tree_const_iterator : public rb_tree_iterator_base<T> {
    typedef rb_tree_traits<T>   tree_traits;

    typedef typename tree_traits::value_type      value_type; 
    typedef typename tree_traits::const_pointer   pointer; 
    typedef typename tree_traits::const_reference reference; 
    typedef typename tree_traits::base_ptr        base_ptr; 
    typedef typename tree_traits::node_ptr        node_ptr;

    typedef rb_tree_iterator<T>                   iterator; 
    typedef rb_tree_const_iterator<T>             const_iterator; 
    typedef const_iterator                        self;

    using rb_tree_iterator_base<T>::node;

    rb_tree_const_iterator() {}

    rb_tree_const_iterator(base_ptr x) {
        node = x;
    }

    rb_tree_const_iterator(node_ptr x) {
        node = x;
    }

    rb_tree_const_iterator(const iterator& rhs) {
        node = rhs.node;
    }

    rb_tree_const_iterator(const const_iterator& rhs) {
        node = rhs.node;
    }

    reference operator*() const {
        return node->get_node_ptr()->value;
    }

    pointer operator->() const {
        return &(operator*());
    }
    
    self& operator++() {
        this->inc();
        return *this;
    }

    self operator++(int) {
        self temp(*this);
        this->inc();
        return temp;
    }

    self& operator--() {
        this->dec();
        return *this;
    }

    self operator--(int) {
        self temp(*this);
        this->dec();
        return temp;
    }
};

// rb_tree algorithm
template<typename NodePtr>
NodePtr rb_tree_min(NodePtr node) noexcept {
    while (node->left != nullptr) {
        node = node->left;
    }
    return node;
}

template<typename NodePtr>
NodePtr rb_tree_max(NodePtr node) noexcept {
    while (node->right != nullptr) {
        node = node->right;
    }
    return node;
}

// 下面两个函数的实现，并没有给出rb_tree_is_rchild和rb_tree_is_black版本
// 因为对于红黑树，节点不是左子节点就是右子节点，不是红就是黑，无需多此一举
template<typename NodePtr>
bool rb_tree_is_lchild(NodePtr node) noexcept {
    return node == node->parent->left;
}

template<typename NodePtr>
bool rb_tree_is_red(NodePtr node) noexcept {
    return node->color == rb_tree_red;
}

template<typename NodePtr>
void rb_tree_set_black(NodePtr& node) noexcept {
    node->color = rb_tree_black;
}

template<typename NodePtr>
void rb_tree_set_red(NodePtr& node) noexcept {
    node->color = rb_tree_red;
}

template<typename NodePtr>
NodePtr rb_tree_next(NodePtr node) noexcept {
    if (node->right != nullptr) {
        return rb_tree_min(node->right);
    }
    while (!rb_tree_is_lchild(node)) {
        node = node->parent;
    }
    return node->parent;
}

// 树的左旋
/*---------------------------------------*\
|       p                         p       |
|      / \                       / \      |
|     x   d    rotate left      y   d     |
|    / \       ===========>    / \        |
|   a   y                     x   c       |
|      / \                   / \          |
|     b   c                 a   b         |
\*---------------------------------------*/

template<typename NodePtr>
void rb_tree_rotate_left(NodePtr x, NodePtr& root) noexcept {
    NodePtr y = x->right;
    x->right = y->left;
    if (y->left != nullptr) {
        y->left->parent = x;
    }
    y->parent = x->parent;
    if (x == root) {
        root = y;
    } else if (rb_tree_is_lchild(x)) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }
    y->left = x;
    x->parent = y;
}

// 树的右旋
/*----------------------------------------*\
|     p                         p          |
|    / \                       / \         |
|   d   x      rotate right   d   y        |
|      / \     ===========>      / \       |
|     y   a                     b   x      |
|    / \                           / \     |
|   b   c                         c   a    |
\*----------------------------------------*/
template<typename NodePtr>
void rb_tree_rotate_right(NodePtr x, NodePtr& root) {
    NodePtr y = x->left;
    x->left = y->right;
    if (y->right != nullptr) {
        y->right->parent = x;
    }
    y->parent = x->parent;
    if (x == root) {
        root = y;
    } else if (rb_tree_is_lchild(x)) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }
    y->right = x;
    x->parent = y;
}

// 插入节点以后，恢复树的平衡
/* 插入平衡的操作主要分为两部分，五个小分支：
 * 1. 父节点为黑色，或者新增加的节点是根节点。
 *   1.1 如果新节点为根节点，将其设为黑色即可。
 *   1.2 如果新节点的父节点是黑色，那么并未破坏红黑树的结构，直接返回即可。
 * 2. 父节点为红色，此时新节点也为红色，破坏红黑树结构，需要平衡操作。
 *   2.1 如果叔节点（父节点的父节点的另一个子节点）为红色，此时需要将父节点
 *       与叔节点变为黑色，祖父节点（父节点的父节点）变为红色，然后继续处理祖父节点。
 *   2.2 如果叔节点是黑色，此时（注意如果没有叔节点，那就是NIL，也是黑色的）。
 *       2.2.1 如果此时父节点为左（右）子节点，而当前节点为右（左）子节点，
 *             以父节点为当前节点，然后进行左（右）旋。这样这种状态会变为下面的状态
 *       2.2.2 如果此时父节点为左（右）子节点，而当前节点为左（右）子节点，
 *             让父节点变为黑色，祖父节点变为红色，以祖父节点为支点右（左）旋。
*/
template<typename NodePtr>
void rb_tree_insert_rebalance(NodePtr x, NodePtr& root) noexcept {
    // 新加入的节点一律为红色
    rb_tree_set_red(x);
    // 如果父节点为黑，或者为根节点，那么无需操作
    while (x != root && rb_tree_is_red(x->parent)) {
        if (rb_tree_is_lchild(x->parent)) {
            NodePtr uncle = x->parent->parent->right;
            // 判断叔节点的状态进行操作
            if (uncle != nullptr && rb_tree_is_red(uncle)) {
                // 叔节点为红色
                rb_tree_set_black(x->parent);
                rb_tree_set_black(uncle);
                x = x->parent->parent;
                rb_tree_set_red(x);
            } else {
                // 叔节点为黑色，先判断2.2.1的情况
                if (!rb_tree_is_lchild(x)) {
                    x = x->parent;
                    rb_tree_rotate_left(x, root);
                }
                // 此时所有状态都会转变为2.2.2
                rb_tree_set_black(x->parent);
                rb_tree_set_red(x->parent->parent);
                rb_tree_rotate_right(x->parent->parent, root);
                break;
            }
        } else {
            // 几乎一致的镜像操作
            NodePtr uncle = x->parent->parent->left;
            if (uncle != nullptr && rb_tree_is_red(uncle)) {
                rb_tree_set_black(x->parent);
                rb_tree_set_black(uncle);
                x = x->parent->parent;
                rb_tree_set_red(x);
            } else {
                if (rb_tree_is_lchild(x)) {
                    x = x->parent;
                    rb_tree_rotate_right(x, root);
                }
                rb_tree_set_black(x->parent);
                rb_tree_set_red(x->parent->parent);
                rb_tree_rotate_left(x->parent->parent, root);
                break;
            }
        }
    }
    // 根节点一律为黑色
    rb_tree_set_black(root);
}

// 删除节点后的平衡操作，第一参数为当前删除节点，第二参数为根节点，第三第四参数分别为最小和最大节点
/* 删除节点主要分为三种情况，前两种比较简单，第三种较为复杂，需要多方面考虑
 * 1. 删除的节点是叶子节点，即没有子节点，那么将父节点的对应位置改为null，删除子节点即可。
 * 2. 删除的节点有一个子节点，将父节点与对应位置与子节点相连，然后删除子节点即可。
 * 3. 删除的节点有两个节点，此时就比较复杂了，需要整体考虑红黑树的平衡问题。
 *    一般来说此时我们采用的方法就是寻找当前节点的前驱节点或者后继节点，交换这两个节点的位置。
 *    此时，待删除节点就的状态就转化为上边1，2的状态。
*/

// 参考自：http://blog.csdn.net/v_JULY_v/article/details/6105630
//        https://www.jianshu.com/p/84416644c080
template<typename NodePtr>
NodePtr rb_tree_erase_rebalance(NodePtr z, NodePtr& root, NodePtr& leftmost, NodePtr& rightmost) {
    // y节点为当前节点的后继节点
    NodePtr y = (z->left == nullptr || z->right == nullptr) ? z : rb_tree_next(z);
    // x节点为后继节点的独子节点或者为null，此时的y不可能同时拥有左右子节点。
    NodePtr x = y->left != nullptr ? y->left : y->right;
    // x节点的父节点，提前记录此节点，方便后面平衡红黑树时使用，此处的x_parent其实就是与y交换后待删除节点的父节点
    // 因为交换后原先y的位置就是待删除节点，而x为其子节点，那么删除以后，x的父节点其实就是之前待删除节点的父节点
    NodePtr x_parent = nullptr;
    // 如果y != z，那么z就存在两个子节点，我们需要将z与y的位置交换
    if (y != z) {
        z->left->parent = y;
        y->left = z->left;
        // 判断后继节点是否为z的右子节点，如果不是，那么z的右子节点就存在左节点
        if (y != z->right) {
            // 此时需要修改其中y，x的指针指向
            x_parent = y->parent;
            if (x != nullptr) {
                x->parent = y->parent;
            }
            y->parent->left = x;
            y->right = z->right;
            z->right->parent = y;
        } else {
            // 如果后继节点为z的右子节点，那么上面的修改指针指向就不需要了
            x_parent = y;
        }
        // 修改y, z的父节点连接
        if (root == z) {
            root = y;
        } else if (rb_tree_is_lchild(z)) {
            z->parent->left = y;
        } else {
            z->parent->right = y;
        }
        y->parent = z->parent;
        mstl::swap(y->color, z->color);
        // 修改完成后将y指向要删除的z节点，此时原先的y节点已经成功替代了z的位置
        // 而z节点是将要删除的节点，我们不再修改其指向
        y = z;
    } else {
        // y == z那么就说明, z最多只有一个子节点
        x_parent = y->parent;
        if (x != nullptr) {
            x->parent = y->parent;
        }
        if (root == z) {
            root = x;
        } else if (rb_tree_is_lchild(z)) {
            z->parent->left = x;
        } else {
            z->parent->right = x;
        }
        // 如果此时删除的是叶子节点，那么就可能删除到树的最大或者最小值，那么此时我们就需要重新维护这两个值
        if (leftmost == z) {
            leftmost = x == nullptr ? x_parent : rb_tree_min(x);
        } else if (rightmost == z) {
            rightmost = x == nullptr ? x_parent : rb_tree_max(x);
        }
    }
    // 操作至此，我们将第三种情况的删除操作转变为前两种，此时y指向删除节点
    // x为当前平衡的节点，x_parent就是待删除节点的父节点，因为我们并未修改待删除节点的指向，
    // 因此，直接使用y->parent表示待删除节点的父节点是错误的，此时待删除节点
    // 的父节点其实是曾经的y->parent，也就是我们提前保存的指针x_parent

    // 对于前两种情况的平衡，则还分为两种主要情况
    /* 1. 待删除节点为黑色。直接删除会破坏红黑树的规则。此时的待删除节点为左（右）子节点
     *   1.1 当前节点的兄弟节点（父节点的另一个子节点）为红色。令父节点为红，兄弟节点为黑，进行左（右）旋，继续处理。
     *   1.2 当前节点的兄弟节点为黑色，且兄弟节点的两个子节点也为黑色，令兄弟节点为红，父节点成为当前节点，继续处理。
     *   1.3 兄弟节点为黑，且兄弟节点的左子节点为红，右子节点为黑。
     *       令兄弟节点为红，兄弟节点的左子节点为黑，以兄弟节点为支点右（左）旋，继续处理。
     *   1.4 兄弟节点为黑，且兄弟节点的左子节点为黑，右子节点为红。令兄弟节点为父节点的颜色，
     *       父节点为黑色，兄弟节点的右子节点为黑色，以父节点为支点左（右）旋，树的性质调整完成，算法结束。
     * 
     * 2. 待删除节点为红色，直接删除节点即可
    */
    if (!rb_tree_is_red(y)) {
        // 先判断x是否为红色，如果为红，直接将x变为黑色红黑树就可以平衡
        while (x != root && !rb_tree_is_red(x)) {
            // 待删除节点为左子节点
            if (x == x_parent->left) {
                NodePtr brother = x_parent->right;
                // 状态1.1
                if (rb_tree_is_red(brother)) {
                    rb_tree_set_red(x_parent);
                    rb_tree_set_black(brother);
                    rb_tree_rotate_left(x_parent, root);
                    // 更新新的兄弟节点
                    brother = x_parent->right;
                }
                // 上面操作使得1.1转化为1.2, 1.3, 1.4的某种。
                if ((brother->left == nullptr || !rb_tree_is_red(brother->left)) && 
                    (brother->right == nullptr || !rb_tree_is_red(brother->right))) {
                    // 状态1.2
                    rb_tree_set_red(brother);
                    x = x_parent;
                    x_parent = x_parent->parent;
                } else {
                    if (brother->right == nullptr || !rb_tree_is_red(brother->right)) {
                        // 状态1.3
                        if (brother->left != nullptr) {
                            rb_tree_set_black(brother->left);
                        }
                        rb_tree_set_red(brother);
                        rb_tree_rotate_right(brother, root);
                        brother = x_parent->right;
                    }
                    // 转为状态1.4
                    brother->color = x_parent->color;
                    rb_tree_set_black(x_parent);
                    if (brother->right != nullptr) {
                        rb_tree_set_black(brother->right);
                    }
                    rb_tree_rotate_left(x_parent, root);
                    break;

                }
            } else {
                // 待删除节点为右节点，镜像操作
                NodePtr brother = x_parent->left;
                // 状态1.1
                if (rb_tree_is_red(brother)) {
                    rb_tree_set_red(x_parent);
                    rb_tree_set_black(brother);
                    rb_tree_rotate_right(x_parent, root);
                    // 更新新的兄弟节点
                    brother = x_parent->left;
                }
                // 上面操作使得1.1转化为1.2, 1.3, 1.4的某种。
                if ((brother->left == nullptr || !rb_tree_is_red(brother->left)) && 
                    (brother->right == nullptr || !rb_tree_is_red(brother->right))) {
                    // 状态1.2
                    rb_tree_set_red(brother);
                    x = x_parent;
                    x_parent = x_parent->parent;
                } else {
                    if (brother->left == nullptr || !rb_tree_is_red(brother->left)) {
                        // 状态1.3
                        if (brother->right != nullptr) {
                            rb_tree_set_black(brother->right);
                        }
                        rb_tree_set_red(brother);
                        rb_tree_rotate_left(brother, root);
                        brother = x_parent->left;
                    }
                    // 转为状态1.4
                    brother->color = x_parent->color;
                    rb_tree_set_black(x_parent);
                    if (brother->left != nullptr) {
                        rb_tree_set_black(brother->left);
                    }
                    rb_tree_rotate_right(x_parent, root);
                    break;

                }
            }
        }
        if (x != nullptr) {
            rb_tree_set_black(x);
        }
    }
    return y;
}

// rb_tree的模板类
template<typename T, typename Compare>
class rb_tree {
public:
    typedef rb_tree_traits<T>             tree_traits;
    typedef rb_tree_value_traits<T>       value_traits;

    typedef typename tree_traits::base_type          base_type;
    typedef typename tree_traits::base_ptr           base_ptr;
    typedef typename tree_traits::node_type          node_type;
    typedef typename tree_traits::node_ptr           node_ptr;
    typedef typename tree_traits::key_type           key_type;
    typedef typename tree_traits::mapped_type        mapped_type;
    typedef typename tree_traits::value_type         value_type;
    typedef Compare                                  key_compare;

    typedef allocator<T>                             allocator_type;
    typedef allocator<T>                             data_allocator;
    typedef allocator<base_type>                     base_allocator;
    typedef allocator<node_type>                     node_allocator;

    typedef typename allocator_type::pointer         poniter;
    typedef typename allocator_type::const_pointer   const_poniter;
    typedef typename allocator_type::reference       reference;
    typedef typename allocator_type::const_reference const_reference;
    typedef typename allocator_type::size_type       size_type;
    typedef typename allocator_type::difference_type difference_type;

    typedef rb_tree_iterator<T>                      iterator;
    typedef rb_tree_const_iterator<T>                const_iterator;
    typedef mstl::reverse_iterator<iterator>         reverse_iterator;
    typedef mstl::reverse_iterator<const_iterator>   const_reverse_iterator;

    allocator_type get_allocator() const {
        return node_allocator();
    }

    key_compare key_comp() const {
        return key_cmp_();
    }

private:
    // rb_tree中的数据
    base_ptr header_;       // 虚拟节点，其left为最小节点，right为最大节点，parent为根节点
    size_type node_count_;   // 记录节点数量
    key_compare key_cmp_;   // key的比较函数

private:
    // 获取rb_tree的根节点和最左最右节点
    base_ptr& root() const {
        return header_->parent;
    }
    base_ptr& leftmost() const {
        return header_->left;
    }
    base_ptr& rightmost() const {
        return header_->right;
    }
public:
    rb_tree() {
        rb_tree_init();
    }

    rb_tree(const rb_tree& rhs);
    rb_tree(rb_tree&& rhs) noexcept;

    rb_tree& operator=(const rb_tree& rhs);
    rb_tree& operator=(rb_tree&& rhs) noexcept;

    ~rb_tree() {
        clear();
    }
public:
    iterator begin() noexcept {
        return leftmost();
    }

    const_iterator begin() const noexcept {
        return leftmost();
    }

    iterator end() noexcept {
        return header_;
    }

    const_iterator end() const noexcept {
        return header_;
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

    bool empty() const noexcept {
        return node_count_ == 0;
    }

    size_type size() const noexcept {
        return node_count_;
    }

    size_type max_size() const noexcept {
        return static_cast<size_type>(-1);
    }

    // emplace方法
    template<typename ...Args>
    iterator emplace_multi(Args&& ...args);

    template<typename ...Args>
    mstl::pair<iterator, bool> emplace_unique(Args&& ...args);

    template<typename ... Args>
    iterator emplace_multi_use_hint(iterator hint, Args&& ...args);

    template<typename ... Args>
    iterator emplace_unique_use_hint(iterator hint, Args&& ...args);

    // insert方法
    iterator insert_multi(const value_type& value);

    iterator insert_multi(value_type&& value) {
        return emplace_multi(mstl::move(value));
    }

    iterator insert_multi(iterator hint, const value_type& value) {
        return emplace_multi_use_hint(hint, value);
    }

    iterator insert_multi(iterator hint, value_type&& value) {
        return emplace_multi_use_hint(hint, mstl::move(value));
    }

    // 在末尾插入[first, last)的元素
    template<typename InputIterator>
    void insert_multi(InputIterator first, InputIterator last) {
        const size_type n = mstl::distance(first, last);
        THROW_LENGTH_ERROR_IF(node_count_ > max_size() - n, "rb_tree<T, Comp>'s size too big");
        for (; n > 0; --n, ++first) {
            insert_multi(end(), *first);
        }
    }

    mstl::pair<iterator, bool> insert_unique(const value_type& value);

    mstl::pair<iterator, bool> insert_unique(value_type&& value) {
        return emplace_unique(mstl::move(value));
    }

    iterator insert_unique(iterator hint, const value_type& value) {
        return emplace_unique_use_hint(hint, value);
    }

    iterator insert_unique(iterator hint, value_type&& value) {
        return emplace_unique_use_hint(hint, mstl::move(value));
    }

    // 在末尾插入[first, last)的元素
    template<typename InputIterator>
    void insert_unique(InputIterator first, InputIterator last) {
        const size_type n = mstl::distance(first, last);
        THROW_LENGTH_ERROR_IF(node_count_ > max_size() - n, "rb_tree<T, Comp>'s size too big");
        for (; n > 0; --n, ++first) {
            insert_unique(end(), *first);
        }
    }

    iterator erase(iterator hint);

    size_type erase_multi(const key_type& key);
    size_type erase_unique(const key_type& key);

    void erase(iterator first, iterator last);

    void clear();

    // 查找相关方法
    iterator find(const key_type& key);

    const_iterator find(const key_type& key) const;

    size_type count_multi(const key_type& key) const {
        auto f = equal_range_multi(key);
        return static_cast<size_type>(mstl::distance(f.first, f.second));
    }

    size_type count_unique(const key_type& key) const {
        return find(key) == end() ? 0 : 1;
    }

    iterator lower_bound(const key_type& key);
    const_iterator lower_bound(const key_type& key) const;
    iterator upper_bound(const key_type& key);
    const_iterator upper_bound(const key_type& key) const;

    mstl::pair<iterator, iterator>
    equal_range_multi(const key_type& key) {
        return mstl::make_pair(lower_bound(key), upper_bound(key));
    }

    mstl::pair<iterator, iterator>
    equal_range_multi(const key_type& key) const {
        return mstl::make_pair(lower_bound(key), upper_bound(key));
    }

    mstl::pair<iterator, iterator>
    equal_range_unique(const key_type& key) {
        iterator iter = find(key);
        iterator next = iter;
        return iter == end() ? mstl::make_pair(iter, iter) : mstl::make_pair(iter, ++next);
    }

    mstl::pair<iterator, iterator>
    equal_range_unique(const key_type& key) const {
        const_iterator iter = find(key);
        const_iterator next = iter;
        return iter == end() ? mstl::make_pair(iter, iter) : mstl::make_pair(iter, ++next);
    }

    void swap(rb_tree& rhs) noexcept;
    // 辅助函数
private:
    template <typename ...Args>
    node_ptr create_node(Args&&... args);
    node_ptr clone_node(base_ptr x);
    void     destory_node(node_ptr p);

    void     rb_tree_init();
    void     reset();

    mstl::pair<base_ptr, bool> 
        get_insert_multi_pos(const key_type& key);
    mstl::pair<mstl::pair<base_ptr, bool>, bool> 
        get_insert_unique_pos(const key_type& key);

    iterator insert_value_at(base_ptr x, const value_type& value, bool add_to_left);
    iterator insert_node_at(base_ptr x, node_ptr node, bool add_to_left);

    iterator insert_multi_use_hint(iterator hint, key_type key, node_ptr node);
    iterator insert_unique_use_hint(iterator hint, key_type key, node_ptr node);

    base_ptr copy_from(base_ptr x, base_ptr p);
    void     erase_since(base_ptr x);
};

template<typename T, typename Compare>
rb_tree<T, Compare>::rb_tree(const rb_tree& rhs) {
    rb_tree_init();
    if (node_count_ != 0) {
        root() = copy_from(rhs.root(), rhs.header_);
        leftmost() = rb_tree_min(root());
        rightmost() = rb_tree_max(root());
    }
    node_count_ = rhs.node_count_;
    key_cmp_ = rhs.key_cmp_;
}

template<typename T, typename Compare>
rb_tree<T, Compare>::rb_tree(rb_tree&& rhs) noexcept
    : header_(mstl::move(rhs.header_)),
    node_count_(rhs.node_count_),
    key_cmp_(rhs.key_cmp_) {
    rhs.reset();
}

template<typename T, typename Compare>
rb_tree<T, Compare>&
rb_tree<T, Compare>::operator=(const rb_tree& rhs) {
    if (this != &rhs) {
        clear();
        if (node_count_ != 0) {
            root() = copy_from(rhs.root(), rhs.header_);
            leftmost() = rb_tree_min(root());
            rightmost() = rb_tree_max(root());
        }
        node_count_ = rhs.node_count_;
        key_cmp_ = rhs.key_cmp_;
    }
    return *this;
}

template<typename T, typename Compare>
rb_tree<T, Compare>&
rb_tree<T, Compare>::operator=(rb_tree&& rhs) noexcept {
    clear();
    header_ = mstl::move(rhs.header_);
    node_count_ = rhs.node_count_;
    key_cmp_ = rhs.key_cmp_;
    rhs.reset();
    return *this;
}

// 就地插入元素，可以重复
template<typename T, typename Compare>
template<typename ...Args>
typename rb_tree<T, Compare>::iterator 
rb_tree<T, Compare>::emplace_multi(Args&& ...args) {
    THROW_LENGTH_ERROR_IF(node_count_ > max_size() - 1, "rb_tree<T, Comp>'s size too big");
    node_ptr np = create_node(mstl::forward<Args>(args)...);
    pair<base_ptr, bool> res = get_insert_multi_pos(value_traits::get_key(np->value));
    return insert_node_at(res.first, np, res.second);
}

// 就地插入元素，不可重复
template<typename T, typename Compare>
template<typename ...Args>
mstl::pair<typename rb_tree<T, Compare>::iterator , bool> 
rb_tree<T, Compare>::emplace_unique(Args&& ...args) {
    THROW_LENGTH_ERROR_IF(node_count_ > max_size() - 1, "rb_tree<T, Comp>'s size too big");
    node_ptr np = create_node(mstl::forward<Args>(args)...);
    pair<pair<base_ptr, bool>, bool> res = get_insert_unique_pos(value_traits::get_key(np->value));
    if (res.second) {
        return mstl::make_pair(insert_node_at(res.first.first, np, res.first.second), true);
    }
    destory_node(np);
    return mstl::make_pair(iterator(res.first.first), false);
}

// 就地插入元素，可以重复，使用hint
template<typename T, typename Compare>
template<typename ... Args>
typename rb_tree<T, Compare>::iterator 
rb_tree<T, Compare>::emplace_multi_use_hint(iterator hint, Args&& ...args) {
    THROW_LENGTH_ERROR_IF(node_count_ > max_size() - 1, "rb_tree<T, Comp>'s size too big");
    node_ptr np = create_node(mstl::forward<Args>(args)...);
    if (node_count_ == 0) {
        return insert_node_at(header_, np, true);
    }
    key_type key = value_traits::get_key(np->value);
    if (hint == begin()) {
        if (key_cmp_(key, value_traits::get_key(*hint))) {
            return insert_node_at(hint.node, np, true);
        } else {
            pair<base_ptr, bool> res = get_insert_multi_pos(key);
            return insert_node_at(res.first, np, res.second);
        }
    } else if (hint == end()) {
        if (!key_cmp_(key, value_traits::get_key(rightmost()->get_node_ptr()->value))) {
            return insert_node_at(rightmost(), np, false);
        } else {
            pair<base_ptr, bool> res = get_insert_multi_pos(key);
            return insert_node_at(res.first, np, res.second);
        }
    }
    return insert_multi_use_hint(hint, key, np);

}

// 就地插入元素，不可重复，使用hint
template<typename T, typename Compare>
template<typename ... Args>
typename rb_tree<T, Compare>::iterator 
rb_tree<T, Compare>::emplace_unique_use_hint(iterator hint, Args&& ...args) {
    THROW_LENGTH_ERROR_IF(node_count_ > max_size() - 1, "rb_tree<T, Comp>'s size too big");
    node_ptr np = create_node(mstl::forward<Args>(args)...);
    if (node_count_ == 0) {
        return insert_node_at(header_, np, true);
    }
    key_type key = value_traits::get_key(np->value);
    if (hint == begin()) {
        if (key_cmp_(key, value_traits::get_key(*hint))) {
            return insert_node_at(hint.node, np, true);
        } else {
            pair<pair<base_ptr, bool>, bool> res = get_insert_unique_pos(key);
            if (!res.second) {
                destory_node(np);
                return res.first.first;
            }
            return insert_node_at(res.first.first, np, res.first.second);
        }
    } else if (hint == end()) {
        if (key_cmp_(value_traits::get_key(rightmost()->get_node_ptr()->value), key)) {
            return insert_node_at(rightmost(), np, false);
        } else {
            pair<pair<base_ptr, bool>, bool> res = get_insert_unique_pos(key);
            if (!res.second) {
                destory_node(np);
                return res.first.first;
            }
            return insert_node_at(res.first.first, np, res.first.second);
        }
    }
    return insert_multi_use_hint(hint, key, np);
}

// 插入元素，可重复
template<typename T, typename Compare>
typename rb_tree<T, Compare>::iterator 
rb_tree<T, Compare>::insert_multi(const value_type& value) {
    THROW_LENGTH_ERROR_IF(node_count_ > max_size() - 1, "rb_tree<T, Comp>'s size too big");
    pair<base_ptr, bool> res = get_insert_multi_pos(value_traits::get_key(value));
    return insert_value_at(res.first, value, res.second);
}

template<typename T, typename Compare>
mstl::pair<typename rb_tree<T, Compare>::iterator, bool>
rb_tree<T, Compare>::insert_unique(const value_type& value) {
    THROW_LENGTH_ERROR_IF(node_count_ > max_size() - 1, "rb_tree<T, Comp>'s size too big");
    pair<pair<base_ptr, bool>, bool> res = get_insert_unique_pos(value_traits::get_key(value));
    if (res.second) {
        return make_pair(insert_value_at(res.first.first, value, res.first.second), true);
    }
    return make_pair(iterator(res.first.first), false);
}

// 删除节点
template<typename T, typename Compare>
typename rb_tree<T, Compare>::iterator 
rb_tree<T, Compare>::erase(iterator hint) {
    node_ptr node = hint.node->get_node_ptr();
    iterator next(node);
    ++next;

    rb_tree_erase_rebalance(hint.node, root(), leftmost(), rightmost());
    destory_node(node);
    --node_count_;
    return next;
}

template<typename T, typename Compare>
typename rb_tree<T, Compare>::size_type
rb_tree<T, Compare>::erase_multi(const key_type& key) {
    auto p = equal_range_multi(key);
    const size_type n = mstl::distance(p.first, p.second);
    erase(p.first, p.second);
    return n;
}

template<typename T, typename Compare>
typename rb_tree<T, Compare>::size_type
rb_tree<T, Compare>::erase_unique(const key_type& key) {
    iterator iter = find(key);
    if (iter != end()) {
        erase(iter);
        return 1;
    }
    return 0;
}

template<typename T, typename Compare>
void rb_tree<T, Compare>::erase(iterator first, iterator last) {
    if (first == begin() && last == end()) {
        clear();
    } else {
        while (first != last) {
            erase(first++);
        }
    }
}

template<typename T, typename Compare>
void rb_tree<T, Compare>::clear() {
    if (node_count_ != 0) {
        erase_since(root());
        leftmost() = header_;
        rightmost() = header_;
        root() = nullptr;
        node_count_ = 0;
    }
}

template<typename T, typename Compare>
typename rb_tree<T, Compare>::iterator
rb_tree<T, Compare>::find(const key_type& key) {
    // 先假设有边界在最大值位置
    base_ptr y = header_;
    base_ptr x = root();
    while (x != nullptr) {
        if (!key_cmp_(value_traits::get_key(x->get_node_ptr()->value), key)) {
            // 如果key小于等于当前位置，则查找左子树，并更新y的值
            y = x;
            x = x->left;
        } else {
            // 如果key大于当前位置，则查找右子树
            x = x->right;
        }
    }
    iterator res = iterator(y);
    // 最后判断找到的位置是否正确
    return (res == end() || key_cmp_(key, value_traits::get_key(*res))) ? end() : res;
}

template<typename T, typename Compare>
typename rb_tree<T, Compare>::const_iterator
rb_tree<T, Compare>::find(const key_type& key) const {
    // 先假设有边界在最大值位置
    base_ptr y = header_;
    base_ptr x = root();
    while (x != nullptr) {
        if (!key_cmp_(value_traits::get_key(x->get_node_ptr()->value), key)) {
            // 如果key小于等于当前位置，则查找左子树，并更新y的值
            y = x;
            x = x->left;
        } else {
            // 如果key大于当前位置，则查找右子树
            x = x->right;
        }
    }
    iterator res = const_iterator(y);
    // 最后判断找到的位置是否正确
    return (res == end() || key_cmp_(key, value_traits::get_key(*res))) ? end() : res;
}

template<typename T, typename Compare>
typename rb_tree<T, Compare>::iterator
rb_tree<T, Compare>::lower_bound(const key_type& key) {
    // 先假设有边界在最大值位置
    base_ptr y = header_;
    base_ptr x = root();
    while (x != nullptr) {
        if (!key_cmp_(value_traits::get_key(x->get_node_ptr()->value), key)) {
            // 如果key小于等于当前位置，则查找左子树，并更新y的值
            y = x;
            x = x->left;
        } else {
            // 如果key大于当前位置，则查找右子树
            x = x->right;
        }
    }
    return iterator(y);
}

template<typename T, typename Compare>
typename rb_tree<T, Compare>::const_iterator
rb_tree<T, Compare>::lower_bound(const key_type& key) const {
    // 先假设有边界在最大值位置
    base_ptr y = header_;
    base_ptr x = root();
    while (x != nullptr) {
        if (!key_cmp_(value_traits::get_key(x->get_node_ptr()->value), key)) {
            // 如果key小于等于当前位置，则查找左子树，并更新y的值
            y = x;
            x = x->left;
        } else {
            // 如果key大于当前位置，则查找右子树
            x = x->right;
        }
    }
    return const_iterator(y);
}

template<typename T, typename Compare>
typename rb_tree<T, Compare>::iterator
rb_tree<T, Compare>::upper_bound(const key_type& key) {
    // 先假设有边界在最大值位置
    base_ptr y = header_;
    base_ptr x = root();
    while (x != nullptr) {
        if (key_cmp_(key, value_traits::get_key(x->get_node_ptr()->value))) {
            // 如果key小于当前位置，则查找左子树，并更新y的值
            y = x;
            x = x->left;
        } else {
            // 如果key大于当前位置，则查找右子树
            x = x->right;
        }
    }
    return iterator(y);
}

template<typename T, typename Compare>
typename rb_tree<T, Compare>::const_iterator
rb_tree<T, Compare>::upper_bound(const key_type& key) const {
    // 先假设有边界在最大值位置
    base_ptr y = header_;
    base_ptr x = root();
    while (x != nullptr) {
        if (key_cmp_(key, value_traits::get_key(x->get_node_ptr()->value))) {
            // 如果key小于当前位置，则查找左子树，并更新y的值
            y = x;
            x = x->left;
        } else {
            // 如果key大于当前位置，则查找右子树
            x = x->right;
        }
    }
    return const_iterator(y);
}

template<typename T, typename Compare>
void rb_tree<T, Compare>::swap(rb_tree& rhs) noexcept {
    if (this != &rhs) {
        mstl::swap(header_, rhs.header_);
        mstl::swap(node_count_, rhs.node_count_);
        mstl::swap(key_cmp_, rhs.key_cmp_);
    }
}

/********************************************************************/
// 辅助函数实现

// 创建一个节点
template<typename T, typename Compare>
template<typename ...Args>
typename rb_tree<T, Compare>::node_ptr
rb_tree<T, Compare>::create_node(Args&& ...args) {
    node_ptr temp = node_allocator::allocate(1);
    try {
        data_allocator::construct(mstl::address_of(temp->value), mstl::forward<Args>(args)...);
        temp->left = nullptr;
        temp->right = nullptr;
        temp->parent = nullptr;
    } catch (...) {
        node_allocator::deallocate(temp);
        throw;
    }
    return temp;
}

// 复制一个节点
template<typename T, typename Compare>
typename rb_tree<T, Compare>::node_ptr
rb_tree<T, Compare>::clone_node(base_ptr x) {
    node_ptr temp = create_node(x->get_node_ptr()->value);
    temp->color = x->color;
    return temp;
}

// 销毁节点
template<typename T, typename Compare>
void rb_tree<T, Compare>::destory_node(node_ptr x) {
    data_allocator::destory(&x->value);
    node_allocator::deallocate(x);
}

// 初始化红黑树
template<typename T, typename Compare>
void rb_tree<T, Compare>::rb_tree_init() {
    header_ = base_allocator::allocate(1);
    // 虚拟的控制节点设置为红色，与根节点作区分
    header_->color = rb_tree_red;
    root() = nullptr;
    leftmost() = header_;
    rightmost() = header_;
    node_count_ = 0;
}

// reset辅助函数
template<typename T, typename Compare>
void rb_tree<T, Compare>::reset() {
    node_count_ = 0;
    header_ = nullptr;
}

// 判断key应该插入到哪个位置，返回pair，第一参数为应插入的叶子节点，第二参数为插入在左子节点(true)还是右子节点(false)
template<typename T, typename Compare>
mstl::pair<typename rb_tree<T, Compare>::base_ptr, bool>
rb_tree<T, Compare>::get_insert_multi_pos(const key_type& key) {
    base_ptr x = root();
    base_ptr y = header_;
    // 此处，如果树为空，那么也会插入header_的左侧
    bool add_to_left = true;
    while (x != nullptr) {
        y = x;
        add_to_left = key_cmp_(key, value_traits::get_key(x->get_node_ptr()->value));
        x = add_to_left ? x->left : x->right;
    }
    return mstl::make_pair(y, add_to_left);
}

// 判断key应该插入到哪个位置，返回pair，第一参数为pair表示插入的父节点以及插入其左还是右子节点，第二参数为是否插入成功
template<typename T, typename Compare>
mstl::pair<mstl::pair<typename rb_tree<T, Compare>::base_ptr, bool>, bool>
rb_tree<T, Compare>::get_insert_unique_pos(const key_type& key) {
    base_ptr x = root();
    base_ptr y = header_;
    bool add_to_left = true;
    while (x != nullptr) {
        y = x;
        add_to_left = key_cmp_(key, value_traits::get_key(x->get_node_ptr()->value));
        x = add_to_left ? x->left : x->right;
    }
    // 构造要插入节点的父节点的迭代器
    iterator j = iterator(y);
    // 如果插入的是左子节点，那么需要进行特殊处理来判断是否重复
    if (add_to_left) {
        // 如果树为空，或者节点为首节点，那么一定可以插入且不重复
        if (y == header_ || j == begin()) {
            return mstl::make_pair(mstl::make_pair(y, add_to_left), true);
        } else {
            // 如果不是上面的情况，我们就需要判断当前父节点的前一个节点是否与现在要插入的节点相同
            // 根据红黑树的性质，如果插入的节点重复，那么只可能与j指向的前一个节点重复
            --j;
        }
    }
    // 判断是否重复，只有要插入的key大于此时j所指的元素才说明插入无误
    if (key_cmp_(value_traits::get_key(*j), key)) {
        return mstl::make_pair(mstl::make_pair(y, add_to_left), true);
    }
    // 程序走到这里，说明插入出错
    return mstl::make_pair(mstl::make_pair(y, add_to_left), false);
}

// 以x为父节点插入新节点值为value，add_to_left标志插入位置是否为左子树
template<typename T, typename Compare>
typename rb_tree<T, Compare>::iterator
rb_tree<T, Compare>::insert_value_at(base_ptr x, const value_type& value, bool add_to_left) {
    node_ptr node = create_node(value);
    node->parent = x;
    base_ptr base_node = node->get_base_ptr();
    if (x == header_) {
        // 此时插入的节点是第一个节点
        root() = base_node;
        leftmost() = base_node;
        rightmost() = base_node;
    } else if (add_to_left) {
        x->left = base_node;
        if (x == leftmost()) {
            leftmost() = base_node;
        }
    } else {
        x->right = base_node;
        if (x == rightmost()) {
            rightmost() = base_node;
        }
    }
    rb_tree_insert_rebalance(base_node, root());
    ++node_count_;
    return iterator(node);
}

template<typename T, typename Compare>
typename rb_tree<T, Compare>::iterator
rb_tree<T, Compare>::insert_node_at(base_ptr x, node_ptr node, bool add_to_left) {
    node->parent = x;
    base_ptr base_node = node->get_base_ptr();
    if (x == header_) {
        // 此时插入的节点是第一个节点
        root() = base_node;
        leftmost() = base_node;
        rightmost() = base_node;
    } else if (add_to_left) {
        x->left = base_node;
        if (x == leftmost()) {
            leftmost() = base_node;
        }
    } else {
        x->right = base_node;
        if (x == rightmost()) {
            rightmost() = base_node;
        }
    }
    // 插入节点完成后平衡红黑树
    rb_tree_insert_rebalance(base_node, root());
    ++node_count_;
    return iterator(node);
}

// 插入节点node，键值为key，采用hint作为辅助，寻找小于hint的最近邻的位置插入节点
// 如果hint的参数正确，那么插入效率将从log级降低为常数级，但是如果hint的参数错误，插入效率将会降低
template<typename T, typename Compare>
typename rb_tree<T, Compare>::iterator
rb_tree<T, Compare>::insert_multi_use_hint(iterator hint, key_type key, node_ptr node) {
    base_ptr cur_node = hint.node;
    iterator before = hint;
    --before;
    base_ptr before_node = before.node;
    if (!key_cmp_(key, value_traits::get_key(*before)) &&
        !key_cmp_(value_traits::get_key(*hint), key)) {
        if (before_node->right == nullptr) {
            // 判断是否可以插入到hint的前一个节点的右子节点上
            return insert_node_at(before_node, node, false);
        } else if (cur_node->left == nullptr) {
            // 判断是否可以插入到hint位置的左子节点上
            return insert_node_at(cur_node, node, true);
        }
    }
    auto pos = get_insert_multi_pos(key);
    return insert_node_at(pos.first, node, pos.second);
}

template<typename T, typename Compare>
typename rb_tree<T, Compare>::iterator
rb_tree<T, Compare>::insert_unique_use_hint(iterator hint, key_type key, node_ptr node) {
    base_ptr cur_node = hint.node;
    iterator before = hint;
    --before;
    base_ptr before_node = before.node;
    if (!key_cmp_(key, value_traits::get_key(*before)) &&
        !key_cmp_(value_traits::get_key(*hint), key)) {
        if (before_node->right == nullptr) {
            // 判断是否可以插入到hint的前一个节点的右子节点上
            return insert_node_at(before_node, node, false);
        } else if (cur_node->left == nullptr) {
            // 判断是否可以插入到hint位置的左子节点上
            return insert_node_at(cur_node, node, true);
        }
    }
    auto pos = get_insert_unique_pos(key);
    if (!pos.second) {
        destory_node(node);
        return pos.first.first;
    }
    return insert_node_at(pos.first, node, pos.second);
}

// 递归复制从 x 开始的所有节点，p 为 x 的父节点
template<typename T, typename Compare>
typename rb_tree<T, Compare>::base_ptr
rb_tree<T, Compare>::copy_from(base_ptr x, base_ptr p) {
    base_ptr top = clone_node(x);
    top->parent = p;
    try {
        if (x->right != nullptr) {
            top->right = copy_from(x->right, top);
        }
        p = top;
        x = x->left;
        while (x != nullptr) {
            base_ptr y = clone_node(x);
            p->left = y;
            y->parent = p;
            if (x->right != nullptr) {
                y->right = copy_from(x->right, y);
            }
            p = y;
            x = x->left;
        }
    } catch (...) {
        earse_since(top);
        throw;
    }
    return top;
}

// 从x节点开始删除自己及其子树
template<typename T, typename Compare>
void rb_tree<T, Compare>::erase_since(base_ptr x) {
    while (x != nullptr) {
        erase_since(x->right);
        base_ptr y = x->left;
        destory_node(x->get_node_ptr());
        x = y;
    }
}

// 重载全局的比较操作符
template<typename T, typename Compare>
bool operator==(const rb_tree<T, Compare>& lhs, const rb_tree<T, Compare>& rhs) {
    return lhs.size() == rhs.size() && mstl::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template<typename T, typename Compare>
bool operator<(const rb_tree<T, Compare>& lhs, const rb_tree<T, Compare>& rhs) {
    return mstl::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template<typename T, typename Compare>
bool operator!=(const rb_tree<T, Compare>& lhs, const rb_tree<T, Compare>& rhs) {
    return !(lhs == rhs);
}

template<typename T, typename Compare>
bool operator>(const rb_tree<T, Compare>& lhs, const rb_tree<T, Compare>& rhs) {
    return rhs < lhs;
}

template<typename T, typename Compare>
bool operator<=(const rb_tree<T, Compare>& lhs, const rb_tree<T, Compare>& rhs) {
    return !(rhs < lhs);
}

template<typename T, typename Compare>
bool operator>=(const rb_tree<T, Compare>& lhs, const rb_tree<T, Compare>& rhs) {
    return !(lhs < rhs);
}

template<typename T, typename Compare>
void swap(rb_tree<T, Compare>& lhs, rb_tree<T, Compare>& rhs) noexcept {
    lhs.swap(rhs);
}

} // mstl

#endif