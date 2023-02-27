#include "JobSystem.h"

#include <math.h>
#include <iostream>

#if defined (WIN32)
    #include <windows.h>
    #include <string>
#else

#endif

namespace ME {

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
    m_ThreadStates= std::vector<ThreadState, STLAlignedAllocator<>>(threadPoolCount + adoptableThreadCount);
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
    requestExit();
    for (auto& state : m_ThreadStates) {
        state.m_Thread.join();
    }
}
void JobSystem::adopt() noexcept
{
    const auto tid = std::this_thread::get_id();

    std::unique_lock<SpinLock> lock(m_ThreadMapLock);
    auto iter = m_ThreadMap.find(tid);
    ThreadState* const state = (iter == m_ThreadMap.end() ? nullptr : iter->second);
    lock.unlock();

    if(state) {
        return;
    }

    uint16_t adopted = m_AdoptedThreads.fetch_add(1, std::memory_order_relaxed);
    size_t index = adopted + m_ThreadCount;

    if(index < m_ThreadStates.size()) {
        return;
    }

    lock.lock();
    m_ThreadMap[tid] = &m_ThreadStates[index];
}

void JobSystem::emancipate() noexcept
{
    const auto tid = std::this_thread::get_id();
    std::lock_guard<SpinLock> lock(m_ThreadMapLock);
    auto iter = m_ThreadMap.find(tid);
    ThreadState* const state = (iter==m_ThreadMap.end()?nullptr : iter->second);
    if(!state || state->m_JS!=this) {
        return;
    }
    m_ThreadMap.erase(iter);
}

JobSystem::Job* JobSystem::create(Job *parent, JobFunc func) noexcept
{
    parent =((parent==nullptr) ? m_RootJob : parent);
    Job* const job = allocateJob();
    if(job) {
        size_t index = 0x7FFF;
        if(parent) {
            auto parentJonCount = parent->m_RunningJobCount.fetch_add(1, std::memory_order_relaxed);
            assert(parentJonCount>0);
            index = parent - m_JobStroageBase;
            assert(index<MAX_JOB_COUNT);
        }
        job->m_Function=func;
        job->m_Parent=uint16_t(index);
    }
    return job;
}

void JobSystem::cancel(Job* &job) noexcept {
    finish(job);
    job = nullptr;
}

JobSystem::Job *JobSystem::retain(Job *job) noexcept
{
    JobSystem::Job* retained = job;
    incRef(retained);
     return retained;
}

void JobSystem::release(Job *&job) noexcept
{
    decRef(job);
    job = nullptr;
}

void JobSystem::run(Job *&job) noexcept
{
    ThreadState& state(getState());
    put(state.m_WorkQueue, job);
    job = nullptr;
}

void JobSystem::signal() noexcept
{
    wakeAll();
}

JobSystem::Job *JobSystem::runAndRetain(Job *job) noexcept
{
    JobSystem::Job* retained = retain(job);
    run(job);
     return retained;
}

void JobSystem::waitAndRelease(Job *&job) noexcept
{
    assert(job);
    assert(job->m_RefCount.load(std::memory_order_relaxed)>1);

    ThreadState& state(getState());
    do {
        if(!execute(state)) {
            //测试任务是否完成，避免获得锁
            if(hasJobCompleted(job)) {
                break;
            }

            //等待任务是否被其他线程处理
            //从execute返回，说明队列是空的，但任务还没完成，它在其他线程中运行，需要时间去等待
            std::unique_lock<std::mutex> lock(m_Mutex);
            if(!hasJobCompleted(job) && !hasActiveJobs() && !exitRequested()) {
                wait(lock,job);
            }
        }
    } while (!hasJobCompleted(job) && !exitRequested());

    if(job==m_RootJob) {
        m_RootJob=nullptr;
    }

    release(job);
}

void JobSystem::runAndWait(Job *&job) noexcept
{
    runAndRetain(job);
    waitAndRelease(job);
}

void JobSystem::loop(ThreadState* state) noexcept {
    setThreadName("JobSystem::loop");
    std::cout << "loop" << std::endl;
    
    //设置线程亲和力，以防止从一个核心跳到另外一个核心。在Android上，亲和力似乎需要不时重置
    setThreadAffinityById(state->m_ID);
    
    m_ThreadMapLock.lock();
    //emplace当插入成功返回true，插入失败则返回false(存在相同的键值对)
    bool inserted = m_ThreadMap.emplace(std::this_thread::get_id(), state).second;
    m_ThreadMapLock.unlock();
    if(!inserted) {
        std::cout << "This thread is already in a loop." << std::endl;
    }
    
    //运行主循环
    do {
        if(!execute(*state)) {
            //没有任务
            std::unique_lock<std::mutex> lock(m_Mutex);
            //没有退出请求
            //没有活动的任务
            while (!exitRequested() && !hasActiveJobs()) {
                wait(lock);
                setThreadAffinityById(state->m_ID);
            }
        }
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
    //线程退出
    return m_ExitRequested.load(std::memory_order_relaxed)>0;
}

bool JobSystem::execute(JobSystem::ThreadState& state) noexcept {
    Job* job = pop(state.m_WorkQueue);
    if(job==nullptr) {
        //我们的队列为空，尝试steal一个任务
        job = steal(state);
    }
    
    if(job) {
        assert(job->m_RunningJobCount.load(std::memory_order_relaxed)>=1);
        if(job->m_Function) {
            job->m_Function(job->m_Storage, *this, job);
        }
        finish(job);
    }
    return job!=nullptr;
}

JobSystem::ThreadState& JobSystem::getState() noexcept {
    std::lock_guard<std::mutex> lock(m_Mutex);
    auto iter = m_ThreadMap.find(std::this_thread::get_id());
    assert(iter != m_ThreadMap.end());
    return *iter->second;
}

void JobSystem::incRef(Job const* job) noexcept {
    //递增引用计数时不执行任何操作，因此我们使用memory_order_relaxed
    job->m_RefCount.fetch_add(1, std::memory_order_relaxed);
}

void JobSystem::decRef(Job const *job) noexcept {
    //我们需要确保在删除作业之前没有其他线程的访问。为了实现这一点，我们需要保证decRef的读写没有重新排序，
    //因为其他的线程可能持有最后一个引用
    auto c = job->m_RefCount.fetch_sub(1,std::memory_order_acq_rel);
    assert(c>0);
    if(c==1) {
        //这是最后一个引用，我们可能安全的删除job
        m_JobPool.destroy(job);
    }
}

JobSystem::Job* JobSystem::allocateJob() noexcept {
    return m_JobPool.make<Job>();
}

JobSystem::ThreadState* JobSystem::getStateToStealFrom(JobSystem::ThreadState& state) noexcept {
    auto& threadStates = m_ThreadStates;
    uint16_t adopted = m_AdoptedThreads.load(std::memory_order_relaxed);
    uint16_t const threadCount = adopted + m_ThreadCount;

    JobSystem::ThreadState* stateToStealFrom = nullptr;
    if (threadCount > 2) {
        do {
            std::uniform_int_distribution<uint16_t> rand(0, threadCount);
            uint16_t index = rand(state.m_Gen);
            assert(index < threadStates.size());
            stateToStealFrom = &threadStates[index];
        } while (stateToStealFrom == &state);
    }
    return stateToStealFrom;
}

bool JobSystem::hasJobCompleted(Job* job) noexcept {
    return job->m_RunningJobCount.load(std::memory_order_acquire)<=0;
}

void JobSystem::requestExit() noexcept {
    m_ExitRequested.store(true);
    std::lock_guard<std::mutex> lock(m_Mutex);
    m_WaiterCondition.notify_all();
}

bool JobSystem::hasActiveJobs() const noexcept {
    return m_ActiveJobs.load(std::memory_order_relaxed)>0;
}

JobSystem::Job* JobSystem::steal(JobSystem::ThreadState& state) noexcept {
    Job* job = nullptr;
    do {
        ThreadState* const stateToStealFrom = getStateToStealFrom(state);
        if(stateToStealFrom) {
            job=steal(stateToStealFrom->m_WorkQueue);
        }
        //如果有活动的任务，则该队列没有可获取的内容，那么尝试在来一次
    } while (!job && hasActiveJobs());
    return job;
}

void JobSystem::finish(Job* job) noexcept {
    bool notify = false;
    
    //终止任务，通知parent
    Job* const storage = m_JobStroageBase;
    do {
        auto runningJobCount = job->m_RunningJobCount.fetch_sub(1,std::memory_order_acq_rel);
        assert(runningJobCount>0);
        
        if(runningJobCount==1) {
            //没有更多的任务，销毁这个对象，通知parent
            notify=true;
            Job* const parent = job->m_Parent==0x7FFF?nullptr: &storage[job->m_Parent];
            decRef(job);
            job = parent;
        }
        else {
            //还有其他任务，但我们完成了
            break;
        }
    } while (job);
    
}

void JobSystem::put(WorkQueue& workQueue, Job* job) noexcept {
    assert(job);
    size_t index=job-m_JobStroageBase;
    assert(index>=0 && index<MAX_JOB_COUNT);
    
    workQueue.push(uint16_t(index+1));
    
    //增加任务活动数量
    uint32_t oldActionJobs=m_ActiveJobs.fetch_add(1, std::memory_order_relaxed);
    //有可能任务已经被提取，所以oldActionJobs是负值，当这种情况下，发出信号
    if(oldActionJobs>=0) {
        //唤醒另外一个线程
        wakeOne();
    }
}

JobSystem::Job* JobSystem::pop(WorkQueue& workQueue) noexcept {
    //递减活动任务，确保只剩下一个任务时（我们将要拾取它），其他线程不会循环尝试做相同的事情
    m_ActiveJobs.fetch_sub(1, std::memory_order_relaxed);
    
    size_t index = workQueue.pop();
    assert(index<=MAX_JOB_COUNT);
    
    Job* job = !index?nullptr: &m_JobStroageBase[index-1];
    //如果任务不存在，我们更正activeJobs
    if(!job) {
        if(m_ActiveJobs.fetch_add(1,std::memory_order_relaxed)>=0) {
            //如果有活跃的任务，我们需要唤醒其他线程。我们知道这不可能是当前线程，因为没获取到任务。
            wakeOne();
        }
    }
    return job;
}

JobSystem::Job* JobSystem::steal(WorkQueue& workQueue) noexcept {
    m_ActiveJobs.fetch_sub(1, std::memory_order_relaxed);

    size_t index = workQueue.steal();
    assert(index <= MAX_JOB_COUNT);

    Job* job = !index ? nullptr : &m_JobStroageBase[index-1];

    if (!job) {
        if (m_ActiveJobs.fetch_add(1, std::memory_order_relaxed) >= 0) {
            wakeOne();
        }
    }
    return job;
}

void JobSystem::wait(std::unique_lock<std::mutex>& lock, Job* job) noexcept {
    do {
        //当线程收到通知或者超时，说明系统已挂起
        std::cv_status status = m_WaiterCondition.wait_for(lock, std::chrono::milliseconds(4000));
        if(status==std::cv_status::no_timeout) {
            //没超时，说明被激活
            break;
        }

        //debug
        auto id = getState().m_ID;
        auto activeJobs = m_ActiveJobs.load();
        
        if (job) {
            //debug
        }
    } while (true);
}

void JobSystem::wakeAll() noexcept {
    std::lock_guard<std::mutex> lock(m_Mutex);
    m_WaiterCondition.notify_all();
}

void JobSystem::wakeOne() noexcept {
    std::lock_guard<std::mutex> lock(m_Mutex);
    m_WaiterCondition.notify_one();
}

}
