#include "Object.h"
#include "JobSystem.h"

#include <iostream>

using namespace ME;

void workStealingDequeueSingleThreaded() {
    struct MyJob {
    };
    WorkStealingDequeue<MyJob*, 4096> queue;
    std::vector<MyJob> jobs;
    jobs.resize(4096);
    
    MyJob aJob;
    queue.push(&aJob);
    
    if(&aJob != queue.pop()) {
        std::cout << "signal compare error" <<std::endl;
    }
    
    for(size_t i=0;i<4096; ++i) {
        queue.push(&jobs[i]);
    }
    for(size_t i=0;i<4096;++i) {
        MyJob* job = queue.pop();
        if(&jobs[4095-i] != job) {
            std::cout << "array compare error" <<std::endl;
        }
    }
    
    for(size_t i=0;i<4096; ++i) {
        queue.push(&jobs[i]);
    }
    for(size_t i=0;i<4096;++i) {
        MyJob* job = queue.steal();
        if(&jobs[i] != job) {
            std::cout << "steal compare error" <<std::endl;
        }
    }
}

void WorkStealingDequeue_PopSteal()
{
    struct MyJob {
    };
    WorkStealingDequeue<MyJob*, 65536> queue;
    size_t size = queue.getSize();
    
    MyJob pJob;
    MyJob sJob;
    
    for(size_t i=0; i<size/2; ++i) {
        queue.push(&sJob);
    }
    for(size_t i=0; i<size/2; ++i) {
        queue.push(&pJob);
    }
    
    size_t pop_size = size/2;
    std::thread pop_thread([&]() {
        //std::cout << "pop thread run\n";
        for(int i=0;i < pop_size; ++i) {
            if(queue.pop() != &pJob) {
                std::cout << "pop error" <<std::endl;
            }
        }
    });
    
    size_t steal_size = size/(2*4);
    std::thread steal_thread0([&]{
        //std::cout << "thread0 run\n";
        for(int i=0;i < steal_size; ++i) {
            if(&sJob != queue.steal()) {
                std::cout << "thread0 steal error" <<std::endl;
            }
        }
    });
    
    std::thread steal_thread1([&]{
        //std::cout << "thread1 run\n";
        for(int i=0;i<steal_size;++i) {
            if(&sJob != queue.steal()) {
                std::cout << "thread1 steal error" <<std::endl;
            }
        }
    });
    
    std::thread steal_thread2([&]{
        //std::cout << "thread2 run\n";
        for(int i=0;i<steal_size;++i) {
            if(&sJob != queue.steal()) {
                std::cout << "thread1 steal error" <<std::endl;
            }
        }
    });
    
    std::thread steal_thread3([&]{
        //std::cout << "thread3 run\n";
        for(int i=0;i<steal_size;++i) {
            if(&sJob != queue.steal()) {
                std::cout << "thread1 steal error" <<std::endl;
            }
        }
    });
    
    steal_thread0.join();
    steal_thread1.join();
    steal_thread2.join();
    steal_thread3.join();
    
    pop_thread.join();
    
    if(queue.getCount()!=0) {
        std::cout << "queue size error" <<std::endl;
    }
}

void WorkStealingDequeue_PushPopSteal()
{
    struct MyJob {
    };
    WorkStealingDequeue<MyJob*, 65536> queue;
    size_t size = queue.getSize();
    
    MyJob pJob;
    
    int pop = 0;
    int steal0 = 0;
    int steal1 = 0;
    int steal2 = 0;
    int steal3 = 0;
    
    size_t push_size = size;
    std::thread push_pop_thread([&]{
        for(int i=0; i<push_size/4; ++i) {
            queue.push(&pJob);
            queue.push(&pJob);
            queue.push(&pJob);
            queue.push(&pJob);
            
            if(queue.pop()) {
                pop++;
            }
            if(queue.pop()) {
                pop++;
            }
            if(queue.pop()) {
                pop++;
            }
            if(queue.pop()) {
                pop++;
            }
        }
    });
    
    size_t steal_size = size;
    std::thread steal_thread0([&]{
        for(int i=0;i<steal_size;++i) {
            if(queue.steal()) {
                steal0++;
            }
        }
    });
    std::thread steal_thread1([&]{
        for(int i=0;i<steal_size;++i) {
            if(queue.steal()) {
                steal1++;
            }
        }
    });
    std::thread steal_thread2([&]{
        for(int i=0;i<steal_size;++i) {
            if(queue.steal()) {
                steal2++;
            }
        }
    });
    std::thread steal_thread3([&]{
        for(int i=0;i<steal_size;++i) {
            if(queue.steal()) {
                steal3++;
            }
        }
    });
    
    steal_thread0.join();
    steal_thread1.join();
    steal_thread2.join();
    steal_thread3.join();
    push_pop_thread.join();
    
    if(pop+steal0+steal1+steal2+steal3 != size) {
        std::cout << "push pop steal error" <<std::endl;
    }
    
    if(queue.getCount()!=0) {
        std::cout << "queue size error" <<std::endl;
    }
}

void JobSystemParallelChildren() {
    static std::atomic_int v = {0};
    
    JobSystem js;
    js.adopt();
    
    struct User {
        std::atomic_int calls = {0};
        void func(JobSystem&, JobSystem::Job*) {
            v++;
            calls++;
        }
    } j;
    
    JobSystem::Job* root = js.createJob<User, &User::func>(nullptr, &j);
//    for(int i=0;i < 256; ++i) {
        JobSystem::Job* job = js.createJob<User, &User::func>(root, &j);
        js.run(job);
//    }
    js.runAndWait(root);
    
    
}

int main() {
    workStealingDequeueSingleThreaded();
    WorkStealingDequeue_PopSteal();
    WorkStealingDequeue_PushPopSteal();
    JobSystemParallelChildren();
    
    std::cout << "end" <<std::endl;
    return 0;
}
