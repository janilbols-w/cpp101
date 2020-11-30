#include "executor_agent.h"

void naive_func(void* arg){
    int value = *(int*)arg;
    printf("naive_func(%d) on thread %d\n", value, pthread_self());
    delete arg;
    usleep(10);
}
void naive_pin_func(void* arg){
    int value = *(int*)arg;
    printf("naive_pin_func(%d) on thread %d\n", value, pthread_self());
    delete arg;
    usleep(10);
}


int executor_test(){
    int n_executors = 4;
    int n_tasks = 40;
    Executor executors[n_executors];
    queue<int> free_executors_queue;
    pthread_mutex_t work_queue_lock;
    pthread_mutex_init(&work_queue_lock, NULL);
    // init all executors
    for(int idx = 0; idx < n_executors; idx ++){
        int res = executors[idx].start(idx, &free_executors_queue, &work_queue_lock);
        printf("executor start with assigned_idx(%d), free_executors_queue(%p), res(%d)\n", 
                idx, &free_executors_queue, res);
    }
    
    // add_tasks
    for (int cnt = 0; cnt < n_tasks; cnt++){
        while(free_executors_queue.empty()){
            printf("no free executors\n");
            usleep(1);
        }
        Executor* executor = executors + free_executors_queue.front();
        free_executors_queue.pop();
        int* arg = new int(cnt);
        int res = executor->add_task(&naive_func, (void *)arg);
        printf("add_task(%d) with res %d\n", cnt, res);
    }

    // pin a executor
    int pinned_list[(int)(n_executors/2)];
    for (int i = 0; i < (int)(n_executors/2); i++){
        while(free_executors_queue.empty()){
            printf("no free executors\n");
            usleep(10);
        }
        int res = -1;
        int idx = free_executors_queue.front();
        pinned_list[i] = idx;
        free_executors_queue.pop();
        Executor* executor = executors + idx;
        while(res != 0){
            res = executor->get_pinned();
            usleep(10);
        }
        printf("pinned(thread %d) with res %d\n", idx, res);
    }

    // add_pinned_tasks
    for (int cnt = 2*n_tasks; cnt < 3*n_tasks; cnt++){
        int idx = pinned_list[cnt%(int)(n_executors/2)];
        Executor* executor = executors + idx;
        int* arg = new int(cnt);
        int res = executor->add_task(&naive_pin_func, (void *)arg);
        printf("add_pinned_task(%d, ) with res %d\n", idx, cnt, res);
    }

    // unpin executor
    for (int i = 0; i < (int)(n_executors/2); i++){
        int res = -1;
        Executor* executor = executors + pinned_list[i];
        while(1){
            res = executor->unpin();
            if(res==0){
                break;
            }
            usleep(10);
        }
        printf("unpin(thread %d) with res %d\n", pinned_list[i], res);
    }

    // add_tasks
    for (int cnt = 3*n_tasks; cnt < 4*n_tasks; cnt++){
        while(free_executors_queue.empty()){
            printf("no free executors\n");
            usleep(1000);
        }
        Executor* executor = executors + free_executors_queue.front();
        free_executors_queue.pop();
        int* arg = new int(cnt);
        int res = executor->add_task(&naive_func, (void *)arg);
        printf("add_task(%d) with res %d\n", cnt, res);
    }

    // shutdown
    for(int idx = 0; idx < n_executors; idx ++){
        int res = executors[idx].shutdown();
        printf("executor shutted with assigned_idx(%d), res(%d)\n", idx, res);
    }
    return 0;
}

int main(int argc, char **argv){
    int n_executors = 4;
    int n_tasks = 40;

    // init
    ExecutorAgent agent;
    agent.start(n_executors);

    printf("=======================================================\n");
    //add_task
    for (int cnt = 0; cnt < n_tasks; cnt++){
        int* arg = new int(cnt);
        int res = agent.add_task(&naive_func, (void *)arg);
        printf("add_task(%d) with res %d\n", cnt, res);
    }

    printf("=======================================================\n");
    // pin_one_executor
    int pinned_list[(int)(n_executors/2)];
    for (int i = 0; i < (int)(n_executors/2); i++){
        int idx;
        int res = agent.pin_one_executor(&idx);
        while(res!=0){
            usleep(EXECUTOR_AGENT_RETRY_INTERVAL);
            res = agent.pin_one_executor(&idx);
        }
        pinned_list[i] = idx;
        printf("pin_one_executor() got idx%d, with res %d\n",idx, res);
    }

    printf("=======================================================\n");
    //add_pinned_task
    for (int cnt = 1*n_tasks; cnt < 2*n_tasks; cnt++){
        int* arg = new int(cnt);
        int idx = pinned_list[cnt%(int)(n_executors/2)];
        int res = agent.add_task(&naive_pin_func, (void *)arg, idx);
        printf("add_pinned_task(%d, %d) with res %d\n", idx, cnt, res);
    }


    printf("=======================================================\n");
    // unpin_executor
    for (int i = 0; i < (int)(n_executors/2); i++){
        int idx = pinned_list[i];
        int res = agent.unpin_executor(idx);
        printf("unpin_executor(%d) with res %d\n",idx, res);
        while(res!=0){
            usleep(EXECUTOR_AGENT_RETRY_INTERVAL);
            res = agent.unpin_executor(idx);
            printf("unpin_executor(%d) with res %d\n",idx, res);
        }
    }

    printf("=======================================================\n");
    //add_task
    for (int cnt = 2*n_tasks; cnt < 5*n_tasks; cnt++){
        int* arg = new int(cnt);
        int res = agent.add_task(&naive_func, (void *)arg);
        printf("add_task(%d) with res %d\n", cnt, res);
    }   


    printf("=======================================================\n");
    // destroy
    agent.shutdown();
}