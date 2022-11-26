#include <stdio.h>  // printf()
#include <unistd.h> // close()
#include <sys/fcntl.h>  // open()

int   main(void) {
  int fd;

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

  close(fd);
}
