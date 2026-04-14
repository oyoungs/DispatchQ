#include "DispatchQueue/StealingThreadPool.hpp"

namespace dispatch {

thread_local size_t StealingThreadPool::localQueueIdx_ = 0;

StealingThreadPool::StealingThreadPool(size_t threadCount)
    : threadCount_(threadCount), queues_(threadCount) {
    running_ = true;
    CreateWorkerThreads();
}

StealingThreadPool::~StealingThreadPool() {
    StopTask();
}

void StealingThreadPool::CreateWorkerThreads() {
    for (size_t i = 0; i < threadCount_; ++i) {
        threads_.emplace_back([this, i]() {
            localQueueIdx_ = i;
            while (running_) {
                Task task;

                {
                    std::lock_guard<std::mutex> lock(queues_[localQueueIdx_].mutex);
                    if (!queues_[localQueueIdx_].queue.empty()) {
                        task = std::move(queues_[localQueueIdx_].queue.front());
                        queues_[localQueueIdx_].queue.pop_front();
                    }
                }

                if (!task) {
                    TrySteal(task);
                }

                if (task) {
                    task();
                }
            }
        });
    }
}

bool StealingThreadPool::TrySteal(Task& task) {
    for (size_t i = 1; i < threadCount_; ++i) {
        size_t idx = (localQueueIdx_ + i) % threadCount_;
        std::lock_guard<std::mutex> lock(queues_[idx].mutex);
        if (!queues_[idx].queue.empty()) {
            task = std::move(queues_[idx].queue.front());
            queues_[idx].queue.pop_front();
            return true;
        }
    }
    return false;
}

void StealingThreadPool::PushTask(Task task) {
    size_t idx = roundRobin_.fetch_add(1) % threadCount_;
    {
        std::lock_guard<std::mutex> lock(queues_[idx].mutex);
        queues_[idx].queue.push_back(std::move(task));
    }
}

void StealingThreadPool::RunLoop() {
}

void StealingThreadPool::StopTask() {
    running_ = false;
    
    for (auto& t : threads_) {
        if (t.joinable()) {
            t.join();
        }
    }
    threads_.clear();
}

} // namespace dispatch