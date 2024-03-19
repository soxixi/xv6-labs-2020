#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

//ls操作是列出指定路径下的所有文件和目录。
char*
fmtname(char *path) //将路径转换为文件名，即将中间的/去掉
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  //从后往前，查找最后一个斜杠之后的第一个字符，也就是文件名的首字母
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  //返回空白填充的名称
  //如果文件名的长度大于DIRSIZ，则直接返回p。p的长度小于DIRSIZ，则在p的文件名后面补齐空格
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p)); //从存储区p复制stelen(p)个字节到存储区buf
  memset(buf+strlen(p), ' ', DIRSIZ-strlen(p)); //将' '填充到p后面
  return buf;
}

void
ls(char *path)
{
  char buf[512], *p; //保存文件名的缓冲数组
  int fd;  //文件描述符
  struct dirent de; //目录相关的结构体
  struct stat st; //文件相关的结构体，包含文件的状态信息

  if((fd = open(path, 0)) < 0){ //open() 函数打开路径，返回一个文件描述符，如果错误返回 -1
    fprintf(2, "ls: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){ //将文件状态保存到st中
    fprintf(2, "ls: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch(st.type){ //文件类型
  case T_FILE: //文件，打印文件名、类型、inode号、大小
    printf("%s %d %d %l\n", fmtname(path), st.type, st.ino, st.size);
    break;

  case T_DIR: //路径
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf("ls: path too long\n");
      break;
    }
    strcpy(buf, path);//将路径放入buf中
    p = buf+strlen(buf);//指针移动到buf最后一个字符后面
    *p++ = '/'; //添加上 /
     //读取 fd ，如果 read 返回字节数与 de 长度相等则循环 可以读取fd中的dirent序列（dirent结构中就有name）
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;
      memmove(p, de.name, DIRSIZ);//把 文件名信息复制 p
      p[DIRSIZ] = 0;//设置文件名结束符
      if(stat(buf, &st) < 0){
        printf("ls: cannot stat %s\n", buf);
        continue;
      }
      printf("%s %d %d %d\n", fmtname(buf), st.type, st.ino, st.size);
    }
    break;
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  int i;

  if(argc < 2){
    ls(".");
    exit(0);
  }
  for(i=1; i<argc; i++)
    ls(argv[i]);
  exit(0);
}
