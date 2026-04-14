#pragma once

#include "Worker.hpp"
#include <memory>
#include <functional>
#include <unordered_map>
#include <string>

namespace dispatch {

enum class DispatchStrategy {
    Serial,
    Concurrent,
    Stealing,
    Bounded
};

class DispatchQueueWorkerCreator {
public:
    using WorkerCreator = std::function<std::unique_ptr<Worker>()>;

    static DispatchQueueWorkerCreator& Instance() {
        static DispatchQueueWorkerCreator instance;
        return instance;
    }

    void Register(const std::string& name, WorkerCreator creator) {
        creators_[name] = std::move(creator);
    }

    void Register(DispatchStrategy strategy, WorkerCreator creator) {
        auto name = StrategyToString(strategy);
        creators_[name] = std::move(creator);
    }

    std::unique_ptr<Worker> Create(const std::string& name) {
        auto it = creators_.find(name);
        if (it != creators_.end()) {
            return it->second();
        }
        return nullptr;
    }

    std::unique_ptr<Worker> Create(DispatchStrategy strategy) {
        return Create(StrategyToString(strategy));
    }

    bool HasStrategy(const std::string& name) const {
        return creators_.find(name) != creators_.end();
    }

    bool HasStrategy(DispatchStrategy strategy) const {
        return HasStrategy(StrategyToString(strategy));
    }

private:
    DispatchQueueWorkerCreator();

    static std::string StrategyToString(DispatchStrategy strategy) {
        switch (strategy) {
            case DispatchStrategy::Serial: return "serial";
            case DispatchStrategy::Concurrent: return "concurrent";
            case DispatchStrategy::Stealing: return "stealing";
            case DispatchStrategy::Bounded: return "bounded";
        }
        return "unknown";
    }

    std::unordered_map<std::string, WorkerCreator> creators_;
};

} // namespace dispatch