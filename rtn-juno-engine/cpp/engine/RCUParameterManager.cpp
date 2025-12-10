#include "RCUParameterManager.hpp"

void RCUParameterManager::set(const std::string &id, float value) {
    std::lock_guard<std::mutex> lock(mutex_);
    params_[id] = value;
}

float RCUParameterManager::get(const std::string &id, float def) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = params_.find(id);
    return (it != params_.end())
           ? it->second
           : def;
}
