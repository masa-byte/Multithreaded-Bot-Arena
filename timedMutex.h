#pragma once
#include <mutex>
#include <chrono>
#include <unordered_map>

class TimedMutex {
private:
    std::mutex internalMutex;
    std::chrono::duration<double> totalWaitTime{ 0 };

public:
    // For per-thread tracking
    std::unordered_map<std::thread::id, std::chrono::duration<double>> threadWaitMap;
    std::mutex statsMutex; // protects threadWaitMap

    void lock();

    void unlock() 
    {
        internalMutex.unlock();
    }

    std::chrono::duration<double> getThreadWaitTime(std::thread::id id);

    std::chrono::duration<double> getTotalWaitTime();
};

class TimedLockGuard {
private:
    TimedMutex& tm;
public:
    TimedLockGuard(TimedMutex& tm) : tm(tm) { tm.lock(); }
    ~TimedLockGuard() { tm.unlock(); }
};