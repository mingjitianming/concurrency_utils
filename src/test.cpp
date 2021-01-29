#include "thread_pool.h"
#include "threadsafe_queue.h"
#include <iostream>

int main()
{
    concurrency::containers::TSQueue<int> que;
    que.push(1);
    auto f = que.wait_and_pop();
    std::cout << *f << std::endl;
    concurrency::pool::ThreadPool tp;
    tp.submit([]() { std::cout << "hh" << std::endl; });
    return 0;
}