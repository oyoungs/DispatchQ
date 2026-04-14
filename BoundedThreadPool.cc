#include "DispatchQueue/BoundedThreadPool.hpp"

namespace dispatch {

BoundedThreadPool::BoundedThreadPool(size_t threadCount, size_t maxQueueSize)
    : threadCount_(threadCount), maxQueueSize_(maxQueueSize) {
    running_ = true;

    for (size_t i = 0; i < threadCount_; ++i) {
        threads_.emplace_back([this]() {
            while (running_) {
                Task task;

                {
                    std::unique_lock<std::mutex> lock(mutex_);
                    cv_.wait(lock, [this]() {
                        return !queue_.empty() || !running_;
                    });

                    if (!running_) {
                        return;
                    }

                    if (!queue_.empty()) {
                        task = std::move(queue_.front());
                        queue_.pop();
                    }
                }

                if (task) {
                    task();
                }
            }
        });
    }
}

BoundedThreadPool::~BoundedThreadPool() {
    StopTask();
}

void BoundedThreadPool::PushTask(Task task) {
    {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this]() {
            return queue_.size() < maxQueueSize_ || !running_;
        });

        if (!running_) {
            return;
        }

        queue_.push(std::move(task));
    }

    cv_.notify_one();
}

void BoundedThreadPool::RunLoop() {
}

void BoundedThreadPool::StopTask() {
    running_ = false;
    cv_.notify_all();

    for (auto& t : threads_) {
        if (t.joinable()) {
            t.join();
        }
    }
    threads_.clear();
}

} // namespace dispatch