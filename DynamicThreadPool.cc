#include "DispatchQueue/DynamicThreadPool.hpp"

namespace dispatch {

thread_local bool DynamicThreadPool::recyclable_ = false;
thread_local std::thread* DynamicThreadPool::currentThread_ = nullptr;

DynamicThreadPool::DynamicThreadPool(size_t minThreads, size_t maxThreads, std::chrono::seconds idleTimeout)
    : minThreads_(minThreads), maxThreads_(maxThreads), idleTimeout_(idleTimeout) {
    running_ = true;

    for (size_t i = 0; i < minThreads_; ++i) {
        CreateThread(false);
    }
}

DynamicThreadPool::~DynamicThreadPool() {
    StopTask();
}

void DynamicThreadPool::CreateThread(bool recyclable) {
    std::lock_guard<std::mutex> lock(mutex_);
    threads_.emplace_back([this, recyclable]() {
        recyclable_ = recyclable;
        currentThread_ = &threads_.back();
        RunLoop();
    });
    totalThreads_++;
}

void DynamicThreadPool::CollectDeadThreads() {
    for (size_t i = minThreads_; i < threads_.size(); ) {
        if (!threads_[i].joinable()) {
            threads_[i] = std::move(threads_.back());
            threads_.pop_back();
        } else {
            ++i;
        }
    }
}

void DynamicThreadPool::RunLoop() {
    while (running_) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            idleCount_++;
        }

        Task task;
        {
            std::unique_lock<std::mutex> lock(mutex_);
            auto timeoutMs = std::chrono::duration_cast<std::chrono::milliseconds>(idleTimeout_);

            bool hasTask = cv_.wait_for(lock, timeoutMs, [this]() {
                return !queue_.empty() || !running_;
            });

            idleCount_--;

            if (!running_) {
                return;
            }

            if (hasTask && !queue_.empty()) {
                task = std::move(queue_.front());
                queue_.pop();
            } else if (recyclable_) {
                totalThreads_--;
                currentThread_->detach();
                return;
            }
        }

        if (task) {
            task();
        }
    }
}

void DynamicThreadPool::PushTask(Task task) {
    bool needCreateThread = false;

    {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(std::move(task));

        if (idleCount_ == 0 && totalThreads_ < maxThreads_) {
            needCreateThread = true;
        }
    }

    CollectDeadThreads();

    cv_.notify_one();

    if (needCreateThread) {
        CreateThread(true);
    }
}

void DynamicThreadPool::StopTask() {
    running_ = false;
    cv_.notify_all();

    for (auto& t : threads_) {
        if (t.joinable()) {
            t.join();
        }
    }
    threads_.clear();
    totalThreads_ = 0;
    idleCount_ = 0;
}

void DynamicThreadPool::SetIdleTimeout(std::chrono::seconds timeout) {
    idleTimeout_ = timeout;
}

} // namespace dispatch