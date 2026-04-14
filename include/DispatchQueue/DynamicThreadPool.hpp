#pragma once

#include "TWorker.hpp"
#include <queue>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <thread>
#include <chrono>

namespace dispatch {

class DynamicThreadPool final : public TWorker<DynamicThreadPool> {
public:
    DynamicThreadPool(size_t minThreads, size_t maxThreads, 
                      std::chrono::seconds idleTimeout = std::chrono::seconds(10));
    ~DynamicThreadPool() override;

    void PushTask(Task task);
    void RunLoop();
    void StopTask();

    void SetIdleTimeout(std::chrono::seconds timeout);

private:
    void CreateThread(bool recyclable);
    void CollectDeadThreads();

    static thread_local bool recyclable_;
    static thread_local std::thread* currentThread_;

    size_t minThreads_;
    size_t maxThreads_;
    std::chrono::seconds idleTimeout_;

    std::queue<Task> queue_;
    std::mutex mutex_;
    std::condition_variable cv_;

    std::atomic<bool> running_{false};
    std::atomic<size_t> totalThreads_{0};
    std::atomic<size_t> idleCount_{0};

    std::vector<std::thread> threads_;
};

} // namespace dispatch