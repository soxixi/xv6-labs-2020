
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char* fmtname(char *path) //参考ls中的fmtname代码
{
	static char buf[DIRSIZ+1];
 	char *p;
  	// Find first character after last slash.
  	for(p=path+strlen(path); p >= path && *p != '/'; p--)
    	;
  	p++;
  	// Return blank-padded name.
  	if(strlen(p) >= DIRSIZ)
    	return p;
  	memmove(buf, p, strlen(p));
	buf[strlen(p)] = 0;  //字符串结束符
  	return buf;
}

void
find(char *path, char *name)
{
 	char buf[512], *p;  //声明的这些结构体、变量等也和ls一样
 	int fd;
 	struct dirent de;
 	struct stat st;

	 if((fd = open(path, 0)) < 0){ //判断，也和ls一样
	   fprintf(2, "find open %s error\n", path);
	   exit(1);
	 }
	 if(fstat(fd, &st) < 0){ // 执行fstat 可以获得fd指向的文件信息（其中就包括文件名name）
	   fprintf(2, "fstat %s error\n", path);
	   close(fd);
	   exit(1);
	 }

	 switch(st.type){
	 case T_FILE:  //如果是文件类型
	 	if(strcmp(fmtname(path), name) == 0) 
	     	printf("%s\n", path); //如果当前路径和文件名相同，则代表找到文件了
	  	break;
	 case T_DIR:  //如果是目录类型
	 	if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
	    	printf("find: path too long\n");
	       	break;
	   	}
	   	strcpy(buf, path); //将输入的目录字符串复制到buf中
	   	p = buf+strlen(buf);
	   	*p++ = '/';  //将`/`拼接在后面
	   	//读取 fd ，如果 read 返回字节数与 de 长度相等则循环   能够正常读取文件句柄fd到de结构体  查找非空目录
	   	while(read(fd, &de, sizeof(de)) == sizeof(de)){
	      	if(de.inum == 0) //de.inum==0表示这是一块已经初始化并且可以用来创建文件或者文件夹的位置，所以在ls读取的过程中应当无视这一块空间
	          	continue;
	      	memmove(p, de.name, DIRSIZ); //将目录项的文件名（de.name）拼接到路径缓冲区（p）的末尾，获得该目录项的完整路径。
	       	p[DIRSIZ] = 0;
	       //不去递归处理.和..
	       if(!strcmp(de.name, ".") || !strcmp(de.name, ".."))
	           continue;
	       find(buf, name); //继续进入下一层目录递归处理
	   }
	   break;
	 }
	 close(fd);
}

int
main(int argc, char *argv[])
{
  if(argc != 3){
    fprintf(2, "usage:find <path> <name>\n");
    exit(1);
  }
    find(argv[1], argv[2]);
    exit(0);
}
