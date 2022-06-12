#include <math.h>
#include <sched.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>
typedef struct
{
    char task_id; // 任务id
    int call_num; // 被调度次数
    int ci;       // 处理时间
    int ti;       // 发生周期
    int ci_left;  // 剩余的处理时间
    int ti_left;  // 剩余的周期时间
    int flag;     // 任务是否活跃
    int arg;      // 线程传参
    pthread_t th; // 线程标识符
} task;

void proc(int *args); // 处理时
void *idle();         // 空闲时
int select_proc();    // 选择线程

int task_num = 0; // 总任务数
int idle_num = 0; // 空闲任务数

int alg;            // 算法选择标识符
int curr_proc = -1; // 先调度的线程id
int prev_proc = -1; // 上一次调度的线程id

int demo_time = 100; // 总运行时间

task *tasks; // task数组

pthread_mutex_t proc_wait[100];
pthread_mutex_t main_wait, idle_wait;

float sum = 0;
pthread_t idle_proc;

int main(int argc, char *argv[])
{
    // 初始化锁
    pthread_mutex_init(&main_wait, NULL);
    pthread_mutex_lock(&main_wait);
    pthread_mutex_init(&idle_wait, NULL);
    pthread_mutex_lock(&idle_wait);

    // 初始化任务数
    printf("Please input number of real time tasks:\n");
    scanf("%d", &task_num);
    tasks = (task *)malloc(sizeof(task) * task_num);

    // 初始化线程锁
    int i = 0;
    for (i = 0; i < task_num; ++i)
    {
        pthread_mutex_init(&proc_wait[i], NULL);
        pthread_mutex_lock(&proc_wait[i]);
    }

    // 初始化线程参数
    for (i = 0; i < task_num; ++i)
    {
        printf("Please input task id, followed by Ci and Ti: \n");
        getchar();
        scanf("%c,%d,%d,", &tasks[i].task_id, &tasks[i].ci, &tasks[i].ti);

        tasks[i].ci_left = tasks[i].ci;
        tasks[i].ti_left = tasks[i].ti;
        tasks[i].flag = 2;
        tasks[i].arg = i;
        tasks[i].call_num = 1;
        sum = sum + (float)tasks[i].ci / (float)tasks[i].ti;
    }

    printf("Please input algorithm, 1 for EDF, 2 for RMS:");
    getchar();
    scanf("%d", &alg);
    printf("Please input demo time:");
    scanf("%d", &demo_time);

    double r = 1; // EDF 算法极限

    if (alg == 2)
    {
        // RMS 算法
        r = ((double)task_num) * (exp(log(2) / (double)task_num) - 1);
        printf("r is %lf\n", r);
    }

    if (sum > r)
    {
        // 不可调度
        printf("(sum = %lf > r = %lf), not schedulable!\n", sum, r);
        exit(2);
    }

    pthread_create(&idle_proc, NULL, (void *)idle, NULL); // 创建空闲进程

    for (i = 0; i < task_num; ++i)
    {
        pthread_create(&tasks[i].th, NULL, (void *)proc, &tasks[i].arg);
    }

    for (i = 0; i < demo_time; ++i)
    {
        int j;
        if ((curr_proc = select_proc(alg)) != -1)
        {
            // 按调度算法选择线程
            pthread_mutex_unlock(&proc_wait[curr_proc]); //唤醒
            pthread_mutex_lock(&main_wait);
        }
        else
        {
            pthread_mutex_unlock(&idle_wait);
            pthread_mutex_lock(&main_wait);
        }

        for (j = 0; j < task_num; ++j)
        {
            if (--tasks[j].ti_left == 0)
            {
                tasks[j].ti_left = tasks[j].ti;
                tasks[j].ci_left = tasks[j].ci;
                pthread_create(&tasks[j].th, NULL, (void *)proc, &tasks[j].arg);
                tasks[j].flag = 2;
            }
        }
    }

    printf("\n");
    sleep(10);
}

void proc(int *args)
{
    while (tasks[*args].ci_left > 0)
    {
        pthread_mutex_lock(&proc_wait[*args]);
        if (idle_num != 0)
        {
            printf("idle(%d)", idle_num);
            idle_num = 0;
        }

        printf("%c%d", tasks[*args].task_id, tasks[*args].call_num);
        tasks[*args].ci_left--; // 执行一个时间单位
        if (tasks[*args].ci_left == 0)
        {
            printf("(%d)\n", tasks[*args].ci);
            tasks[*args].flag = 0;
            tasks[*args].call_num++;
        }

        pthread_mutex_unlock(&main_wait);
    }
}

void *idle()
{
    while (1)
    {
        pthread_mutex_lock(&idle_wait);
        printf("->");
        idle_num++;
        pthread_mutex_unlock(&main_wait);
    }
}

int select_proc(int alg)
{
    int j;
    int temp1, temp2; //辅助变量，用于寻找下一个调度的线程
    temp1 = 10000;
    temp2 = -1;
    if ((alg == 2) && (curr_proc != -1) && (tasks[curr_proc].flag != 0))
    {
        return curr_proc;
    }
    for (j = 0; j < task_num; ++j)
    {
        if (tasks[j].flag == 2)
        {
            switch (alg)
            {
            case 1: // EDF
                if (temp1 >= tasks[j].ti_left)
                {
                    temp1 = tasks[j].ti_left;
                    if (temp1 > tasks[j].ti_left)
                    {
                        temp2 = j;
                    }
                    else
                    {
                        if (prev_proc != -1 && temp2 == prev_proc)
                        {
                            // do nothing
                        }
                        else
                        {
                            temp2 = j;
                        }
                    }
                }
                break;
            case 2: // RMS
                if (temp1 > tasks[j].ti)
                {
                    temp1 = tasks[j].ti;
                    temp2 = j;
                }
                break;
            }
        }
    }
    prev_proc = temp2;
    return temp2;
}
