#include "DispatchQueue/SerialWorker.hpp"

namespace dispatch {

SerialWorker::SerialWorker() {
    running_ = true;
    thread_ = std::thread([this]() {
        RunLoop();
    });
}

SerialWorker::~SerialWorker() {
    StopTask();
    if (thread_.joinable()) {
        thread_.join();
    }
}

void SerialWorker::PushTask(Task task) {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(std::move(task));
    }
    cv_.notify_one();
}

void SerialWorker::RunLoop() {
    while (running_) {
        Task task;
        {
            std::unique_lock<std::mutex> lock(mutex_);
            cv_.wait(lock, [this]() { return !queue_.empty() || !running_; });
            
            if (!running_ && queue_.empty()) {
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
}

void SerialWorker::StopTask() {
    running_ = false;
    cv_.notify_all();
}

} // namespace dispatch