// ==================================
//  Author : wanghuize@virtaitehc.com
//  Date   : Nov 2020
// ==================================

#ifndef _DEF_EXECUTOR_AGENT_
#define _DEF_EXECUTOR_AGENT_

#include <iostream>
#include <queue>
#include <algorithm>
#include <unistd.h>

#define MAXIMUM_EXECUTORS_NUM 64

#define EXECUTOR_AGENT_RETRY_INTERVAL 10 //mu sec
#define EXECUTOR_AGENT_MAXIMUM_RETRY_TIMES 6000000/EXECUTOR_AGENT_RETRY_INTERVAL

using namespace std;

struct TaskRunable{
    void (*function)(void *);
    void *argument;
};

enum ExecutorShutdownCode{
    executor_unshut                  = 0,
    executor_graceful_shutdown       = 1,
    executor_immediately_shutdown    = 2,
};

enum ExecutorErrorCode {
    executor_success         = 0,
    executor_invalid         = 1,
    executor_lock_failure    = 2,
    executor_queue_full      = 3,
    executor_shutdown        = 4,
    executor_failure         = 5,
    executor_busy            = 6, 
};


enum ExecutorAgentType{
    ea_wait_for_success    = 1,
    ea_immediately_return  = 2,
};

class Executor{
private:
    pthread_mutex_t executor_lock;                               // mutex_lock
    pthread_cond_t notify;                              // condition
    queue<TaskRunable> task_queue;                      // queue
    queue<int>* work_queue;                             // Manager's work_queue: once free regist current executor  thread idx back into work queue
    pthread_mutex_t* work_queue_lock;
    ExecutorShutdownCode shutdown_flag;                 // flag to shutdown
    int thread_idx;                                     // assigned from the outside, default with -1
    pthread_t work_thread;                              // current
    static void *main_loop(void * this_thread);         // execution func
    bool pinned;

public:
    Executor();
    ~Executor();
    int start(int assigned_idx = -1, queue<int>* work_queue = nullptr, pthread_mutex_t* work_queue_lock = nullptr); // init func
    int shutdown(ExecutorShutdownCode flag = executor_graceful_shutdown); // destory whole thead pool, terminate current working tasks
    int add_task(void (*function)(void *), void *argument = nullptr); // adding new task
    int get_pinned(); // when executor is free, get pinned and return success; otherwise return executor_busy
    int unpin();
    bool is_pinned();
};

class ExecutorAgent{
private:
    ExecutorAgentType agent_type;
    pthread_mutex_t agent_lock;
    int num_executors;
    queue<int> free_executors_queue;
    Executor* executors;
public:
    ExecutorAgent();
    ~ExecutorAgent();
    int start(int n_executors, ExecutorAgentType agent_type = ea_wait_for_success);
    int shutdown(ExecutorShutdownCode flag = executor_graceful_shutdown);
    int add_task(void (*function)(void *), void *argument = nullptr, int executor_idx = -1);
    int pin_one_executor(int* executor_idx);
    int unpin_executor(int executor_idx);
};

#endif //_DEF_EXECUTOR_AGENT_
