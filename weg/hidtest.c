// from http://www.linuxquestions.org/questions/linux-hardware-18/installing-a-usb-scale-503125/

#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <asm/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/hiddev.h>

#define EV_NUM 3

int main (int argc, char **argv) {

  int fd = -1;
  int i, flags;
  struct hiddev_event ev[EV_NUM];
  char name[100];

  if ((fd = open("/dev/usb/hiddev0", O_RDONLY)) < 0) {
    perror("couldn't open /dev/usb/hiddev0");
    exit(1);
  }
  flags = fcntl(fd, F_GETFL, 0);
  fcntl(fd, F_SETFL, flags | O_NONBLOCK);

  ioctl(fd, HIDIOCGNAME(100), name);
  printf("OK name = %s\n", name);
  
  printf("Reading values .. \n");
  while (1) {
      memset(ev, 0, sizeof(struct hiddev_event) * EV_NUM); 
      if (read(fd, ev, sizeof(struct hiddev_event) * EV_NUM) > 0) 
      {
        for (i = 0; i < EV_NUM; i++) 
        {
            printf("EV: PARAM %x : %d\n", ev[i].hid, ev[i].value);
        }
      }
  }

  close(fd);

  exit(0);
}
