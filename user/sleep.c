#include "kernel/types.h" //一些数据类型声明
#include "user/user.h"//用户可以使用的系统调用声明
//编写 main(int argc,char* argv[]) 函数。
//其中，参数 argc 是命令行总参数的个数，参数 argv[] 是 argc 个参数，
//其中第 0 个参数是程序的全名，其他的参数是命令行后面跟的用户输入的参数。
//eg:sleep 10
int main(int argn, char *argv[]){
	if(argn != 2){//命令行参数不为2
    //参数 fd 是文件描述符，0 表示标准输入，1 表示标准输出，2 表示标准错误
		fprintf(2, "must 1 argument for sleep\n"); //将提示信息写入标准错误端
		exit(1);//退出，参数1表示非正常退出
	}
	int sleepNum = atoi(argv[1]);//将字符串转为整数
	printf("(nothing happens for a little while)\n"); 
	sleep(sleepNum);//调用系统调用sleep
	exit(0); //正常退出
}
