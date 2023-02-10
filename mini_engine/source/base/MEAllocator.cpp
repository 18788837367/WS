#include "MEAllocator.h"

LinearAllocator::LinearAllocator(void* begin, void* end) noexcept
    :m_Begin(begin), m_Size(uintptr_t(end)-uintptr_t(begin)){
}

LinearAllocator::LinearAllocator(LinearAllocator&& rhs) noexcept {
    this->swap(rhs);
}

LinearAllocator& LinearAllocator::operator=(LinearAllocator&& rhs) noexcept {
    if(this != &rhs) {
        this->swap(rhs);
    }
    return *this;
}

void LinearAllocator::swap(LinearAllocator& rhs) noexcept {
    std::swap(m_Begin, rhs.m_Begin);
    std::swap(m_Size, rhs.m_Size);
    std::swap(m_Cur, rhs.m_Cur);
}


FreeList::FreeList(void* begin, void* end, size_t elementSize, size_t alignment, size_t extra) noexcept
    :m_Head(init(begin, end, elementSize, alignment, extra)){
    
}

FreeList::Node* FreeList::init(void* begin, void* end, size_t elementSize, size_t alignment, size_t extra) noexcept {
    void* const p = pointermath::align(begin, alignment);
    void* const n = pointermath::align(pointermath::add(p, elementSize), alignment, extra);
    assert(p>=begin && p<end);
    assert(n>=begin && n<end && n>p);
    
    const size_t d = uintptr_t(n) - uintptr_t(p);
    const size_t num = (uintptr_t(end)-uintptr_t(begin))/d;
    
    
    
    
    
    
}
