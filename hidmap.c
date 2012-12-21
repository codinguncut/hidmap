// Key Mapping for Infinity Foot Pedal
//
// Reads foot pedal events from /dev/usb/hiddev0
// and translates them into /dev/uinput keyboard events
//
// Johannes Ahlmann, 2012-12-21
// Released under MIT Expat License
//
// based on 
//   http://www.einfochips.com/download/dash%5Fjan%5Ftip.pdf
//   and
//   http://www.linuxquestions.org/questions/linux-hardware-18/installing-a-usb-scale-503125/

#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/uinput.h>
#include <linux/hiddev.h>

// foot pedal sends 3 events for the three pedals
#define NUM_EVENTS 3

static int uinp_fd = -1;
struct uinput_user_dev uinp;

int init_uinput()
{
  int i=0;

  uinp_fd = open("/dev/uinput", O_WRONLY | O_NDELAY);
  if (uinp_fd == 0)
  {
    printf("Unable to open /dev/uinput\n");
    return -1;
  }

  memset(&uinp,0,sizeof(uinp));

  strncpy(uinp.name, "Infinity Foot Pedal", UINPUT_MAX_NAME_SIZE);
  uinp.id.version = 4;
  uinp.id.bustype = BUS_USB;

  // Setup the uinput device as a keyboard with key repetition
  ioctl(uinp_fd, UI_SET_EVBIT, EV_KEY);
  ioctl(uinp_fd, UI_SET_EVBIT, EV_REP);
  
  for (i=0; i < 256; i++) {
    ioctl(uinp_fd, UI_SET_KEYBIT, i);
  }

  write(uinp_fd, &uinp, sizeof(uinp));
  if (ioctl(uinp_fd, UI_DEV_CREATE))
  {
    printf("Unable to create uinput device.");
    return -1;
  }
  return 1;
}

// send keyboard event with
// "type" (e.g. EV_KEY)
// "code" (e.g. KEY_A)
// "value" (1 for down, 0 for up)
void send_button(__u16 type, __u16 code, __s32 value)
{
  struct input_event event;

  memset(&event, 0, sizeof(event));
  gettimeofday(&event.time, NULL);
  event.type = type;
  event.code = code;
  event.value = value;
  write(uinp_fd, &event, sizeof(event));

  event.type = EV_SYN;
  event.code = SYN_REPORT;
  event.value = 0;
  write(uinp_fd, &event, sizeof(event));
}

void key_action(int which, int is_down) 
{
  int code;
  switch (which)
  {
    case 0: code = KEY_F7; break;
    case 1: code = KEY_F8; break;
    case 2: code = KEY_F9; break;
  }
  send_button(EV_KEY, code, is_down);
}

/* This function will open the uInput device. Please make
sure that you have inserted the uinput.ko into kernel. */
int main()
{
  int j,k,tmp,rd;
  int hid, flags;
  struct hiddev_event ev[NUM_EVENTS];
  char pedals[3]    = {0,0,0};
  int pedal_ids[3]  = {0x90001, 0x90002, 0x90003};
  int size          = sizeof(struct hiddev_event);

  if (init_uinput() < 0)
  {
    printf("Unable to find uinput device\n");
    return -1;
  }
  
  hid = open("/dev/usb/hiddev0", O_RDONLY);
  if (hid == 0)
  {
    printf("Unable to open /dev/usb/hiddev0\n");
    return -1;
  }
  // set "hid" to non-blocking reading
  flags = fcntl(hid, F_GETFL, 0);
  fcntl(hid, F_SETFL, flags | O_NONBLOCK);


  rd = 0;
  while (1) 
  {
    // if read succeeded
    if ((tmp = read(hid, &(ev[rd/size]), size * EV_NUM - rd)) > 0) 
    {
      rd += tmp;
      // read may only read a partial set of events.
      // therefore wait until we've read all three
      if (rd == EV_NUM * size)
      {
        rd = 0;
        for (j = 0; j < EV_NUM; j++) 
        {
          for (k = 0; k < 3; k++)
          {
            if (ev[j].hid == pedal_ids[k])
            {
              // if pedal state changed, trigger a keyboard event
              if (pedals[k] != ev[j].value)
              {
                key_action(k, ev[j].value);
              }
              pedals[k] = ev[j].value;
            }
          }
        }
      }
    }
  }

  // TODO: need to catch signals to properly close file handles, etc.

  /* Destroy the uinput device */
  ioctl(uinp_fd, UI_DEV_DESTROY);

  close(hid);
  close(uinp_fd);
}
