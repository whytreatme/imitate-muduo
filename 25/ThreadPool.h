#include <vector>
#include <thread>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <queue>

class thpool{
private:
    std::vector<std::thread> threads;                 //建立线程池
    std::queue<std::function<void()>> taskQueue;         //建立任务队列  
    std::mutex mutex_;                                   //设置锁保护任务队列 
    std::condition_variable  tCondition;                  //保证同步的条件变量  
    volatile bool stop_;                                  //结束标志位

public:  
    thpool(int nums_threads);
    ~thpool();
    void addTask(std::function<void()> task);
};