#pragma once

#include "Worker.hpp"

namespace dispatch {

template<typename Derived>
class TWorker : public Worker {
public:
    using Worker::Task;

    ~TWorker() override = default;

    void Push(Task task) final {
        auto* self = static_cast<Derived*>(this);
        self->PushTask(std::move(task));
    }

    void Run() final {
        auto* self = static_cast<Derived*>(this);
        self->RunLoop();
    }

    void Stop() final {
        auto* self = static_cast<Derived*>(this);
        self->StopTask();
    }

protected:
    TWorker() = default;
    TWorker(const TWorker&) = delete;
    TWorker& operator=(const TWorker&) = delete;
};

} // namespace dispatch