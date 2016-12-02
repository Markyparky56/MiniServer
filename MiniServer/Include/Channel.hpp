#pragma once
#include <mutex>
#include <list>
#include <condition_variable>

template<typename DataType>
class Channel
{
public:
    Channel()
        : bufferEmpty(true)
        , shouldExit(false)
    { }

    void Exit()
    {
        shouldExit = true;
        bufferCV.notify_one();
    }

    void Write(DataType data)
    {
        std::unique_lock<std::mutex> lock(bufferMutex);
        buffer.push_back(data);
        bufferEmpty = false;
        bufferCV.notify_one();
    }

    DataType Read()
    {
        std::unique_lock<std::mutex> lock(bufferMutex);
        while (bufferEmpty && !shouldExit)
        {
            bufferCV.wait(lock);
        }
        if (shouldExit)
        {
            return DataType();
        }
        DataType data = buffer.front();
        buffer.pop_front();
        if (buffer.empty()) bufferEmpty = true;
        return data;
    }

private:
    std::list<DataType> buffer;
    std::mutex bufferMutex;
    std::condition_variable bufferCV;
    bool bufferEmpty;
    bool shouldExit;
};
