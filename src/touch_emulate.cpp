#include "touch_emulate.h"
#include <cerrno>
#include <fcntl.h>
#include <iostream>
#include <linux/input.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <unistd.h>

// SYN report event
struct input_event syn_report_event = {};

int main(int argc, char *argv[]) {
  string command = argv[1];

  syn_report_event.type = EV_SYN;
  syn_report_event.code = SYN_REPORT;
  syn_report_event.value = 0;

  if (command == "--help" || argc < 2) {
    cout << "Usage: " << argv[0] << " <input_device> [command] <arguments>\n";
    cout << "Available commands:\n";
    cout << "--help - display this help message\n";
    cout << "touch <x> <y> - emulate touch event at coordinates (x, y)\n";
  } else if (command == "touch" && argc == 5) {
    int x = stoi(argv[3]);
    int y = stoi(argv[4]);
    emulate_touch_input_event(argv[2], x, y);
  } else if (command == "power" && argc == 3) {
    emulate_power_button_input_event(argv[2]);
  } else {
    cout << "Invalid command or arguments. Use '--help' for usage "
            "information.\n";
    exit(1);
  }

  return 0;
}

int open_wrap(string device) {
  int fd = open(device.c_str(), O_WRONLY);
  if (fd < 0) {
    cout << "Failed to open device";
  }
  return fd;
}

// Example input
/*
Event: time 1335925615.950937, type 3 (EV_ABS), code 1 (ABS_Y), value 249
Event: time 1335925615.950947, type 3 (EV_ABS), code 0 (ABS_X), value 751
Event: time 1335925615.950950, type 3 (EV_ABS), code 24 (ABS_PRESSURE), value
101 Event: time 1335925615.950953, type 1 (EV_KEY), code 330 (BTN_TOUCH), value
1 Event: time 1335925615.950956, -------------- SYN_REPORT ------------ Event:
time 1335925615.964333, type 3 (EV_ABS), code 1 (ABS_Y), value 250 Event: time
1335925615.964342, type 3 (EV_ABS), code 24 (ABS_PRESSURE), value 100 Event:
time 1335925615.964347, -------------- SYN_REPORT ------------ Event: time
1335925615.971093, type 3 (EV_ABS), code 24 (ABS_PRESSURE), value 0 Event: time
1335925615.971103, type 1 (EV_KEY), code 330 (BTN_TOUCH), value 0 Event: time
1335925615.971105, -------------- SYN_REPORT ------------
*/
// Note: x and y can be shifted
// Note: It's maybe not the best implementation, but it works
void emulate_touch_input_event(string device, int x, int y) {
  cout << "Sending touch event to: " << device << " value x: " << x
       << " value y: " << y << "\n";

  // First events
  struct input_event event_1 = {};
  event_1.type = EV_ABS;
  event_1.code = ABS_Y;
  event_1.value = y;

  struct input_event event_2 = {};
  event_2.type = EV_ABS;
  event_2.code = ABS_X;
  event_2.value = x;

  struct input_event event_3 = {};
  event_3.type = EV_ABS;
  event_3.code = ABS_PRESSURE;
  event_3.value = 101;

  struct input_event event_4 = {};
  event_4.type = EV_KEY;
  event_4.code = BTN_TOUCH;
  event_4.value = 1;

  // Second ones
  struct input_event event_5 = {};
  event_5.type = EV_ABS;
  event_5.code = ABS_Y;
  event_5.value = y + 1; // overflow?

  struct input_event event_6 = {};
  event_6.type = EV_ABS;
  event_6.code = ABS_PRESSURE;
  event_6.value = 100;

  // Third
  struct input_event event_7 = {};
  event_7.type = EV_ABS;
  event_7.code = ABS_PRESSURE;
  event_7.value = 0;

  struct input_event event_8 = {};
  event_8.type = EV_KEY;
  event_8.code = BTN_TOUCH;
  event_8.value = 0;

  // Send press event
  int delay = 50;
  int fd = open_wrap(device);
  write(fd, &event_1, sizeof(event_1));
  write(fd, &event_2, sizeof(event_2));
  write(fd, &event_3, sizeof(event_3));
  write(fd, &event_4, sizeof(event_4));
  write(fd, &syn_report_event, sizeof(syn_report_event));

  usleep(delay);

  write(fd, &event_5, sizeof(event_5));
  write(fd, &event_6, sizeof(event_6));
  write(fd, &syn_report_event, sizeof(syn_report_event));
  
  usleep(delay);

  write(fd, &event_7, sizeof(event_7));
  write(fd, &event_8, sizeof(event_8));
  write(fd, &syn_report_event, sizeof(syn_report_event));
}

void emulate_power_button_input_event(string device) {
  cout << "Sending power event to" << device << "\n";

  // Input event
  struct input_event power_input_event = {};
  power_input_event.type = EV_KEY;
  power_input_event.code = KEY_POWER;

  int fd = open(device.c_str(), O_WRONLY);
  if (fd != -1) {
    // Send press event
    power_input_event.value = 1;
    write(fd, &power_input_event, sizeof(power_input_event));

    // Send SYN report event
    write(fd, &syn_report_event, sizeof(syn_report_event));

    // Some sleep
    usleep(50);

    // Send release event
    power_input_event.value = 0;
    write(fd, &power_input_event, sizeof(power_input_event));
    write(fd, &syn_report_event, sizeof(syn_report_event));

    close(fd);
  } else {
    fprintf(stderr, "Failed to open '/dev/input/event0' input device node\n");
  }
}
