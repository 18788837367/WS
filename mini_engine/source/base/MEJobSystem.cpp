#include "MEJobSystem.h"

JobSystem::JobSystem(size_t threadCount, size_t adoptableThreadCount) noexcept
    :m_JobPool("JobSystem Job pool",MAX_JOB_COUNT*sizeof(Job))
    ,m_JobStroageBase(static_cast<Job*>(m_JobPool.getAllocator().getCurrent()))
{



    
}

JobSystem::~JobSystem() {
    
}
