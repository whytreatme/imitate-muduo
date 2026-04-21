#include "ThreadPool.h"
#include <string>
#include <unistd.h>
#include <sys/syscall.h>
#include <iostream>

/*
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
*/  

thpool::thpool(int nums_threads) : stop_(false)
{
    for(int i = 0; i < nums_threads; i++){
      
        //使用lambda函数创建线程
        threads.emplace_back([this]{
            printf("create thread(%ld).\n", syscall(SYS_gettid));      //显示线程id
            std::cout << "子线程：" << std::this_thread::get_id() << std::endl;
            //1.建立thread_do类似的线程执行循环 
            while(!stop_){
                std::function<void()> task;
                {  //unique_lock在离开作用域自动调用析构函数，所以要有作用域限制范围
                std::unique_lock<std::mutex> lock(mutex_);
                tCondition.wait(lock, [this]{
                    return (stop_ == true) || (taskQueue.empty() == false);
                });    
                //2.判断该退出循环的情况  
                if((stop_ == true) && (taskQueue.empty() == true)) return;

                task = std::move(taskQueue.front());
                taskQueue.pop();
                } //对taskQueue的操作完成，离开锁的作用域
                printf("thread is %ld.\n",syscall(SYS_gettid));
                task();
            }
        });
    }    
}  

void thpool::addTask(std::function<void()> task)
{ //在Pithikos的实现里，要把函数对象指针和参数指针一起传给任务，但是C++
  //有包装器，可以连参数也一起绑定了
    {
        std::unique_lock<std::mutex> lock(mutex_);
        taskQueue.push(std::move(task));     //涉及任务队列操作一定要有同步机制防止竞态
    } 
    tCondition.notify_one();     //由条件变量通知阻塞的线程
}  

thpool::~thpool()
{
    stop_ = true;
    tCondition.notify_all();
    for(std::thread &th : threads)
        th.join();
}  

void test(){
    printf("我很喜欢玩空洞骑士！\n");
}

void show(int num, std::string& name){
    printf("我想要%d个护符，%s!!!\n", num, name.c_str());
}

int main(){
    thpool threadsPool(3);
    std::string name = "快速劈砍";
    threadsPool.addTask(std::bind(show, 3, std::ref(name)));
    sleep(1);
    threadsPool.addTask(std::bind(test));
    sleep(1);
    threadsPool.addTask(std::bind([]{
        printf("我要干掉辐光!\n");
    }));
    sleep(100);

    return 0;
}