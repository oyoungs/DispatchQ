#pragma once

#include "Worker.hpp"

namespace dispatch {

/**
 * @brief 模板Worker类 - CRTP + Template Method模式的实现
 * @details
 * TWorker<Derived> 使用 CRTP (Curiously Recurring Template Pattern) 技术，
 * 将自身转换为 Derived 类型后调用子类的实现方法。
 * 
 * 设计模式: Template Method + CRTP
 * - 以 final 形式实现 Worker 的三个虚函数 (Push/Run/Stop)
 * - 内部调用子类的 PushTask/RunLoop/StopTask
 * - 子类只需实现具体的任务处理逻辑，无需关注虚函数调度
 * 
 * @tparam Derived 具体Worker实现类
 */
template<typename Derived>
class TWorker : public Worker {
public:
    using Worker::Task;

    ~TWorker() override = default;

    /**
     * @brief Push的final实现 - 将任务转发给子类的PushTask
     * @param task 要执行的任务
     * @note final修饰，子类不可重写
     */
    void Push(Task task) final {
        auto* self = static_cast<Derived*>(this);
        self->PushTask(std::move(task));
    }

    /**
     * @brief Run的final实现 - 启动子类的RunLoop
     * @note final修饰，子类不可重写
     */
    void Run() final {
        auto* self = static_cast<Derived*>(this);
        self->RunLoop();
    }

    /**
     * @brief Stop的final实现 - 调用子类的StopTask
     * @note final修饰，子类不可重写
     */
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