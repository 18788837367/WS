//
//  MEJobSystem.h
//  WS
//
//  Created by zrf on 2023/1/9.
//
#pragma once

#include "MEObject.h"
#include <functional>
#include <atomic>
#include <mutex>
#include <thread>
#include <random>
#include <map>
#include "MEWorkStealingDequeue.h"
#include "MESpinLock.h"
#include "MEAllocator.h"

class JobSystem : public MEObject {
    static constexpr size_t MAX_JOB_COUNT = 16384;
    using WorkQueue = MEWorkStealingDequeue<uint16_t, MAX_JOB_COUNT>;
public:
    
    class Job;
    using JobFunc = void(*)(void*, JobSystem&, Job*);
    
    //64字节对齐
    class alignas(64) Job {
    public:
        Job() noexcept {}
        Job(const Job&)=delete;
        Job(Job&&)=delete;
        
    private:
        friend class JobSystem;
        static constexpr size_t JOB_STORAGE_SIZE_BYTES=sizeof(std::function<void()>) > 48 ? sizeof(std::function<void()>) : 48;
        static constexpr size_t JOB_STORAGE_SIZE_WORDS=(JOB_STORAGE_SIZE_BYTES+sizeof(void*)-1)/sizeof(void*);
        
        void* m_Storage[JOB_STORAGE_SIZE_WORDS];
        JobFunc m_Function;
        uint16_t m_Parent;
        std::atomic<uint16_t> m_RunningJobCount={1};
        mutable std::atomic<uint16_t> m_RefCount={1};
    };
    
    explicit JobSystem(size_t userThreadCount=0, size_t adoptableThreadCount=1) noexcept;
    ~JobSystem();
    
    
private:
    struct alignas(64) ThreadState {
        WorkQueue m_WorkQueue;
        JobSystem* m_JS=nullptr;
        std::thread m_Thread;
        std::default_random_engine m_Gen;
        uint32_t m_ID = { 0 };
    };

    void loop(ThreadState* state) noexcept;
    void setThreadName(const char* name) noexcept;
private:

    //以下存在线程竞争
    std::mutex m_Mutex;
    std::condition_variable m_WaiterCondition;
    std::atomic<uint32_t> m_ActiveJobs={0};

    Arena<ThreadSafeObjectPoolAllocator<Job>,LockingPolicy::NoLock> m_JobPool;

    std::vector<ThreadState> m_ThreadStates;
    std::atomic<bool> m_ExitRequested={false};
    std::atomic<uint16_t> m_AdoptedThreads;
    Job* const m_JobStroageBase=nullptr;
    uint16_t m_ThreadCount=0;
    uint8_t m_ParallelSplitCount=0;
    Job* m_RootJob=nullptr;
    
    MESpinLock m_ThreadMapLock;
    std::map<std::thread::id, ThreadState*> m_ThreadMap;
};
