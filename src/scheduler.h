#pragma once

#include <boost/asio.hpp>
#include <memory>
#include <functional>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <atomic>
#include <queue>

namespace te {

class scheduler
{
public:
    using task_type = std::function<void ()>;
    using clock_type = std::chrono::steady_clock;
    using time_point_type = clock_type::time_point;
    using mutex_guard = std::unique_lock<std::mutex>;

private:
    struct task_descr{
        time_point_type  at;
        task_type   task;   // Task is assumed to be a small structure.
                            // Otherwise we probably need to allocate and
                            // store shared_ptr here. But then we might need
                            // to allocate it through a special allocator
                            // to avoid memory fragmentation.

        bool operator > (const task_descr& other) const {
            return at > other.at;
        }
    };

public:
    scheduler(const scheduler&) = delete;

    scheduler(boost::asio::thread_pool& tpool)
        : thead_pool(tpool)
        , stop_flag(false)
    {}

    ~scheduler(){
        run_loop_thread.join();
    }

    void enqueue_at(time_point_type at, task_type task){
        mutex_guard lock(tasks_mutex);
        tasks.push({
            at, 
            std::move(task)
        });
        new_tasks_available.notify_all();
    }

    void start(){
        run_loop_thread = std::thread([&]{
            this->run_loop();
        });
    };

    void stop(){
        stop_flag.store(true);
        new_tasks_available.notify_all();
    };

private:
    void run_loop(){
        mutex_guard lock(tasks_mutex);
        while (!stop_flag){
            time_point_type at = schedule_work();
            new_tasks_available.wait_until(lock, at);
        }
    }

    time_point_type schedule_work(){
        auto now = clock_type::now();
        while (!tasks.empty() && tasks.top().at < now){
            boost::asio::post(thead_pool,
                [task = std::move(tasks.top().task)]{
                    task();
                });
            tasks.pop();
        }
        return (!tasks.empty()) 
                ? tasks.top().at
                :now + std::chrono::seconds(60);
    }

private:

    boost::asio::thread_pool& thead_pool;
    std::mutex tasks_mutex;
    std::condition_variable new_tasks_available;
    std::priority_queue<task_descr,
                        std::vector<task_descr>,
                        std::greater<task_descr>> tasks;
    std::thread run_loop_thread;
    std::atomic_bool stop_flag;
};

}