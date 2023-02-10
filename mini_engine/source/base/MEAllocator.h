//
//  MEAllocator.cpp
//  WS
//
//  Created by zrf on 2023/1/9.
//
#pragma once

#include <stdlib.h>
#include <type_traits>
#include "MESpinLock.h"
#include <mutex>

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
    assert(align && !(align&align-1));
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
using SpinLock = MESpinLock;
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
    void* alloc(size_t size=ELEMENT_SIZE, size_t alignment=ALIGNMENT, size_t offset=OFFSET) noexcept {
     
        return nullptr;
    }
    
private:
    FREELIST m_FreeList;
};
