#include <stdio.h>  // printf()
#include <unistd.h> // close(), read(), write()
#include <sys/fcntl.h>  // open()

int   main(void) {
  int fd;
  char buf[1000];
  int read_ret, write_ret;


  fd = open("/dev/driver", O_RDWR);
  if (fd < 0)
  {
    printf("failed opening device\n");
    return 0;
  }
  else
  {
    printf("succeess opening device\n");
  }

  write_ret = write(fd, "hello", 5);
  read_ret = read(fd, buf, 5);
  printf("fd = %d, ret write = %d, ret read = %d\n", fd, write_ret, read_ret);
  printf("content = %s\n", buf);

  close(fd);
}
