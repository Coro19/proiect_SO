#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>
#include "city_manager.h"

#define PID_FILE ".monitor_pid"

void log_action(const char *district, const char *user, const char *role, const char *action) {
    char path[256];
    snprintf(path,sizeof(path), "%s/logged_district", district);
    int fd = open(path, O_WRONLY | O_APPEND);
    if(fd < 0) return;
    char entry[512];
    int len = snprintf(entry, sizeof(entry), "%ld\t%s\t%s\t%s\n", time(NULL), user, role, action);
    write(fd, entry, len);
    close(fd);
}
void notify_monitor(const char *district){
    int fd = open(PID_FILE, O_RDONLY);
    if(fd < 0) {
        log_action(district, "system", "system", "monitor could not be informed: no PID file");
        return;
    }
    char pidbuf[32];
    int len = read(fd, pidbuf, sizeof(pidbuf) - 1);
    close(fd);
    if (len <= 0) {
        log_action(district, "system", "system", "monitor could not be informed: empty PID file");
        return;
    }
    pidbuf[len] = '\0';
    pid_t monitor_pid = (pid_t)atoi(pidbuf);
    if (kill(monitor_pid, SIGUSR1) < 0) {
        log_action(district, "system", "system", "monitor could not be informed: signal failed");
        return;
    }
     log_action(district, "system", "system", "monitor informed of new report");
}
void cmd_add(const char *role, const char *user, const char *district){
    init_district(district);

    Report r;
    memset(&r,0,sizeof(r));
    printf("X: "); scanf("%lf", &r.lat);
    printf("Y: "); scanf("%lf", &r.lon);
    printf("Category (road/lighting/flooding/other): "); scanf("%31s",r.category);
    printf("Severity level(1/2/3): "); scanf("%d", &r.severity);
    printf("Description: "); scanf(" %127[^\n]",r.description);

    strncpy(r.inspector, user, INSPECTOR_LEN -1);
    r.timestamp = time(NULL);

    char path[256];
    snprintf(path,sizeof(path), "%s/reports.dat",district);
    if (!check_permission(path, role, 0, 1)) return;
    int fd = open(path, O_WRONLY | O_APPEND);
    if(fd < 0) return;
    // id assigned based on count
    struct stat st;
    fstat(fd, &st);
    r.id = (int)(st.st_size/sizeof(Report)) + 1;

    write(fd, &r, sizeof(Report));
    close(fd);

    log_action(district, user, role, "add");
    notify_monitor(district);
    printf("Report %d successfully added to %s\n", r.id, district);
}