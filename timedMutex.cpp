#include "timedMutex.h"

void TimedMutex::lock() 
{
    auto start = std::chrono::high_resolution_clock::now();
    internalMutex.lock();
    auto end = std::chrono::high_resolution_clock::now();

    auto waitTime = end - start;

    std::lock_guard<std::mutex> guard(statsMutex);
    threadWaitMap[std::this_thread::get_id()] += waitTime;
}

std::chrono::duration<double> TimedMutex::getThreadWaitTime(std::thread::id id) 
{
    std::lock_guard<std::mutex> guard(statsMutex);
    return threadWaitMap[id];
}

std::chrono::duration<double> TimedMutex::getTotalWaitTime() 
{
    std::lock_guard<std::mutex> guard(statsMutex);
    std::chrono::duration<double> sum{ 0 };
    for (const auto& [_, dur] : threadWaitMap)
        sum += dur;
    return sum;
}