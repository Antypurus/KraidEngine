#include "JobSystem.h"

namespace Kraid
{

    JobRunnerThread::JobRunnerThread()
    {
        this->job_queue_lock = {};
        this->job_running_thread = Thread([this](void* data) -> DWORD {
                //runner code
                while(this->is_running)
                {
                    this->job_queue_lock.associated_mutex.Lock();
                    if(this->job_queue.empty())
                    {
                        this->job_queue_lock.Sleep();
                    }
                    this->job_queue_lock.associated_mutex.Unlock();

                    while(!this->job_queue.empty())
                    {
                        Job job;
                        this->job_queue_lock.associated_mutex.Lock();
                        
                        job = this->job_queue.front();
                        this->job_queue.pop();
                        this->job_queue_lock.associated_mutex.Unlock();

                        if(job.initialized)
                        {
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
        this->job_queue_lock.associated_mutex.Lock();
        this->job_queue.push(job);
        this->job_queue_lock.associated_mutex.Unlock();
        this->job_queue_lock.Wake();
    }

}
