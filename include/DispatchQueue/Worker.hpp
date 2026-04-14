#pragma once

#include <functional>
#include <queue>
#include <mutex>
#include <condition_variable>

namespace dispatch {

/**
 * @brief Abstract Worker class for task execution
 * 
 * Worker provides the interface for task scheduling and execution.
 * Subclasses must implement PushTask and RunLoop methods.
 */
class Worker {
public:
    using Task = std::function<void()>;

    virtual ~Worker() = default;

    /**
     * @brief Push a task to the work queue
     * @param task The task function to execute
     */
    virtual void Push(Task task) = 0;

    /**
     * @brief Run the worker's event loop
     * 
     * This method blocks until the worker is stopped.
     */
    virtual void Run() = 0;

    /**
     * @brief Stop the worker
     */
    virtual void Stop() = 0;

protected:
    Worker() = default;
    Worker(const Worker&) = delete;
    Worker& operator=(const Worker&) = delete;
};

} // namespace dispatch