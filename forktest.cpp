#include <iostream>
#include <unistd.h>

int main() {
  int ppid = fork();
  if (ppid > 0) {
    std::cout << "我是父进程 " << getpid() << std::endl;
  } else if (ppid == 0) {
    std::cout << "我是子进程 " << getpid() << std::endl;
  } else {
    std::cout << "fork 失败" << std::endl;
  }
  return 0;
}