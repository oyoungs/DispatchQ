#pragma once

#include "TWorker.hpp"
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <thread>

namespace dispatch {

class SerialWorker final : public TWorker<SerialWorker> {
public:
    SerialWorker();
    ~SerialWorker() override;

    void PushTask(Task task);
    void RunLoop();
    void StopTask();

private:
    std::queue<Task> queue_;
    std::mutex mutex_;
    std::condition_variable cv_;
    std::atomic<bool> running_{false};
    std::thread thread_;
};

} // namespace dispatch