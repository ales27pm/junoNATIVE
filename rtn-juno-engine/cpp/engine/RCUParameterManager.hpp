#pragma once
#include <unordered_map>
#include <string>
#include <mutex>

class RCUParameterManager {
public:
    void set(const std::string &id, float value);
    float get(const std::string &id, float defaultValue = 0.0f) const;

private:
    mutable std::mutex mutex_;
    std::unordered_map<std::string, float> params_;
};
