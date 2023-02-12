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
        //获取系统核心树
        int hwThreads = std::thread::hardware_concurrency();
        threadPoolCount = hwThreads - 1;
    }
    threadPoolCount = (std::max)(1, threadPoolCount);
    //限制最大32个
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
}

void JobSystem::setThreadName(const char* name) noexcept {
#if defined(__LINUX__)

#elif defined(__APPLE__) 

#elif defined(WIN32) 
    std::string_view u8Name(name);
    size_t size = MultiByteToWideChar(CP_UTF8, 0, u8Name.data(), u8Name.size(), nullptr, 0);

    std::wstring u16Name;
    u16Name.resize(size);
    MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, u8Name.data(), u8Name.size(), u16Name.data(), u16Name.size());

    SetThreadDescription(GetCurrentThread(), u16Name.data());
#endif
}