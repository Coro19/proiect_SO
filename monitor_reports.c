#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>

#define PID_FILE ".monitor_pid"

void handle_signal(int sig) {
    if (sig == SIGUSR1)
        write(STDOUT_FILENO, "Monitor: new report added.\n", 27);
    else if (sig == SIGINT) {
        write(STDOUT_FILENO, "Monitor: shutting down.\n", 24);
        unlink(PID_FILE);
        _exit(0);
    }
}

int main(void) {
    int fd = open(PID_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        printf("Error: cannot create %s\n", PID_FILE);
        return 1;
    }
    char pidbuf[32];
    int len = snprintf(pidbuf, sizeof(pidbuf), "%d\n", getpid());
    write(fd, pidbuf, len);
    close(fd);

    printf("Monitor started with PID %d\n", getpid());

    struct sigaction sa_usr1;
    sa_usr1.sa_handler = handle_signal;
    sigemptyset(&sa_usr1.sa_mask);
    sa_usr1.sa_flags = SA_RESTART;
    sigaction(SIGUSR1, &sa_usr1, NULL);

    struct sigaction sa_int;
    sa_int.sa_handler = handle_signal;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = 0;
    sigaction(SIGINT, &sa_int, NULL);

    while (1) {
        pause();
    }

    return 0;
}