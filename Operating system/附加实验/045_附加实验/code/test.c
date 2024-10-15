#include <stdio.h>
#include <signal.h>

void int_handler(int signal_num) {
    printf("signal receive %d\n", signal_num);
    signal(SIGINT, NULL);
}

int main(int argc, char ** argv) {
    signal(SIGINT, int_handler);
    printf("hello world!\n");
    for(;;)
        pause();
    return 0;
}
