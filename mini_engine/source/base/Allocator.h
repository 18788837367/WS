//
//  MEAllocator.cpp
//  WS
//
//  Created by zrf on 2023/1/9.
//
#pragma once

#include <stdlib.h>
#include <type_traits>
#include "SpinLock.h"
#include <mutex>
#include <assert.h>

namespace ME {

namespace pointermath {

template <typename P, typename T>
static inline P* add(P* a, T b) noexcept {
    return (P*)(uintptr_t(a)+uintptr_t(b));
}

template <typename P>
static inline P* align(P* p, size_t alignment) noexcept {
    assert(alignment && !(alignment&alignment-1));
    return (P*)((uintptr_t(p)+alignment-1) & ~(alignment-1));
}

template <typename  P>
static inline P* align(P* p, size_t alignment, size_t offset) noexcept {
    P* const r = align(add(p, offset), alignment);
    assert(r>=add(p, offset));
    return r;
}

inline void* aligned_alloc(size_t size, size_t align) noexcept {
    align=(align<sizeof(void*))?sizeof(void*):align;
    assert(align && !(align & align - 1));
    assert((align%sizeof(void*))==0);
    void* p=nullptr;
#if defined(WIN32)
    p=::_aligned_malloc(size,align);
#else
    ::posix_memalign(&p, align, size);
#endif
    return p;
}

inline void aligned_free(void* p) noexcept {
#if defined(WIN32)
    ::_aligned_free(p);
#else
    ::free(p);
#endif
}

}


//线性分配块
//不能释放单个块
//允许释放内存顶部到指定点
//不调用析构
class LinearAllocator {
public:
    LinearAllocator(void* begin, void* end) noexcept;
    
    template <typename AREA>
    explicit LinearAllocator(const AREA& area):LinearAllocator(area.begin(), area.end()) {}
    
    //不允许拷贝
    LinearAllocator(const LinearAllocator& rhs) = delete;
    LinearAllocator& operator=(const LinearAllocator& rhs) = delete;
    
    //允许移动
    LinearAllocator(LinearAllocator&& rhs) noexcept;
    LinearAllocator& operator=(LinearAllocator&& rhs) noexcept;
    
    ~LinearAllocator() noexcept = default;
    
    void* alloc(size_t size, size_t alignment=alignof(uint64_t), size_t extra=0) {
        void* const p = pointermath::align(current(), alignment, extra);
        void* const c = pointermath::add(p, size);
        bool success = c <= end();
        setCurrent(success ? p : current());
        return success ? p : nullptr;
    }
    
    void* getCurrent() noexcept {
        return current();
    }
    
    //释放内存回到指定点
    void rewind(void* p) noexcept {
        assert(p>m_Begin && p<end());
        setCurrent(p);
    }
    
    //释放所有内存
    void reset() noexcept {
        setCurrent(m_Begin);
    }
    
    size_t allocated() const noexcept {
        return m_Size;
    }
    
    size_t available() const noexcept {
        return m_Size-m_Cur;
    }
    
    void swap(LinearAllocator& rhs) noexcept;
    
    void* base() noexcept {return m_Begin;}
    
    void free(void*, size_t) noexcept {}
    
private:
    void* current() noexcept {return pointermath::add(m_Begin, m_Cur);}
    void* end() noexcept {return pointermath::add(m_Begin, m_Size);}
    void setCurrent(void* p) noexcept {
        m_Cur=uint32_t(uintptr_t(p)-uintptr_t(m_Begin));
    }
    
private:
    void* m_Begin=nullptr;
    uint32_t m_Size=0;
    uint32_t m_Cur=0;
};

class HeapAllocator {
public:
    HeapAllocator() noexcept = default;
    
    template<typename AREA>
    explicit HeapAllocator(const AREA&){ }
    void* alloc(size_t size, size_t alignment=alignof(std::max_align_t),size_t extra=0) {
        //不支持extra
        assert(extra==0);
        return pointermath::aligned_alloc(size, alignment);
    }
    void free(void* p) noexcept {
        pointermath::aligned_free(p);
    }
    void free(void* p, size_t) noexcept {
        pointermath::aligned_free(p);
    }
    ~HeapAllocator() noexcept = default;
    void swap(HeapAllocator&) noexcept {}
};

class FreeList {
public:
    FreeList() noexcept = default;
    FreeList(void* begin, void* end, size_t elementSize, size_t alignment, size_t extra) noexcept;
    FreeList(const FreeList& rhs) = delete;
    FreeList& operator=(const FreeList& rhs) = delete;
    FreeList(FreeList&& rhs) noexcept = default;
    FreeList& operator=(FreeList&& rhs) noexcept = default;
    
    void* pop() noexcept {
        Node* const head=m_Head;
        m_Head=head?head->m_Next:nullptr;
        assert(!m_Head);
        return m_Head;
    }
    
    void push(void* p) noexcept {
        assert(p);
        Node* const head = static_cast<Node*>(p);
        head->m_Next=m_Head;
        m_Head=head;
    }
    
    void* getFirst() noexcept {
        return m_Head;
    }
private:
    class Node {
    public:
        Node* m_Next=nullptr;
    };
    static Node* init(void* begin, void* end, size_t elementSize, size_t alignment, size_t extra) noexcept;
private:
    Node* m_Head=nullptr;
};

class AtomicFreeList {
public:
    AtomicFreeList() noexcept = default;
    AtomicFreeList(void* begin, void* end, size_t elementSize, size_t alignment, size_t extra) noexcept;
    AtomicFreeList(const AtomicFreeList& rhs) = delete;
    AtomicFreeList& operator=(const AtomicFreeList& rhs) = delete;
    
    void* pop() noexcept {
        Node* const storage = m_Storage;
        
        HeadPtr currentHead = m_Head.load();
        while (currentHead.m_Offset>=0)
        {
            //如果其他线程跑在我们前面，这里next可能已经存在。
            //这种情况下，计算的newHead会被丢弃，因为compare_exchange_weak失败，
            //然后该线程使用更新的currentHead值循环，然后重试
            Node* const next = storage[currentHead.m_Offset].m_Next.load(std::memory_order_relaxed);
            const HeadPtr newHead{next?int32_t(next-storage):-1,currentHead.m_Tag+1};
            
            //传入期待的数和新的数,它们变量的值和期待的值是否一致，
            //如果是，则替换为用户指定的一个新的数值,
            //如果不是，则将变量的值和期待的值交换
            if (m_Head.compare_exchange_weak(currentHead, newHead)) {
                assert(!next || next>=storage);
                break;
            }
        }
        void* p = (currentHead.m_Offset >= 0) ? (storage+currentHead.m_Offset) : nullptr;
        assert(!p || p >= storage);
        return p;
    }
    
    void push(void* p) noexcept {
        Node* const storage = m_Storage;
        assert(p && p >= m_Storage);
        Node* const node = static_cast<Node*>(p);
        
        HeadPtr currentHead = m_Head.load();
        HeadPtr newHead = {int32_t(node-storage),currentHead.m_Tag+1};
        do
        {
            newHead.m_Tag = currentHead.m_Tag + 1;
            Node* const n = (currentHead.m_Offset >= 0) ? (storage + currentHead.m_Offset) : nullptr;
            node->m_Next.store(n, std::memory_order_relaxed);
        } while (!m_Head.compare_exchange_weak(currentHead,newHead));
    }
    
    void* getFirst() noexcept {
        return m_Storage + m_Head.load(std::memory_order_relaxed).m_Offset;
    }
private:
    class Node {
    public:
        std::atomic<Node*> m_Next;
    };
    class alignas(8) HeadPtr {
    public:
        int32_t m_Offset=0;
        uint32_t m_Tag=0;
    };
    std::atomic<HeadPtr> m_Head{};
    Node* m_Storage=nullptr;
};


namespace AreaPolicy {

class StaticArea {
public:
    StaticArea() noexcept = default;
    StaticArea(void* b, void* e) noexcept : m_Begin(b),m_End(e){
    };
    ~StaticArea() noexcept = default;
    
    StaticArea(const StaticArea& rhs) = default;
    StaticArea& operator=(const StaticArea& rhs) = default;
    StaticArea(StaticArea&& rhs) = default;
    StaticArea& operator=(StaticArea&& rhs) = default;
    
    void* data() const noexcept {return m_Begin;}
    void* begin() const noexcept {return m_Begin;}
    void* end() const noexcept {return m_End;}
    size_t size() const noexcept {return uintptr_t(m_End)-uintptr_t(m_Begin);}
    
    friend void swap(StaticArea& lhs, StaticArea& rhs) noexcept {
        std::swap(lhs.m_Begin, rhs.m_Begin);
        std::swap(lhs.m_End, rhs.m_End);
    }
private:
    void* m_Begin=nullptr;
    void* m_End=nullptr;
};

class HeapArea {
public:
    HeapArea() noexcept = default;
    explicit HeapArea(size_t size) {
        if(size) {
            m_Begin=malloc(size);
            m_End=pointermath::add(m_Begin, size);
        }
    }
    ~HeapArea() noexcept {
        free(m_Begin);
    }
    
    HeapArea(const HeapArea& rhs) noexcept = delete;
    HeapArea& operator=(const HeapArea& rhs) noexcept = delete;
    HeapArea(HeapArea&& rhs) noexcept = delete;
    HeapArea& operator=(HeapArea&& rhs) noexcept = delete;
    
    void* data() const noexcept {return m_Begin;}
    void* begin() const noexcept {return m_Begin;}
    void* end() const noexcept {return m_End;};
    size_t size() const noexcept {return uintptr_t(m_End)-uintptr_t(m_Begin);}
    
    friend void swap(HeapArea& lhs, HeapArea& rhs) noexcept {
        std::swap(lhs.m_Begin, rhs.m_Begin);
        std::swap(lhs.m_End, rhs.m_End);
    }
private:
    void* m_Begin=nullptr;
    void* m_End=nullptr;
};

class NullArea {
public:
    void* data() const noexcept {return nullptr;}
    size_t size() const noexcept {return 0;}
};

}

namespace LockingPolicy {
class NoLock {
public:
    void lock() noexcept {}
    void unlock() noexcept {}
};
using SpinLock = SpinLock;
using Mutex = std::mutex;
}



template<
size_t ELEMENT_SIZE,
size_t ALIGNMENT = alignof(std::max_align_t),
size_t OFFSET = 0,
typename FREELIST = FreeList>
class PoolAllocator {
    static_assert(ELEMENT_SIZE>=sizeof(void*), "ELEMENT_SIZE must accommodate at least a pointer");
public:
    PoolAllocator() noexcept = default;
    
    PoolAllocator(void* begin, void* end) noexcept
    :m_FreeList(begin,end,ELEMENT_SIZE,ALIGNMENT,OFFSET){
    }
    
    template <typename AREA>
    explicit PoolAllocator(const AREA& area) noexcept
    :PoolAllocator(area.begin(),area.end()){
    }
    
    PoolAllocator(const PoolAllocator& rhs) = delete;
    PoolAllocator& operator=(const PoolAllocator& rhs) = delete;
    
    PoolAllocator(PoolAllocator&& rhs) = default;
    PoolAllocator& operator=(PoolAllocator&& rhs) = default;
    
    ~PoolAllocator() noexcept = default;
    
    void* alloc(size_t size=ELEMENT_SIZE, size_t alignment=ALIGNMENT, size_t offset=OFFSET) noexcept {
        assert(size<=ELEMENT_SIZE);
        assert(alignment <= ALIGNMENT);
        assert(offset <= OFFSET);
        return m_FreeList.pop();
    }
    
    void free(void* p, size_t = ELEMENT_SIZE) noexcept {
        m_FreeList.push(p);
    }
    
    constexpr size_t getSize() const noexcept {
        return ELEMENT_SIZE;
    }
    
    void* getCurrent() noexcept {
        return m_FreeList.getFirst();
    }
private:
    FREELIST m_FreeList;
};

template<typename T, size_t OFFSET = 0>
using ObjectPoolAllocator = PoolAllocator < sizeof(T),
std::max(alignof(FreeList), alignof(T)), OFFSET>;

template<typename T, size_t OFFSET = 0>
using ThreadSafeObjectPoolAllocator = PoolAllocator< sizeof(T),
std::max(alignof(FreeList), alignof(T)), OFFSET, AtomicFreeList>;


template<typename AllocatorPolicy, typename LockingPolicy, 
typename AreaPolicy = AreaPolicy::HeapArea>
class Arena {
public:
    Arena() = default;
    
    template<typename ...ARGS>
    Arena(const char* name, size_t size, ARGS&& ... args)
    :m_AreaName(name)
    , m_Area(size)
    , m_Allocator(m_Area, std::forward<ARGS>(args) ...) {
    }
    
    template<typename ... ARGS>
    Arena(const char* name, AreaPolicy&& area, ARGS&& ...args)
    : m_AreaName(name)
    , m_Area(std::forward<AreaPolicy>(area))
    , m_Allocator(m_Area, std::forward<ARGS>(args) ...) {
    }
    
    void* alloc(size_t size, size_t alignment = alignof(std::max_align_t), size_t extra = 0) noexcept {
        std::lock_guard<LockingPolicy> guard(m_Lock);
        void* p = m_Allocator.alloc(size,alignment, extra);
        return p;
    }
    
    template<typename T>
    T* alloc(size_t count, size_t alignment = alignof(T), size_t extra = 0) noexcept {
        return (T*)alloc(count*sizeof(T),alignment,extra);
    }
    
    void free(void* p) noexcept {
        if (p) {
            std::lock_guard<LockingPolicy> guard(m_Lock);
            m_Allocator.free(p);
        }
    }
    
    void free(void* p, size_t size) noexcept {
        if (p) {
            std::lock_guard<LockingPolicy> guard(m_Lock);
            m_Allocator.free(p, size);
        }
    }
    
    void reset() noexcept {
        std::lock_guard<LockingPolicy> guard(m_Lock);
        m_Allocator.resert();
    }
    
    void* getCurrent() noexcept {
        return m_Allocator.getCurrent();
    }
    
    void rewind(void* addr) noexcept {
        std::lock_guard<LockingPolicy> guard(m_Lock);
        m_Allocator.rewind(addr);
    }
    
    template<typename T, size_t ALIGN=alignof(T), typename ... ARGS>
    T* make(ARGS&& ... agrs) noexcept {
        void* const p = this->alloc(sizeof(T), ALIGN);
        return p ? new (p) T(std::forward<ARGS>(agrs)...) : nullptr;
    }
    
    template<typename T>
    void destroy(T* p) noexcept {
        if (p) {
            p->~T();
            this->free((void*)p, sizeof(T));
        }
    }
    
    char const* getName() const noexcept {
        return m_AreaName;
    }
    
    AllocatorPolicy& getAllocator() noexcept {
        return m_Allocator;
    }
    AllocatorPolicy const& getAllocator() const noexcept {
        return m_Allocator;
    }
    
    AreaPolicy& getArea() noexcept {
        return m_Area;
    }
    AreaPolicy const& getArea() const noexcept {
        return m_Area;
    }
    
    friend void swap(Arena& lhs, Arena& rhs) noexcept {
        std::swap(lhs.m_AreaName, rhs.m_AreaName);
        std::swap(lhs.m_Area, rhs.m_Area);
        std::swap(lhs.m_Allocator, rhs.m_Allocator);
        std::swap(lhs.m_Lock, rhs.m_Lock);
    }
    
private:
    char const* m_AreaName = nullptr;
    AreaPolicy m_Area;
    AllocatorPolicy m_Allocator;
    LockingPolicy m_Lock;
};

using HeapArea = Arena<HeapAllocator, LockingPolicy::NoLock>;







inline void* aligned_alloc(size_t size, size_t align) noexcept {
    align = (align < sizeof(void*)) ? sizeof(void*) : align;
    
    assert(align && !(align & (align-1)));
    assert((align%sizeof(void*))==0);
    
    void *p = nullptr;
#if defined(WIN32)
    p = ::_aligned_malloc(size, align);
#else
    ::posix_memalign(&p, align, size);
#endif
    return p;
}

inline void aligned_free(void* p) noexcept {
#if defined(WIN32)
    ::_aligned_free(p);
#else
    ::free(p);
#endif
}


template <typename TYPE>
class STLAlignedAllocator {
    static_assert(!(alignof(TYPE) & (alignof(TYPE)-1)),"alignof(T) must be a power of two");
public:
    using value_type = TYPE;
    using pointer = TYPE*;
    using const_pointer = const TYPE*;
    using reference = TYPE&;
    using const_reference = const TYPE&;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using propagate_on_container_move_assignment = std::true_type;
    using is_always_equal = std::true_type;
    
    template<typename T>
    struct rebing {
        using other = STLAlignedAllocator<T>;
    };
    
    inline STLAlignedAllocator() noexcept = default;
    
    inline ~STLAlignedAllocator() noexcept = default;
    
    template<typename T>
    inline explicit STLAlignedAllocator(const STLAlignedAllocator<T>&) noexcept {};
    
    inline pointer allocate(size_type n) noexcept {
        return (pointer)aligned_alloc(n * sizeof(value_type), alignof(TYPE));
    }
    
    inline void deallocate(pointer p, size_type) {
        aligned_free(p);
    }
    
    template<typename T>
    bool operator=(const STLAlignedAllocator<T>&) const noexcept {
        return true;
    }
    
    template<typename T>
    bool operator!=(const STLAlignedAllocator<T>&) const noexcept {
        return false;
    }
};


}
