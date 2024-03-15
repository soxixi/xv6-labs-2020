/*
要求：使用管道将 2 至 35 中的素数筛选出来
使用 pipe 和 fork 来创建管道。第一个进程将数字 2 到 35 送入管道中。
对于每个质数，你要安排创建一个进程，从其左邻通过管道读取，并在另一条管道上写给右邻。
关闭进程不需要的文件描述符，否则程序将在第一个进程到达 35 之前耗尽 xv6 资源。
*/

/*
提示：
1.关闭进程不需要的文件描述符,否则程序将在第一个进程到达 35 之前耗尽 xv6 资源。
2.一旦第一个进程到达 35 ，它应该等到整个管道终止，包括所有的子进程，孙子进程等。
因此，主进程只有在打印完所有输出后才能退出，即所有其他进程已退出后主进程才能退出。
3.当管道的写入端关闭时， read 函数返回 0 。
4.最简单的方式是直接将 32 位（4 字节）的整型写到管道中，而不是使用格式化的 ASCII I/O 
*/

/*
思路：
①主进程中创建一个管道，将2-11输入到管道中。
②创建一个子进程，读取管道的第1个数2并打印。创建一个新管道，将2的倍数全部筛除，剩下的(3、5、7、9、11)输入新管道中。
②创建一个子子进程，读取管道的第1个数3并打印。创建一个新管道，将3的倍数全部筛除，剩下的(5、7、11)输入新管道中。
···直到所有的数都被筛选完。
*/

#include "kernel/types.h"
#include "user/user.h"

void func(int *input, int num){
	if(num == 1){
		printf("prime %d\n", *input);
		return;
	}
	int p[2],i;
	int prime = *input;
	int temp;
	printf("prime %d\n", prime);
	pipe(p);
    if(fork() == 0){
        for(i = 0; i < num; i++){
            temp = *(input + i);
			write(p[1], (char *)(&temp), 4);
		}
        exit(0);
    }
	close(p[1]);
	if(fork() == 0){
		int counter = 0;
		char buffer[4];
		while(read(p[0], buffer, 4) != 0){
			temp = *((int *)buffer);
			if(temp % prime != 0){
				*input = temp;
				input += 1;
				counter++;
			}
		}
		func(input - counter, counter);
		exit(0);
    }
	wait(0);
	wait(0);
}

int main(){
    int input[34];
	int i = 0;
	for(; i < 34; i++){
		input[i] = i+2;
	}
	func(input, 34);
    exit(0);
}
