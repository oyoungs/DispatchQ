#pragma once

#include "TWorker.hpp"
#include <deque>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <thread>
#include <chrono>

namespace dispatch {

class StealingThreadPool final : public TWorker<StealingThreadPool> {
public:
    explicit StealingThreadPool(size_t threadCount = 4);
    ~StealingThreadPool() override;

    void PushTask(Task task);
    void RunLoop();
    void StopTask();

private:
    struct LocalQueue {
        std::deque<Task> queue;
        std::mutex mutex;
    };

    void CreateWorkerThreads();
    bool TrySteal(Task& task);

    static thread_local size_t localQueueIdx_;

    size_t threadCount_;
    std::vector<LocalQueue> queues_;
    std::vector<std::thread> threads_;
    std::atomic<bool> running_{false};
    std::atomic<size_t> roundRobin_{0};
};

} // namespace dispatch