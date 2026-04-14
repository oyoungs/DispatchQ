#include "DispatchQueue/DispatchQueue.hpp"

namespace dispatch {

DispatchQueue::DispatchQueue(DispatchStrategy strategy) {
    worker_ = DispatchQueueWorkerCreator::Instance().Create(strategy);
}

DispatchQueue::~DispatchQueue() {
    if (worker_) {
        worker_->Stop();
    }
}

void DispatchQueue::Push(Worker::Task task) {
    if (worker_) {
        worker_->Push(std::move(task));
    }
}

void DispatchQueue::Stop() {
    if (worker_) {
        worker_->Stop();
    }
}

} // namespace dispatch