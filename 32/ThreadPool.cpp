#include "ThreadPool.h"
#include <string>
#include <unistd.h>
#include <sys/syscall.h>
#include <iostream>


thpool::thpool(int nums_threads, std::string threadtype) : stop_(false), thread_type_(threadtype)
{
    for(int i = 0; i < nums_threads; i++){
      
        //使用lambda函数创建线程
        threads.emplace_back([this]{
            printf("create %s thread(%ld).\n", thread_type_.c_str(), syscall(SYS_gettid));      //显示线程id

            //1.建立thread_do类似的线程执行循环 
            while(!stop_){
                TaskItem task;
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
                printf("%s thread %ld run task: %s.\n", thread_type_.c_str(), 
                            syscall(SYS_gettid), task.fn_name.c_str());
                task.fn();
            }
        });
    }    
}  

void thpool::addTask(std::function<void()> task)
{
    addTask("unamed", std::move(task));
}

void thpool::addTask(std::string name, std::function<void()> task)
{ //在Pithikos的实现里，要把函数对象指针和参数指针一起传给任务，但是C++
  //有包装器，可以连参数也一起绑定了
    {
        std::unique_lock<std::mutex> lock(mutex_);
        taskQueue.push(TaskItem{std::move(name), std::move(task)});     //涉及任务队列操作一定要有同步机制防止竞态
    } 
    tCondition.notify_one();     //由条件变量通知阻塞的线程
}  

size_t thpool::size() const
{
    return threads.size();
}

thpool::~thpool()
{
    stop_ = true;
    tCondition.notify_all();
    for(std::thread &th : threads)
        th.join();
}  

// class AA
// {
// public:
//     void show()
//     {
//         printf("我是一只傻傻鸟。\n");
//     }

//     ~AA() 
//     {
//         printf("调用了析构函数。\n");
//     }
// };

// #include <memory>

// void fun(AA *aa)
// {
//     sleep(5);
//     aa->show();
// }

// int main()
// {
//     thpool tp(2,"TEST");

//     {
//         AA *aa = new AA;
//         tp.addTask(std::bind(fun,aa));
//         delete aa;
//     }

//     sleep(10);   // 让线程有时间可以运行。

//     return 0;
// }
// //////////////////////////////////////////////////////////////////


// // g++ -o test ThreadPool.cpp -lpthread