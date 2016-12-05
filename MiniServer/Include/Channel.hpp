#pragma once
#include <mutex>
#include <list>
#include <stack>
#include <queue>
#include <condition_variable>

// Container must implement front(), as well as push(), pop() and empty()
// Or specify as queue or stack, which have specialised variants
template<typename DataType, typename Container>
class Channel
{
public:
    Channel()
        : bufferEmpty(true)
        , shouldExit(false)
    { }

    bool Empty() { return bufferEmpty; }

    void Exit()
    {
        shouldExit = true;
        bufferCV.notify_one();
    }

    void Write(DataType data)
    {
        std::unique_lock<std::mutex> lock(bufferMutex);
        buffer.push(data);
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
        buffer.pop();
        if (buffer.empty()) bufferEmpty = true;
        return data;
    }

private:
    Container buffer;
    std::mutex bufferMutex;
    std::condition_variable bufferCV;
    bool bufferEmpty;
    bool shouldExit;
};

template<typename DataType>
class Channel<DataType, std::stack<DataType>>
{
public:
    Channel()
        : bufferEmpty(true)
        , shouldExit(false)
    { }

    bool Empty() { return bufferEmpty; }

    void Exit()
    {
        shouldExit = true;
        bufferCV.notify_one();
    }

    void Write(DataType data)
    {
        std::unique_lock<std::mutex> lock(bufferMutex);
        buffer.push(data);
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
        DataType data = buffer.top();        
        buffer.pop();
        if (buffer.empty()) bufferEmpty = true;
        return data;
    }
private:
    std::stack<DataType> buffer;
    std::mutex bufferMutex;
    std::condition_variable bufferCV;
    bool bufferEmpty;
    bool shouldExit;

};

template<typename DataType>
class Channel<DataType, std::queue<DataType>>
{
public:
    Channel()
        : bufferEmpty(true)
        , shouldExit(false)
    { }

    bool Empty() { return bufferEmpty; }

    void Exit()
    {
        shouldExit = true;
        bufferCV.notify_one();
    }

    void Write(DataType data)
    {
        std::unique_lock<std::mutex> lock(bufferMutex);
        buffer.push(data);
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
        buffer.pop();
        if (buffer.empty()) bufferEmpty = true;
        return data;
    }
private:
    std::queue<DataType> buffer;
    std::mutex bufferMutex;
    std::condition_variable bufferCV;
    bool bufferEmpty;
    bool shouldExit;
};
