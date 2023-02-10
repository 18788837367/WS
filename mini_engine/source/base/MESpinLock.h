//
//  MESpinLock.cpp
//  WS
//
//  Created by zrf on 2023/1/9.
//
#pragma once

#include <atomic>

class MESpinLock {
public:
    void lock() noexcept {
        do {
            
        } while (m_Lock.test_and_set(std::memory_order_acquire));
    }
    
    void unlock() noexcept {
        m_Lock.clear(std::memory_order_release);
    }
private:
    std::atomic_flag m_Lock=ATOMIC_FLAG_INIT;
};
