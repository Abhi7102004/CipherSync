#include "../include/ThreadPool.h"
#include <iostream>

ThreadPool::ThreadPool(size_t numThreads) : stop(false), activeTasks(0) {
    for (size_t i = 0; i < numThreads; i++) {
        workers.emplace_back(&ThreadPool::workerThread, this);
    }
}

ThreadPool::~ThreadPool() {
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        stop = true;
    }
    condition.notify_all();

}

void ThreadPool::enqueueTask(std::function<void()> task) {
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        tasks.push(std::move(task));
        activeTasks++; // Increment active task counter
    }
    condition.notify_one();
}

void ThreadPool::workerThread() {
    while (true) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            condition.wait(lock, [this] {
                return stop || !tasks.empty();
            });

            if (stop && tasks.empty()) {
                std::cout << "Worker " << std::this_thread::get_id() << " shutting down\n";
                return;
            }

            task = std::move(tasks.front());
            tasks.pop();
        }

        std::cout << "Worker " << std::this_thread::get_id() << " is executing a task\n";

        task();

        // Decrement the active task count after completion
        activeTasks--;
        condition.notify_all();
    }
}

void ThreadPool::waitForAll() {
    std::unique_lock<std::mutex> lock(queueMutex);
    // Wait until the task queue is empty AND all active tasks are done
    condition.wait(lock, [this] {
        return tasks.empty() && activeTasks == 0;
    });
}
