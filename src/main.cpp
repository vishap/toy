
#include "scheduler.h"
#include <iostream>
#include <sstream>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

int main(int argc, char ** argv)
try{

    // boost::asio::io_context io_context;
    // boost::asio::io_context::work work(io_context);
    boost::asio::thread_pool pool(1);
    // for (std::size_t i = 0; i < 4; ++i){
    //     threads.create_thread([&io_context](){
    //         io_context.run();
    //     });
    // }

    te::scheduler scheduler(pool);
    // scheduler.test();

    for ( int i = 0; i < 10; i++){
        scheduler.enqueue_at(
            std::chrono::steady_clock::now()+std::chrono::seconds(i),
            [&, i]{
                std::stringstream ss;
                auto t = std::chrono::system_clock::to_time_t(
                            std::chrono::system_clock::now());
                ss << "task "  << i << " -  time " << t << std::endl;
                puts(ss.str().c_str());
            });
    }
    scheduler.start();
    std::this_thread::sleep_for(std::chrono::seconds(12));
    scheduler.stop();
    // io_service.stop();
    // threads.join_all();

    pool.stop();
    pool.join();
} catch(std::exception e){
    std::cerr << "std::exception " << e.what() << std::endl;
} catch(...){
    std::cerr << "Unknown exception caught." << std::endl;
}