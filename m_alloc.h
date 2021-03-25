#ifndef M_ALLOC_H_
#define M_ALLOC_H_

// 以内存池的方式实现内存的管理

#include <new>
#include <cstddef>
#include <cstdlib>

namespace mstl {

// 采用union的方式以嵌入式指针形式完成内存的管理
union FreeList {
    union FreeList* next;   // 指向下一块内存地址
    char data[1];           // 储存本块内存的首地址
};

// 位于不同的内存范围，需要上调内存的大小
enum {
    EAlign128 = 8,
    EAlign256 = 16,
    EAlign512 = 32,
    EAlign1024 = 64,
    EAlign2048 = 128,
    EAlign4096 = 256,
};

// 大于ESmallObjectBytes的内存需求直接交给malloc进行
enum { ESmallObjectBytes = 4096 };

// FreeList的个数
enum { EFreeListsNumber = 56 };

class alloc {
private:
    static char* start_free;    // 内存池的开始地址
    static char* end_free;      // 内存池的结束地址
    static size_t heap_size;    // 申请的内存需要额外附加的大小

    // 维护内存池的链表
    static FreeList* free_list[EFreeListsNumber];
public:
    // 对外的分配内存接口
    static void* allocate(size_t n);
    static void deallocate(void* p, size_t n);
    static void* reallocate(void* p, size_t old_size, size_t new_size);
private:
    static size_t M_align(size_t bytes);
    static size_t M_round_up(size_t bytes);
    static size_t M_freelist_index(size_t bytes);
    static void* M_refill(size_t n);
    static char* M_chunk_alloc(size_t size, size_t &nobj);
};

// 初始化静态成员
char* alloc::start_free = nullptr;
char* alloc::end_free = nullptr;
size_t alloc::heap_size = 0;

FreeList* alloc::free_list[EFreeListsNumber] = {
    nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,
    nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,
    nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,
    nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,
    nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,
    nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,
    nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr
};

void* alloc::allocate(size_t n) {
    FreeList** my_free_list;
    FreeList* result;
    if (n > static_cast<size_t>(ESmallObjectBytes)) {
        return std::malloc(n);
    }
    my_free_list = free_list + M_freelist_index(n);
    result = *my_free_list;
    if (result == nullptr) {
        void* r = M_refill(M_round_up(n));
        return r;
    }
    *my_free_list = result->next;
    return result;
}

void alloc::deallocate(void* p, size_t n) {
    if (n > static_cast<size_t>(ESmallObjectBytes)) {
        std::free(p);
        return;
    }
    FreeList* ptr = reinterpret_cast<FreeList*>(p);
    FreeList** my_free_list = free_list + M_freelist_index(n);
    ptr->next = *my_free_list;
    *my_free_list = ptr;
}

void* alloc::reallocate(void* p, size_t old_size, size_t new_size) {
    deallocate(p, old_size);
    return allocate(new_size);
}

size_t alloc::M_align(size_t bytes) {
    if (bytes < 512) {
        return bytes <= 256 ? (bytes <= 128 ? EAlign128 : EAlign256) : EAlign512;
    } else {
        return bytes <= 2048 ? (bytes <= 1024 ? EAlign1024 : EAlign2048) : EAlign4096;
    }
}

// 上调bytes为8的整数倍
size_t alloc::M_round_up(size_t bytes) {
    return ((bytes + M_align(bytes) - 1) & ~(M_align(bytes) - 1));
}

// 返回当前大小应该在哪个链表上
size_t alloc::M_freelist_index(size_t bytes) {
    if (bytes <= 512) {
        return bytes <= 256 ? (bytes <= 128 ? ((bytes + EAlign128 - 1) / EAlign128 - 1)
        : (15 + (bytes + EAlign256 - 129) / EAlign256)) 
        : (23 + (bytes + EAlign512 - 257) / EAlign512);
    }
    return bytes <= 2048 ? (bytes <= 1024 ? (31 + (bytes + EAlign1024 - 513) / EAlign1024)
        : (39 + (bytes + EAlign2048 - 1025) / EAlign2048)) 
        : (47 + (bytes + EAlign4096 - 2049) / EAlign4096);
}

// 重新填满内存池
void* alloc::M_refill(size_t n) {
    size_t nblock = 10;
    char* c = M_chunk_alloc(n, nblock);
    FreeList** my_free_list;
    FreeList* result, *cur, *next;
    // 如果只得到一块，那么直接返回给客户
    if (nblock == 1) {
        return (void*)c;
    }
    // 否则返回一块给客户，其余用free_list维护
    // 取出对应free_list位置的指针
    my_free_list = free_list + M_freelist_index(n);
    result = (FreeList*)c;
    // 指向得到的内存剩余部分
    *my_free_list = next = (FreeList*)(c + n);
    for (size_t i = 1; ; ++i) {
        cur = next;
        next = (FreeList*)((char*)next + n);
        if (nblock - 1 == i) {
            cur->next = nullptr;
            break;
        } else {
            cur->next = next;
        }
    }
    return result;

}

// 内存分配函数
char* alloc::M_chunk_alloc(size_t size, size_t& nblock) {
    char* result;
    size_t need_bytes = size * nblock;
    size_t pool_bytes = end_free - start_free;

    // 内存池充足，直接返回首部指针即可
    if (need_bytes <= pool_bytes) {
        result = start_free;
        start_free += need_bytes;
        return result;
    } else if (size <= pool_bytes) {
        // 内存池大小不能完全满足需求，但是大于size大小，那么有多少尽量给多少
        nblock = pool_bytes / size;
        need_bytes = nblock * size;
        result = start_free;
        start_free += need_bytes;
        return result;
    } else {
        // 内存不足，需要额外申请
        // 如果内存池中还有剩余，那么榨干剩余
        if (pool_bytes > 0) {
            FreeList** m_free_list = free_list + M_freelist_index(pool_bytes);
            ((FreeList*)start_free)->next = *m_free_list;
            *m_free_list = (FreeList*)start_free;
        }
        // 申请内存，申请量为需求的两倍再加上一个随着申请次数逐渐增加的变化值
        size_t bytes_to_get = (need_bytes << 1) + M_round_up(heap_size >> 4);
        start_free = (char*)std::malloc(bytes_to_get);

        // 堆上的内存也不够用了
        if (start_free == nullptr) {
            FreeList* my_free_list, *ptr;
            // 尝试在free_list中向上寻找有内存的链表
            for (size_t i = size; i < ESmallObjectBytes; i += M_align(i)) {
                my_free_list = free_list[M_freelist_index(i)];
                ptr = my_free_list;
                if (ptr != nullptr) {
                    my_free_list = ptr->next;
                    start_free = (char*)ptr;
                    end_free = start_free + i;
                    return M_chunk_alloc(size, nblock);
                }
            }
            end_free = nullptr;
            throw std::bad_alloc();
        }
        end_free = start_free + bytes_to_get;
        heap_size += bytes_to_get;
        return M_chunk_alloc(size, nblock);
    }
}


}// mstl

#endif

// 简单的测试程序
// int main() {
//     mstl::alloc allocator;
//     int (*a)[5] = (int(*)[5])allocator.allocate(sizeof(int) * 5);
//     for (int i = 0; i < 5; ++i) {
//         *((*a) + i) = 2 * i;
//     }
//     double *b0 = (double*)allocator.allocate(sizeof(double));
//     double *b1 = (double*)allocator.allocate(sizeof(double));
//     allocator.deallocate(b1, sizeof(double));
//     double *b2 = (double*)allocator.allocate(sizeof(double));
//     // 指针相减，差值为1代表内存相邻，为0代表指向同一块内存
//     std::cout << "b1 - b0 = " << b1 - b0 << "\n" << "b2 - b1 = " << b2 - b1;
//     return 0;
// }