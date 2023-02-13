#include "MEJobSystem.h"

#include <math.h>
#include <iostream>

#if defined (WIN32)
    #include <windows.h>
    #include <string>
#else

#endif

JobSystem::JobSystem(size_t userThreadCount, size_t adoptableThreadCount) noexcept
    :m_JobPool("JobSystem Job pool",MAX_JOB_COUNT*sizeof(Job))
    ,m_JobStroageBase(static_cast<Job*>(m_JobPool.getAllocator().getCurrent()))
{
    int threadPoolCount = (int)userThreadCount;
    if (threadPoolCount == 0) {
        //获取硬件核心
        int hwThreads = std::thread::hardware_concurrency();
        threadPoolCount = hwThreads - 1;
    }
    threadPoolCount = (std::max)(1, threadPoolCount);
    //限制不超过32
    threadPoolCount = (std::min)(threadPoolCount, 32);
    m_ThreadStates=std::vector<ThreadState>(threadPoolCount + adoptableThreadCount);
    m_ThreadCount = uint16_t(threadPoolCount);
    static_assert(std::atomic<bool>::is_always_lock_free);
    static_assert(std::atomic<uint16_t>::is_always_lock_free);

    std::random_device rd;
    const size_t hardwareThreadCount = m_ThreadCount;
    auto& states = m_ThreadStates;
 
    for (size_t i = 0, n = states.size(); i < n; ++i) {
        auto& state = states[i];
        state.m_Gen = std::default_random_engine(rd());
        state.m_JS = this;
        state.m_ID = (uint32_t)i;
        if (i < hardwareThreadCount) {
            state.m_Thread = std::thread(&JobSystem::loop,this, &state);
        }
    }
}

JobSystem::~JobSystem() {
    
}

void JobSystem::loop(ThreadState* state) noexcept {
    setThreadName("JobSystem::loop");
    std::cout << "loop" << std::endl;
    
    //设置线程亲和力，以防止从一个核心跳到另外一个核心。在Android上，亲和力似乎需要不时重置
    setThreadAffinityById(state->m_ID);
    
    m_ThreadMapLock.lock();
    bool inserted = m_ThreadMap.emplace(std::this_thread::get_id(), state).second;
    m_ThreadMapLock.unlock();
    if(!inserted) {
        std::cout << "This thread is already in a loop." << std::endl;
    }
    
    //运行主循环
    do {
        
        
        
    } while (!exitRequested());
    
}

void JobSystem::setThreadName(const char* name) noexcept {
#if defined(__LINUX__)
    pthread_setname_np(pthread_self(),name);
#elif defined(__APPLE__) 
    pthread_setname_np(name);
#elif defined(WIN32) 
    std::string_view u8Name(name);
    size_t size = MultiByteToWideChar(CP_UTF8, 0, u8Name.data(), u8Name.size(), nullptr, 0);

    std::wstring u16Name;
    u16Name.resize(size);
    MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, u8Name.data(), u8Name.size(), u16Name.data(), u16Name.size());

    SetThreadDescription(GetCurrentThread(), u16Name.data());
#endif
}

void JobSystem::setThreadAffinityById(size_t id) noexcept {
#if defined(__linux__)
    cpu_set_t set;
    CPU_ZERO(&set);
    CPU_SET(id,&set);
    sched_setaffinity(gettid(),sizeof(set),&set);
#endif
}

bool JobSystem::exitRequested() const noexcept {
    return m_ExitRequested.load(std::memory_order_relaxed)>0;
}

bool JobSystem::execute(JobSystem::ThreadState& state) noexcept {
 
    return false;
}

JobSystem::ThreadState& JobSystem::getState() noexcept {
    JobSystem::ThreadState state;
    
    return state;
}

JobSystem::Job* JobSystem::allocateJon() noexcept {
    return nullptr;
}

JobSystem::ThreadState* JobSystem::getStateToStealFrom(JobSystem::ThreadState& state) noexcept {
    return nullptr;
}

bool JobSystem::hasJobCompleted(Job* job) noexcept {
    return false;
}

void JobSystem::requestExit() noexcept {
    
}

bool JobSystem::hasActiveJobs() const noexcept {
    
}

JobSystem::Job* JobSystem::steal(JobSystem::ThreadState& state) noexcept {
    return nullptr;
}

void JobSystem::finish(Job* job) noexcept {
    
}

void JobSystem::put(WorkQueue& workQueue, Job* job) noexcept {
    
}

JobSystem::Job* JobSystem::pop(WorkQueue& workQueue) noexcept {
    return nullptr;
}

JobSystem::Job* JobSystem::steal(WorkQueue& workQueue) noexcept {
    return nullptr;
}

void JobSystem::wait(std::unique_lock<std::mutex>& lock, Job* job) noexcept {
    
}

void JobSystem::wakeAll() noexcept {
    
}

void JobSystem::wakeOne() noexcept {
    
}
