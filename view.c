
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include "city_manager.h"

void cmd_view(const char *role, const char *user, const char *district, int id) {
    check_symlink(district);
    char path[256];
    snprintf(path, sizeof(path), "%s/reports.dat", district);
    if (!check_permission(path, role, 1, 0)) return;
    int fd = open(path, O_RDONLY);
    if (fd < 0) { printf("District '%s' not found.\n", district); return; }
    
    Report r;
    int found = 0;
    while (read(fd, &r, sizeof(Report)) == sizeof(Report)) {
        if (r.id == id) {
            found = 1;
            break;
        }
    }
    close(fd);

    if (!found) { printf("Report %d not found.\n", id); return; }

    char ts[64];
    strftime(ts, sizeof(ts), "%Y-%m-%d %H:%M:%S", localtime(&r.timestamp));
    printf("ID:          %d\n", r.id);
    printf("Inspector:   %s\n", r.inspector);
    printf("Location:    %.4f, %.4f\n", r.lat, r.lon);
    printf("Category:    %s\n", r.category);
    printf("Severity:    %d\n", r.severity);
    printf("Description: %s\n", r.description);
    printf("Timestamp:   %s\n", ts);

    log_action(district, user, role, "view");
}