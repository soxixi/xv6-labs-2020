/*
要求：
在一对管道上实现两个进程之间的通信。
父进程通过第一个管道给子进程发送一个信息“ping”，子进程接收父进程的信息后打印"<pid>: received ping" ，其中pid是子进程的ID。
然后子进程通过另一个管道发送一个信息“pong”给父进程，父进程接收子进程的信息然后打印"<pid>: received pong"，其中pid是父进程的ID，然后退出。
*/


#include "kernel/types.h"
#include "user/user.h"

int main()
{
    int pfd[2];  //父进程的文件描述符数组
    int cfd[2];  //子进程的文件描述符数组
    // pipe(pipefd) 系统调用会创建管道，并把读取和写入的文件描述符返回到 pipefd 中。 
    // pipefd[0] 指管道的读取端， pipefd[1] 指管道的写入端。
    pipe(pfd);   //父进程创建管道 父进程写，子进程读
    pipe(cfd);   //子进程创建管道 子进程写，父进程读
    char buf[10];  //缓冲区字符数组，存放传递的信息
    int pid = fork(); //创建子进程

    if(pid < 0){
        fprintf(0, "fork error\n");
        exit(1); //创建子进程错误
    }
    else if(pid == 0){ //子进程
        close(pfd[1]); //关闭父进程管道的写入端
        read(pfd[0], buf, 4); //将父进程管道的读取端数据读取到buf中
        printf("%d: received %s\n", getpid(), buf); //打印收到的信息 通过getpid获取当前进程的pid
        close(cfd[0]); //关闭子进程管道的读取端
        write(cfd[1],"pong", 4); //将"pong"写入子进程管道的写入端
    }else{ //父进程
        close(pfd[0]); //关闭父进程管道的读取端
        write(pfd[1], "ping", 4); //将"ping"写入父进程管道的写入端
        close(cfd[1]); //关闭子进程管道的写入端
        read(cfd[0], buf, 4); //将子进程管道的读取端信息读取到buf中
        printf("%d: received %s\n", getpid(), buf); //打印收到的信息
    }

    exit(0); //正常退出
    return 0;
}
