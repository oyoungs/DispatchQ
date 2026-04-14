#pragma once

#include "Worker.hpp"
#include "DispatchQueueWorkerCreator.hpp"
#include <memory>

namespace dispatch {

class DispatchQueue {
public:
    explicit DispatchQueue(DispatchStrategy strategy);
    ~DispatchQueue();

    void Push(Worker::Task task);
    void Stop();

    DispatchQueue(const DispatchQueue&) = delete;
    DispatchQueue& operator=(const DispatchQueue&) = delete;

private:
    std::unique_ptr<Worker> worker_;
};

} // namespace dispatch