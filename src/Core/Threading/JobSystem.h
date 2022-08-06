#pragma once

#include <Core/Threading/Thread.h>
#include <Core/Threading/Lock.h>
#include <queue>//TODO(Tiago): create our own queue

namespace Kraid
{

    struct Job
    {
        std::function<void(void*)> job;
        uint8* job_data = nullptr;
        std::function<void(void*)> deleter;
        std::function<void(void)> callback;
    };

    class JobRunnerThread
    {
    private:
        Thread job_running_thread;
        std::queue<Job> job_queue;
        Mutex queue_lock;
        bool is_running = true;
    public:
        JobRunnerThread();
        ~JobRunnerThread();
        void AddJob(Job job);
    };

}
