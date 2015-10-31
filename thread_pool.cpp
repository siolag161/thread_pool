#include "thread_pool.hpp"

ThreadPool::ThreadPool(unsigned max_workers, unsigned max_tasks)
        : max_workers(max_workers), max_tasks(max_tasks), stop(false) {
    for (unsigned w = 0; w < max_workers; ++w) {
        workers.push_back(std::make_shared<ThreadWorker>(*this));
    }
}

ThreadPool::~ThreadPool() {
    if (!done)
        this->shutdown();
}


void ThreadPool::shutdown() {
    {
        std::unique_lock<std::mutex> lock(q_mutex);
        stop = true;
    }
    has_task_q.notify_all();
    for (auto &w : workers) {
        w->join();
    }
    tasks.empty();
    done = true;
}


ThreadWorker::ThreadWorker(ThreadPool& p): pool(p) {
    thread = std::thread(&ThreadWorker::run, this);
}

void ThreadWorker::join() {
    return thread.join();
}

void ThreadWorker::run() {
    Task t;
    while (true) {
        {
            std::unique_lock<std::mutex> lock(pool.q_mutex);
            while (!pool.stop && pool.tasks.empty()) {
                pool.has_task_q.wait(lock);
            }
            if (pool.stop && pool.tasks.empty()) {
                return;
            }

            t = pool.tasks.front();
            pool.tasks.pop();
        }
        t();
    }
}