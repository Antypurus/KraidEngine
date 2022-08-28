#pragma once

#include <Core/Threading/Thread.h>
#include <Core/Threading/Lock.h>
#include <queue>//TODO(Tiago): create our own queue

namespace Kraid
{

    struct Job
    {
    public:
        //std::function<void(void*)> job;
        void (*job)(void*) = nullptr;
        std::function<void(void*)> deleter;
        std::function<void(void)> callback;
        uint8* job_data = nullptr;
        volatile bool initialized = false;

    public:
        Job() = default;
        Job(const Job& other)
        {
            this->job = other.job;
            this->deleter = other.deleter;
            this->callback = other.callback;
            this->job_data = other.job_data;
            this->initialized = other.initialized;
        }
        Job& operator=(const Job& other)
        {
            this->job = other.job;
            this->deleter = other.deleter;
            this->callback = other.callback;
            this->job_data = other.job_data;
            this->initialized = other.initialized;
            return *this;
        }
    };

    class JobRunnerThread
    {
    private:
        Thread job_running_thread;
        std::queue<Job> job_queue;
        ConditionVariable job_queue_lock;
        volatile bool is_running = true;
    public:
        JobRunnerThread();
        ~JobRunnerThread();
        void AddJob(Job job);
    };

}
