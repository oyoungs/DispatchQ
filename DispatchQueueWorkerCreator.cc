#include "DispatchQueue/DispatchQueueWorkerCreator.hpp"
#include "DispatchQueue/SerialWorker.hpp"
#include "DispatchQueue/DynamicThreadPool.hpp"
#include "DispatchQueue/StealingThreadPool.hpp"
#include "DispatchQueue/BoundedThreadPool.hpp"

namespace dispatch {

DispatchQueueWorkerCreator::DispatchQueueWorkerCreator() {
    Register(DispatchStrategy::Serial, []() {
        return std::make_unique<SerialWorker>();
    });

    Register(DispatchStrategy::Concurrent, []() {
        return std::make_unique<DynamicThreadPool>(2, 8);
    });

    Register(DispatchStrategy::Stealing, []() {
        return std::make_unique<StealingThreadPool>(4);
    });

    Register(DispatchStrategy::Bounded, []() {
        return std::make_unique<BoundedThreadPool>(4, 100);
    });
}

} // namespace dispatch