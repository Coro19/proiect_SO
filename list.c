#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include "city_manager.h"

void permission_to_string(mode_t mode, char *str) {
    str[0] = (mode & S_IRUSR) ? 'r' : '-';
    str[1] = (mode & S_IWUSR) ? 'w' : '-';
    str[2] = (mode & S_IXUSR) ? 'x' : '-';
    str[3] = (mode & S_IRGRP) ? 'r' : '-';
    str[4] = (mode & S_IWGRP) ? 'w' : '-';
    str[5] = (mode & S_IXGRP) ? 'x' : '-';
    str[6] = (mode & S_IROTH) ? 'r' : '-';
    str[7] = (mode & S_IWOTH) ? 'w' : '-';
    str[8] = (mode & S_IXOTH) ? 'x' : '-';
    str[9] = '\0';
}

void cmd_list(const char *role, const char *user, const char *district) {
    check_symlink(district);
    char path[256];
    snprintf(path, sizeof(path), "%s/reports.dat", district);
    if (!check_permission(path, role, 1, 0)) return;
    struct stat st;
    if (stat(path, &st) < 0) {
        printf("District '%s' not found.\n", district);
        return;
    }

    char permstr[10];
    permission_to_string(st.st_mode, permstr);
    char timebuf[64];
    //formatting time
    strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", localtime(&st.st_mtime));
    printf("reports.dat  %s  %ld bytes  %s\n", permstr, st.st_size, timebuf);

    int fd = open(path, O_RDONLY);
    if (fd < 0) { printf("Error opening reports.dat\n"); return; }

    Report r;
    int count = 0;
    while (read(fd, &r, sizeof(Report)) == sizeof(Report)) {
        char ts[64];
        //formatting time for each individual report
        strftime(ts, sizeof(ts), "%Y-%m-%d %H:%M:%S", localtime(&r.timestamp));
        printf("[%d] %s | %s | %.4f,%.4f | sev:%d | %s | %s\n",
            r.id, r.inspector, r.category, r.lat, r.lon,
            r.severity, r.description, ts);
        count++;
    }

    close(fd);
    if (count == 0) printf("No reports in district '%s'.\n", district);
    //adding the final action
    log_action(district, user, role, "list");
}
