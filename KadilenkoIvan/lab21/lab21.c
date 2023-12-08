#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

int signal_count = 0;

void sigint_handler(int signum) {
    system("echo -e '\a'");
    signal_count++;
}

void sigquit_handler(int signum) {
    printf("\nSignal played %d times\n", signal_count);
    _exit(0);
}

int main() {
    if (signal(SIGINT, sigint_handler) == SIG_ERR)
        return 1;

    if (signal(SIGQUIT, sigquit_handler) == SIG_ERR) 
        return 1;

    while (1) { }

    return 0;
}
