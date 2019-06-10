#include <gtest/gtest.h>
#include <scheduler.h>

TEST(Scheduler, scheduler_bring_up_and_down)
{
    boost::asio::thread_pool pool(1);
    te::scheduler scheduler(pool);
    scheduler.start();
    scheduler.stop();
}

TEST(Scheduler, schedule_task_and_check_it_runs_immediately)
{
    std::condition_variable run_cv;
    std::mutex mutex;
    bool run = false;

    boost::asio::thread_pool pool(1);
    te::scheduler scheduler(pool);
    scheduler.start();
    scheduler.enqueue_at(
        std::chrono::steady_clock::now(),
        [&]{
            std::unique_lock<std::mutex> lock(mutex);
            run = true;
            run_cv.notify_all();
        });

    std::unique_lock<std::mutex> lock(mutex);
    while ( !run ){
        run_cv.wait(lock);
    }

    scheduler.stop();
}

TEST(Scheduler, schedule_task_and_check_it_runs_after_awhile)
{
    std::condition_variable run_cv;
    std::mutex mutex;
    bool run = false;
    std::chrono::seconds awhile(1);

    boost::asio::thread_pool pool(1);
    te::scheduler scheduler(pool);
    scheduler.start();
    auto begin = std::chrono::steady_clock::now();
    scheduler.enqueue_at(
        begin + awhile,
        [&]{
            std::unique_lock<std::mutex> lock(mutex);
            run = true;
            run_cv.notify_all();
        });

    std::unique_lock<std::mutex> lock(mutex);
    while ( !run ){
        run_cv.wait(lock);
    }

    auto end = std::chrono::steady_clock::now();

    scheduler.stop();
    
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end-begin-awhile);
    EXPECT_NEAR(ms.count(), 0, 100);
    EXPECT_TRUE(run);
}

TEST(Scheduler, schedule_task_but_stop_will_not_run_task)
{
    bool run = false;
    std::chrono::seconds awhile(1);
    boost::asio::thread_pool pool(1);

    auto begin = std::chrono::steady_clock::now();
    {
        te::scheduler scheduler(pool);
        scheduler.start();
        scheduler.enqueue_at(
            begin + awhile,
            [&]{
                run = true;
            });
        scheduler.stop();
    }

    auto end = std::chrono::steady_clock::now();

    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end-begin);
    EXPECT_NEAR(ms.count(), 0, 100);
    EXPECT_FALSE(run);
}

TEST(Scheduler, schedule_visual_test)
{
    std::condition_variable run_cv;
    std::mutex mutex;
    int scheduled_task = 5;
    std::atomic_int counter(0);

    boost::asio::thread_pool pool(1);
    te::scheduler scheduler(pool);
    scheduler.start();
    auto begin = std::chrono::steady_clock::now();

    for ( int i = 0; i < scheduled_task; i++){
        scheduler.enqueue_at(
            begin + std::chrono::seconds(i),
            [&, i]{
                std::cout << "task "  << i << std::endl;
                counter++;
                std::unique_lock<std::mutex> lock(mutex);
                run_cv.notify_all();
            });
    }

    std::unique_lock<std::mutex> lock(mutex);
    while ( counter.load() < scheduled_task ){
        run_cv.wait(lock);
    }

    auto end = std::chrono::steady_clock::now();

    scheduler.stop();
    
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end-begin);
    EXPECT_NEAR(ms.count(), 4000, 100);
}