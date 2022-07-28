// ==================================
//  Author : wanghuize@virtaitehc.com
//  Date   : Nov 2020
// ==================================

#include "executor_agent.h"




// =============================================================================
//                              Class Executor
// =============================================================================

// ===================================================================
//             >>>>>>      General Design      <<<<<<
// This implementation makes executor working with its own task_queue,
// that it only recieve tasks passively.
// Whoever want to command this executor should initiatively push the 
// task to its queue.
// 
// In order to infer manager when executor is free, we provide executor an 
// queue address, once executors free, thy will regist their indexes 
// back to the queue
//
// ===================================================================
//             >>>>>>          Usage           <<<<<<
// 1. init
//    Executor executor;
//    int assigned_idx = 0;
//    queue<int> free_executors;
//    executor(assigned_idx, &free_executors);
// 2. add work
//    Executor executor = free_executors.front();
//    executor.add_task(&myTask, (void*) args);
// 3. destroy
//    executor.shutdown();//
// ===================================================================

Executor::Executor(){
    shutdown_flag = executor_unshut;
    thread_idx = -1;                                // default with -1
    work_thread = -1;                                   // default with -1
    pinned = false;
    work_queue = nullptr;
    work_queue_lock = nullptr;
}

Executor::~Executor(){
    pthread_mutex_lock(&executor_lock);
    pthread_mutex_destroy(&executor_lock);
    pthread_cond_destroy(&notify);
}

// =================================================================================
// For executor::start, it's commander could give an assigned_idx to the executor 
// as long as an assigned_work_queue; This could be also
// Once the executor  has done it's work, and its task_queue is empty,
// executor  will regist the assigned_idx back to manager's assigned_work_queue for next
// ==================================================================================
int Executor::start(int assigned_idx, queue<int>* assigned_work_queue, pthread_mutex_t* work_queue_lock ){
    if (work_thread != -1){
        // duplicate init
        return executor_invalid;
    }
    pthread_mutex_init(&executor_lock, NULL);
    pthread_cond_init(&notify, NULL);
    thread_idx = assigned_idx;
    if(assigned_work_queue != nullptr){
        if(work_queue_lock == nullptr){
            return executor_invalid;
        }
        work_queue = assigned_work_queue;
        this->work_queue_lock = work_queue_lock;
    }
    if (pthread_create(&work_thread, NULL, main_loop, (void *)this) != 0){
        shutdown(executor_immediately_shutdown);
        return executor_failure;
    }
    return executor_success;
}


int Executor::shutdown(ExecutorShutdownCode flag){
    if (pthread_mutex_lock(&executor_lock) != 0){
        return executor_lock_failure;
    }
    // update signal for working thread
    shutdown_flag = flag;
    if (pthread_cond_broadcast(&notify)!= 0){
        return executor_lock_failure;
    }
    if (pthread_mutex_unlock(&executor_lock) != 0){
        return executor_lock_failure;
    }
    pthread_join(work_thread, NULL);
    return executor_success;
} // Executor::shutdown

int Executor::get_pinned(){
    ExecutorErrorCode res = executor_success;
    if (pthread_mutex_lock(&executor_lock) != 0){
        return executor_lock_failure;
    }
    if(task_queue.empty()){
        pinned = true;
        if (pthread_cond_broadcast(&notify) != 0){
            return executor_lock_failure;
        }
    }else{
        // when executor is busy, no one can pin it
        res = executor_busy;
    }
    if (pthread_mutex_unlock(&executor_lock) != 0){
        return executor_lock_failure;
    }
    return res;
} // Executor::get_pinned

int Executor::unpin(){
    // unpin will set false to this->pinned
    // this will allow this executor to regist back at work_queue, when free
    if (pthread_mutex_lock(&executor_lock) != 0){
        return executor_lock_failure;
    }
    pinned = false;
    // send signal
    if (pthread_cond_signal(&notify) != 0){
        return executor_lock_failure;
    }    
    if (pthread_mutex_unlock(&executor_lock) != 0){
        return executor_lock_failure;
    }
    return executor_success;
} // Executor::unpin

bool Executor::is_pinned(){
    return pinned;
}

int Executor::add_task(void (*function)(void *), void *argument)
{
    /* fetch mutex */
    if (pthread_mutex_lock(&executor_lock) != 0)
        return executor_lock_failure;

    // check if threadpool is shutdown
    if (shutdown_flag)
    {
        return executor_shutdown;
    }

    // init new comer
    TaskRunable newRunable;
    newRunable.function = function;
    newRunable.argument = argument;
    // in queue
    task_queue.push(newRunable);
    
    // send signal
    if (pthread_cond_signal(&notify) != 0){
        return executor_lock_failure;
    }        
    if (pthread_mutex_unlock(&executor_lock)!=0){
        return executor_lock_failure;
    }
    return executor_success;
} // Executor::add_task



void* Executor::main_loop(void* this_thread){
    // when queue empty, and not pinned waiting at queue
        // regist back to work_queue
        // condition wait here
    Executor* executor  = (Executor*)this_thread;
    printf("[executor]: This is executor(%d) running on thread %d!\n", executor->thread_idx, pthread_self());
    while(1){
                

        // regist back to work_queue, if only if:
        //      1. it is initialized with idx & work_queue,
        //      2. it is not pinned
        //      3. task_queue is empty
        pthread_mutex_lock(executor->work_queue_lock);
        if(!executor->pinned && executor->work_queue!=nullptr && executor->task_queue.empty()){
            executor->work_queue->push(executor->thread_idx);
        }
        pthread_mutex_unlock(executor->work_queue_lock);

        if (pthread_mutex_lock(&executor->executor_lock) != 0){
            break;
        }
        // condition wait when not shutting down, and task queue empty
        while(executor->task_queue.empty() && (!executor->shutdown_flag)){
            printf("[executor]: Thread-%d is waiting at task_queue!\n", executor->thread_idx);
            pthread_cond_wait(&(executor->notify), &(executor->executor_lock));
            printf("[executor]: Thread-%d is leaving task_queue!\n", executor->thread_idx);          
        }

        if((executor->shutdown_flag == executor_immediately_shutdown) ||
           ((executor->shutdown_flag == executor_graceful_shutdown) && (executor->task_queue.empty()))) {
            // update num of working threads
            /* releasing mutex, correspond to  */
            pthread_mutex_unlock(&(executor->executor_lock));
            pthread_exit(NULL);
            printf("[executor]: Thread-%d is shut!\n", executor->thread_idx);  
            break;
        }
        
        // fetch task in queue
        TaskRunable task;
        task.function = NULL;
        if (!executor->task_queue.empty())
        {
            task = executor->task_queue.front();
            executor->task_queue.pop();
        }

        /* release mutex */
        pthread_mutex_unlock(&(executor->executor_lock));

        // execute task
        if(task.function != NULL){
            (*(task.function))(task.argument);
        }   
    }
    return (NULL);
}// end for main_loop


// =============================================================================
//                              Class ExecutorAgent
// =============================================================================
// Description: 
//     this class is used to handle class Executor, to minimize the complexity 
// of Executor usage
//     it will manage a executor pool for you, and depatch your task to specific
// executor. all executor will regist back to the agent when they are free again.
//     for those pinned executors, only if they are unpinned and task_queue is
// empty, will they regist back to queue; 
//     you need to keep the idx of executor, to add task to this pinned executor
// =============================================================================
// TODO:
//      1. require a new function to unpin all executors
// =============================================================================

ExecutorAgent::ExecutorAgent(){
    agent_type = ea_wait_for_success;
    num_executors = 0;
    executors = nullptr;
} // ExecutorAgent::ExecutorAgent

ExecutorAgent::~ExecutorAgent(){
    pthread_mutex_lock(&agent_lock);
    pthread_mutex_destroy(&agent_lock);
    delete[] executors;
} // ExecutorAgent::~ExecutorAgent

int ExecutorAgent::start(int n_executors, ExecutorAgentType agent_type){
    if(n_executors <= 0 || n_executors > MAXIMUM_EXECUTORS_NUM || this->num_executors >0 ){
        return executor_invalid;
    }

    this->agent_type = agent_type;
    pthread_mutex_init(&agent_lock, NULL);
    executors = new Executor[n_executors];

    // start executors one by one
    for(int idx = 0; idx < n_executors; idx ++){
        int res = executors[idx].start(idx, &free_executors_queue, &agent_lock);
        printf("[ea] executor start with assigned_idx(%d), free_executors_queue(%p), res(%d)\n", 
                idx, &free_executors_queue, res);
        if( res != 0 ){
            // recycle all started executors
            shutdown(executor_immediately_shutdown);
            return executor_failure;
        }
        this->num_executors ++;
    }

    return executor_success;
} // ExecutorAgent::start

int ExecutorAgent::shutdown(ExecutorShutdownCode flag){
    printf("[ea] executor is trying to shutdown\n");
    // pthread_mutex_lock(&agent_lock);
    for(int idx = 0; idx < this->num_executors; idx++){
        printf("[ea] executor shutting with assigned_idx(%d)\n", idx);
        int res = executors[idx].shutdown(flag);
        printf("[ea] executor shutted with assigned_idx(%d), res(%d)\n", idx, res);
    }
    delete[] executors;
    this->num_executors = 0;
    // pthread_mutex_unlock(&agent_lock);
    return executor_success;
} // ExecutorAgent::shutdown

int ExecutorAgent::add_task(void (*function)(void *), void *argument, int executor_idx){
    // if idx == -1, execute with any free executor
    // else try add task to the executors[idx]
    //      if executors[idx] is not pinned, return invalid usage
    //      if ExecutorAgentType is wait_for_success, wait for success if executor busy, 
    //          otherwise return failure code
    //      elif ExecutorAgentType is immediately_return, try once return whatever returns
    Executor* executor;
    if(executor_idx == -1){
        // add task to a free executor
        if (free_executors_queue.empty() && agent_type==ea_immediately_return){
            return executor_busy;
        }
        int cnt_retry = 0;
        while(free_executors_queue.empty()){
            printf("[ea] no free executors, retried %d times\n", cnt_retry);
            usleep(EXECUTOR_AGENT_RETRY_INTERVAL);
            cnt_retry ++;
            if (cnt_retry > EXECUTOR_AGENT_MAXIMUM_RETRY_TIMES){
                return executor_busy;
            }
        }
        pthread_mutex_lock(&agent_lock);
        // pick a free executor to assign task
        executor = executors + free_executors_queue.front();
        free_executors_queue.pop();
        pthread_mutex_unlock(&agent_lock);
    }else if ( executor_idx>=0 && executor_idx < num_executors){
        // add_task to a pinned executor
        executor = executors + executor_idx;
        if(!executor->is_pinned()){
            // if this executor is not pinned, this approach is invalid
            printf("[ea] this executor should get pinned first!\n");
            return executor_invalid;
        }
    }else{
        // invalid executor_idx
        return executor_invalid;
    }

    // now executor is valid to add_task
    return executor->add_task(function, argument);

} // ExecutorAgent::add_task


int ExecutorAgent::pin_one_executor(int* executor_idx){
    if (free_executors_queue.empty() && agent_type==ea_immediately_return){
        return executor_busy;
    }
    int cnt_retry = 0;
    while(free_executors_queue.empty()){
        printf("[ea] no free executors, retried %d times\n", cnt_retry);
        usleep(EXECUTOR_AGENT_RETRY_INTERVAL);
        cnt_retry ++;
        if (cnt_retry > EXECUTOR_AGENT_MAXIMUM_RETRY_TIMES){
            return executor_busy;
        }       
    }
    pthread_mutex_lock(&agent_lock);
    int idx = free_executors_queue.front();
    free_executors_queue.pop();
    pthread_mutex_unlock(&agent_lock);
    Executor* executor = executors + idx;
    int res = executor->get_pinned();
    cnt_retry = 0;
    while(res != 0 && agent_type==ea_wait_for_success){
        res = executor->get_pinned();
        usleep(EXECUTOR_AGENT_RETRY_INTERVAL);
        cnt_retry ++;
        if (cnt_retry > EXECUTOR_AGENT_MAXIMUM_RETRY_TIMES){
            break;
        }
    }
    printf("[ea] pinned(thread %d) with res %d, retried %d times\n", idx, res, cnt_retry);
    *executor_idx = idx;
    return res;
}

int ExecutorAgent::unpin_executor(int executor_idx){
    return executors[executor_idx].unpin();
}