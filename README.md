## 简单的stl库的实现

参考自[MyTinySTL](https://github.com/Alinshans/MyTinySTL)

简单实现了部分type_traits，iterator，iterator_traits，allocator（其中包括一个内存池），functor，基本算法以及stl中的容器。

容器的实现包括：pair，basic_string，vector，list，deque（包括以此为基础的stack和queue），priority_queue，hashtable（以此为基础的unordered_map，unordered_set），rb_tree（以此为基础的map，set）