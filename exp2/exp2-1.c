#include "sys/types.h"
#include "sys/file.h"
#include "sys/wait.h"
#include "unistd.h"
#include <stdio.h>
#include <string.h>
#define buffersize 20
char r_buf[buffersize]; //读缓冲
char w_buf[buffersize]; //写缓冲
int pipe_fd[2];
pid_t pid1, pid2, pid3, pid4;

int producer(int id);
int consumer(int id);

int main()
{
    if (pipe(pipe_fd) < 0)
    {
        printf("pipe create error\n");
        exit(-1);
    }
    else
    {
        printf("pipe has created successfully!\n");
        if ((pid1 = fork()) == 0)
        {
            producer(1);
        }
        if ((pid2 = fork()) == 0)
        {
            producer(2);
        }
        if ((pid3 = fork()) == 0)
        {
            consumer(1);
        }
        if ((pid4 = fork()) == 0)
        {
            consumer(2);
        }
    }
    close(pipe_fd[0]); //管道读
    close(pipe_fd[1]); //管道写

    int pid, status;
    for (int i = 0; i < 4; i++)
    {
        /* code */
        pid = wait(&status);
    }
    exit(0);
}

int producer(int id)
{
    printf("producer %d is running!\n", id);
    close(pipe_fd[0]);
    for (int i = 1; i < 10; i++)
    {
        /* code */
        sleep(3);
        if (id == 1)
        {
            strcpy(w_buf, "producer 1\0");
        }
        if (id == 2)
        {
            strcpy(w_buf, "producer 2\0");
        }
        if (write(pipe_fd[1], w_buf, buffersize) == -1)
        {
            printf("write to pipe error!\n");
        }
    }
    close(pipe_fd[1]);
    printf("producer %d is over!\n", id);
    exit(id);
}

int consumer(int id)
{
    printf("consumer %d is running!\n", id);
    close(pipe_fd[1]);
    if (id == 1)
    {
        strcpy(w_buf, "consumer 1\0");
    }
    if (id == 2)
    {
        strcpy(w_buf, "consumer 2\0");
    }
    while (1)
    {
        /* code */
        sleep(1);
        strcpy(r_buf, "tmp data\0");
        if (read(pipe_fd[0], r_buf, buffersize) == 0)
        {
            break;
        }
        printf("consumer %d get %s,while the w_buf is %s\n", id, r_buf, w_buf);
    }
    close(pipe_fd[0]);
    printf("consumber %d is over!\n", id);
    exit(id);
}