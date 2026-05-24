#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
pid_t g_hub_mon_pid = -1;

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
    g_hub_mon_pid = hub_mon_pid;
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
void cmd_calculate_scores(char *args) {
    char *districts[32];
    int dist_count = 0;
    char *token = strtok(args, " ");
    while (token && dist_count < 32) {
        districts[dist_count++] = token;
        token = strtok(NULL, " ");
    }
    if (dist_count == 0) {
        printf("Usage: calculate_scores <district1> <district2> ...\n");
        return;
    }
    int pipes[32][2];
    pid_t pids[32];
    for (int i = 0; i < dist_count; i++) {
        char path[256];
        snprintf(path, sizeof(path), "%s/reports.dat", districts[i]); //verifying if the district exists
        if (access(path, F_OK) != 0) {
            printf("District '%s' not found, skipping.\n", districts[i]);
            pids[i] = -1;  // mark as skipped
            continue;
        }
        if (pipe(pipes[i]) < 0) {
            printf("Error: pipe failed for '%s'\n", districts[i]);
            continue;
        }
        pids[i] = fork();
        if (pids[i] < 0) {
            printf("Error: fork failed for '%s'\n", districts[i]);
            continue;
        }
        if (pids[i] == 0) {
            dup2(pipes[i][1], STDOUT_FILENO);
            close(pipes[i][0]);
            close(pipes[i][1]);
            execl("./scorer", "scorer", districts[i], NULL);
            _exit(1);
        }
        close(pipes[i][1]);
    }
    printf("=== Workload Report ===\n");
    for (int i = 0; i < dist_count; i++) {
        if (pids[i] == -1) continue; // skipped district
        char buf[2048];
        int total = 0;
        int n;
        while ((n = read(pipes[i][0], buf + total, sizeof(buf) - total - 1)) > 0)
            total += n;
        buf[total] = '\0';
        close(pipes[i][0]);
        printf("%s", buf);
        waitpid(pids[i], NULL, 0);
    }
    printf("=== End of Report ===\n");
}
void cleanup(void) {
    if (g_hub_mon_pid > 0) {
        kill(g_hub_mon_pid, SIGTERM);
        waitpid(g_hub_mon_pid, NULL, WNOHANG);
    }
    int fd = open(".monitor_pid", O_RDONLY);
    if (fd >= 0) {
        char buf[32];
        int len = read(fd, buf, sizeof(buf) - 1);
        close(fd);
        if (len > 0) {
            buf[len] = '\0';
            pid_t mon_pid = (pid_t)atoi(buf);
            kill(mon_pid, SIGINT);
        }
    }
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
        else if (strcmp(input, "exit") == 0){
            printf("Exiting city_hub...\n");
            cleanup();
            break;
        }
        else if (strlen(input) == 0)
            continue;
        else if (strncmp(input, "calculate_scores", 16) == 0) {
            char *args = input + 16;
            while (*args == ' ') args++;
            cmd_calculate_scores(args);
        }
        else
            printf("Unknown command: %s\n", input);
    }

    return 0;
}