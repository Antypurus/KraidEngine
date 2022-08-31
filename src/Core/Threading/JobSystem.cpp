#include "JobSystem.h"

namespace Kraid
{
    
    /** ================== Job Runner Implementation ======================= **/

    JobRunner::JobRunner()
    {
        this->job_running_thread = Thread([this](void* data) -> DWORD {
                //runner code
                while(this->is_running)
                {
                    this->job_queue_lock.Lock();
                    if(this->job_queue.empty())
                    {
                        this->job_queue_lock.Sleep();
                    }
                    this->job_queue_lock.Unlock();

                    while(!this->job_queue.empty())
                    {
                        this->job_queue_lock.Lock();
                        Job job = this->job_queue.front();
                        this->job_queue.pop();
                        this->job_queue_lock.Unlock();
                        
                        if(job.job)
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
        this->job_running_thread.SetName(L"Job Runner Thread");
    }
    
    JobRunner::~JobRunner()
    {
        this->is_running = false;
    }

    void JobRunner::AddJob(Job job)
    {
        this->job_queue_lock.Lock();
        this->job_queue.push(job);
        this->job_queue_lock.Unlock();
        this->job_queue_lock.Wake();
    }
    
    /** ================== Job Runner Implementation ======================= **/

    JobPoolManager::JobPoolManager(uint8 runner_count)
    {
        for(uint8 i = 0; i < runner_count; ++i)
        {
            this->runner_pool.emplace_back(new JobRunner());
        }
    }

    JobPoolManager::~JobPoolManager()
    {
        for(uint8 i = 0; i < this->runner_pool.size(); ++i)
        {
            delete this->runner_pool[i];
        }
    }

    void JobPoolManager::AddJob(Job job)
    {
        this->runner_pool[current_submission_runner]->AddJob(job);
        //TODO(Tiago):is this the correct way to access and upadte the atomic variable that controls the runner we are going to submit work to?
        current_submission_runner = (current_submission_runner + 1) % this->runner_pool.size();
    }

}
