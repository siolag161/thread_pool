
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

typedef std::function<void()> Task;

class ThreadWorker;

class ThreadPool {

public:
    ThreadPool(unsigned max_workers=5, unsigned max_tasks = 20);
    virtual ~ThreadPool();

    template<class F, class... Args>
    bool enqueue(F&& f, Args&&... args) {
        if (tasks.size() >= max_tasks) return false;
        auto task = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
        {
            std::unique_lock<std::mutex> lock(q_mutex);
            tasks.push(task);
        }
        has_task_q.notify_one();
        return true;
    };

    void shutdown();
    friend class ThreadWorker;

private:
    std::vector<std::shared_ptr<ThreadWorker>> workers;

    std::queue<Task> tasks;

    unsigned max_workers;
    unsigned max_tasks;
    std::mutex q_mutex;
    std::condition_variable has_task_q;
    bool stop;
    bool done;
};


class ThreadWorker {
public:
    ThreadWorker(ThreadPool& pool);
    virtual ~ThreadWorker() {}
    void join();

private:
    void run();

private:
    ThreadPool& pool;
    std::thread thread;

};

