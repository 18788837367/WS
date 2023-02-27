//
//  MEJobSystem.h
//  WS
//
//  Created by zrf on 2023/1/9.
//
#pragma once

#include "Object.h"
#include <functional>
#include <atomic>
#include <mutex>
#include <thread>
#include <random>
#include <map>
#include "WorkStealingDequeue.h"
#include "SpinLock.h"
#include "Allocator.h"

namespace ME {

class JobSystem : public Object {
    static constexpr size_t MAX_JOB_COUNT = 16384;
    using WorkQueue = WorkStealingDequeue<uint16_t, MAX_JOB_COUNT>;
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
    
    //当前线程加入线程池
    void adopt() noexcept;

    //移除当前线程
    void emancipate() noexcept;

    //设置当前节点为根节点
    Job* setRootJob(Job* job) noexcept { return m_RootJob = job; }
    
    //创建Job
    Job* create(Job* parent, JobFunc func) noexcept;

    Job* create(Job* parent = nullptr) noexcept {
        return create(parent, nullptr);
    }
   
    template<typename T, void(T::*method)(JobSystem&, Job*)>
    Job* createJob(Job* parent, T* data) noexcept {
        Job* job = create(parent, [](void* user, JobSystem& js, Job* job) {
            (*static_cast<T**>(user)->*method)(js, job);
        });
        if(job) {
            job->m_Storage[0]=data;
        }
        return job;
    }

    template<typename T, void(T::*method)(JobSystem&, Job*)>
    Job* createJob(Job* parent, T data) noexcept {
        static_assert(sizeof(data)<=sizeof(Job::m_Storage), "user data too large");
        Job* job = create(parent, [](void* user, JobSystem& js, Job* job) {
            T* that = static_cast<T*>(user);
            (that->*method)(js, job);
            that->~T();
        });
        if(job) {
            new (job->m_Storage) T(std::move(data));
        }
        return job;
    }

    template<typename T>
    Job* createJob(Job* parent, T functor) noexcept {
        static_assert(sizeof(functor)<=sizeof(Job::m_Storage), "functor too large");
        Job* job = create(parent, [](void* user, JobSystem& js, Job* job) {
            T& that = *static_cast<T*>(user);
            that(js, job);
            that->~T();
        });
        if(job) {
            new (job->m_Storage) T(std::move(functor));
        }
        return job;
    }

    //通常任务是自动完成的，可在任务运行之前取消，一旦调用run,就不要尝试取消它
    void cancel(Job* &job) noexcept;

    //添加任务的引用，运行调用者从多个线程对此任务执行runAndRelease。
    //如果任务不需要线程等待，执行runAndWait。
    //该任务必须使用runAndRelease、release来释放
    Job* retain(Job* job) noexcept;

    //释放通过retain或者runAndRetain引用的任务，调用之后无法使用该任务
    void release(Job*& job) noexcept;
    void release(Job* && job) noexcept {
        Job* p = job;
        release(p);
    }

    //将任务添加到该线程的执行队列，引用计数自动下降，当前线程必须属于JobSystem
    void run(Job*& job) noexcept;
    void run(Job*&& job) noexcept {
        Job* p = job;
        run(p);
    }

    void signal() noexcept;

    //将任务添加到该线程的执行队列并保留它的引用，当前线程必须属于JobSystem
    //该任务必须wait等待，或者release释放
    Job* runAndRetain(Job* job) noexcept;

    //等待任务并销毁它，当前线程必须属于JobSystem，
    //任务必须从retain、runAndRetain中获取
    void waitAndRelease(Job*& job) noexcept;

    //运行并等待任务，这相当于调用runAndRetain(job)、wait(job)
    void runAndWait(Job*& job) noexcept;
    void runAndWait(Job*&& job) noexcept {
        Job* p = job;
        runAndWait(p);
    }

    size_t getThreadCount() noexcept {
        return m_ThreadCount;
    }
public:
    static void setThreadName(const char* name) noexcept;
    //设置线程亲和力
    static void setThreadAffinityById(size_t id) noexcept;
    
private:
    struct alignas(64) ThreadState {
        WorkQueue m_WorkQueue;
        JobSystem* m_JS=nullptr;
        std::thread m_Thread;
        std::default_random_engine m_Gen;
        uint32_t m_ID = { 0 };
    };
    
    ThreadState& getState() noexcept;
    
    void incRef(Job const *job) noexcept;
    void decRef(Job const *job) noexcept;
    
    Job* allocateJob() noexcept;
    JobSystem::ThreadState* getStateToStealFrom(JobSystem::ThreadState& state) noexcept;
    bool hasJobCompleted(Job* job) noexcept;
    
    void requestExit() noexcept;
    bool exitRequested() const noexcept;
    bool hasActiveJobs() const noexcept;
    
    void loop(ThreadState* state) noexcept;
    bool execute(JobSystem::ThreadState& state) noexcept;
    Job* steal(JobSystem::ThreadState& state) noexcept;
    void finish(Job* job) noexcept;
    
    void put(WorkQueue& workQueue, Job* job) noexcept;
    Job* pop(WorkQueue& workQueue) noexcept;
    Job* steal(WorkQueue& workQueue) noexcept;
    
    void wait(std::unique_lock<std::mutex>& lock, Job* job=nullptr) noexcept;
    void wakeAll() noexcept;
    void wakeOne() noexcept;
private:

    //以下存在线程竞争
    std::mutex m_Mutex;
    std::condition_variable m_WaiterCondition;
    std::atomic<uint32_t> m_ActiveJobs={0};

    Arena<ThreadSafeObjectPoolAllocator<Job>,LockingPolicy::NoLock> m_JobPool;

    template<class T>
    using aligned_vector=std::vector<T, STLAlignedAllocator<T>>;
    
    std::vector<ThreadState, STLAlignedAllocator<> > m_ThreadStates;
    std::atomic<bool> m_ExitRequested={false};
    std::atomic<uint16_t> m_AdoptedThreads;
    Job* const m_JobStroageBase=nullptr;
    uint16_t m_ThreadCount=0;
    uint8_t m_ParallelSplitCount=0;
    Job* m_RootJob=nullptr;
    
    SpinLock m_ThreadMapLock;
    std::map<std::thread::id, ThreadState*> m_ThreadMap;
};

}
