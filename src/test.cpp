#include "containers/threadsafe_queue.h"
#include "thread_pool.h"
#include <chrono>
#include <iostream>

int main()
{
    concurrency::containers::TSQueue<int> que;
    que.push(1);
    auto f = que.wait_and_pop();
    std::cout << *f << std::endl;
    concurrency::pool::ThreadPool tp;

    auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < 10; ++i)
    {
        tp.submit([i](int a) {
            std::cout << i << ":" << a << " ----" << std::this_thread::get_id() << std::endl;
        },
                  5);
    }
    std::chrono::duration<double> time = std::chrono::steady_clock::now() - start;
    std::cout << "time:" << time.count() << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));

    return 0;
}