#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include "city_manager.h"

void cmd_remove_report(const char *role, const char *user, const char *district, int id) {
    if (strcmp(role, "manager") != 0) {
        printf("Error: only managers can remove reports.\n");
        return;
    }

    char path[256];
    snprintf(path, sizeof(path), "%s/reports.dat", district);
    if (!check_permission(path, role, 1, 1)) return;
    int fd = open(path, O_RDWR);
    if (fd < 0) { printf("District '%s' not found.\n", district); return; }
    
    struct stat st;
    fstat(fd, &st);
    int total = (int)(st.st_size / sizeof(Report));

    // finding the right index
    int target_idx = -1;
    Report r;
    for (int i = 0; i < total; i++) {
        lseek(fd, (off_t)(i * sizeof(Report)), SEEK_SET);
        read(fd, &r, sizeof(Report));
        if (r.id == id) { target_idx = i; break; }
    }

    if (target_idx < 0) {
        printf("Report %d not found.\n", id);
        close(fd);
        return;
    }
    // shifting from the deleted one
    for (int i = target_idx + 1; i < total; i++) {
        lseek(fd, (off_t)(i * sizeof(Report)), SEEK_SET);
        read(fd, &r, sizeof(Report));
        lseek(fd, (off_t)((i - 1) * sizeof(Report)), SEEK_SET);
        write(fd, &r, sizeof(Report));
    }
    //truncating file size 
    ftruncate(fd, (off_t)((total - 1) * sizeof(Report)));
    close(fd);

    log_action(district, user, role, "remove_report");
    printf("Report %d removed from %s.\n", id, district);
}