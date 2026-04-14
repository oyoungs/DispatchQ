#pragma once

#include "TWorker.hpp"
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <thread>
#include <chrono>

namespace dispatch {

class BoundedThreadPool final : public TWorker<BoundedThreadPool> {
public:
    BoundedThreadPool(size_t threadCount, size_t maxQueueSize);
    ~BoundedThreadPool() override;

    void PushTask(Task task);
    void RunLoop();
    void StopTask();

private:
    size_t threadCount_;
    size_t maxQueueSize_;
    std::queue<Task> queue_;
    std::mutex mutex_;
    std::condition_variable cv_;
    std::atomic<bool> running_{false};
    std::vector<std::thread> threads_;
};

} // namespace dispatch