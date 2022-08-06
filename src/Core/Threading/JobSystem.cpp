#include "JobSystem.h"

namespace Kraid
{

    JobRunnerThread::JobRunnerThread()
    {
        this->job_running_thread = Thread([this](void* data) -> DWORD {
                //runner code
                while(this->is_running)
                {
                    while(!this->job_queue.empty())
                    {
                        this->queue_lock.Lock();
                        Job job = this->job_queue.front();
                        this->job_queue.pop();
                        this->queue_lock.Unlock();

                        job.job(job.job_data);
                        if(job.callback)
                        {
                            job.callback();
                        }
                        if(job.deleter)
                        {
                            job.deleter(job.job_data);
                        }
                    }
                }
                return 0;
            });
    }

    JobRunnerThread::~JobRunnerThread()
    {
        this->is_running = false;
    }

    void JobRunnerThread::AddJob(Job job)
    {
        this->queue_lock.Lock();
        this->job_queue.push(job);
        this->queue_lock.Unlock();
    }

}
