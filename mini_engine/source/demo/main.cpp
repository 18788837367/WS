#include "Object.h"
#include "JobSystem.h"
#include "vec2.h"

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
    for(int i=0;i < 256; ++i) {
        JobSystem::Job* job = js.createJob<User, &User::func>(root, &j);
        js.run(job);
    }
    js.runAndWait(root);
    
    if(v.load()!=257 || j.calls!=257) {
        std::cout << "JobSystemParallelChildren error" <<std::endl;
    }
    
    js.emancipate();
}

void JobSystemSequentialChildren()
{
    JobSystem js;
    js.adopt();
    
    struct User {
        int c;
        int i, j;
        void func(JobSystem& js, JobSystem::Job* job) {
            if(c < 43) {
                User u{c+1};
                JobSystem::Job* p = js.createJob<User, &User::func>(job, &u);
                js.runAndWait(p);
                
                i=u.i+u.j;
                j=u.i;
            }
            else {
                i=0;
                j=1;
            }
        }
    };
    
    User u{0};
    
    JobSystem::Job* root = js.createJob<User, &User::func>(nullptr, &u);
    js.runAndWait(root);
    
    if(u.i!=433494437) {
        std::cout << "JobSystemSequentialChildren error:" << u.i << std::endl;
    }
    
    js.emancipate();
}

void JobSystemParallelFor()
{
    
    
    TVec2<float> xx;
    xx.x=1;
    xx.y=2;
    TVec2<double> yy;
    yy.x=11;
    yy.y=12;
    xx+=yy;
    std::cout << "size:" << xx.x << std::endl;
    
    auto a1 = xx+yy;
    std::cout << "size:" << a1.x << "  " << a1.y << std::endl;
    
    auto a2 = xx+5;
    std::cout << "size:" << a2.x << "  " << a2.y << std::endl;
    
    auto a3 = 5+xx;
    std::cout << "size:" << a3.x << "  " << a3.y << std::endl;
    
    auto z1 = xx-yy;
    auto z2 = xx-5;
    auto z3 = 5-xx;
    
    std::cout << "size:" << z1.x << "  " << z1.y << std::endl;
    std::cout << "size:" << z2.x << "  " << z2.y << std::endl;
    std::cout << "size:" << z3.x << "  " << z3.y << std::endl;
    
    
    xx*=yy;
    std::cout << "size:" << xx.x << "  " << xx.y << std::endl;
    xx*=5;
    std::cout << "size:" << xx.x << "  " << xx.y << std::endl;
    
    xx/=yy;
    std::cout << "size:" << xx.x << "  " << xx.y << std::endl;
    xx/=5;
    std::cout << "size:" << xx.x << "  " << xx.y << std::endl;
    
    {
        auto a4 = xx * 5;
        auto a5 = 5 * xx;
        auto a6 = xx * yy;
        auto a7 = xx / 5;
        auto a8 = 5 / xx;
        auto a9 = xx / yy;
        std::cout << "size:" << a4.x << "  " << a4.y << std::endl;
        std::cout << "size:" << a5.x << "  " << a5.y << std::endl;
        std::cout << "size:" << a6.x << "  " << a6.y << std::endl;
        std::cout << "size:" << a7.x << "  " << a7.y << std::endl;
        std::cout << "size:" << a8.x << "  " << a8.y << std::endl;
        std::cout << "size:" << a9.x << "  " << a9.y << std::endl;
    }
    
    {
        auto a10 = -xx;
        std::cout << "size:" << a10.x << "  " << a10.y << std::endl;
    }
    
    {
        xx.x=1;
        xx.y=2;
        TVec2<float> xx1;
        xx1.x=1;
        xx1.y=2;
        auto a11 = equal(xx, xx1);
        auto a12 = notEqual(xx, xx1);
        auto a13 = lessThan(xx, xx1);
        auto a14 = lessThanEqual(xx, xx1);
        auto a15 = greaterThan(xx, xx1);
        auto a16 = greaterThanEqual(xx, xx1);
        std::cout << "size:" << (xx1==xx) << std::endl;
        std::cout << "size:" << (xx1!=xx) << std::endl;
        std::cout << "size:" << a11.x << " " << a11.y << std::endl;
        std::cout << "size:" << a12.x << " " << a12.y << std::endl;
        std::cout << "size:" << a13.x << " " << a13.y << std::endl;
        std::cout << "size:" << a14.x << " " << a14.y << std::endl;
        std::cout << "size:" << a15.x << " " << a15.y << std::endl;
        std::cout << "size:" << a16.x << " " << a16.y << std::endl;
        xx1.x=11;
        auto a21 = equal(xx, xx1);
        auto a22 = notEqual(xx, xx1);
        auto a23 = lessThan(xx, xx1);
        auto a24 = lessThanEqual(xx, xx1);
        auto a25 = greaterThan(xx, xx1);
        auto a26 = greaterThanEqual(xx, xx1);
        std::cout << "size:" << (xx1==xx) << std::endl;
        std::cout << "size:" << (xx1!=xx) << std::endl;
        std::cout << "size:" << a11.x << " " << a11.y << std::endl;
        std::cout << "size:" << a12.x << " " << a12.y << std::endl;
        std::cout << "size:" << a13.x << " " << a13.y << std::endl;
        std::cout << "size:" << a14.x << " " << a14.y << std::endl;
        std::cout << "size:" << a15.x << " " << a15.y << std::endl;
        std::cout << "size:" << a16.x << " " << a16.y << std::endl;
    }
}


int main() {
    workStealingDequeueSingleThreaded();
    WorkStealingDequeue_PopSteal();
    WorkStealingDequeue_PushPopSteal();
    JobSystemParallelChildren();
    JobSystemSequentialChildren();
    JobSystemParallelFor();
    
    std::cout << "end" <<std::endl;
    return 0;
}
