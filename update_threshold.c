#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include "city_manager.h"

void cmd_update_threshold(const char *role, const char *user, const char *district, int threshold) {
    if (strcmp(role, "manager") != 0) {
        printf("Error: only managers can update the threshold.\n");
        return;
    }

    char path[256];
    snprintf(path, sizeof(path), "%s/district.cfg", district);
    if (!check_permission(path, role, 0, 1)) return;
    struct stat st;
    if (stat(path, &st) < 0) {
        printf("Error: district.cfg not found.\n");
        return;
    }
    mode_t perms = st.st_mode & 0777;
    if (perms != 0640) {
        printf("Error: district.cfg permissions are not 640 (currently %o). Aborting.\n", perms);
        return;
    }

    int fd = open(path, O_WRONLY | O_TRUNC);
    if (fd < 0) { printf("Error opening district.cfg.\n"); return; }

    char buf[64];
    int len = snprintf(buf, sizeof(buf), "threshold=%d\n", threshold);
    write(fd, buf, len);
    close(fd);

    log_action(district, user, role, "update_threshold");
    printf("Threshold updated to %d in %s.\n", threshold, district);
}
