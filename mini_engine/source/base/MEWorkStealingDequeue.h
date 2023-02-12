//
//  MEWorkStealingDequeue.h
//  WS
//
//  Created by zrf on 2023/1/9.
//
#pragma once

#include <atomic>
#include <assert.h>

/*
 * 模版类,无锁,固定大小队列
 *
 *
 *     top                          bottom
 *      v                             v
 *      |----|----|----|----|----|----|
 *    steal()                      push(), pop()
 *  any thread                     main thread
 *
 *
 */

template <typename TYPE, size_t COUNT>
class MEWorkStealingDequeue {
    static_assert(!(COUNT & (COUNT-1)), "COUNT must be a power of two");
    static constexpr size_t MASK=COUNT-1;
    
    using index_t = int64_t;
    std::atomic<index_t> m_Top={0};
    std::atomic<index_t> m_Buttom={0};
    TYPE m_Items[MASK];
    
    TYPE getItemAt(index_t index) noexcept {return m_Items[index&MASK];}
    void setItemAt(index_t index, TYPE item) noexcept {m_Items[index&MASK]=item;}
    
public:
    using value_type = TYPE;
    //在队列底部添加元素
    //必须在主线程调用
    inline void push(TYPE item) noexcept;

    //在队列底部移除元素
    //必须在主线程调用
    inline TYPE pop() noexcept;
    
    //从其他线程顶部窃取元素
    //可以与push、pop同时调用
    //成功返回元素，失败返回空元素
    inline TYPE steal() noexcept;
    
    size_t getSize() const noexcept {return COUNT;}
    
    size_t getCount() const noexcept {
        auto bottom=m_Buttom.load(std::memory_order_relaxed);
        auto top=m_Top.load(std::memory_order_relaxed);
        return bottom-top;
    }
};


template<typename TYPE, size_t COUNT>
void MEWorkStealingDequeue<TYPE, COUNT>::push(TYPE item) noexcept {
    auto bottom = m_Buttom.load(std::memory_order_relaxed);
    setItemAt(bottom, item);

    m_Buttom.store(bottom + 1, std::memory_order_relaxed);
}

template<typename TYPE, size_t COUNT>
TYPE MEWorkStealingDequeue<TYPE, COUNT>::pop() noexcept {
    auto bottom = m_Buttom.fetch_sub(1, std::memory_order_seq_cst) - 1;
    assert(bottom >= -1);

    auto top = m_Top.fetch_sub(1, std::memory_order_seq_cst);
    if (top < bottom) {
        return getItemAt(bottom);
    }

    TYPE item{};
    if (top == bottom) {
        item = getItemAt(bottom);

        if (m_Top.compare_exchange_strong(top, top + 1, std::memory_order_seq_cst, std::memory_order_relaxed)) {
            top++;
        }
        else {
            item = TYPE();
        }
    }
    else {
        assert(top - bottom == 1);
    }

    m_Buttom.store(top, std::memory_order_relaxed);
    return item;
}

template<typename TYPE, size_t COUNT>
TYPE MEWorkStealingDequeue<TYPE,COUNT>::steal() noexcept {
    while (true) {
        auto top = m_Top.load(std::memory_order_seq_cst);
        auto bottom = m_Buttom.load(std::memory_order_seq_cst);

        if (top >= bottom) {
            return TYPE();
        }

        TYPE item(getItemAt(top));
        if (m_Top.compare_exchange_strong(top, top + 1, std::memory_order_seq_cst, std::memory_order_relaxed)) {
            return item;
        }
    }
}
