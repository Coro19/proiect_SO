#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

void cmd_start_monitor(void) {
    int pipefd[2];
    if (pipe(pipefd) < 0) {
        printf("Error: pipe failed.\n");
        return;
    }
    pid_t hub_mon_pid = fork();
    if (hub_mon_pid < 0) {
        printf("Error: fork failed.\n");
        return;
    }
    if (hub_mon_pid == 0) {
        pid_t mon_pid = fork();
        if (mon_pid < 0) {
            _exit(1);
        }
        if (mon_pid == 0) {
            dup2(pipefd[1], STDOUT_FILENO);
            close(pipefd[0]);
            close(pipefd[1]);
            execl("./monitor_reports", "monitor_reports", NULL);
            _exit(1);
        }
        close(pipefd[1]);
        char buf[512];
        int pos = 0;
        char c;
        while (read(pipefd[0], &c, 1) > 0) {
            if (c == '\n') {
                buf[pos] = '\0';
                char *sep = strchr(buf, '|');
                if (sep) {
                    *sep = '\0';
                    char *type = buf;
                    char *msg  = sep + 1;
                    if (strcmp(type, "EXIT") == 0) {
                        printf("[Monitor] %s\n", msg);
                        printf("hub_mon: monitor has ended.\n");
                        fflush(stdout);
                        break;
                    } else if (strcmp(type, "ERROR") == 0) {
                        printf("[Monitor ERROR] %s\n", msg);
                        printf("hub_mon: monitor has ended.\n");
                        fflush(stdout);
                        break;
                    } else {
                        printf("[Monitor] %s\n", msg);
                        fflush(stdout);
                    }
                }
                pos = 0;
            } else {
                if (pos < (int)sizeof(buf) - 1)
                    buf[pos++] = c;
            }
        }

        close(pipefd[0]);
        _exit(0);
    }
    close(pipefd[0]);
    close(pipefd[1]);
    printf("Monitor started in background (hub_mon PID: %d)\n", hub_mon_pid);
}
int main(void) {
    char input[256];
    printf("city_hub started. Type 'exit' to quit.\n");
    while (1) {
        printf("city_hub> ");
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL) break;
        input[strcspn(input, "\n")] = '\0';

        if (strcmp(input, "start_monitor") == 0)
            cmd_start_monitor();
        else if (strcmp(input, "exit") == 0)
            break;
        else if (strlen(input) == 0)
            continue;
        else
            printf("Unknown command: %s\n", input);
    }

    return 0;
}