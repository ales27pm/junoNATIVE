#include "RCUParameterManager.hpp"

void RCUParameterManager::set(const std::string &id, float value) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = params_.find(id);
    if (it == params_.end()) {
        params_.emplace(id, std::atomic<float>{value});
    } else {
        it->second.store(value, std::memory_order_relaxed);
    }
}

float RCUParameterManager::get(const std::string &id, float def) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = params_.find(id);
    return (it != params_.end())
           ? it->second.load(std::memory_order_relaxed)
           : def;
}
