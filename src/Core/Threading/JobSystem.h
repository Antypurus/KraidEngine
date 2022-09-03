#pragma once

#include <Core/Threading/Thread.h>
#include <Core/Threading/Lock.h>
#include <queue>//TODO(Tiago): create our own queue

namespace Kraid
{

    struct Job
    {
    public:
        std::function<void(void*)> job;
        std::function<void(void*)> deleter;
        std::function<void(void)> callback;
        uint8* job_data = nullptr;
    };

    class JobRunner
    {
    private:
        Thread job_running_thread;
        std::queue<Job> job_queue;
        ConditionVariable job_queue_lock;
        volatile bool is_running = true;
    public:
        JobRunner();
        ~JobRunner();
        void AddJob(Job job);
        //NOTE(Tiago): Condition variables cannot be copies or moved, therefore neither can a job runner.
        JobRunner(JobRunner&& other) = delete;
        JobRunner(const JobRunner& other) = delete;
        JobRunner& operator=(JobRunner&& other) = delete;
        JobRunner& operator=(const JobRunner& other) = delete;
    };

    class JobPoolManager
    {
    private:
        std::vector<JobRunner*> runner_pool;
        std::atomic_uchar current_submission_runner = 0;
    public:
        JobPoolManager() = default;
        JobPoolManager(uint8 runner_count);
        ~JobPoolManager();
        void AddJob(Job job);
    };

    enum class JobPriority
    {
        Low,
        Medium,
        High
    };

    class JobSystem
    {
    private:
        JobPoolManager high_priority_pool;
        JobPoolManager medium_priority_pool;
        JobPoolManager low_priority_pool;
        static JobSystem m_instance;
    private:
        JobSystem();
        ~JobSystem() = default;
    public:
        static void AddJob(Job job, JobPriority priority = JobPriority::Low);
        static void AddJob(const std::function<void(void*)>& job,
                    JobPriority priority = JobPriority::Low,
                    uint8* data = nullptr,
                    const std::function<void(void*)>& deleter = {},
                    const std::function<void(void)>& callback = {});
    };

}
