#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>
#include <string.h>
#include "touch_emulate.h"

int main(int argc, char ** argv) {
    if(strcmp(argv[1], "power") == 0) {
        emulate_power_button_input_event();
    }
}

void emulate_power_button_input_event() {
    // Input event
    struct input_event power_input_event = {};
    power_input_event.type = EV_KEY;
    power_input_event.code = KEY_POWER;
    // SYN report event
    struct input_event syn_report_event = {};
    syn_report_event.type = EV_SYN;
    syn_report_event.code = SYN_REPORT;
    syn_report_event.value = 0;

    int fd = open("/dev/input/event0", O_WRONLY);
    if(fd != -1) {
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
    }
    else {
        fprintf(stderr, "Failed to open '/dev/input/event0' input device node\n");
    }
}
