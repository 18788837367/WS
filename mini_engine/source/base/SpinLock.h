#pragma once

#include <atomic>

namespace ME {

class SpinLock {
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

}
